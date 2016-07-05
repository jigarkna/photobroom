
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(SYSTEM ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})

include_directories(backends/sql_backends)

set(SRC
        backends/sql_backends/sql_action_query_generator.cpp
        backends/sql_backends/sql_filter_query_generator.cpp
        backends/sql_backends/variant_converter.cpp
        implementation/action.cpp
        implementation/filter.cpp
        implementation/photo_data.cpp
        implementation/photo_info.cpp
        implementation/photo_types.cpp
        implementation/virtual_destructors.cpp

        # sql tests:
        unit_tests/sql_filter_query_generator_tests.cpp
        unit_tests/sql_action_query_generator_tests.cpp

        # main()
        unit_tests/main.cpp
    )

add_executable(database_tests ${SRC})

target_link_libraries(database_tests PRIVATE core Qt5::Core Qt5::Gui ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
enableCodeCoverage(database_tests)

add_test(database database_tests)