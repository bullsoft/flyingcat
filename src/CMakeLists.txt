set(flyingcat_SRCS
    fc_main.c
)

add_executable(flyingcat ${flyingcat_SRCS})
target_link_libraries(flyingcat
                      phenom ssl)

install(TARGETS flyingcat DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

