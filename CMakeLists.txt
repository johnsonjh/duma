cmake_minimum_required(VERSION 3.5)
project(DUMA C CXX)

# See README.md for help building and using DUMA

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED off)

if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()

if(POLICY CMP0065)
  cmake_policy(SET CMP0065 NEW)
endif()

if(POLICY CMP0083)
  cmake_policy(SET CMP0083 NEW)
endif()
include(CheckPIESupported OPTIONAL RESULT_VARIABLE CHECK_PIE_SUPPORTED)
if(CHECK_PIE_SUPPORTED)
  check_pie_supported(LANGUAGES CXX)
  if(CMAKE_CXX_LINK_PIE_SUPPORTED)
    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
  endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

option( DUMA_WITH_GLOBAL_MALLOC_FREE "export malloc/free" ON)  # NOT DUMA_NO_GLOBAL_MALLOC_FREE
option( DUMA_WITH_EXPLICIT_INIT      "duma_init() is to be called from user" OFF)  # DUMA_EXPLICIT_INIT
option( DUMA_WITH_THREAD_SAFETY      "build thread safe duma library" ON)  # NOT DUMA_NO_THREAD_SAFETY
option( DUMA_WITH_STRERROR           "use strerror - leads to recusion on some platforms!" ON)  # NOT DUMA_NO_STRERROR

option( DUMA_WITH_OLD_NEW_MACRO      "use NEW_ELEM() / NEW_ARRAY() macros - DEL_ELEM/DEL_ARRAY have to be used anyway" OFF)  # DUMA_OLD_NEW_MACRO

option( DUMA_SHARED_WITH_CPP_SUPPORT "have new/delete shared library" ON)  # NOT DUMA_SO_NO_CPP_SUPPORT

option( DUMA_SHARED_WITH_LEAK_DETECTION "shared library: have leak detection" ON)  # NOT DUMA_SO_NO_LEAKDETECTION
option( DUMA_STATIC_WITH_LEAK_DETECTION "static library: have leak detection" ON)  # NOT DUMA_LIB_NO_LEAKDETECTION

option( DUMA_SHARED_PREFER_ATEXIT       "shared library: have atexit() (vs. compiler's destructor)" OFF)  # DUMA_SO_PREFER_ATEXIT
option( DUMA_STATIC_PREFER_ATEXIT       "static library: have atexit() (vs. compiler's destructor)" OFF)  # DUMA_LIB_PREFER_ATEXIT

option( DUMA_SHARED_PREFER_GETENV       "shared library: have getenv() (vs. char **environ)" OFF)  # DUMA_SO_PREFER_GETENV
option( DUMA_STATIC_PREFER_GETENV       "static library: have getenv() (vs. char **environ)" OFF)  # DUMA_LIB_PREFER_GETENV

option( DUMA_SHARED_NO_HANG_MSG         "shared library: suppress the extra messages around atexit()" OFF)  # DUMA_SO_NO_HANG_MSG
option( DUMA_STATIC_NO_HANG_MSG         "static library: suppress the extra messages around atexit()" OFF)  # DUMA_LIB_NO_HANG_MSG


option(BUILD_TESTS    "build and run tests" ON)
option(BUILD_EXAMPLES "build examples" ON)


# C90 requires the gcc extensions for function attributes like always_inline
# C99 provides the function attributes: no gcc extensions required
# set(CMAKE_C_STANDARD 99)
# set(CMAKE_C_EXTENSIONS OFF)

# set(CMAKE_CXX_STANDARD 98)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)


##############################################################

# howto create the header
# https://stackoverflow.com/questions/36084785/building-a-tool-immediately-so-it-can-be-used-later-in-same-cmake-run?noredirect=1&lq=1

if(EARLY_BUILD)
    # This is the nested build and we will only be asked to
    # build the createconf target (see (c) below)
    add_subdirectory(confdir)

    # End immediately, we don't want anything else in the nested build
    return()
endif()

# This is the main build, setup and execute the nested build
# to ensure the createconf executable exists before continuing

# (a) When cross compiling, we cannot re-use the same binary dir
#     because the host and target are different architectures
if(CMAKE_CROSSCOMPILING)
    set(workdir "${CMAKE_BINARY_DIR}/host")
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${workdir}")
else()
    set(workdir "${CMAKE_BINARY_DIR}")
endif()

# (b) Nested CMake run. May need more -D... options than shown here.
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"
                        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                        -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
                        -DEARLY_BUILD=ON
                        ${CMAKE_SOURCE_DIR}
               WORKING_DIRECTORY "${workdir}")

