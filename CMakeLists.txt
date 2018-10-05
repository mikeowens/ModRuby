cmake_minimum_required( VERSION 2.6 )
mark_as_advanced(CMAKE_BACKWARDS_COMPATIBILITY)

# Allow more human readable "if then else" constructs
set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

#-------------------------------------------------------------------------------
# Platform Options
#-------------------------------------------------------------------------------

include(CheckIncludeFiles)

# MODRUBY
set( MODRUBY_MAJ_V 3 )
set( MODRUBY_MIN_V 0 )
set( MODRUBY_PL    0 )
set( MODRUBY_CL      )
set( MODRUBY_BN    3 )
set(VERSION ${MODRUBY_MAJ_V}.${MODRUBY_MIN_V}.${MODRUBY_PL}${MODRUBY_CL})
set(SOVERSION ${MODRUBY_MAJ_V}.${MODRUBY_MIN_V}.${MODRUBY_PL})

set(MODRUBY_RELEASE_VERSION ${MODRUBY_MAJ_V}.${MODRUBY_MIN_V}.${MODRUBY_PL}${MODRUBY_CL}-${MODRUBY_BN})

# Add OS params

string(TOLOWER ${CMAKE_SYSTEM_NAME} PLATFORM)
string(TOLOWER ${CMAKE_SYSTEM} SYSTEM)
string(TOLOWER ${CMAKE_SYSTEM_VERSION} SYSTEM)

message( STATUS "PLATFORM: ${PLATFORM}" )
message( STATUS "SYSTEM: ${CMAKE_SYSTEM}" )
message( STATUS "VERSION: ${CMAKE_SYSTEM_VERSION}" )
message( STATUS "ARCH: ${CMAKE_SYSTEM_PROCESSOR}" )

if( PLATFORM STREQUAL "freebsd" )
  include(${CMAKE_CURRENT_SOURCE_DIR}/config/freebsd/settings.cmake)
endif()

if( PLATFORM STREQUAL "netbsd" )
  include(${CMAKE_CURRENT_SOURCE_DIR}/config/netbsd/settings.cmake)
endif()

if( PLATFORM STREQUAL "darwin" )
  include(${CMAKE_CURRENT_SOURCE_DIR}/config/osx/settings.cmake)
endif()

if( PLATFORM STREQUAL "linux" )
  include(${CMAKE_CURRENT_SOURCE_DIR}/config/linux/debian/settings.cmake)
endif()

execute_process( COMMAND ${DATE_COMMAND}
                 OUTPUT_VARIABLE MODRUBY_RELEASE_DATE )

# Need to chomp the \n at end of output.
string(REGEX REPLACE "[\n]+" "" MODRUBY_RELEASE_DATE "${MODRUBY_RELEASE_DATE}")

message( STATUS "RELEASE_DATE: ${MODRUBY_RELEASE_DATE}" )
message( STATUS "VERSION:      ${VERSION}" )
message( STATUS "SOVERSION:    ${SOVERSION}" )

#-------------------------------------------------------------------------------
# Ruby
#-------------------------------------------------------------------------------

include(${CMAKE_CURRENT_SOURCE_DIR}/config/ruby.cmake)

# Create debian/changelog file
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog.in 
                ${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/debian/mod_ruby-git.gitmap.in 
                ${CMAKE_CURRENT_SOURCE_DIR}/debian/mod_ruby-git.gitmap )

# Create configuration file from .in file
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/include/platform.h.in 
                ${CMAKE_CURRENT_BINARY_DIR}/include/platform.h )

# Create configuration file from .in file
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/include/common.h.in 
                ${CMAKE_CURRENT_BINARY_DIR}/include/common.h )

# Create configuration file from .in file
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/include/apr.h.in 
                ${CMAKE_CURRENT_BINARY_DIR}/include/apr.h )

# Apache config file
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config/mod_ruby.conf.in 
                ${CMAKE_CURRENT_SOURCE_DIR}/config/mod_ruby.conf )

#-------------------------------------------------------------------------------
# Project Options
#-------------------------------------------------------------------------------

option( BUILD_SHARED_LIBS "Set to OFF to build static libraries" OFF )
option( INSTALL_DOC "Set to OFF to skip build/install Documentation" ON )

#-------------------------------------------------------------------------------
# Linking
#-------------------------------------------------------------------------------

# Set default build type to (None Debug Release RelWithDebInfo MinSizeRel)

if( NOT CMAKE_BUILD_TYPE )
set( CMAKE_BUILD_TYPE "RelWithDebInfo" )
endif()

#-------------------------------------------------------------------------------
# Subprojects
#-------------------------------------------------------------------------------

add_subdirectory(src)

#-------------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------------

if( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
set( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" )
endif()

# Display status message for important variables
message( STATUS )
message( STATUS "-------------------------------------------------------------------------------" )

message( STATUS " BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}" )
message( STATUS " CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
message( STATUS " CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
message( STATUS " CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}" )
message( STATUS " ${PROJECT_NAME}_DEPENDS = \"${${PROJECT_NAME}_DEPENDS}\"" )
message( STATUS " BUILD_WITH = \"${BUILD_WITH}\"" )
message( STATUS " INSTALL_DOC = ${INSTALL_DOC}" )
message( STATUS " Change a value with: cmake -D<Variable>=<Value>" )
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS )

# Export build settings
include( CMakeExportBuildSettings )

cmake_export_build_settings( "config/BuildSettings.cmake" )

