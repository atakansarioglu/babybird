/**
 * @file      Tweet.h
 * @author    Atakan S.
 * @version   1.0
 * @brief     Tweet object used for BabyBird OOPD design.
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

#ifndef _H_TWEET_H_
#define _H_TWEET_H_

#include <iostream>
#include <cpprest/json.h>

class Tweet
{
private:
    int m_tweetId;
    int m_userId;
    std::string m_content;

public:
    /*
     * @brief Constructor.
     * @param content The text of the tweet.
     * @param tweetId Unique id number.
     * @param userId User
     */
    Tweet(const std::string &content, const int tweetId, const int userId)
        : m_content(content), m_tweetId(tweetId), m_userId(userId) {}

    /*
     * @brief Converting constructor.
     * @param serializedTweet Tweet as JSON formatted string.
     */
    Tweet(const std::string &serializedTweet)
    {
        auto tweetJson = web::json::value::parse(serializedTweet);
        auto content = tweetJson["content"].as_string();
        auto tweetId = tweetJson["tweetId"].as_integer();
        auto userId = tweetJson["userId"].as_integer();
        *this = Tweet(content, tweetId, userId);
    }

    /*
     * @brief Getter for Content.
     * @return Content string.
     */
    std::string GetContent() const
    {
        return m_content;
    }

    /*
     * @brief Getter for Content.
     * @return Content string.
     */
    int GetTweetId() const
    {
        return m_tweetId;
    }

    /*
     * @brief Getter for Content.
     * @return Content string.
     */
    int GetUserId() const
    {
        return m_userId;
    }

    /*
     * @brief Get as JSON.
     * @return Tweet converted to JSON object.
     */
    web::json::value GetJson() const
    {
        auto tweetJson = web::json::value::object();
        tweetJson["tweetId"] = web::json::value::number(GetTweetId());
        tweetJson["userId"] = web::json::value::number(GetUserId());
        tweetJson["content"] = web::json::value::string(GetContent());
        return tweetJson;
    }

    /*
     * @brief Less operator for std::less
     * @return True if smaller than other.
     */
    bool operator<(const Tweet &other) const
    {
        return GetTweetId() < other.GetTweetId();
    }

    /*
     * @brief Greater operator for std::greater
     * @return True if greater than other.
     */
    bool operator>(const Tweet &other) const
    {
        return GetTweetId() > other.GetTweetId();
    }
};

#endif
