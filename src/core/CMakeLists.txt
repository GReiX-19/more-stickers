add_library(stickers_core STATIC
    sticker.hpp
    sticker.cpp
)

target_include_directories(stickers_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(stickers_core PUBLIC Qt6::Core)