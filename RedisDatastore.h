/**
 * @file      RedisDatastore.h
 * @author    Atakan S.
 * @version   1.0
 * @brief     Redis Datastore connector, concrete class.
 *
 * @copyright Copyright (c) 2020 Atakan SARIOGLU ~ www.atakansarioglu.com
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

#ifndef _H_REDISDATASTORE_H_
#define _H_REDISDATASTORE_H_

#include "IDatastore.h"
#include <cpp_redis/cpp_redis>
#include <string>
#include <chrono>

class RedisDatastore : public IDatastore
{
private:
    std::string m_endpoint;
    int m_port;
    std::string m_credentials;
    cpp_redis::client m_client;
    std::chrono::duration<double> m_commitTimeout;

public:
    /*
     * @brief Constructor for Redis connector.
     * @param endpoint Endpoint address.
     * @param port Port number of Redis.
     * @param credentials Database password.
     * @param timeout Time to wait for a request.
     */
    RedisDatastore(const std::string &endpoint, const int port, const std::string &credentials, const double timeout = 1.0)
        : m_endpoint(endpoint), m_port(port), m_credentials(credentials), m_commitTimeout(timeout) {}

    /*
     * @brief Connect to Redis
     * @return True on success.
     */
    bool Connect()
    {
        if (IsConnected())
        {
            return true;
        }

        // Connect to Redis.
        m_client.connect(m_endpoint, m_port);

        // Authenticate using the provided credentials.
        m_client.auth(m_credentials);

        // Return the connection state.
        return IsConnected();
    }

    /*
     * @brief Disconnect from Redis
     * @return True on success.
     */
    bool Disconnect()
    {
        // Disconnect.
        m_client.disconnect();
        return true;
    }

    /*
     * @brief Get connection state.
     * @return True if connected.
     */
    bool IsConnected() const
    {
        // Return the connection state.
        return m_client.is_connected();
    }

    /*
     * @brief Generate Unique Increasing ID Number
     * @return Non-negative unique ID on success, -1 on error.
     */
    int GetUniqueNumber()
    {
        if (!IsConnected())
        {
            return -1;
        }

        // Increment and get the value on Redis.
        auto request = m_client.incr("uniqueNumber");
        m_client.sync_commit(m_commitTimeout);

        auto response = request.get();
        if (response.ok() && response.is_integer())
        {
            // Success.
            return response.as_integer();
        }

        // Failure.
        return -1;
    }

    /*
     * @brief Adds tweet to Redis memory cache.
     * @param userId
     * @param tweetAsString Serialized tweet object.
     * @param maxTweets Keep no more than this number on datastore, -1 for unlimited.
     * @return True on success.
     */
    bool AddTweet(const int userId, const std::string &tweetAsString, int maxTweets = 10)
    {
        if (!IsConnected())
        {
            return false;
        }

        // Push the new tweet to the corresponding users tweets list.
        auto request1 = m_client.lpush("tweets:" + std::to_string(userId), {tweetAsString});
        if(maxTweets > 0) {
            auto request2 = m_client.ltrim("tweets:" + std::to_string(userId), 0, maxTweets - 1);
        }

        // Commit.
        m_client.sync_commit(m_commitTimeout);

        // Return.
        return request1.get().ok();
    }

    /*
     * @brief Get recent tweets of all followed users.
     * @param userIdVector users to fetch the recent tweets.
     * @param tweets Output vector for fetched tweets.
     * @param numberOfTweets Number of tweets for each user, -1 for all tweets.
     * @return True on success.
     */
    bool GetRecentTweets(const std::vector<int> &userIdVector,
                         std::vector<std::string> &tweets, int numberOfTweets = -1)
    {
        if (!IsConnected())
        {
            return false;
        }

        // Issue all requests in a loop.
        std::vector<std::future<cpp_redis::reply>> requestVector;
        for (auto userId : userIdVector)
        {
            // Get left items of Redis List object.
            requestVector.push_back(m_client.lrange("tweets:" + std::to_string(userId),
                                                    0, (numberOfTweets == -1) ? -1 : numberOfTweets - 1));
        }

        // Commit once.
        m_client.sync_commit(m_commitTimeout);

        // Add the returned tweets to the output vector.
        for (auto &request : requestVector)
        {
            auto response = request.get();
            if (response.ok() == false || response.is_array() == false)
            {
                return false;
            }

            for (const auto &element : response.as_array())
            {
                tweets.push_back(element.as_string());
            }
        }

        // Return.
        return true;
    }

    /*
     * @brief Get followed users of userId.
     * @param userId users to fetch the recent tweets.
     * @param followees Output vector for followees.
     * @return True on success.
     */
    bool GetFollowees(const int userId, std::vector<int> &followees)
    {
        if (!IsConnected())
        {
            return false;
        }

        // Get all members of Redis Hash Set object.
        auto request = m_client.smembers("followees:" + std::to_string(userId));

        // Commit.
        m_client.sync_commit(m_commitTimeout);

        // Check the response.
        auto response = request.get();
        if (response.ok() == false || response.is_array() == false)
        {
            return false;
        }

        // Cast the returned strings into integer and push to output vector.
        for (const auto &element : response.as_array())
        {
            try
            {
                int followeeId = std::stoi(element.as_string());
                followees.push_back(followeeId);
            }
            catch (...)
            {
                return false;
            }
        }

        // Return.
        return true;
    }

    /*
     * @brief Create userId->followeeId record.
     * @param userId follower
     * @param followeeId followee
     * @return True on success.
     */
    bool AddFollowee(const int userId, const int followeeId)
    {
        if (!IsConnected())
        {
            return false;
        }

        // Add to Hash Set.
        auto request = m_client.sadd("followees:" + std::to_string(userId), {std::to_string(followeeId)});

        // Commit.
        m_client.sync_commit(m_commitTimeout);

        // Return the result.
        return request.get().ok();
    }

    /*
     * @brief Remove userId->followeeId record.
     * @param userId follower
     * @param followeeId followee
     * @return True on success.
     */
    bool DelFollowee(const int userId, const int followeeId)
    {
        if (!IsConnected())
        {
            return false;
        }

        // Remove from Hash Set.
        auto request = m_client.srem("followees:" + std::to_string(userId), {std::to_string(followeeId)});

        // Commit.
        m_client.sync_commit(m_commitTimeout);

        // Return the result.
        return request.get().ok();
    }

    /*
     * @brief Destructor. Disconnect if necessary.
     */
    ~RedisDatastore()
    {
        if (!IsConnected())
        {
            Disconnect();
        }
    }
};

#endif
