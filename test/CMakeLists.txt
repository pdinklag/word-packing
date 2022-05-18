add_executable(test-packed-int-vector test_packed_int_vector.cpp)
target_link_libraries(test-packed-int-vector PRIVATE word-packing)
add_test(packed-int-vector ${CMAKE_CURRENT_BINARY_DIR}/test-packed-int-vector)
