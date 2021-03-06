
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)


addTestTarget(photos_crawler
                SOURCES
                    default_analyzers/file_analyzer.cpp
                    implementation/ifile_system_scanner.cpp
                    implementation/photo_crawler.cpp

                    unit_tests/analyzerTests.cpp
                    unit_tests/photo_crawler_tests.cpp
                    unit_tests/photo_crawler_builder_tests.cpp

                LIBRARIES
                    core
                    Qt5::Core
                    ${GMOCK_MAIN_LIBRARY}
                    ${GMOCK_LIBRARY}
                    ${GTEST_LIBRARY}
                    ${CMAKE_THREAD_LIBS_INIT}

                INCLUDES
                    ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                SYSTEM_INCLUDES
                    ${GMOCK_INCLUDE_DIRS}
                    ${GTEST_INCLUDE_DIRS}
)
