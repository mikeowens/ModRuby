CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

# Allow more human readable "if then else" constructs
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

#-------------------------------------------------------------------------------
# Platform Options
#-------------------------------------------------------------------------------

INCLUDE(CheckIncludeFiles)

# MODRUBY
SET( MODRUBY_MAJ_V 3 )
SET( MODRUBY_MIN_V 0 )
SET( MODRUBY_PL    0 )
SET( MODRUBY_CL      )
SET( MODRUBY_BN    2 )
SET(VERSION ${MODRUBY_MAJ_V}.${MODRUBY_MIN_V}.${MODRUBY_PL}${MODRUBY_CL})
SET(SOVERSION ${MODRUBY_MAJ_V}.${MODRUBY_MIN_V}.${MODRUBY_PL})

SET(MODRUBY_RELEASE_VERSION ${MODRUBY_MAJ_V}.${MODRUBY_MIN_V}.${MODRUBY_PL}${MODRUBY_CL}-${MODRUBY_BN})

# Add OS params

string(TOLOWER ${CMAKE_SYSTEM_NAME} PLATFORM)
string(TOLOWER ${CMAKE_SYSTEM} SYSTEM)
string(TOLOWER ${CMAKE_SYSTEM_VERSION} SYSTEM)

MESSAGE( STATUS "PLATFORM: ${PLATFORM}" )
MESSAGE( STATUS "SYSTEM: ${CMAKE_SYSTEM}" )
MESSAGE( STATUS "VERSION: ${CMAKE_SYSTEM_VERSION}" )
MESSAGE( STATUS "ARCH: ${CMAKE_SYSTEM_PROCESSOR}" )

IF( PLATFORM STREQUAL "freebsd" )
  INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/config/freebsd/settings.cmake)
ENDIF()

IF( PLATFORM STREQUAL "netbsd" )
  INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/config/netbsd/settings.cmake)
ENDIF()

IF( PLATFORM STREQUAL "darwin" )
  INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/config/osx/settings.cmake)
ENDIF()

IF( PLATFORM STREQUAL "linux" )
  INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/config/linux/debian/settings.cmake)
ENDIF()

execute_process( COMMAND ${DATE_COMMAND}
                 OUTPUT_VARIABLE MODRUBY_RELEASE_DATE )

# Need to chomp the \n at end of output.
string(REGEX REPLACE "[\n]+" "" MODRUBY_RELEASE_DATE "${MODRUBY_RELEASE_DATE}")

MESSAGE( STATUS "RELEASE_DATE: ${MODRUBY_RELEASE_DATE}" )
MESSAGE( STATUS "VERSION:      ${VERSION}" )
MESSAGE( STATUS "SOVERSION:    ${SOVERSION}" )

# Create debian/changelog file
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog.in 
                ${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog )

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/debian/mod_ruby-git.gitmap.in 
                ${CMAKE_CURRENT_SOURCE_DIR}/debian/mod_ruby-git.gitmap )

# Create configuration file from .in file
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/include/platform.h.in 
                ${CMAKE_CURRENT_BINARY_DIR}/include/platform.h )

# Create configuration file from .in file
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/include/common.h.in 
                ${CMAKE_CURRENT_BINARY_DIR}/include/common.h )

# Create configuration file from .in file
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/include/apr.h.in 
                ${CMAKE_CURRENT_BINARY_DIR}/include/apr.h )

# Apache config file
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config/mod_ruby.conf.in 
                ${CMAKE_CURRENT_SOURCE_DIR}/config/mod_ruby.conf )

#-------------------------------------------------------------------------------
# Ruby
#-------------------------------------------------------------------------------

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/config/ruby.cmake)

#-------------------------------------------------------------------------------
# Project Options
#-------------------------------------------------------------------------------

OPTION( BUILD_SHARED_LIBS "Set to OFF to build static libraries" OFF )
OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" ON )

#-------------------------------------------------------------------------------
# Locations
#-------------------------------------------------------------------------------

# find and setup Qt4 for this project
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_DONT_USE_QTGUI 1)
SET(QT_USE_QTNETWORK 1)
INCLUDE(${QT_USE_FILE})

#-------------------------------------------------------------------------------
# Compilation
#-------------------------------------------------------------------------------

# Common libraries.

ADD_LIBRARY(libruby_ex STATIC IMPORTED)
SET_PROPERTY(TARGET libruby_ex PROPERTY
             IMPORTED_LOCATION ${RUBY_STATIC_LIBRARY})

IF(UNIX)

SET(CMAKE_INSTALL_PREFIX ${PREFIX})
SET(CMAKE_INCLUDE_PATH ${PREFIX}/include ${ALTERNATE_PREFIX/include})
SET(CMAKE_LIBRARY_PATH ${PREFIX}/lib ${ALTERNATE_PREFIX/lib})

INCLUDE_DIRECTORIES( -I ${CMAKE_INCLUDE_PATH} 
                     -I ${CMAKE_INCLUDE_PATH}/${APR_ROOT} 
                     -I ${RUBY_INCLUDE_PATH}  
                     -I ${RUBY_INCLUDE_PATH}/${RUBY_SITE_ARCH}  
                     -I "${PROJECT_SOURCE_DIR}/include" )
ENDIF(UNIX)

SET(CMAKE_BUILD_TYPE Debug)

# Add debug definitions
IF( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
 ADD_DEFINITIONS( "-DDEBUG -ggdb" )
 SET(CMAKE_CXX_FLAGS "")
ENDIF()

#-------------------------------------------------------------------------------
# Linking
#-------------------------------------------------------------------------------

# Set default build type to (None Debug Release RelWithDebInfo MinSizeRel)

IF( NOT CMAKE_BUILD_TYPE )
SET( CMAKE_BUILD_TYPE "RelWithDebInfo" )
ENDIF()

#-------------------------------------------------------------------------------
# Subprojects
#-------------------------------------------------------------------------------

ADD_SUBDIRECTORY(src)

#-------------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------------

IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
SET( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" )
ENDIF()

# Install Ruby library
INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/lib/core/ DESTINATION ${RUBY_RUBY_LIB_DIR} COMPONENT)

# Display status message for important variables
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )

MESSAGE( STATUS " BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS " CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS " CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS " CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}" )
MESSAGE( STATUS " ${PROJECT_NAME}_DEPENDS = \"${${PROJECT_NAME}_DEPENDS}\"" )
MESSAGE( STATUS " BUILD_WITH = \"${BUILD_WITH}\"" )
MESSAGE( STATUS " INSTALL_DOC = ${INSTALL_DOC}" )
MESSAGE( STATUS " Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )

# Export build settings
INCLUDE( CMakeExportBuildSettings )

CMAKE_EXPORT_BUILD_SETTINGS( "config/BuildSettings.cmake" )
# Export library dependencies (keep this as the last line in the file)
EXPORT_LIBRARY_DEPENDENCIES( "config/LibDeps.cmake" )

