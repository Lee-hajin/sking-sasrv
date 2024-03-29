cmake_minimum_required(VERSION 3.10)
project(saac)

# set(CMAKE_C_FLAGS -m32)
set(CMAKE_C_STANDARD 11)

add_executable(saacjt main.c)
add_library(saacproto saacproto_serv.c saacproto_util.c mail.c)
target_sources(saacjt
    PRIVATE acfamily.c
    PRIVATE char.c
    PRIVATE db.c
    PRIVATE lock.c
    PRIVATE main.c
    PRIVATE md5.c
    PRIVATE recv.c
    PRIVATE sasql.c
    PRIVATE util.c
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
find_package(MySQL)
target_include_directories(saacjt PRIVATE include)
target_include_directories(saacjt PUBLIC ${MYSQL_INCLUDE_DIR})
target_include_directories(saacproto PRIVATE include)
target_link_libraries(saacjt PUBLIC ${MYSQL_LIBRARIES})
target_link_libraries(saacjt PRIVATE saacproto)