# (c) Build just createconf in the nested build. Don't specify a --config
#     because we cannot know what config the developer will be using
#     at this point. For non-multi-config generators, we've already
#     specified CMAKE_BUILD_TYPE above in (b).
execute_process(COMMAND ${CMAKE_COMMAND} --build . --target createconf
                WORKING_DIRECTORY "${workdir}")


# (d) We want everything from confdir in our main build,
#     not just the createconf target
#add_subdirectory(confdir)

# (e) Run createconf on the sources to generate a CMakeLists.txt in the
#     ${CMAKE_BINARY_DIR}/foobar directory. Note that because we want
#     to support cross compiling, working out the location of the
#     executable is a bit more tricky. We cannot know whether the user
#     wants debug or release build types for multi-config generators
#     so we have to choose one. We cannot query the target properties
#     because they are only known at generate time, which is after here.
#     Best we can do is hardcode some basic logic.
if(MSVC)
    set(createconfsuffix "Debug/createconf.exe")
elseif(CMAKE_GENERATOR STREQUAL "Xcode")
    set(createconfsuffix "Debug/createconf")
else()
    set(createconfsuffix "createconf")
endif()
set(createconf_EXECUTABLE "${workdir}/confdir/${createconfsuffix}")

add_custom_command( OUTPUT "${CMAKE_SOURCE_DIR}/duma_config.h"
  COMMAND "${createconf_EXECUTABLE}"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)


#  -outdir foobar ${CMAKE_SOURCE_DIR}/foo.my ${CMAKE_SOURCE_DIR}/bar.my)

# (f) Now pull that generated CMakeLists.txt into the main build.
#     It will create a CMake library target called foobar.
# add_subdirectory(${CMAKE_BINARY_DIR}/foobar ${CMAKE_BINARY_DIR}/foobar-build)

# (g) Another target which links to the foobar library
#     and includes headers from there
#add_executable(gumby gumby.cpp)
#target_link_libraries(gumby PUBLIC foobar)
#target_include_directories(gumby PUBLIC foobar)

##############################################################

add_custom_command( OUTPUT "${CMAKE_SOURCE_DIR}/verinfo.h"
  COMMAND "${CMAKE_SOURCE_DIR}/make_git_source_version.sh"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)


set( DUMA_PUB_HDRS
  noduma.h
  duma.h
  dumapp.h
  "${CMAKE_SOURCE_DIR}/duma_config.h"
  duma_sem.h  # not for public use .. but required from dumapp.h
)

set( DUMA_HDRS
  ${DUMA_PUB_HDRS}
  # createconf.h  # required but we can't put dependency here
  verinfo.h
  duma_hlp.h
  paging.h
  print.h
)

set( DUMA_SRCS
  src/duma.c
  src/dumapp.cpp
  src/print.c
  src/sem_inc.c
)

