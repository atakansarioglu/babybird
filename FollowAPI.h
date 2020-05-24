/**
 * @file      FollowAPI.h
 * @author    Atakan S.
 * @version   1.0
 * @brief     FollowAPI of BabyBird project.
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

#ifndef _H_FOLLOWAPI_H_
#define _H_FOLLOWAPI_H_

#include "IDatastore.h"
#include <iostream>

class FollowAPI
{
private:
    // Datastore object.
    std::shared_ptr<IDatastore> m_spDatastore;

public:
    /*
     * @brief Constructor of FollowAPI
     * @param spDatastore Dependency injection for Datastore.
     */
    FollowAPI(std::shared_ptr<IDatastore> spDatastore)
        : m_spDatastore(spDatastore) {}

    /*
     * @brief Adds follower->followee pair to datastore
     * @param spDatastore Dependency injection for Datastore.
     * @return True on success.
     */
    bool Follow(const int followerId, const int followeeId)
    {
        if (!m_spDatastore->IsConnected() && m_spDatastore->Connect() == false)
        {
            return false;
        }

        return m_spDatastore->AddFollowee(followerId, followeeId);
    }

    /*
     * @brief Removes follower->followee pair if exitst.
     * @param spDatastore Dependency injection for Datastore.
     * @return True on success.
     */
    bool Unfollow(const int followerId, const int followeeId)
    {
        if (!m_spDatastore->IsConnected() && m_spDatastore->Connect() == false)
        {
            return false;
        }

        return m_spDatastore->DelFollowee(followerId, followeeId);
    }
};

#endif
