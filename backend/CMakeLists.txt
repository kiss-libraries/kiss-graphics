add_subdirectory(sdl)
add_subdirectory(pipe)
add_subdirectory(socket)

set(BACKEND_SRCS ${BACKEND_SRCS} PARENT_SCOPE)
set(BACKEND_INCLUDES ${BACKEND_INCLUDES} PARENT_SCOPE)
set(BACKEND_DEPS ${BACKEND_DEPS} PARENT_SCOPE)