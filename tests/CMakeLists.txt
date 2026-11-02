add_executable(test_sticker test_sticker.cpp)
target_link_libraries(test_sticker PRIVATE stickers_core Qt6::Test)

add_test(NAME test_sticker COMMAND test_sticker)