# cross platform project compile definition

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project( CHT C )

add_definitions(-DCURL_STATICLIB=ON)
add_executable(cht cht.c simple_config.c simple_config.h cfgpath.h optparse.h rlutil.h)

find_package (CURL) # use systemwide curl found for dynamic linking only

if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
  target_link_libraries(cht ${CURL_LIBRARIES})
else()
    # not systemwide, find it near
    find_path(LIBCURL_INCLUDE_DIRS NAMES curl/curl.h PATHS
            ${PROJECT_SOURCE_DIR}/curl/include
            ${PROJECT_SOURCE_DIR}/../curl/include
            ${PROJECT_SOURCE_DIR}/../../curl/include
    )
  if (WIN32)
     SET(LIBNAME libcurl) # finds .lib (dll cannot find)
  else(WIN32)
     SET(LIBNAME curl) # finds libcurl.a (or libcurl.so)
  endif (WIN32)
  
  find_library(LIBCURL ${LIBNAME} PATHS
    ${PROJECT_SOURCE_DIR}/curl/lib
    ${PROJECT_SOURCE_DIR}/../curl/lib
    ${PROJECT_SOURCE_DIR}/../../curl/lib
  )
  include_directories (${LIBCURL_INCLUDE_DIRS})
  MESSAGE(STATUS "LibCURL is : " ${LIBCURL} )
endif (CURL_FOUND)

if(WIN32)
  set(LIBS ${LIBCURL} wldap32 wsock32 ws2_32)
else (WIN32)
  set(LIBS ${LIBCURL} pthread ssl crypto z)
#  set(LIBS ${CURL_LIBRARY}) # syswide
endif(WIN32)

target_link_libraries(cht ${LIBS})

INSTALL(TARGETS cht DESTINATION bin)

######## QA ########
enable_testing()

## Local tests
add_test(OptionNone ${PROJECT_BIN_DIR}/cht)
set_tests_properties(OptionNone PROPERTIES FAIL_REGULAR_EXPRESSION 
  "Usage:" WILL_FAIL true) ## exits faiulre so test fails if not set

add_test(OptionVersion ${PROJECT_BIN_DIR}/cht -v)
set_tests_properties(OptionVersion PROPERTIES PASS_REGULAR_EXPRESSION "version:")

# remote quering
add_test(RemoteLS ${PROJECT_BIN_DIR}/cht ls)
set_tests_properties(RemoteLS PROPERTIES PASS_REGULAR_EXPRESSION "target directory")

add_test(RemoteQ_python_reverse_string ${PROJECT_BIN_DIR}/cht -Q python reverse string)
set_tests_properties(RemoteQ_python_reverse_string 
  PROPERTIES PASS_REGULAR_EXPRESSION "hello world")

