function(add_mrst_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE mrst_core Qt6::Test)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_mrst_test(test_parse_sticker)
add_mrst_test(test_json_read)
add_mrst_test(test_parse_profileJson)