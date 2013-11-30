if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# Generate source code in src/ directory
execute_process( COMMAND flex -R -P Rhtml -orhtml.yy.cpp rhtml.lex
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

# Add optimzation flags only in release
if( CMAKE_BUILD_TYPE STREQUAL "Release")
 add_definitions( "-O2" )
endif()

# Require proper C++
add_definitions( "-pedantic -Wno-long-long " ) # skip -ansi

# Needed for APR
add_definitions( -fPIC -W -D_GNU_SOURCE -D_LARGEFILE64_SOURCE )

#-------------------------------------------------------------------------------
# Compilation
#-------------------------------------------------------------------------------

# Common libraries.

add_library(libruby_ex STATIC IMPORTED)
set_property(TARGET libruby_ex PROPERTY
             IMPORTED_LOCATION ${RUBY_STATIC_LIBRARY})

set(CMAKE_INSTALL_PREFIX ${PREFIX})
set(CMAKE_INCLUDE_PATH ${PREFIX}/include ${ALTERNATE_PREFIX/include})
set(CMAKE_LIBRARY_PATH ${PREFIX}/lib ${ALTERNATE_PREFIX/lib})

include_directories( -I ${CMAKE_INCLUDE_PATH} 
                     -I ${CMAKE_INCLUDE_PATH}/${APR_ROOT}                     
                     -I ${RUBY_INCLUDE_PATH}  
                     -I ${RUBY_INCLUDE_ARCH_PATH} 
                     -I ${RUBY_INCLUDE_PATH}/${RUBY_SITE_ARCH}  
                     -I "${PROJECT_SOURCE_DIR}/include" )

set(CMAKE_BUILD_TYPE Debug)

# Add debug definitions
if( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
 add_definitions( "-DDEBUG -ggdb" )
 set(CMAKE_CXX_FLAGS "")
endif()

#-------------------------------------------------------------------------------
# Project / Files
#-------------------------------------------------------------------------------

set(PROJECT_NAME ruby)

# Create configuration file from .in file
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in 
                ${CMAKE_CURRENT_BINARY_DIR}/config.h )

# List of source files
aux_source_directory( . library_sources )

# Library targets

# 1. Shared Library
add_library( shared_lib SHARED ${library_sources} )
set_target_properties( shared_lib PROPERTIES

  # For Unix style library versions + symbolic links
  #VERSION   ${VERSION}
  #SOVERSION ${SOVERSION}

  # Allow creating static and shared libs without conflicts
  CLEAN_DIRECT_OUTPUT 1

  PREFIX ""

  # Avoid conflicts between library and binary target names
  OUTPUT_NAME mod_${PROJECT_NAME} )

target_link_libraries( shared_lib -L${CMAKE_LIBRARY_PATH} 
   ${SYSTEM_LINK_FLAGS} ${RUBY_LIBRARY} )

# 2. Static library for testing
add_library( static_lib STATIC ${library_sources} )

set_target_properties( static_lib PROPERTIES OUTPUT_NAME ${PROJECT_NAME} )

# 3. RHTML Shared Library
add_library( rhtml_lib SHARED rhtml.cpp ruby_rhtml.cpp rhtml.yy.cpp ruby.cpp )

set_target_properties( rhtml_lib PROPERTIES

  # For Unix style library versions + symbolic links
  VERSION   ${VERSION}
  SOVERSION ${SOVERSION}

  # Allow creating static and shared libs without conflicts
  CLEAN_DIRECT_OUTPUT 1

  PREFIX ""

  # Avoid conflicts between library and binary target names
  OUTPUT_NAME librhtml )

target_link_libraries( rhtml_lib -L${CMAKE_LIBRARY_PATH} 
  ${SYSTEM_LINK_FLAGS} ${RUBY_LIBRARY} )

#-------------------------------------------------------------------------------
# Compilation
#-------------------------------------------------------------------------------

# Add debug definitions
if( CMAKE_BUILD_TYPE STREQUAL "Debug" OR
 CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
 add_definitions( "-DDEBUG" )
endif()

#-------------------------------------------------------------------------------
# Linking
#-------------------------------------------------------------------------------

# Set default build type to (None Debug Release RelWithDebInfo MinSizeRel)

if( NOT CMAKE_BUILD_TYPE )
set( CMAKE_BUILD_TYPE "RelWithDebInfo" )
endif()

#-------------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------------

# Install library

set(APACHE_MODULES_DIR "/usr/lib/apache2/modules")

# Install Ruby library
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ruby/ DESTINATION ${RUBY_RUBY_LIB_DIR} COMPONENT)

install( TARGETS shared_lib
  LIBRARY DESTINATION ${APACHE_MODULES_DIR} PERMISSIONS
  ARCHIVE DESTINATION lib PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE )

install( TARGETS rhtml_lib
  LIBRARY DESTINATION lib PERMISSIONS
  ARCHIVE DESTINATION lib PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE )

# Export build settings
include( CMakeExportBuildSettings )

cmake_export_build_settings( "config/BuildSettings.cmake" )
# Export library dependencies (keep this as the last line in the file)
export_library_dependencies( "config/LibDeps.cmake" )

# Enable with cmake -Ddev=1 .
if(${dev})
add_subdirectory(test)
endif(${dev})
