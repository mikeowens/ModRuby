message( STATUS "SYSTEM: ${SYSTEM} (solaris/settings.cmake)" )

# Note: for OmniOSCE, you must install lint in order to get crt1:
# Also need system headers files too:
#
#   pkg install lint system/header

# Paths, compiler and linker settings

set(PREFIX "/opt/local")
set(ETC_DIR "/opt/local/etc")
set(TMP_DIR "/tmp")
SET(APR_ROOT "apr-1")
SET(APACHE_INCLUDE_ROOT "httpd")
SET(APACHE_MODULES_DIR "/opt/local/lib/httpd")

set(solaris_c_flags  "-std=c99")
set(solaris_defines  "-Dsun -D__EXTENSIONS__ -DUNIX -DHAVE_UNISTD_H -DMSGPACK_ZONE_ALIGN=8 -DSQLITE_THREADSAFE=1")
set(solaris_ld_flags "-Wl,-R${PREFIX}/lib -ldlpi -lsocket -lnsl -lrt -lresolv")

set(platform_defs ${solaris_defines} CACHE STRING "platform defs")
set(platform_c_flags ${solaris_c_flags} CACHE STRING "platform c flags")

message(STATUS "PLATFORM DEFS: ${platform_defs}")

set(OS_INCLUDE_DIR "opt/local/include")
set(OS_LIB_DIR "opt/local/lib")

set(OS_LIBS -lm -pthread -lkvm -lgmp -lssl -lcrypto -lstdc++ -lz -lkstat -lsendfile ${solaris_ld_flags})
set(SHLIB_LINK_FLAGS -rdynamic ${solaris_ld_flags})

# System commands
set(DATE_COMMAND "date")

# Parameters for locating Ruby
SET(RUBY_POSSIBLE_INCLUDE_PATHS
  /opt/local/include/ruby-2.5.0
  /opt/local/include/ruby-2.4.0
  /opt/local/include/ruby-2.3.0
  /opt/local/include/ruby-2.2.0
  /opt/local/include/ruby-2.1.0
  /opt/local/include/ruby-2.0.0 )

SET(RUBY_POSSIBLE_INCLUDE_ARCH_PATHS /opt/local/include/)