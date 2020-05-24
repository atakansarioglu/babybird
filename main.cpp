/**
 * @file      main.cpp
 * @author    Atakan S.
 * @version   1.0
 * @brief     BabyBird project for Twiter Object Oriented System Design.
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

#include "RedisDatastore.h"
#include "TweetAPI.h"
#include "FollowAPI.h"
#include "TimelineAPI.h"
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <iostream>
#include <memory>

int main()
{
    // Initialize Redis Datastore connector.
    auto spDatastore = std::make_shared<RedisDatastore>(REDISENDP, REDISPORT, REDISPASS);

    // Create several API backend services.
    TweetAPI tweetApi(spDatastore);
    TimelineAPI timelineApi(spDatastore);
    FollowAPI followApi(spDatastore);

    // Start API Server.
    web::uri_builder uri(APIADDR);
    uri.append_path(APIVERS);
    web::http::experimental::listener::http_listener apiServer(uri.to_uri().to_string());

    // Dispatcher for POST requests.
    apiServer.support(web::http::methods::POST, [&](web::http::http_request request) {
        // Sptlit the path.
        auto uriParts = web::uri::split_path(request.relative_uri().path());

        // Serve TweetAPI request.
        if (uriParts.size() == 1 && uriParts[0] == "tweet")
        {
            // Parse the JSON body.
            auto bodyJson = request.extract_json(true).get();
            if (bodyJson.has_string_field("content") && bodyJson.has_integer_field("userId"))
            {
                // Create new Tweet.
                auto content = bodyJson["content"].as_string();
                auto userId = bodyJson["userId"].as_integer();
                request.reply(tweetApi.AddTweet(content, userId) ? 
                    web::http::status_codes::Created : 
                    web::http::status_codes::InternalError);
                return;
            }
        }

        // No API exists for that request.
        request.reply(web::http::status_codes::NotFound);
    });

    // Dispatcher for GET requests.
    apiServer.support(web::http::methods::GET, [&](web::http::http_request request) {
        // Sptlit the path.
        auto uriParts = web::uri::split_path(request.relative_uri().path());

        // Serve TimelineAPI request.
        if (uriParts.size() == 2 && uriParts[0] == "timeline")
        {
            // Extract userId.
            int userId = -1;
            try
            {
                userId = std::stoi(uriParts[1]);
            }
            catch (...)
            {
                request.reply(web::http::status_codes::BadRequest);
                return;
            }

            // Get and return timeline for the user.
            std::string timeline;
            if (timelineApi.GetTimeline(userId, timeline))
            {
                request.reply(web::http::status_codes::OK, timeline);
                return;
            } 
            else
            {
                request.reply(web::http::status_codes::InternalError);
                return;
            }                
        }

        // No API exists for that request.
        request.reply(web::http::status_codes::NotFound);
    });

    // Dispatcher for PUT and DEL requests.
    auto followRequestDispatcher = [&](web::http::http_request request) {
        // Sptlit the path.
        auto uriParts = web::uri::split_path(request.relative_uri().path());
                
        // Serve FollowAPI request.
        if (uriParts.size() == 3 && uriParts[0] == "follow")
        {            
            // Extract followerId and followeeId.
            int followerId = -1, followeeId = -1;
            try
            {
                followerId = std::stoi(uriParts[1]);
                followeeId = std::stoi(uriParts[2]);
            }
            catch (...)
            {
                request.reply(web::http::status_codes::BadRequest);
                return;
            }

            // Process follow and unfollow requests.
            if (request.method() == web::http::methods::PUT 
                && followApi.Follow(followerId, followeeId))
            {
                request.reply(web::http::status_codes::Created);
                return;
            }
            else if (request.method() == web::http::methods::DEL 
                && followApi.Unfollow(followerId, followeeId))
            {
                request.reply(web::http::status_codes::NoContent);
                return;
            } else {
                request.reply(web::http::status_codes::InternalError);
                return;
            }
        }

        // No API exists for that request.
        request.reply(web::http::status_codes::NotFound);
    };

    // Listen for PUT and DEL requests.
    apiServer.support(web::http::methods::PUT, followRequestDispatcher);
    apiServer.support(web::http::methods::DEL, followRequestDispatcher);

    // Start API server.
    apiServer
        .open()
        .then([]() { std::cout << "Started"; })
        .wait();

    // Stop API Server on keystroke.
    std::getchar();
    apiServer
        .close()
        .wait();

    // Exit.
    return 0;
}