add_library(DUMA_STATIC STATIC ${DUMA_SRCS} ${DUMA_HDRS})
target_include_directories(DUMA_STATIC INTERFACE "${CMAKE_SOURCE_DIR}")
target_include_directories(DUMA_STATIC PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
if (NOT DUMA_WITH_GLOBAL_MALLOC_FREE)
  target_compile_definitions(DUMA_STATIC PRIVATE DUMA_NO_GLOBAL_MALLOC_FREE)
endif()
if (DUMA_WITH_EXPLICIT_INIT)
  target_compile_definitions(DUMA_STATIC PUBLIC DUMA_EXPLICIT_INIT)
endif()
if (NOT DUMA_WITH_THREAD_SAFETY)
  target_compile_definitions(DUMA_STATIC PRIVATE DUMA_NO_THREAD_SAFETY)
else()
  target_link_libraries(DUMA_STATIC PUBLIC pthread)
endif()
if (NOT DUMA_WITH_STRERROR)
  target_compile_definitions(DUMA_STATIC PRIVATE DUMA_NO_STRERROR)
endif()
if (NOT DUMA_WITH_OLD_NEW_MACRO)
  target_compile_definitions(DUMA_STATIC PRIVATE DUMA_OLD_NEW_MACRO)
endif()
if (NOT DUMA_STATIC_WITH_LEAK_DETECTION)
  target_compile_definitions(DUMA_STATIC PRIVATE DUMA_LIB_NO_LEAKDETECTION)
endif()
if (DUMA_STATIC_PREFER_ATEXIT)
  target_compile_definitions(DUMA_STATIC PRIVATE DUMA_LIB_PREFER_ATEXIT)
endif()
if (DUMA_STATIC_PREFER_GETENV)
  target_compile_definitions(DUMA_STATIC PRIVATE DUMA_LIB_PREFER_GETENV)
endif()
if (DUMA_STATIC_NO_HANG_MSG)
  target_compile_definitions(DUMA_STATIC PRIVATE DUMA_LIB_NO_HANG_MSG)
endif()


add_library(DUMA_SHARED SHARED ${DUMA_SRCS} ${DUMA_HDRS})
target_include_directories(DUMA_SHARED INTERFACE "${CMAKE_SOURCE_DIR}")
target_include_directories(DUMA_SHARED PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
if (NOT DUMA_SHARED_WITH_CPP_SUPPORT)
  target_compile_definitions(DUMA_SHARED PRIVATE DUMA_SO_NO_CPP_SUPPORT)
endif()
if (NOT DUMA_SHARED_WITH_LEAK_DETECTION)
  target_compile_definitions(DUMA_SHARED PRIVATE DUMA_SO_NO_LEAKDETECTION)
endif()
if (DUMA_SHARED_PREFER_ATEXIT)
  target_compile_definitions(DUMA_SHARED PRIVATE DUMA_SO_PREFER_ATEXIT)
endif()
if (DUMA_SHARED_PREFER_GETENV)
  target_compile_definitions(DUMA_SHARED PRIVATE DUMA_SO_PREFER_GETENV)
endif()
if (DUMA_SHARED_NO_HANG_MSG)
  target_compile_definitions(DUMA_SHARED PRIVATE DUMA_SO_NO_HANG_MSG)
endif()


if (WIN32 AND MINGW)
  message(STATUS "MINGW")
else()
  message(STATUS "target link pthread to DUMA_SHARED")
  target_link_libraries( DUMA_SHARED PUBLIC pthread )
endif()


# produce same filenames - as GNUmakefile
set_target_properties(DUMA_STATIC PROPERTIES OUTPUT_NAME "duma")
set_target_properties(DUMA_SHARED PROPERTIES OUTPUT_NAME "duma")
set_target_properties(DUMA_SHARED PROPERTIES VERSION 0.0.0 SOVERSION 0)

# todo: install readme, man file, ..?
install( TARGETS DUMA_STATIC DUMA_SHARED  DESTINATION lib)
install( FILES  ${DUMA_PUB_HDRS}  DESTINATION include)

if (NOT WIN32)
  # determine filename
  get_target_property(DUMA_SO_VERSION DUMA_SHARED VERSION)
  set(DUMA_SO_FILENAME "${CMAKE_SHARED_LIBRARY_PREFIX}duma${CMAKE_SHARED_LIBRARY_SUFFIX}")
  # message(STATUS "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}")
  # message(STATUS "DUMA_SO_FILENAME ${DUMA_SO_FILENAME}")

  configure_file(duma.sh.in ${CMAKE_CURRENT_BINARY_DIR}/duma.sh @ONLY)

  install( FILES  ${CMAKE_CURRENT_BINARY_DIR}/duma.sh  DESTINATION bin
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                GROUP_EXECUTE GROUP_READ
                WORLD_EXECUTE WORLD_READ
  )
endif()

add_custom_target(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/uninstall.cmake"
)


enable_testing()

if (BUILD_TESTS)

  add_executable(dumatest_static tests/dumatest.c)
  target_link_libraries(dumatest_static DUMA_STATIC)

  add_executable(tstheap_static tests/tstheap.c)
  target_link_libraries(tstheap_static DUMA_STATIC)

  add_executable(thread-test_static tests/thread-test.c)
  target_link_libraries(thread-test_static DUMA_STATIC)

  add_executable(testmt_static tests/testmt.c)
  target_link_libraries(testmt_static DUMA_STATIC)

  add_executable(dumatestpp_static tests/dumatestpp.cpp)
  target_link_libraries(dumatestpp_static DUMA_STATIC)

  add_executable(testoperators_static tests/testoperators.cpp)
  target_link_libraries(testoperators_static DUMA_STATIC)

  add_executable(testmemlimit_static tests/experimental/testmemlimit.c)
  target_link_libraries(testmemlimit_static DUMA_STATIC)

  # shared variants

  add_executable(dumatest_shared tests/dumatest.c)
  target_link_libraries(dumatest_shared DUMA_SHARED)

  add_executable(tstheap_shared tests/tstheap.c)
  target_link_libraries(tstheap_shared DUMA_SHARED)

  add_executable(thread-test_shared tests/thread-test.c)
  target_link_libraries(thread-test_shared DUMA_SHARED)

  add_executable(testmt_shared tests/testmt.c)
  target_link_libraries(testmt_shared DUMA_SHARED)

  add_executable(dumatestpp_shared tests/dumatestpp.cpp)
  target_link_libraries(dumatestpp_shared DUMA_SHARED)

  add_executable(testoperators_shared tests/testoperators.cpp)
  target_link_libraries(testoperators_shared DUMA_SHARED)

  add_executable(testmemlimit_shared tests/experimental/testmemlimit.c)
  target_link_libraries(testmemlimit_shared DUMA_SHARED)

  # tests are executed with 'ctest' or 'cmake --build . --target test'  - each from build directory
  add_test(NAME dumatest_static      COMMAND "${CMAKE_CURRENT_BINARY_DIR}/dumatest_static")
  add_test(NAME tstheap_static       COMMAND "${CMAKE_CURRENT_BINARY_DIR}/tstheap_static" 3072)
  add_test(NAME thread-test_static   COMMAND "${CMAKE_CURRENT_BINARY_DIR}/thread-test_static")
  # add_test(NAME testmt_static        COMMAND "${CMAKE_CURRENT_BINARY_DIR}/testmt_static")
  add_test(NAME dumatestpp_static    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/dumatestpp_static")
  add_test(NAME testoperators_static COMMAND "${CMAKE_CURRENT_BINARY_DIR}/testoperators_static")
  add_test(NAME testmemlimit_static  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/testmemlimit_static")

  add_test(NAME dumatest_shared      COMMAND "${CMAKE_CURRENT_BINARY_DIR}/dumatest_shared")
  add_test(NAME tstheap_shared       COMMAND "${CMAKE_CURRENT_BINARY_DIR}/tstheap_shared" 3072)
  add_test(NAME thread-test_shared   COMMAND "${CMAKE_CURRENT_BINARY_DIR}/thread-test_shared")
  # add_test(NAME testmt_shared        COMMAND "${CMAKE_CURRENT_BINARY_DIR}/testmt_shared")
  add_test(NAME dumatestpp_shared    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/dumatestpp_shared")
  add_test(NAME testoperators_shared COMMAND "${CMAKE_CURRENT_BINARY_DIR}/testoperators_shared")
  add_test(NAME testmemlimit_shared  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/testmemlimit_shared")

endif()


if (BUILD_EXAMPLES)

  # no linked against duma
  add_executable(example1_pure examples/example1.cpp)
  add_executable(example2_pure examples/example2.cpp)
  # add_executable(example3_pure examples/example3.cpp)  # uses duma.h
  # add_executable(example4_pure examples/example4.cpp)  # uses duma.h
  # add_executable(example5_pure examples/example5.cpp)  # uses dumapp.h
  add_executable(example6_pure examples/example6.cpp)
  add_executable(example7_pure examples/example7.cpp)
  # add_executable(example8_pure examples/example8.cpp)  # uses dumapp.h


  # linked statically against duma
  add_executable(example1_static examples/example1.cpp)
  target_link_libraries(example1_static DUMA_STATIC)

  add_executable(example2_static examples/example2.cpp)
  target_link_libraries(example2_static DUMA_STATIC)

  add_executable(example3_static examples/example3.cpp)
  target_link_libraries(example3_static DUMA_STATIC)

  add_executable(example4_static examples/example4.cpp)
  target_link_libraries(example4_static DUMA_STATIC)

  add_executable(example5_static examples/example5.cpp)
  target_link_libraries(example5_static DUMA_STATIC)

  add_executable(example6_static examples/example6.cpp)
  target_link_libraries(example6_static DUMA_STATIC)

  add_executable(example7_static examples/example7.cpp)
  target_link_libraries(example7_static DUMA_STATIC)

  add_executable(example8_static examples/example8.cpp)
  target_link_libraries(example8_static DUMA_STATIC)


  # linked against shared duma-library (.so)

  add_executable(example1_shared examples/example1.cpp)
  target_link_libraries(example1_shared DUMA_SHARED)

  add_executable(example2_shared examples/example2.cpp)
  target_link_libraries(example2_shared DUMA_SHARED)

  add_executable(example3_shared examples/example3.cpp)
  target_link_libraries(example3_shared DUMA_SHARED)

  add_executable(example4_shared examples/example4.cpp)
  target_link_libraries(example4_shared DUMA_SHARED)

  add_executable(example5_shared examples/example5.cpp)
  target_link_libraries(example5_shared DUMA_SHARED)

  add_executable(example6_shared examples/example6.cpp)
  target_link_libraries(example6_shared DUMA_SHARED)

  add_executable(example7_shared examples/example7.cpp)
  target_link_libraries(example7_shared DUMA_SHARED)

  add_executable(example8_shared examples/example8.cpp)
  target_link_libraries(example8_shared DUMA_SHARED)

endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
