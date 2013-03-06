MESSAGE( STATUS "SYSTEM: ${SYSTEM} (osx/settings.cmake)" )

SET(PREFIX "/usr")
SET(APR_ROOT "apr-1")
SET(APACHE_MODULES_DIR "/usr/local/libexec/apache22/modules")

ADD_DEFINITIONS(-arch x86_64 -arch ppc -arch i386)
SET(SYSTEM_LINK_FLAGS -rdynamic "-undefined dynamic_lookup" 
      -arch i386 -arch ppc -arch x86_64 )

SET(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
SET(DATE_COMMAND "date")
SET(HTTPD "httpd")

SET(RUBY_POSSIBLE_INCLUDE_PATHS
   /opt/local/include/ruby1.9-1.9.1/ )

SET(RUBY_POSSIBLE_INCLUDE_ARCH_PATHS 
   /opt/local/include/ruby1.9-1.9.1/i386-darwin9/ )
