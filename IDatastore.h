/**
 * @file      IDatastore.h
 * @author    Atakan S.
 * @version   1.0
 * @brief     Abstraction for Datastore interface. Strategy design pattern.
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

#ifndef _H_IDATASTORE_H_
#define _H_IDATASTORE_H_

#include <iosfwd>
#include <vector>

struct IDatastore
{
    virtual bool Connect() = 0;
    virtual bool Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    virtual int GetUniqueNumber() = 0;
    virtual bool AddTweet(const int userId, const std::string &tweetAsString, int maxTweets = 10) = 0;
    virtual bool GetRecentTweets(const std::vector<int> &userIdVector,
                                 std::vector<std::string> &tweets, int numberOfTweets = -1) = 0;
    virtual bool GetFollowees(const int userId, std::vector<int> &followees) = 0;
    virtual bool AddFollowee(const int userId, const int followeeId) = 0;
    virtual bool DelFollowee(const int userId, const int followeeId) = 0;
    virtual ~IDatastore() = default;
};

#endif
