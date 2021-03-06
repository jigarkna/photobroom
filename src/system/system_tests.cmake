
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(Qt5Core 5.4 REQUIRED)
find_package(Threads REQUIRED)


addTestTarget(system
                SOURCES
                    unit_tests/common_path.cpp
                    implementation/filesystem.cpp
                LIBRARIES
                    ${GMOCK_MAIN_LIBRARY}
                    ${GMOCK_LIBRARY}
                    ${GTEST_LIBRARY}
                    ${CMAKE_THREAD_LIBS_INIT}
                    Qt5::Core

                SYSTEM_INCLUDES
                    ${GMOCK_INCLUDE_DIRS}
                    ${GTEST_INCLUDE_DIRS}
                    ${Qt5Core_INCLUDE_DIRS}
                INCLUDES
                    ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_BINARY_DIR}
)
