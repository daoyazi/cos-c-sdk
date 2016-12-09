PROJECT(cos_c_sdk)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)  

set(CMAKE_VERSION 3.0.0)
SET (CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)   #Make if/else statements easier
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

IF (CMAKE_BUILD_TYPE)
    SET (CMAKE_BUILD_TYPE "Debug")
ELSE ()
    SET (CMAKE_BUILD_TYPE "Release")
ENDIF ()

# Compile and link lib_cos_c_sdk
IF(WIN32)
  include_directories(./3rd-libs/include/apr)
  include_directories(./3rd-libs/include/aprutil)
  include_directories(./3rd-libs/include/mxml)
  include_directories(./3rd-libs/include)
  include_directories(./cos_c_sdk)
  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd-libs/lib)
ELSEIF(UNIX)
  IF (APR_INCLUDE_DIR)
    include_directories(${APR_INCLUDE_DIR})
  else()
    include_directories(/usr/local/apr/include/apr-1)
  ENDIF()

  IF (APR_UTIL_INCLUDE_DIR)
    include_directories(${APR_UTIL_INCLUDE_DIR})
  else()
    include_directories(/usr/local/apr/include/apr-1)
  ENDIF()

  IF (CURL_INCLUDE_DIR)
    include_directories(${CURL_INCLUDE_DIR})
  ENDIF()

  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cos_c_sdk)

  link_directories(${APR_LIBRARY_DIR} ${AAPR_UTIL_LIBRARY_DIR} ${CURL_LIBRARY_DIR})
  link_directories(/usr/local/apr/lib)
ENDIF()

#handle the compiler option on Linux and windows.
IF (WIN32)      #it is only effective in cmake. 
    ADD_DEFINITIONS (-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS (-D_WINDOWS)
    ADD_DEFINITIONS (-DFD_SETSIZE=40000)
    IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
        #remove /ZI and /Gm, may conflict with Z7
        SET (CMAKE_C_FLAGS_DEBUG "/MDd /Od /RTC1 /nologo /Gm /W3 /ZI /GF /DEBUG")
        SET (CMAKE_EXE_LINKER_FLAGS "/IGNORE:4099")
        ADD_DEFINITIONS (-DENABLE_DEBUG)
        ADD_DEFINITIONS (-DENABLE_TRACE2)
        ADD_DEFINITIONS (-DENABLE_TRACE3)
    ELSEIF (CMAKE_BUILD_TYPE STREQUAL "Release")
        SET (CMAKE_C_FLAGS_RELEASE "/MD /O2")
    ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

ELSEIF (UNIX)
    IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
        ADD_DEFINITIONS (-g)     #for DEBUG
        ADD_DEFINITIONS (-DENABLE_DEBUG)
            ADD_DEFINITIONS (-DENABLE_TRACE2)
            ADD_DEFINITIONS (-DENABLE_TRACE3)
        ADD_DEFINITIONS (-Wall)
        # you should not define _GNU_SOURCE, or SYSERR_STR will not work
    ELSEIF (CMAKE_BUILD_TYPE STREQUAL "Release")
        ADD_DEFINITIONS (-O3)
    ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")
ENDIF (WIN32)

aux_source_directory(cos_c_sdk SRC_LIST)

add_library(${CMAKE_PROJECT_NAME} SHARED ${SRC_LIST})
add_library(${CMAKE_PROJECT_NAME}_static STATIC ${SRC_LIST})
IF (WIN32)
  target_link_libraries(${CMAKE_PROJECT_NAME} curl.lib libapr-1.lib libaprutil-1.lib ws2_32.lib)
  target_link_libraries(${CMAKE_PROJECT_NAME}_static curl.lib libapr-1.lib libaprutil-1.lib ws2_32.lib)
ELSEIF(UNIX)
  target_link_libraries(${CMAKE_PROJECT_NAME} -lapr-1 -laprutil-1 -lcurl -lm)
  target_link_libraries(${CMAKE_PROJECT_NAME}_static -lapr-1 -laprutil-1 -lcurl -lm)
ENDIF()

aux_source_directory(cos_c_sdk_sample COS_SAMPLE_SOURCE_FILES)
add_executable(cos-sample ${COS_SAMPLE_SOURCE_FILES})
target_link_libraries(cos-sample ${CMAKE_PROJECT_NAME}) 

aux_source_directory(cos_c_sdk_test COS_TEST_SOURCE_FILES)
add_executable(cos-test ${COS_TEST_SOURCE_FILES})
target_link_libraries(cos-test ${CMAKE_PROJECT_NAME}) 

INSTALL(TARGETS   ${CMAKE_PROJECT_NAME} LIBRARY DESTINATION lib)
INSTALL(TARGETS   cos_c_sdk_static ARCHIVE DESTINATION lib)
INSTALL(DIRECTORY cos_c_sdk DESTINATION inlcude PATTERN "cos_c_sdk/*.h")
