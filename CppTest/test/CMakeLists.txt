project(Cpp)

# include test framework headers
include_directories(${PROJECT_SOURCE_DIR}/../include)

set(TEST_SOURCE_FILES
        main.cpp
        finally.hpp
        lambda_test.cpp
        value_reference_test.cpp
        resource_management_test.cpp)

add_executable(CppTest ${TEST_SOURCE_FILES})
