if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

IF(UNIX)

# Generate source code in src/ directory
EXECUTE_PROCESS( COMMAND flex -olex.yy.cpp rhtml.lex
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

# Add optimzation flags only in release
IF( CMAKE_BUILD_TYPE STREQUAL "Release")
 ADD_DEFINITIONS( "-O2" )
ENDIF()

# Require proper C++
ADD_DEFINITIONS( "-pedantic -Wno-long-long" ) # skip -ansi

# Needed for APR
# "-fPIC -fno-strict-aliasing -g -fpic"
ADD_DEFINITIONS( -D UNIX -D_REENTRANT -fPIC -W -DLINUX=2 -D_REENTRANT 
                 -D_GNU_SOURCE -D_LARGEFILE64_SOURCE )

IF(${unittest})
ADD_DEFINITIONS( "-DUNIT_TEST=1" )
ENDIF(${unittest})

ENDIF(UNIX)

#-------------------------------------------------------------------------------
# Project / Files
#-------------------------------------------------------------------------------

set(PROJECT_NAME ruby)

# Create configuration file from .in file
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in 
                ${CMAKE_CURRENT_BINARY_DIR}/config.h )

# List of source files
AUX_SOURCE_DIRECTORY( . library_sources )

# Library targets

# 1. Shared Library
ADD_LIBRARY( shared_lib SHARED ${library_sources} )
SET_TARGET_PROPERTIES( shared_lib PROPERTIES

  # For Unix style library versions + symbolic links
  #VERSION   ${VERSION}
  #SOVERSION ${SOVERSION}

  # Allow creating static and shared libs without conflicts
  CLEAN_DIRECT_OUTPUT 1

  PREFIX ""

  # Avoid conflicts between library and binary target names
  OUTPUT_NAME mod_${PROJECT_NAME} )

TARGET_LINK_LIBRARIES( shared_lib -L${CMAKE_LIBRARY_PATH} ${SYSTEM_LINK_FLAGS}
                       crypto bz2 ${QT_QTCORE_LIBRARY} ${RUBY_LIBRARY} )

# 3. Static library
ADD_LIBRARY( static_lib STATIC ${library_sources} )

SET_TARGET_PROPERTIES( static_lib PROPERTIES 
  COMPILE_FLAGS "-DUNIT_TEST"
  OUTPUT_NAME ${PROJECT_NAME} )

# 3. Symbolic target for calling both shared and static targets
ADD_CUSTOM_COMMAND( OUTPUT lib DEPENDS shared_lib static_lib )

# 1. RHTML Shared Library
ADD_LIBRARY( rhtml_lib SHARED rhtml.cpp ruby_rhtml.cpp lex.yy.cpp 
             library.cpp object.cpp )
SET_TARGET_PROPERTIES( rhtml_lib PROPERTIES

  # For Unix style library versions + symbolic links
  VERSION   ${VERSION}
  SOVERSION ${SOVERSION}

  # Allow creating static and shared libs without conflicts
  CLEAN_DIRECT_OUTPUT 1

  PREFIX ""

  # Avoid conflicts between library and binary target names
  OUTPUT_NAME librhtml )

TARGET_LINK_LIBRARIES( rhtml_lib -L${CMAKE_LIBRARY_PATH} ${SYSTEM_LINK_FLAGS}
  bz2 ${RUBY_LIBRARY} ${QT_QTCORE_LIBRARY} )

#-------------------------------------------------------------------------------
# Compilation
#-------------------------------------------------------------------------------

# Add debug definitions
IF( CMAKE_BUILD_TYPE STREQUAL "Debug" OR
 CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
 ADD_DEFINITIONS( "-DDEBUG" )
ENDIF()

#-------------------------------------------------------------------------------
# Linking
#-------------------------------------------------------------------------------

# Set default build type to (None Debug Release RelWithDebInfo MinSizeRel)

IF( NOT CMAKE_BUILD_TYPE )
SET( CMAKE_BUILD_TYPE "RelWithDebInfo" )
ENDIF()

#-------------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------------

# Install library

file(GLOB HEADERS . *.h)
SET_PROPERTY(TARGET shared_lib PROPERTY PUBLIC_HEADER ${HEADERS})

SET(APACHE_MODULES_DIR "/usr/lib/apache2/modules")

INSTALL( TARGETS shared_lib
  PUBLIC_HEADER DESTINATION include/mod_ruby
  LIBRARY DESTINATION ${APACHE_MODULES_DIR} PERMISSIONS
  ARCHIVE DESTINATION lib PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE )

INSTALL( TARGETS static_lib
  PUBLIC_HEADER DESTINATION include/mod_ruby
  LIBRARY DESTINATION lib PERMISSIONS
  ARCHIVE DESTINATION lib PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE )

INSTALL( TARGETS rhtml_lib
  PUBLIC_HEADER DESTINATION include/mod_ruby
  LIBRARY DESTINATION lib PERMISSIONS
  ARCHIVE DESTINATION lib PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE )

# Force some variables that could be defined in the command line to be written to cache
SET( BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}" CACHE BOOL
"Set to OFF to build static libraries" FORCE )
SET( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH
"Where to install ${PROJECT_NAME}" FORCE )
SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE )
SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" CACHE PATH
"Path to custom CMake Modules" FORCE )
SET( INSTALL_DOC "${INSTALL_DOC}" CACHE BOOL
"Set to OFF to skip build/install Documentation" FORCE )

# Export build settings
INCLUDE( CMakeExportBuildSettings )

CMAKE_EXPORT_BUILD_SETTINGS( "config/BuildSettings.cmake" )
# Export library dependencies (keep this as the last line in the file)
EXPORT_LIBRARY_DEPENDENCIES( "config/LibDeps.cmake" )

# Testing
#ADD_SUBDIRECTORY(Admin/test/lib)

# Adds a 'test' target. To run unit test, just do a 'make test'
#ADD_CUSTOM_TARGET(test COMMAND ctest -S ${CMAKE_CURRENT_SOURCE_DIR}/CTestfile.cmake -V)
