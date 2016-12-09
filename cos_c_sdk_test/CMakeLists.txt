include_directories(${CMAKE_SOURCE_DIR})

set(SAMPLE_SOURCE_FILES CuTest.c cos_test_util.c cos_config.c test_cos_bucket.c 
    test_cos_object.c test_cos_multipart.c test_aos.c test_all.c)


find_library(APR_LIBRARY apr-1 PATHS /usr/local/apr/lib/)
find_library(APR_UTIL_LIBRARY aprutil-1 PATHS /usr/local/apr/lib/)
find_library(CURL_LIBRARY curl)
find_library(PTHREAD_LIBRARY pthread)
find_library(RT_LIBRARY rt)

add_executable(cos_c_sdk_test ${SAMPLE_SOURCE_FILES})

target_link_libraries(cos_c_sdk_test cos_c_sdk)
target_link_libraries(cos_c_sdk_test ${APR_UTIL_LIBRARY})
target_link_libraries(cos_c_sdk_test ${APR_LIBRARY})
target_link_libraries(cos_c_sdk_test ${MINIXML_LIBRARY})
target_link_libraries(cos_c_sdk_test ${CURL_LIBRARY})
target_link_libraries(cos_c_sdk_test ${PTHREAD_LIBRARY})
target_link_libraries(cos_c_sdk_test ${RT_LIBRARY})
