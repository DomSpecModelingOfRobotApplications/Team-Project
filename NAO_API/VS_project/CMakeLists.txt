cmake_minimum_required(VERSION 2.8)
project(VS_project)

find_package(qibuild)

# Create a executable named VS_project
# with the source file: main.cpp
qi_create_bin(VS_project "main.cpp")

# Add a simple test:
enable_testing()
qi_create_test(test_VS_project "test.cpp")
