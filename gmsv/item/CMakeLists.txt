cmake_minimum_required(VERSION 3.10)
project(item)

add_library(item
	item.c
	item_event.c
	item_gen.c
)
target_include_directories(char PUBLIC ${LUA_INCLUDE_DIR})
target_include_directories(item PUBLIC ${CMAKE_SOURCE_DIR}/include)

set(LINK_LIBS
	${LUA_LIBRARY}
	#
	#
)

target_link_libraries(item ${LINK_LIBS})
