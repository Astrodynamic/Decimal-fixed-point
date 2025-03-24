cmake_minimum_required(VERSION 3.28.3)
project(TEST VERSION 1.0 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  ${PROJECT_NAME}
  tests.cc
)

target_link_libraries(${PROJECT_NAME}
  GTest::gtest_main
  Decimal
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
