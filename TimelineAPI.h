/**
 * @file      TimelineAPI.h
 * @author    Atakan S.
 * @version   1.0
 * @brief     Timeline service of BabyBird OOP design.
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

#ifndef _H_TIMELINEAPI_H_
#define _H_TIMELINEAPI_H_

#include "Tweet.h"
#include "IDatastore.h"
#include <cpprest/json.h>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

class TimelineAPI
{
private:
    // Datastore object.
    std::shared_ptr<IDatastore> m_spDatastore;

    /*
     * @brief Create the response string.
     * @param tweets Tweets to include in the response.
     * @return Response body.
     */
    std::string createResponse(const std::vector<Tweet> &tweets) const
    {
        auto jsonObject = web::json::value::array();
        for (size_t i = 0; i < tweets.size(); ++i)
        {
            jsonObject[i] = tweets[i].GetJson();
        }
        return jsonObject.serialize();
    }

    /*
     * @brief Create the timeline by picking most recent tweets in all tweets.
     *        Runs in O(NlogK) time where N is total tweets and K is maxTweets.
     * @param tweets All tweets that belong to the users followees.
     * @return The most recent tweets.
     */
    std::vector<Tweet> createTimeline(const std::vector<Tweet> &tweets, const int maxTweets) const
    {
        // Reference type to use in STL container.
        using tweetref_t = std::reference_wrapper<const Tweet>;

        // Minimum Priority Queue
        std::priority_queue<tweetref_t, std::deque<tweetref_t>, std::greater<Tweet>> minPq;

        // Check all the tweets.
        for (size_t i = 0; i < tweets.size(); ++i)
        {
            // Push tweets to the MinPQ only if they are more recent than the oldest tweet in MinPQ.
            if (minPq.size() < maxTweets || !minPq.empty() && minPq.top().get() < tweets[i])
            {
                minPq.push(std::ref(tweets[i]));
            }

            // Pop the least recent if the size exceeds.
            if (minPq.size() > maxTweets)
            {
                minPq.pop();
            }
        }

        // Get the objects out of the MinPQ and push to a temporary vector.
        std::vector<Tweet> timelineTweets;
        while (!minPq.empty())
        {
            timelineTweets.push_back(minPq.top().get());
            minPq.pop();
        }

        // Return the reverse ordered tweets i.e. the most recent goes first.
        return { timelineTweets.rbegin(), timelineTweets.rend() };
    }

public:
    /*
     * @brief Constructor of TimelineAPI
     * @param spDatastore Dependency injection for Datastore.
     */
    TimelineAPI(std::shared_ptr<IDatastore> spDatastore)
        : m_spDatastore(spDatastore) { }

    /*
     * @brief Get timeline of the corresponding user.
     * @param userId User
     * @param timeline The output string having the JSON formatted timeline.
     * @param maxTweets Number of max tweets to return.
     * @return True on success.
     */
    bool GetTimeline(int userId, std::string &timeline, const int maxTweets = 10)
    {
        if (!m_spDatastore->IsConnected() && m_spDatastore->Connect() == false)
        {
            return false;
        }

        // Get the users followed by the user.
        std::vector<int> followees;
        if (m_spDatastore->GetFollowees(userId, followees) == false)
        {
            return false;
        }

        // Include senf tweets.
        followees.push_back(userId);

        // Get tweets of all users followed by the user.
        std::vector<std::string> tweetsAsString;
        if (m_spDatastore->GetRecentTweets(followees, tweetsAsString) == false)
        {
            return false;
        }

        // Deserialize tweets and push to vector.
        std::vector<Tweet> allTweets;
        for (const auto &str : tweetsAsString)
        {
            allTweets.emplace_back(str);
        }

        // Create the timeline.
        auto timelineTweets = createTimeline(allTweets, maxTweets);

        // Create the response string.
        timeline = createResponse(timelineTweets);

        // Success.
        return true;
    }
};

#endif
