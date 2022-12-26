add_library(foundation 
vector-point.cpp 
constants.cpp 
color.cpp 
ray.cpp 
camera.cpp 
pixels.cpp 
random.cpp 
_lodepng.cpp
)

target_include_directories(foundation INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})