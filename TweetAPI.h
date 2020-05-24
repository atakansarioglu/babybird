/**
 * @file      TweetAPI.h
 * @author    Atakan S.
 * @version   1.0
 * @brief     Tweet service class used for BabyBird project.
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

#ifndef _H_TWEETAPI_H_
#define _H_TWEETAPI_H_

#include "Tweet.h"
#include "IDatastore.h"
#include <iostream>
#include <memory>

class TweetAPI
{
private:
    // Datastore object.
    std::shared_ptr<IDatastore> m_spDatastore;

public:
    /*
     * @brief Constructor of TweetAPI, Lazy connection.
     * @param spDatastore Dependency injection for Datastore.
     */
    TweetAPI(std::shared_ptr<IDatastore> spDatastore)
        : m_spDatastore(spDatastore) { }

    bool AddTweet(const std::string &content, const int userId)
    {
        if (!m_spDatastore->IsConnected() && m_spDatastore->Connect() == false)
        {
            return false;
        }

        // Obtain unique tweet Id.
        auto tweetId = m_spDatastore->GetUniqueNumber();
        if (tweetId == -1)
        {
            return false;
        }

        // Create new tweet object and serialize.
        auto tweetAsString = Tweet(content, tweetId, userId).GetJson().serialize();

        // Send to the datastore.
        return m_spDatastore->AddTweet(userId, tweetAsString);
    }
};

#endif
