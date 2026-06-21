add_executable(cli_tests
        test_command_parser.cpp
        test_json_models.cpp
)

target_link_libraries(cli_tests PRIVATE
        cli_core
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(cli_tests)