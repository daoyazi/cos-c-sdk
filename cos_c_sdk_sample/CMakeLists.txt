set(COS_SAMPLE_SOURCE_FILES cos_sample_util.c cos_config.c main.c 
    cos_put_object_sample.c cos_append_object_sample.c cos_get_object_sample.c
    cos_head_object_sample.c cos_delete_object_sample.c cos_multipart_upload_sample.c)


find_library(APR_LIBRARY apr-1 PATHS /usr/local/apr/lib/)
find_library(APR_UTIL_LIBRARY aprutil-1 PATHS /usr/local/apr/lib/)
find_library(CURL_LIBRARY curl)
find_library(PTHREAD_LIBRARY pthread)
find_library(RT_LIBRARY rt)
find_library(M_LIBRARY m)

SET(SAMPLE_BIN_NAME cos_c_sdk_sample)

add_executable(${SAMPLE_BIN_NAME} ${COS_SAMPLE_SOURCE_FILES})
target_link_libraries(${SAMPLE_BIN_NAME} ${CMAKE_PROJECT_NAME})
target_link_libraries(${SAMPLE_BIN_NAME} ${APR_UTIL_LIBRARY})
target_link_libraries(${SAMPLE_BIN_NAME} ${APR_LIBRARY})
target_link_libraries(${SAMPLE_BIN_NAME} ${CURL_LIBRARY})
target_link_libraries(${SAMPLE_BIN_NAME} ${PTHREAD_LIBRARY})
target_link_libraries(${SAMPLE_BIN_NAME} ${RT_LIBRARY})
target_link_libraries(${SAMPLE_BIN_NAME} ${M_LIBRARY})

