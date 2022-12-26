add_library(world_creation 
hit.cpp 
shapes.cpp 
world.cpp 
material.cpp 
texture.cpp
)

target_include_directories(world_creation INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(world_creation PRIVATE foundation)
