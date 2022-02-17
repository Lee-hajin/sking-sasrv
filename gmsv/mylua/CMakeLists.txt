cmake_minimum_required(VERSION 3.10)

project(mylua)

add_library(mylua
    mylua.c
    ablua.c
    init.c
    charbase.c
    npcbase.c
    function.c
    lssprotobase.c
    battlebase.c
    objectbase.c
    mapbase.c
    otherbase.c
    configbase.c
    itembase.c
    magicbase.c
    offlinebase.c
    netbase.c
    enemytempbase.c
    sasqlbase.c
    saacproto.c
    family.c
    petskill.c
    profession_skill.c
)

target_include_directories(mylua PUBLIC ${MYSQL_INCLUDE_DIR} ${LUA_INCLUDE_DIR})
target_include_directories(mylua PUBLIC ${CMAKE_SOURCE_DIR}/include)

set(LINK_LIBS
	${MYSQL_LIBRARIES}
	#
	item
	#
	${LUA_LIBRARY}
)

target_link_libraries(mylua ${LINK_LIBS})