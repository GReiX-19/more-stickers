qt_add_executable(sticker-config
    main.cpp
)
target_link_libraries(sticker-config PRIVATE 
    Qt6::Core
    mrst_core
)

install(TARGETS sticker-config RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES qml/Overlay.qml qml/StickerWindow.qml DESTINATION ${CMAKE_INSTALL_DATADIR}/more-stickers/qml)
install(PROGRAMS sticker-overlay DESTINATION ${CMAKE_INSTALL_BINDIR})