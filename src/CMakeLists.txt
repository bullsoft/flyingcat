set(flyingcat_SRCS
    fc_main.c
    fc_log.c
    fc_string.c
    fc_alloc.c
    fc_pool.c
    fc_signal.c
    fc_context.c
    fc_array.c
)

add_executable(flyingcat ${flyingcat_SRCS})
target_link_libraries(flyingcat
#                     phenom
                      ssl)

install(TARGETS flyingcat DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

