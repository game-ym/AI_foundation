cmake_minimum_required(VERSION 3.20)
project(AI_foundation VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


file(GLOB_RECURSE SOURCES "src/*.cpp")

file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

add_executable(AI_foundation ${SOURCES} ${HEADERS})

target_include_directories(AI_foundation PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(WIN32)
    target_link_directories(AI_foundation PRIVATE ${CMAKE_SOURCE_DIR}/lib)
    target_link_libraries(AI_foundation PRIVATE 
        llama.lib ggml.lib ggml-base.lib ggml-cpu.lib ggml-vulkan.lib
    )
    
    target_link_options(AI_foundation PRIVATE 
        /DELAYLOAD:llama.dll
        /DELAYLOAD:ggml.dll
        /DELAYLOAD:ggml-base.dll
        /DELAYLOAD:ggml-cpu.dll
        /DELAYLOAD:ggml-vulkan.dll
        delayimp.lib 
    )

    add_custom_command(TARGET AI_foundation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:AI_foundation>/dlls
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/bin $<TARGET_FILE_DIR:AI_foundation>/dlls
    )
endif()
target_compile_options(AI_foundation PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")