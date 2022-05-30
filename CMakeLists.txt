cmake_minimum_required(VERSION 3.16)
project(impossible-rocket CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external)

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
	add_compile_options(-Werror -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/WX /W4 /permissive-)
endif()

add_executable(impossible-rocket src/Main.cpp src/App.cpp src/AssetHolder.cpp src/BaseState.cpp
			src/GameLevel.cpp src/InputHandler.cpp src/MenuState.cpp src/ParticleEffect.cpp 
			src/PhysicsWorld.cpp src/PlayerRocket.cpp src/PlayState.cpp)

target_link_libraries(impossible-rocket PRIVATE SFML::Graphics SFML::Audio ImGui-SFML::ImGui-SFML spdlog)
