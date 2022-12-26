add_library(parse 
parser.cpp
)

target_include_directories(parse INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(parse PRIVATE foundation world_creation)