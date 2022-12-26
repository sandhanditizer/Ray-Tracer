add_executable(test-vec test-vec.cpp)
target_link_libraries(test-vec PRIVATE foundation)

add_executable(thread-render thread-render.cpp)
target_link_libraries(thread-render PRIVATE foundation world_creation parse)