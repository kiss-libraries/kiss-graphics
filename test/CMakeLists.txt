add_executable(polygon polygon.cpp)
target_link_libraries(polygon kiss-graphics_static)
include_directories(${CMAKE_SOURCE_DIR}/deploy/include)