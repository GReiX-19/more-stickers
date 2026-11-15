qt_add_executable(sticker-config
    main.cpp
)
target_link_libraries(sticker-config PRIVATE 
    Qt6::Core
    mrst_core
)