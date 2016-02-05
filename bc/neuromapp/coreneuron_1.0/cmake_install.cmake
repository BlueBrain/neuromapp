# Install script for directory: /Users/ewart/Desktop/LICENCE/neuromapp/neuromapp/coreneuron_1.0

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/ewart/Desktop/LICENCE/neuromapp/bc/neuromapp/coreneuron_1.0/libcoreneuron10_kernel.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_kernel.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_kernel.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_kernel.a")
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/ewart/Desktop/LICENCE/neuromapp/bc/neuromapp/coreneuron_1.0/libcoreneuron10_solver.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_solver.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_solver.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_solver.a")
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/ewart/Desktop/LICENCE/neuromapp/bc/neuromapp/coreneuron_1.0/libcoreneuron10_cstep.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_cstep.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_cstep.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_cstep.a")
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/ewart/Desktop/LICENCE/neuromapp/bc/neuromapp/coreneuron_1.0/libcoreneuron10_common.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_common.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_common.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_common.a")
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/ewart/Desktop/LICENCE/neuromapp/bc/neuromapp/coreneuron_1.0/libcoreneuron10_queueing.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_queueing.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_queueing.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libcoreneuron10_queueing.a")
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/Users/ewart/Desktop/LICENCE/neuromapp/neuromapp/coreneuron_1.0/kernel/mechanism/mechanism.h"
    "/Users/ewart/Desktop/LICENCE/neuromapp/neuromapp/coreneuron_1.0/kernel/kernel.h"
    "/Users/ewart/Desktop/LICENCE/neuromapp/neuromapp/coreneuron_1.0/solver/solver.h"
    "/Users/ewart/Desktop/LICENCE/neuromapp/neuromapp/coreneuron_1.0/cstep/cstep.h"
    "/Users/ewart/Desktop/LICENCE/neuromapp/neuromapp/coreneuron_1.0/common/data/helper.h"
    "/Users/ewart/Desktop/LICENCE/neuromapp/neuromapp/coreneuron_1.0/queueing/queue.h"
    "/Users/ewart/Desktop/LICENCE/neuromapp/neuromapp/coreneuron_1.0/queueing/pool.h"
    "/Users/ewart/Desktop/LICENCE/neuromapp/neuromapp/coreneuron_1.0/queueing/queueing.h"
    "/Users/ewart/Desktop/LICENCE/neuromapp/neuromapp/coreneuron_1.0/queueing/spinlock_apple.h"
    "/Users/ewart/Desktop/LICENCE/neuromapp/neuromapp/coreneuron_1.0/queueing/spinlock_queue.h"
    "/Users/ewart/Desktop/LICENCE/neuromapp/neuromapp/coreneuron_1.0/queueing/thread.h"
    )
endif()

