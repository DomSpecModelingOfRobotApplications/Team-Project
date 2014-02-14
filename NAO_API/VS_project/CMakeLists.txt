
cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)

# Define the name of the project
project(apidemonstration)

# This include enable you to use qibuild framework
find_package(qibuild)


# Create an option to make is possible compiling the module
# as a remote executable, or as a local shared library
option(APIDEMONSTRATION_IS_REMOTE
  "module is compiled as a remote module (ON or OFF)"
  ON)

# Create a list of source files
set(_srcs
  apidemonstration.cpp
  apidemonstration.h
  main.cpp
)

if(APIDEMONSTRATION_IS_REMOTE)
  # Add a compile flag because code changes a little bit
  # when we are compiling an executable
  # This will let you use #ifdef APIDEMONSTRATION_IS_REMOTE
  # in the C++ code
  add_definitions( " -DAPIDEMONSTRATION_IS_REMOTE ")

  # Create an executable
  qi_create_bin(apidemonstration ${_srcs})
else()
  # Create a plugin, that is a shared library, and make
  # sure it is built in lib/naoqi, so that the naoqi executable
  # can find it later
  qi_create_lib(apidemonstration SHARED ${_srcs} SUBFOLDER naoqi)
endif()

# Tell CMake that apidemonstration depends on ALCOMMON.
# This will set the libraries to link apidemonstration with,
# the include paths, and so on
qi_use_lib(apidemonstration ALCOMMON)



# Also create a simple executable capable of creating
# a proxy to the apidemonstration module
qi_create_bin(testapidemonstration testapidemonstration.cpp)
qi_use_lib(testapidemonstration ALCOMMON)


