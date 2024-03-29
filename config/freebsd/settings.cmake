MESSAGE( STATUS "SYSTEM: ${SYSTEM} (freebsd/settings.cmake)" )

# Paths, compiler and linker settings
SET(PREFIX "/usr/local")
SET(APR_ROOT "apr-1")
SET(APACHE_INCLUDE_ROOT "apache24")
SET(APACHE_MODULES_DIR "/usr/local/libexec/apache24")
SET(SYSTEM_LINK_FLAGS -rdynamic)

# System commands
SET(DATE_COMMAND "date")
SET(HTTPD "apachectl")

# Parameters for locating Ruby
SET(RUBY_POSSIBLE_INCLUDE_PATHS
   /usr/local/include/ruby-2.0/
   /usr/local/include/ruby-2.1/
   /usr/local/include/ruby-2.2/
   /usr/local/include/ruby-2.3/
   /usr/local/include/ruby-2.4/
   /usr/local/include/ruby-2.5/
   /usr/local/include/ruby-2.6/)
