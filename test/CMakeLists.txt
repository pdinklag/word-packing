add_executable(test-packed-int-vector-8 test_packed_int_vector_8.cpp)
target_link_libraries(test-packed-int-vector-8 PRIVATE word-packing)
add_test(packed-int-vector-8 ${CMAKE_CURRENT_BINARY_DIR}/test-packed-int-vector-8)

add_executable(test-packed-int-vector-16 test_packed_int_vector_16.cpp)
target_link_libraries(test-packed-int-vector-16 PRIVATE word-packing)
add_test(packed-int-vector-16 ${CMAKE_CURRENT_BINARY_DIR}/test-packed-int-vector-16)

add_executable(test-packed-int-vector-32 test_packed_int_vector_32.cpp)
target_link_libraries(test-packed-int-vector-32 PRIVATE word-packing)
add_test(packed-int-vector-32 ${CMAKE_CURRENT_BINARY_DIR}/test-packed-int-vector-32)

add_executable(test-packed-int-vector-64 test_packed_int_vector_64.cpp)
target_link_libraries(test-packed-int-vector-64 PRIVATE word-packing)
add_test(packed-int-vector-64 ${CMAKE_CURRENT_BINARY_DIR}/test-packed-int-vector-64)
