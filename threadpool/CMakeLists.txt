# Minimum version of cmake.
cmake_minimum_required(VERSION 3.10)

# Set the project name and version.
project(threadpool VERSION 1.0)

# Specify the C++ standard and the compile flag requiremetns (e.g. using pthread)
set(CMAKE_CXX_STANDARD 17)

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
    include/threadpool/thread_pool.h
    src/thread_pool.cpp
)

# This creates a namespace and alias for linking used in the cmake test file and other projects?
add_library(ThreadPool::threadpool ALIAS threadpool)

# This includes the directories for the target.
target_include_directories(${PROJECT_NAME}
    PUBLIC 
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# TODO: -Wall and debug?
target_compile_options(threadpool PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall -Wextra -Wpedantic>)

###
# Export Library
#
# Create an alias for the config dir.
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/ThreadPool)

# TODO: Shouldn't I be using INSTALL_CONFIGDIR?
# Install the library and register the target in threadpool-targets
install(TARGETS ${PROJECT_NAME}
   EXPORT threadpool-targets
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# This is required so that the exported target has the name ThreadPool and not threadpool.
# set_target_properties(threadpool PROPERTIES EXPORT_NAME ThreadPool)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install the export target defined above, set the output .cmake file name for
# the target, give it a namespace and the destination will be the the alias
# of the configdir.
#
# Callers and importing CMakefiles will use the ThreadPoolTargets.cmake file to
# import the lib.
install(EXPORT threadpool-targets
   FILE
     ThreadPoolTargets.cmake
   NAMESPACE
     ThreadPool::
   DESTINATION
     ${INSTALL_CONFIGDIR}
)

# Create a ConfigVersion.cmake file???
# TODO: Look up CMakePackageConfigHelpers, I think it saves us from manually creating
# that cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
   ${CMAKE_CURRENT_BINARY_DIR}/ThreadPoolConfigVersion.cmake
   VERSION ${PROJECT_VERSION}
   COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/ThreadPoolConfig.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/ThreadPoolConfig.cmake
   INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

# Install the config and config version to the Destination install config dir.
install(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/ThreadPoolConfig.cmake
   ${CMAKE_CURRENT_BINARY_DIR}/ThreadPoolConfigVersion.cmake
   DESTINATION ${INSTALL_CONFIGDIR}
)

# TODO: Export the cmake targets so it's a lib???
export(EXPORT threadpool-targets
   FILE ${CMAKE_CURRENT_BINARY_DIR}/ThreadPoolTargets.cmake
   NAMESPACE ThreadPool::)

# Register the library in the user package registry, this allows other projects
# to use find_package() to consider looking in the user package registry.
export(PACKAGE ThreadPool)

enable_testing()
add_subdirectory(tests)
