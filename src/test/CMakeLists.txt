#-------------------------------------------------------------------------------
# Platform Options
#-------------------------------------------------------------------------------

INCLUDE(CheckIncludeFiles)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/apr.cmake)

CHECK_INCLUDE_FILES( 
  malloc.h HAVE_MALLOC_H 
  unistd.h HAVE_UNISTD_H 
)

#-------------------------------------------------------------------------------
# Project Options
#-------------------------------------------------------------------------------

OPTION( BUILD_SHARED_LIBS "Set to OFF to build static libraries" OFF )
OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" ON )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

ENABLE_TESTING()

SET( CMAKE_BUILD_TYPE "Debug" )

set(CMAKE_CXX_FLAGS  ${CMAKE_ORIGCXX_FLAGS} -std=c++0x  )

#-------------------------------------------------------------------------------
# Custom test programs
#-------------------------------------------------------------------------------

# Static library for common test environment
ADD_LIBRARY( test_static_lib STATIC common.cpp )

SET_TARGET_PROPERTIES(test_static_lib PROPERTIES OUTPUT_NAME "modrubytest")

#-------------------------------------------------------------------------------
# Standard unit tests
#-------------------------------------------------------------------------------

SET( test_targets test_uri )

FOREACH(x ${test_targets})
  LIST(APPEND test_src ${x}.cpp)
ENDFOREACH(x)

# Generate test targets
FOREACH(x ${test_targets})
  ADD_EXECUTABLE(${x} ${x}.cpp)
  add_dependencies(${x} ruby_ext)
  INCLUDE_DIRECTORIES( -I "${CMAKE_CURRENT_SOURCE_DIR}/src/cpp" 
                       -I ${RUBY_INCLUDE_PATH}
                       -I ${RUBY_INCLUDE_PATH}/${RUBY_SITE_ARCH} )

  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

  TARGET_LINK_LIBRARIES(${x} static_lib libgtest.a 
    ${APR_LIBRARY} ${APRUTIL_LIBRARY} ${QT_LIBRARIES} -pthread )
  ADD_TEST(${x}_test ${EXECUTABLE_OUTPUT_PATH} ${x})
ENDFOREACH(x)


