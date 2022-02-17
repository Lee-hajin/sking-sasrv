cmake_minimum_required(VERSION 3.10)
project(newbilu)

add_library(newbilu
    characters.c
    crypto.c
    newshop.c
    sasql.c
)
target_include_directories(newbilu PUBLIC ../include)
target_include_directories(newbilu PUBLIC /opt/lua)
