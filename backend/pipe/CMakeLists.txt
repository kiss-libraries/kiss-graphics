option(sdl "Enable named pipe backend" ON)

if(sdl)
  set(BACKEND_SRCS ${BACKEND_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/pipe_display.cpp PARENT_SCOPE)
  set(BACKEND_INCLUDES ${BACKEND_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/pipe_display.hpp PARENT_SCOPE)
endif()