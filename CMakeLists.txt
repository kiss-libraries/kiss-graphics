project(kiss-graphics)

cmake_minimum_required(VERSION 3.0)

file(GLOB SRCS src/*)
file(GLOB_RECURSE INCLUDES include/*)

include_directories(include)

add_definitions(-g -pg)

add_subdirectory(backend)

add_library(kiss-graphics SHARED ${SRCS} ${BACKEND_SRCS})
add_library(kiss-graphics_static STATIC ${SRCS} ${BACKEND_SRCS})

target_link_libraries(kiss-graphics ${BACKEND_DEPS})
target_link_libraries(kiss-graphics_static ${BACKEND_DEPS})

add_subdirectory(test)

install(FILES ${INCLUDES} ${BACKEND_INCLUDES} DESTINATION include/kiss-graphics)
install(TARGETS kiss-graphics
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
install(TARGETS kiss-graphics_static
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)