cmake_minimum_required(VERSION 3.10)

# Set the project name and version.
project(actor VERSION 1.0)

# Specify the C++ standard and the compile flag requiremetns (e.g. using pthread)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS -pthread)

# TODO: Maybe make this switch according to the build required, prod, debug etc..
# but it's required for gdb
set(CMAKE_BUILD_TYPE Debug)

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

# Add the ThreadPool library
find_package(ThreadPool 1.0 REQUIRED)

##############################################################################

add_library(
    actor STATIC
    include/actor/actor.h
    include/actor/scheduler.h
    src/scheduler.cpp
)

# This creates a namespace and alias for linking used in the cmake test file?
add_library(Actor::actor ALIAS actor)
target_link_libraries(actor ThreadPool::threadpool)

# This includes the directories for compilation for the "actor" target.
target_include_directories(actor 
    PUBLIC 
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_features(actor PRIVATE cxx_auto_type)
target_compile_options(actor PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall -Wextra -Wpedantic>)

##############################################################################
# Export Library
#
# Create an alias for the config dir.
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Actor)
message(${INSTALL_CONFIGDIR})

# Install the library and register the target in actor-targets
install(TARGETS actor
   EXPORT actor-targets
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

#This is required so that the exported target has the name Actor and not actor.
# set_target_properties(actor PROPERTIES EXPORT_NAME Actor)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install the export target defined above, set the output .cmake file name for
# the target, give it a namespace and the destination will be the the alias
# of the  configdir.
#
# Callers and importing CMakefiles will use the ActorTargets.cmake file to
# import the lib.
install(EXPORT actor-targets
   FILE
     ActorTargets.cmake
   NAMESPACE
     Actor::
   DESTINATION
     ${INSTALL_CONFIGDIR}
)

##Create a ConfigVersion.cmake file???
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
   ${CMAKE_CURRENT_BINARY_DIR}/ActorConfigVersion.cmake
   VERSION ${PROJECT_VERSION}
   COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/ActorConfig.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/ActorConfig.cmake
   INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/ActorConfig.cmake
   ${CMAKE_CURRENT_BINARY_DIR}/ActorConfigVersion.cmake
   DESTINATION ${INSTALL_CONFIGDIR}
)

export(EXPORT actor-targets
   FILE ${CMAKE_CURRENT_BINARY_DIR}/ActorTargets.cmake
   NAMESPACE Actor::)

#Register package in the User Package Registry
export(PACKAGE Actor)

enable_testing()
add_subdirectory(tests)
