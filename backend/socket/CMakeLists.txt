option(sdl "Enable socket backend" ON)

if(sdl)
  set(BACKEND_SRCS ${BACKEND_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/socket_display.cpp PARENT_SCOPE)
  set(BACKEND_INCLUDES ${BACKEND_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/socket_display.hpp PARENT_SCOPE)
  set(BACKEND_DEPS ${BACKEND_DEPS} z PARENT_SCOPE)
endif()