# Minimum version of cmake.
cmake_minimum_required(VERSION 3.10)

# Set the project name and version.
project(wal VERSION 1.0)

# Specify the C++ standard and the compile flag requiremetns (e.g. using pthread)
set(CMAKE_CXX_STANDARD 17)

# TODO: TESTS and GDB
set(CMAKE_BUILD_TYPE Debug)

###
# Dependencies
#
# Download Google Test as a depdendency.
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# We are creating a library using thread_pool as the main file.
# ${PROJECT_NAME} references the name assigned above in "project(threadpool, ...)".
add_library(
    ${PROJECT_NAME} STATIC
    include/wal/wal.h
    src/wal.cpp
)

# This creates a namespace and alias for linking used in the cmake test file and other projects?
add_library(WriteAheadLog::wal ALIAS wal)

# This includes the directories for the target.
target_include_directories(${PROJECT_NAME}
    PUBLIC 
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# TODO: -Wall and debug?
target_compile_options(wal PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall -Wextra -Wpedantic>)

# TODO: Link std::filesystem in this lib and tests.
target_link_libraries(
  wal
  stdc++fs
)

###
# Export Library
#
# Create an alias for the config dir.
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/WriteAheadLog)

# TODO: Shouldn't I be using INSTALL_CONFIGDIR?
# Install the library and register the target in threadpool-targets
install(TARGETS ${PROJECT_NAME}
   EXPORT wal-targets
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install the export target defined above, set the output .cmake file name for
# the target, give it a namespace and the destination will be the the alias
# of the configdir.
#
# Callers and importing CMakefiles will use the ThreadPoolTargets.cmake file to
# import the lib.
install(EXPORT wal-targets
   FILE
   WriteAheadLogTargets.cmake
   NAMESPACE
   WriteAheadLog::
   DESTINATION
     ${INSTALL_CONFIGDIR}
)

# Create a ConfigVersion.cmake file???
# TODO: Look up CMakePackageConfigHelpers, I think it saves us from manually creating
# that cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
   ${CMAKE_CURRENT_BINARY_DIR}/WriteAheadLogConfigVersion.cmake
   VERSION ${PROJECT_VERSION}
   COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/WriteAheadLogConfig.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/WriteAheadLogConfig.cmake
   INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

# Install the config and config version to the Destination install config dir.
install(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/WriteAheadLogConfig.cmake
   ${CMAKE_CURRENT_BINARY_DIR}/WriteAheadLogConfigVersion.cmake
   DESTINATION ${INSTALL_CONFIGDIR}
)

# TODO: Export the cmake targets so it's a lib???
export(EXPORT wal-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/WriteAheadLogTargets.cmake
    NAMESPACE WriteAheadLog::)

# Register the library in the user package registry, this allows other projects
# to use find_package() to consider looking in the user package registry.
export(PACKAGE WriteAheadLog)
enable_testing()
add_subdirectory(tests)
