# Impossible Rocket - In Development

## What is this?
Impossible Rocket is a simple 2D physics game written in C++, using the SFML binding of SFML.
The player takes control of a rocket with difficult controls and battles against the force of gravity to collect objectives
on each level whilst the clock times their success. 

## Requirements
- A Compiler
- CMake (>=3.16)

## Build Instructions

```
cmake -B build -DBUILD_SHARED_LIBS="FALSE"
cmake --build build
```

## Run Instructions
### Windows
```
cp ./build/_deps/sfml-src/extlibs/bin/x64/openal32.dll ./build/Debug/.
./build/Debug/impossible-rocket.exe
```

### MacOS & Linux 
```
./build/impossible-rocket
```
