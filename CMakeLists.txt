cmake_minimum_required(VERSION 3.0)
project(FinalProject)

find_package(OpenGL REQUIRED)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
include_directories(${CMAKE_SOURCE_DIR}/external/stb)

add_subdirectory(external)

include_directories(
		external/glfw-3.1.2/include/
		external/glm-0.9.7.1/
		external/glad-opengl-3.3/include/
		external/tinygltf-2.9.3/
		FinalProject/
		external/
)

# Lab 4 Skeleton
add_executable(lab4_skeleton
		FinalProject/lab4_skeleton.cpp
		FinalProject/render/shader.cpp
)
target_link_libraries(lab4_skeleton
		${OPENGL_LIBRARY}
		glfw
		glad
)

# Lab 4 Character
add_executable(lab4_character
		FinalProject/lab4_character.cpp
		FinalProject/render/shader.cpp
)
target_link_libraries(lab4_character
		${OPENGL_LIBRARY}
		glfw
		glad
)

# Lab 3 Cornellbox
add_executable(lab3_cornellbox
		FinalProject/lab3_cornellbox.cpp
		FinalProject/render/shader.cpp
		FinalProject/Building2.cpp
)
target_link_libraries(lab3_cornellbox
		${OPENGL_LIBRARY}
		glfw
		glad
)

# Lab 2 Building
add_executable(lab2_building
		FinalProject/lab2_building.cpp
		FinalProject/render/shader.cpp
)
target_link_libraries(lab2_building
		${OPENGL_LIBRARY}
		glfw
		glad
)

# Lab 2 Skybox
add_executable(lab2_skybox
		FinalProject/lab2_skybox.cpp
		FinalProject/render/shader.cpp
		FinalProject/Skybox.cpp
)
target_link_libraries(lab2_skybox
		${OPENGL_LIBRARY}
		glfw
		glad
)

# Lab 2 Main
add_executable(lab2
		FinalProject/lab2.cpp
		FinalProject/render/shader.cpp
		FinalProject/Building.cpp
		FinalProject/Skybox.cpp
		FinalProject/Car.cpp
		FinalProject/Car.h
)
target_link_libraries(lab2
		${OPENGL_LIBRARY}
		glfw
		glad
)