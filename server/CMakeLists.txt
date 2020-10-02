cmake_minimum_required(VERSION 3.3)

project(autocraft_server)

if ("cxx_std_17" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    set(CXX17_SUPPORTED TRUE)
else()
    set(CXX17_SUPPORTED FALSE)
endif()

if ("cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    set(CXX20_SUPPORTED TRUE)
else()
    set(CXX20_SUPPORTED FALSE)
endif()

set(BOOST_REQUIRED_COMPONENTS)

if (CXX17_SUPPORTED)
else()
    list(APPEND BOOST_REQUIRED_COMPONENTS filesystem)
endif()

find_package(Boost
    REQUIRED
    COMPONENTS ${BOOST_REQUIRED_COMPONENTS}
)


#find_package(Boost)

set(INCDIR .)
set(SRCDIR .)

set(SOURCES
  config.hpp
  lock.hpp
  noncopyable.hpp
  containers.hpp
  common.hpp
  vector.hpp
  vector.cpp
  time.hpp
  time.cpp
  random.hpp
  random.cpp
  save_load.hpp
  save_load.cpp
  log.hpp
  log.cpp
  any.hpp
  utils.hpp
  utils.cpp
  filesystem.hpp
  
  persistence.hpp
  redis_helper.hpp
  redis_helper.cpp
  redis_persistence.hpp
  redis_persistence.cpp
  
  entities.hxx
  entities.hpp
  entities.cpp
  entity.hpp
  entity.cpp
  
  player.hpp
  player.cpp

  connection.hpp
  connection.cpp
  player_conn.hpp
  player_conn.cpp
  master_conn.hpp
  master_conn.cpp
  server.hpp
  server.cpp

  main.cpp
)

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")
endif()

link_directories(${Boost_LIBRARY_DIRS})


add_definitions(-DBOOST_BIND_NO_PLACEHOLDERS)
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif()

if (CXX20_SUPPORTED)
    set(REQUIRED_STANDARD 20)
else()
    if (CXX17_SUPPORTED)
        set(REQUIRED_STANDARD 17)
    else()
        set(REQUIRED_STANDARD 11)
    endif()
endif()




if(MSVC)
    add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
endif()

add_executable(autocraft-server
  ${SOURCES}
  )

set_property(TARGET autocraft-server PROPERTY CXX_STANDARD ${REQUIRED_STANDARD})
set_property(TARGET autocraft-server PROPERTY CXX_STANDARD_REQUIRED ON)  
  
target_compile_definitions(autocraft-server PRIVATE AUTOCRAFT_BUILDING_THE_LIB)
  
target_include_directories(autocraft-server
PRIVATE
    ${INCDIR}
PUBLIC
    third
    ${Boost_INCLUDE_DIRS}
)

set(DEPS)

if(UNIX)
    list(APPEND DEPS pthread)
endif()

if (CXX17_SUPPORTED)
    if(UNIX)
        list(APPEND DEPS stdc++fs)
    endif()
else()
    list(APPEND DEPS ${Boost_FILESYSTEM_LIBRARY})
endif()

if(UNIX)
    list(APPEND DEPS pthread)
endif()

target_link_libraries(autocraft-server ${DEPS})
