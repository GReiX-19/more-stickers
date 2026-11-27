qt_add_executable(sticker-feed
    main.cpp
)
target_link_libraries(sticker-feed PRIVATE
    Qt6::Core
    mrst_core
)

install(TARGETS sticker-feed RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})