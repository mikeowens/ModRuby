MESSAGE( STATUS "SYSTEM: ${SYSTEM} (netbsd/settings.cmake)" )

# Paths, compiler and linker settings
SET(PREFIX "/usr/pkg")
SET(ALTERNATE_PREFIX "/usr/local")
SET(APR_ROOT "apr-1")
SET(APACHE_MODULES_DIR "/usr/local/libexec/apache22")
SET(SYSTEM_LINK_FLAGS -rdynamic)

# System commands
SET(DATE_COMMAND "date")
SET(HTTPD "/usr/pkg/sbin/apachectl")

# Parameters for locating Ruby
SET(RUBY_POSSIBLE_INCLUDE_PATHS
   /usr/local/include/ruby-1.9/
   /usr/local/include/ruby-1.9.1/ )

SET(RUBY_POSSIBLE_INCLUDE_ARCH_PATHS
  /usr/local/include/ruby-1.9.1/${CMAKE_SYSTEM_PROCESSOR}-netbsdelf${CMAKE_SYSTEM_VERSION}/ )
