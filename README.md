# Impossible Rocket - In Development

## What is this?
Impossible Rocket is a simple 2D physics game written in C++, using the SFML binding of SFML.
The player takes control of a rocket with difficult controls and battles against the force of gravity to collect objectives
on each level whilst the clock times their success. 

## Requirements
- Microsoft Visual Studio 2022
- vcpkg
- SFML 3 (x64)
- spdlog (spdlog:x64-window)

## Build Instructions (Windows)
- Download SFML 3 (master on SFML repository)
- Extract SFML to be under the `C:\` folder
- Install spdlog:x64-windows on vcpkg
- Open solution in Visual Studio
- Copy SFML .dll files to the project root directory
- Build x64 configuration in release or debug
- Run and enjoy!