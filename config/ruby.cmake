# Find Ruby -- This is an override of the default cmake FindRuby module.  The
# default does not work well finding ruby.h with ruby 1.9.

FIND_PROGRAM( RUBY_EXECUTABLE 
              NAMES ruby1.9.3 ruby1.9 ruby19 ruby
              PATHS /usr/bin /usr/loca/bin )

IF(RUBY_EXECUTABLE  AND NOT  RUBY_ARCH_DIR)

   EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['arch']"
      OUTPUT_VARIABLE RUBY_ARCH)

   EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['sitearch']"
      OUTPUT_VARIABLE RUBY_SITE_ARCH)

   EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['archdir']"
      OUTPUT_VARIABLE RUBY_ARCH_DIR)

   EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['libdir']"
      OUTPUT_VARIABLE RUBY_POSSIBLE_LIB_DIR)

   EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['rubylibdir']"
      OUTPUT_VARIABLE RUBY_RUBY_LIB_DIR)

   # site_ruby
   EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['sitearchdir']"
      OUTPUT_VARIABLE RUBY_SITEARCH_DIR)

   EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['sitelibdir']"
      OUTPUT_VARIABLE RUBY_SITELIB_DIR)

   # vendor_ruby available ?
   EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r vendor-specific -e "print 'true'"
      OUTPUT_VARIABLE RUBY_HAS_VENDOR_RUBY  ERROR_QUIET)

   IF(RUBY_HAS_VENDOR_RUBY)
      EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['vendorlibdir']"
         OUTPUT_VARIABLE RUBY_VENDORLIB_DIR)

      EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['vendorarchdir']"
         OUTPUT_VARIABLE RUBY_VENDORARCH_DIR)
   ENDIF(RUBY_HAS_VENDOR_RUBY)

   # save the results in the cache so we don't have to run ruby the next time again
   SET(RUBY_ARCH             ${RUBY_ARCH}             CACHE PATH "The Ruby arch")
   SET(RUBY_SITE_ARCH        ${RUBY_SITE_ARCH}        CACHE PATH "The Ruby site arch")
   SET(RUBY_ARCH_DIR         ${RUBY_ARCH_DIR}         CACHE PATH "The Ruby arch dir")
   SET(RUBY_POSSIBLE_LIB_DIR ${RUBY_POSSIBLE_LIB_DIR} CACHE PATH "The Ruby lib dir")
   SET(RUBY_RUBY_LIB_DIR     ${RUBY_RUBY_LIB_DIR}     CACHE PATH "The Ruby ruby-lib dir")
   SET(RUBY_SITEARCH_DIR     ${RUBY_SITEARCH_DIR}     CACHE PATH "The Ruby site arch dir")
   SET(RUBY_SITELIB_DIR      ${RUBY_SITELIB_DIR}      CACHE PATH "The Ruby site lib dir")
   SET(RUBY_HAS_VENDOR_RUBY  ${RUBY_HAS_VENDOR_RUBY}  CACHE BOOL "Vendor Ruby is available")
   SET(RUBY_VENDORARCH_DIR   ${RUBY_VENDORARCH_DIR}   CACHE PATH "The Ruby vendor arch dir")
   SET(RUBY_VENDORLIB_DIR    ${RUBY_VENDORLIB_DIR}    CACHE PATH "The Ruby vendor lib dir")

ENDIF(RUBY_EXECUTABLE  AND NOT  RUBY_ARCH_DIR)

# for compatibility
SET(RUBY_POSSIBLE_LIB_PATH ${RUBY_POSSIBLE_LIB_DIR})
SET(RUBY_RUBY_LIB_PATH ${RUBY_RUBY_LIB_DIR})

FIND_PATH(RUBY_INCLUDE_PATH
   NAMES ruby.h
   PATHS
   ${RUBY_POSSIBLE_INCLUDE_PATHS}
   ${RUBY_ARCH_DIR} )

FIND_PATH(RUBY_INCLUDE_ARCH_PATH
   NAMES ruby/config.h
   PATHS
   ${RUBY_ARCH_DIR}
   ${RUBY_POSSIBLE_INCLUDE_ARCH_PATHS} )

# Concatenate ARCH include path with include path
SET(RUBY_INCLUDE_PATH ${RUBY_INCLUDE_PATH} ${RUBY_INCLUDE_ARCH_PATH})

# Find ruby library
IF(PLATFORM STREQUAL "darwin")
FIND_FILE( RUBY_LIBRARY
           NAMES libruby-1.9.3.dylib libruby1.9.dylib libruby.dylib
           PATHS /opt/local/lib/ )
ELSE(PLATFORM STREQUAL "darwin")
FIND_LIBRARY( RUBY_LIBRARY
  NAMES ruby-1.9.3 ruby1.9 ruby19 ruby msvcrt-ruby19 msvcrt-ruby19-static
  PATHS ${RUBY_POSSIBLE_LIB_DIR} /usr/local/lib )
ENDIF()

MARK_AS_ADVANCED(
  RUBY_EXECUTABLE
  RUBY_LIBRARY
  RUBY_INCLUDE_PATH
  RUBY_ARCH
  RUBY_SITE_ARCH
  RUBY_ARCH_DIR
  RUBY_POSSIBLE_LIB_DIR
  RUBY_RUBY_LIB_DIR
  RUBY_SITEARCH_DIR
  RUBY_SITELIB_DIR
  RUBY_HAS_VENDOR_RUBY
  RUBY_VENDORARCH_DIR
  RUBY_VENDORLIB_DIR )

MESSAGE( STATUS "Ruby arch include: ${RUBY_INCLUDE_ARCH_PATH}" )
MESSAGE( STATUS "Ruby include: ${RUBY_INCLUDE_PATH}" )
MESSAGE( STATUS "Ruby arch dir: ${RUBY_ARCH_DIR}" )
MESSAGE( STATUS "RUBY_LIBRARY : ${RUBY_LIBRARY}" )
MESSAGE( STATUS "RUBY lib dur : ${RUBY_RUBY_LIB_DIR}" )
