# Cmake version
cmake_minimum_required (VERSION 3.8)

# Name
project(Server)
project(Client)
project(Offline)

# Debug
set(CMAKE_BUILD_TYPE Debug)

# At least c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# OpenGL
cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)

# X11 and PNG
find_package(X11 REQUIRED)
find_package(PNG REQUIRED)

# Add sources to this project's executable.
file(GLOB_RECURSE sources src/*.cpp header/*.h)
add_executable (Server ${sources})
add_executable (Client ${sources})
add_executable (Offline ${sources})

# static linking
target_link_libraries(Server        PRIVATE -lX11 -lGL -lpthread -lpng -lstdc++fs)
target_link_libraries(Client        PRIVATE -lX11 -lGL -lpthread -lpng -lstdc++fs)
target_link_libraries(Offline       PRIVATE -lX11 -lGL -lpthread -lpng -lstdc++fs)

# library include
target_include_directories (Server  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/header/)
target_include_directories (Client  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/header/)
target_include_directories (Offline PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/header/)

# Preprocessor defines
target_compile_definitions(Server   PRIVATE SERVER=1 CLIENT=0 OFFLINE=0)
target_compile_definitions(Client   PRIVATE SERVER=0 CLIENT=1 OFFLINE=0)
target_compile_definitions(Offline  PRIVATE SERVER=0 CLIENT=0 OFFLINE=1)

# Build type
set(CMAKE_BUILD_TYPE, RELEASE) # RELEASE OR DEBUG