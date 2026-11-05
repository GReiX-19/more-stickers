add_library(stickers_core STATIC
    sticker.hpp
    sticker.cpp
    json_read.hpp
    json_read.cpp
    json_warn.hpp
    json_warn.cpp
    Warning.hpp
)

target_include_directories(stickers_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(stickers_core PUBLIC Qt6::Core)