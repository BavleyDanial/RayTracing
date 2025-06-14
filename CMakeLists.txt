cmake_minimum_required(VERSION 3.13)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(THREADS_PREFER_PTHREAD_FLAG ON)

project(RayTracing)
# Set Build Type to Debug by default
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Set architecture to either 32 or 64 depending on system
set(ARCHITECTURE "32")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCHITECTURE "64")
endif()

# OutputDir to be used for all projects, when using multi-config build it should only create a bin directory
if (NOT CMAKE_CONFIGURATION_TYPES)
    set(OutputDir "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
else()
    set(OutputDir "${CMAKE_SOURCE_DIR}/bin/")
endif()

if(MSVC)
    # MSVC should automatically link the multithreading library
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")  # Use /MT for multithreading
elseif(MINGW)
    # Add pthread flag for GCC-based compilers (e.g., MinGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

file(GLOB_RECURSE SOURCE "${CMAKE_SOURCE_DIR}/src/**.cpp")
file(GLOB_RECURSE GLAD_SRC "${CMAKE_SOURCE_DIR}/vendor/glad/src/**.c")

file(GLOB_RECURSE HEADER_SOURCE "${CMAKE_SOURCE_DIR}/header/**.h")
set(HEADER "${CMAKE_SOURCE_DIR}/header/")

set(GLM_HEADER "${CMAKE_SOURCE_DIR}/vendor/glm/")
set(STB_HEADER "${CMAKE_SOURCE_DIR}/vendor/stb/")
set(GLFW_HEADER "${CMAKE_SOURCE_DIR}/vendor/glfw/include/")
set(IMGUI_HEADER "${CMAKE_SOURCE_DIR}/vendor/imgui/")
set(GLAD_HEADER "${CMAKE_SOURCE_DIR}/vendor/glad/include/")

add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/glfw/")
add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/imgui/")
find_package( OpenGL REQUIRED )
find_package(Threads REQUIRED)

set(BIN_NAME "RayTracing-${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}")
add_executable(${BIN_NAME} "${SOURCE}" "${GLAD_SRC}" "${HEADER_SOURCE}")

target_link_libraries(${BIN_NAME} glfw)
target_link_libraries(${BIN_NAME} OpenGL::GL)
target_link_libraries(${BIN_NAME} imgui)
target_link_libraries(${BIN_NANE} Threads::Threads)

target_include_directories(${BIN_NAME} PUBLIC "${HEADER}" "${GLAD_HEADER}" "${GLM_HEADER}" "${STB_HEADER}" "${GLFW_HEADER}" "${IMGUI_HEADER}")
set_target_properties(${BIN_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OutputDir}"
)
