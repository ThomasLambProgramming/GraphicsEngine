
cmake_minimum_required(VERSION 3.15)
project(VulkanEngine VERSION 1.0 DESCRIPTION "Vulkan Graphics Engine" LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)
#Allow to build with lower than c++17 if not avaliable
set (CMAKE_CXX_STANDARD_REQUIRED NO)
#Allow gcc extensions that isnt in c++ standard but are there to help.
set (CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} VulkanSource/main.cpp)

set (OSByte 32)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set (OSByte 64)
  message(STATUS "64 Bit")
else()
  message(STATUS "32 Bit")
endif()

#find_package(Vulkan REQUIRED)
#message(STATUS ${VULKAN_INCLUDE_DIR})
#include_directories(${VULKAN_INCLUDE_DIR})
include_directories(C:/VulkanSDK/1.3.280.0/Include)

include_directories(${CMAKE_SOURCE_DIR}/Libraries/assimp)
add_library(assimp ${CMAKE_SOURCE_DIR}/Libraries/assimp/assimp-vc143-mtd.dll)
set_target_properties(assimp PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(${CMAKE_SOURCE_DIR}/Libraries/glfw)
include_directories(${CMAKE_SOURCE_DIR}/Libraries/glfw/include)

add_subdirectory(${CMAKE_SOURCE_DIR}/Libraries/glm)

add_library(stbImage ${CMAKE_SOURCE_DIR}/Libraries/stb_image/stb_image.cpp)
include_directories(${CMAKE_SOURCE_DIR}/Libraries/stb_image)

include_directories(${CMAKE_SOURCE_DIR}/Libraries/imgui)
add_library(imgui)
target_sources(imgui PRIVATE
    ${CMAKE_SOURCE_DIR}/Libraries/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/Libraries/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/Libraries/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/Libraries/imgui/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/Libraries/imgui/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/Libraries/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/Libraries/imgui/imgui_widgets.cpp
    ) 

include_directories(${CMAKE_SOURCE_DIR}/Libraries/glew/include)
add_library(glew ${CMAKE_SOURCE_DIR}/Libraries/glew/lib/Release/x64/glew32.lib ${CMAKE_SOURCE_DIR}/Libraries/glew/lib/Release/x64/glew32s.lib)
set_target_properties(glew PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${PROJECT_NAME} glew imgui stbImage glm assimp glfw C:/VulkanSDK/1.3.280.0/Lib/vulkan-1.lib)

set (OutputDirectory ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}${OSByte}/${CMAKE_BUILD_TYPE})
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OutputDirectory}/Libraries)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OutputDirectory})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OutputDirectory})

#SHARED = make it a lib to share / dll (link at runtime), otherwise the default if unspecified is static and it will make something similar to a dll but the .exe will need to be updated instead of changing the dll.
#add_library(console SHARED console.cpp)
#target_link_libraries(${PROJECT_NAME} console)

#this is some package manager bs that the tutorial used.
#find_package(SDL2 REQUIRED)
#include_directories(${SDL2_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

# Somehow the below will force the x64 and for it to correctly setup and build with gcc? on visual studio code, hate that the tutorial doesnt actually explain what vscode is doing just that its a magic button that makes things work.
# -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=C:/ProgramData\mingw64\mingw64\bin\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH=C:\ProgramData\mingw64\mingw64\bin\g++.exe --no-warn-unused-cli -SD:/PersonalProjects/CMakeBuild -Bd:/PersonalProjects/CMakeBuild/build -G "MinGW Makefiles"
