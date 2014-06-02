MESSAGE( STATUS "SYSTEM: ${SYSTEM} (linux/debian/settings.cmake)" )

# Paths, compiler and linker settings
SET(PREFIX "/usr")
SET(APR_ROOT "apr-1.0")
SET(APACHE_INCLUDE_ROOT "apache2")
SET(APACHE_MODULES_DIR "/usr/lib/apache/modules")
SET(SYSTEM_LINK_FLAGS -Wl,--as-needed -Wl,-z,relro)

# System commands
SET(DATE_COMMAND date -R)
SET(HTTPD "/usr/sbin/apache2")

# Parameters for locating Ruby
SET(RUBY_POSSIBLE_INCLUDE_PATHS
  /usr/include/ruby-2.1.0/
  /usr/include/ruby-2.1.0/${CMAKE_SYSTEM_PROCESSOR}-linux/
  /usr/include/ruby-2.0.0/
  /usr/include/ruby-2.0.0/${CMAKE_SYSTEM_PROCESSOR}-linux/ )

SET(RUBY_POSSIBLE_INCLUDE_ARCH_PATHS 
  /usr/include/ruby-2.1.0/
  /usr/include/ruby-2.1.0/${CMAKE_SYSTEM_PROCESSOR}-linux/
  /usr/include/ruby-2.1.0/i486-linux/
  /usr/include/ruby-2.1.0/i586-linux/
  /usr/include/ruby-2.1.0/i686-linux/
  /usr/include/ruby-2.0.0/
  /usr/include/ruby-2.0.0/${CMAKE_SYSTEM_PROCESSOR}-linux/
  /usr/include/ruby-2.0.0/i486-linux/
  /usr/include/ruby-2.0.0/i586-linux/
  /usr/include/ruby-2.0.0/i686-linux/ )

# Linux distro info
execute_process( COMMAND cat /etc/lsb-release
                 COMMAND grep DISTRIB_CODENAME
                 COMMAND awk -F "=" "{ print $2 }"
                 OUTPUT_VARIABLE LINUX_DISTRO )

# Need to chomp the \n at end of output.
string(REGEX REPLACE "[\n]+" "" LINUX_DISTRO "${LINUX_DISTRO}")
MESSAGE( STATUS "LINUX_DISTRO: ${LINUX_DISTRO}" )
