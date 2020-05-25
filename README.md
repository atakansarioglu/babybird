# Twitter-like Messaging Service for Fun!
## Prerequisites
* Compatible with Ubuntu 20.04 LTS
* Get the packages `apt-get install cmake g++ libcpprest-dev -y`
* Install [`cpp_redis`](https://github.com/cpp-redis/cpp_redis/wiki/Mac-&-Linux-Install) 
* Redis in-memory datastore

## Compile
Put your actual credentials below.
~~~~
cmake . -DREDISENDP="example.redis.server.com" -DREDISPORT=12345 -DREDISPASS="secret_password"
make
~~~~

## API Usage with cURL
### User 1 follows user 2
`curl -v --request PUT localhost:8080/api/v1/follow/1/2`

### Unfollow
`curl -v --request DELETE localhost:8080/api/v1/follow/1/2`

### Get Timeline for user 1
`curl -v --request GET localhost:8080/api/v1/timeline/1`

### Post tweet for user 1
`curl -v --request POST --data '{"userId": 1, "content": "Hello World!"}' localhost:8080/api/v1/tweet`

