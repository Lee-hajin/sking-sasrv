cmake_minimum_required(VERSION 3.10)
project(gmsv)

# set(CMAKE_C_FLAGS "-std=c11")
# set(CMAKE_C_COMPILER gcc)
# set(CMAKE_C_FLAGS -m32)
set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_FLAGS "-std=gnu99")
# set(CMAKE_BUILD_TYPE RelWithDebInfo)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
find_package(MySQL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Lua51 REQUIRED)
# find_library(LUA_LIBRARY NAMES libluajit.a PATHS /opt/lua REQUIRED NO_DEFAULT_PATH)

add_subdirectory(mylua)
add_subdirectory(map)
add_subdirectory(char)
add_subdirectory(battle)
add_subdirectory(magic)
add_subdirectory(item)
add_subdirectory(npc)
add_subdirectory(newbilu)

add_library(descrypt descrypt.c)
target_include_directories(descrypt PUBLIC include)

add_library(lssproto lssproto_serv.c lssproto_util.c)
target_include_directories(lssproto PUBLIC include ${ZLIB_INCLUDE_DIRS} ${LUA_INCLUDE_DIR})
target_link_libraries(lssproto ${LUA_LIBRARY} ${ZLIB_LIBRARIES})

add_library(saacproto saacproto_cli.c saacproto_util.c)
target_include_directories(saacproto PUBLIC include)
target_link_libraries(saacproto lssproto)

add_executable(gmsvjt 
	main.c
	autil.c
	buf.c
	callfromac.c
	callfromcli.c
	configfile.c
	function.c
	handletime.c
	init.c
	link.c
	log.c
	msignal.c
	net.c
	object.c
	util.c)
target_include_directories(gmsvjt PUBLIC ${MYSQL_INCLUDE_DIR} ${LUA_INCLUDE_DIR})
target_include_directories(gmsvjt PUBLIC ${CMAKE_SOURCE_DIR}/include)

# message("------------------")
# message(${MYSQL_INCLUDE_DIR})
# message("------------------")
# message(${MYSQL_LIBRARIES})
# message("------------------")

set(LINK_LIBS 
	${MYSQL_LIBRARIES}
	# 
	char
	npc
	map
	item
	magic
	battle
	newbilu
	mylua
	# 
	${LUA_LIBRARY}
	# 
	saacproto
	lssproto
	descrypt
	pthread
	dl
	m
)
	
# target_link_libraries(gmsvjt PUBLIC ${MYSQL_LIBRARIES})
target_link_libraries(gmsvjt
	${LINK_LIBS})
