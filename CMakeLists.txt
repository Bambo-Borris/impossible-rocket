cmake_minimum_required(VERSION 3.16)
project(impossible-rocket CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external)

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
    add_compile_options(-Werror -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4 /permissive-)
endif()

set(SOURCE_LIST
                src/Main.cpp
                src/App.cpp
                src/AssetHolder.cpp
                src/BaseState.cpp
                src/GameLevel.cpp
                src/InputHandler.cpp
                src/MenuState.cpp
                src/ParticleEffect.cpp
                src/PhysicsWorld.cpp
                src/PlayerRocket.cpp
                src/PlayState.cpp)

set(DEPS_LIST SFML::Graphics SFML::Audio ImGui-SFML::ImGui-SFML spdlog) 

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(SOURCE_LIST ${SOURCE_LIST} src/appicon.rc)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(impossible-rocket WIN32 ${SOURCE_LIST})
    target_link_libraries(impossible-rocket PRIVATE ${DEPS_LIST} SFML::Main)
else()
    add_executable(impossible-rocket ${SOURCE_LIST})
    target_link_libraries(impossible-rocket PRIVATE ${DEPS_LIST})
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(impossible-rocket PRIVATE IMPOSSIBLE_ROCKET_DEBUG)
endif()
