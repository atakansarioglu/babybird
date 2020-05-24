cmake_minimum_required (VERSION 3.16.3)

project (babybird)
add_executable (babybird main.cpp)
set_property(TARGET babybird PROPERTY CXX_STANDARD 17)
target_link_libraries (babybird cpprest ssl crypto cpp_redis tacopie pthread)

add_definitions(-DREDISENDP="${REDISENDP}")
add_definitions(-DREDISPORT=${REDISPORT})
add_definitions(-DREDISPASS="${REDISPASS}")
add_definitions(-DAPIADDR="http://0.0.0.0:8080/api")
add_definitions(-DAPIVERS="v1")
