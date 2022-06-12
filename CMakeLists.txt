cmake_minimum_required(VERSION 3.16)
project(impossible-rocket CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external)

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
    add_compile_options(-Werror -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4 /permissive-)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(impossible-rocket WIN32)
    target_link_libraries(impossible-rocket PRIVATE SFML::Main)
else()
    add_executable(impossible-rocket)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_sources(impossible-rocket PRIVATE src/appicon.rc)
endif()

target_sources(impossible-rocket PRIVATE
    src/Main.cpp
    src/App.cpp
    src/AssetHolder.cpp
    src/BaseState.cpp
    src/GameLevel.cpp
    src/InputHandler.cpp
    src/MenuState.cpp
    src/ParticleEffect.cpp
    src/PauseMenu.cpp
    src/PhysicsWorld.cpp
    src/PlayerRocket.cpp
    src/PlayState.cpp
    src/SoundCentral.cpp)
target_link_libraries(impossible-rocket PRIVATE SFML::Graphics SFML::Audio ImGui-SFML::ImGui-SFML spdlog)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(impossible-rocket PRIVATE IMPOSSIBLE_ROCKET_DEBUG)
endif()

add_custom_target(format
    COMMAND clang-format -i `git ls-files *.hpp *.cpp`
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
add_custom_target(run COMMAND impossible-rocket WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
