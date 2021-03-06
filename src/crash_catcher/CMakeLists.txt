
include(GenerateExportHeader)

find_package(KF5Crash QUIET)

include_directories(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


# crash handling
set(CRASH_HANDLER_FOUND FALSE)
set(CRASH_HANDLER_SOURCES)
set(CRASH_HANDLER_LIBRARIES)

if(KF5Crash_FOUND)

    include_directories(SYSTEM $<TARGET_PROPERTY:KF5::Crash,INTERFACE_INCLUDE_DIRECTORIES>)

    set(CRASH_HANDLER_SOURCES crash_catcher_kde5.cpp)
    set(CRASH_HANDLER_LIBRARIES KF5::Crash)
    set(CRASH_HANDLER_FOUND TRUE)

endif(KF5Crash_FOUND)

if(NOT CRASH_HANDLER_FOUND)

    include_directories(${PROJECT_SOURCE_DIR}/tools
                        $<TARGET_PROPERTY:catcher,INTERFACE_INCLUDE_DIRECTORIES>
    )

    set(CRASH_HANDLER_SOURCES crash_catcher_internal.cpp)
    set(CRASH_HANDLER_LIBRARIES catcher)
    set(CRASH_HANDLER_FOUND TRUE)

endif()

set(SOURCES ${CRASH_HANDLER_SOURCES})

# common stuff
set(SOURCES ${SOURCES}
            crash_catcher.cpp
)

set(HEADERS
        crash_catcher.hpp
)

add_library(crash_catcher ${SOURCES} ${HEADERS})
target_include_directories(crash_catcher PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(crash_catcher PRIVATE
                        ${CRASH_HANDLER_LIBRARIES}
                        core
)

hideSymbols(crash_catcher)
generate_export_header(crash_catcher)

if(BUILD_SHARED_LIBS)
    install(TARGETS crash_catcher RUNTIME DESTINATION ${PATH_LIBS}
                                  LIBRARY DESTINATION ${PATH_LIBS})
endif()
