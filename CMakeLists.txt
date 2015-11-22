cmake_minimum_required(VERSION 2.8.0)

project(PARTICLES)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

find_package(OpenGL REQUIRED)

list(APPEND PARTICLES_SOURCES
    src/go.c
    src/inputs.c
    src/inputs.h
    src/mat4.c
    src/mat4.h
    src/particle_system.c
    src/particle_system.h
    src/shaders.c
    src/shaders.h
    src/camera.h
    src/camera.c
)

set(VERTEX_SHADER ${PARTICLES_SOURCE_DIR}/shaders/vertex_shader.glsl)
set(FRAGMENT_SHADER ${PARTICLES_SOURCE_DIR}/shaders/fragment_shader.glsl)
set(GLEW_RUNTIME ${PARTICLES_SOURCE_DIR}/external/glew/bin/Release/Win32/glew32.dll)

add_executable(particles ${PARTICLES_SOURCES})
target_include_directories(particles
    PUBLIC src
    PUBLIC external/glew/include
    PUBLIC external/glfw3/include
)
target_link_libraries(particles
    ${OPENGL_LIBRARIES}
    ${PARTICLES_SOURCE_DIR}/external/glew/lib/Release/Win32/glew32.lib
    ${PARTICLES_SOURCE_DIR}/external/glfw3/lib-vc2013/glfw3.lib
)

add_custom_command(TARGET particles
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VERTEX_SHADER} $<TARGET_FILE_DIR:particles>)

add_custom_command(TARGET particles
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FRAGMENT_SHADER} $<TARGET_FILE_DIR:particles>)

add_custom_command(TARGET particles
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLEW_RUNTIME} $<TARGET_FILE_DIR:particles>)
