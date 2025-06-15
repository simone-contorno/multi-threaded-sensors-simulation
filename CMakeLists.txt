cmake_minimum_required(VERSION 3.10)
project(multi-threaded-sensors-simulation VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add compiler warnings
#if(MSVC)
#    add_compile_options(/W4)
#else()
#    add_compile_options(-Wall -Wextra -Wpedantic)
#endif()

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/simulator
    ${PROJECT_SOURCE_DIR}/include/sensors
    ${PROJECT_SOURCE_DIR}/include/processing
    ${PROJECT_SOURCE_DIR}/include/fdir
    ${PROJECT_SOURCE_DIR}/include/logging
)

# Add source files
set(SOURCES
    src/simulator/Simulator.cpp
    src/sensors/ImuSensor.cpp
    src/sensors/GnssSensor.cpp
    src/processing/ProcessingUnit.cpp
    src/fdir/Fdir.cpp
    src/logging/Logger.cpp
    main.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        Threads::Threads
)

# Install rules
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin)
