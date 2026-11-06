add_library(mrst_core STATIC
    sticker.hpp
    sticker.cpp
    json_read.hpp
    json_read.cpp
    json_warn.hpp
    json_warn.cpp
    Warning.hpp
    profileJson.hpp
    profileJson.cpp
)

target_include_directories(mrst_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mrst_core PUBLIC Qt6::Core)