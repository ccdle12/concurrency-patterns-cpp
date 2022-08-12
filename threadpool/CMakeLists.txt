cmake_minimum_required(VERSION 3.10)

# Set the project name and version.
project(threadpool VERSION 1.0)

# Specify the C++ standard and the compile flag requiremetns (e.g. using pthread)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS -pthread)

##############################################################################
# Dependencies
#
# Download Google Test as a depdendency
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
##############################################################################

# Add the threadpool.cpp implementation file as a target.
add_library(
    threadpool STATIC
    include/threadpool/thread_pool.h
    src/thread_pool.cpp
)

# This creates a namespace and alias for linking used in the cmake test file?
add_library(ThreadPool::threadpool ALIAS threadpool)

# This includes the directories for compilation for the "threadpool" target.
target_include_directories(threadpool 
    PUBLIC 
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_features(threadpool PRIVATE cxx_auto_type)
target_compile_options(threadpool PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall -Wextra -Wpedantic>)

##############################################################################
# Export Library
#
# Create an alias for the config dir.
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/ThreadPool)
message(${INSTALL_CONFIGDIR})

# Install the library and register the target in threadpool-targets
install(TARGETS threadpool
   EXPORT threadpool-targets
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

#This is required so that the exported target has the name ThreadPool and not threadpool.
# set_target_properties(threadpool PROPERTIES EXPORT_NAME ThreadPool)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install the export target defined above, set the output .cmake file name for
# the target, give it a namespace and the destination wiull be the the alias
# of the  configdir.
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

##Create a ConfigVersion.cmake file???
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

#Install the config, configversion and custom find modules
install(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/ThreadPoolConfig.cmake
   ${CMAKE_CURRENT_BINARY_DIR}/ThreadPoolConfigVersion.cmake
   DESTINATION ${INSTALL_CONFIGDIR}
)

export(EXPORT threadpool-targets
   FILE ${CMAKE_CURRENT_BINARY_DIR}/ThreadPoolTargets.cmake
   NAMESPACE ThreadPool::)

#Register package in the User Package Registry
export(PACKAGE ThreadPool)

enable_testing()
add_subdirectory(tests)
