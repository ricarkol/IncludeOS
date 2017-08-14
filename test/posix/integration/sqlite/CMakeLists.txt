cmake_minimum_required(VERSION 2.8.9)

# IncludeOS install location
if (NOT DEFINED ENV{INCLUDEOS_PREFIX})
  set(ENV{INCLUDEOS_PREFIX} /usr/local)
endif()

include($ENV{INCLUDEOS_PREFIX}/includeos/pre.service.cmake)

project (test_sqlite)

# Human-readable name of your service
set(SERVICE_NAME "sqlite test")

# Name of your service binary
set(BINARY       "test_sqlite")

# Maximum memory can be hard-coded into the binary
set(MAX_MEM 128)

# Source files to be linked with OS library parts to form bootable image
set(SOURCES
    service.cpp
    #sqlite_test.c
    tcph.c
    sqlite3.c
    )

if ("$ENV{PLATFORM}" STREQUAL "x86_solo5")
  set(DRIVERS solo5blk)
else()
  set(DRIVERS virtioblk)
endif()


# To add your own include paths:
#set(LOCAL_INCLUDES ".")

# include service build script
include($ENV{INCLUDEOS_PREFIX}/includeos/post.service.cmake)
