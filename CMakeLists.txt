cmake_minimum_required(VERSION 3.14)

project(
    VITA49
    VERSION 1.0
    LANGUAGES CXX
)

message(STATUS "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}")
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

OPTION(ENABLE_STATIC_LIBS "Build static library instead of shared library." OFF)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_VERBOSE_MAKEFILE TRUE)
ENDIF()

#-------------------------
# opmesh core
file(GLOB ${PROJECT_NAME}_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/src/*.cc"
)

# Set CXX11
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

option(${PROJECT_NAME}_INSTALL "Set to OFF to disable install." ON)

include(GenerateExportHeader)

# Build object first
add_library(
    ${PROJECT_NAME}
    ${${PROJECT_NAME}_sources}
)
add_library(
    ${PROJECT_NAME}::${PROJECT_NAME}
    ALIAS
    ${PROJECT_NAME}
)
generate_export_header(${PROJECT_NAME})
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION}"
        #DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
)
# Compiler options
target_compile_options(${PROJECT_NAME}
    PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Wno-long-long
)

# Linker options
target_link_options(${PROJECT_NAME}
    PRIVATE
    # Add undefined symbol check at linker
    -Wl,--no-undefined
)

# Include Local directories
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cpp/include>
    INTERFACE
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

##################################################
# Dependencies
#

# Search default path, basically require libuuid-devel be installed in a standard spot
find_path(
    LIBUUID_INCLUDE_DIR
    uuid/uuid.h
    REQUIRED
)
find_library(
    LIBUUID_LIBRARY
    libuuid.so
    REQUIRED
)

if (${${PROJECT_NAME}_INSTALL})
    # Install library and test executables
    install(
        TARGETS ${PROJECT_NAME} ${${PROJECT_NAME}_EXPORT_LIBRARIES} EXPORT "${PROJECT_NAME}Targets"
        INCLUDES
        LIBRARY
        ARCHIVE
    )
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cpp/include/
        DESTINATION include/${PROJECT_NAME}
    )

    # Make it usable and findable by other cmake projects
    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    export(EXPORT "${PROJECT_NAME}Targets"
        FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Targets.cmake"
        NAMESPACE ${PROJECT_NAME}::
    )

    configure_file(cmake/${PROJECT_NAME}Config.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake"
        COPYONLY
    )


    set(ConfigPackageLocation lib/cmake/${PROJECT_NAME})

    install(EXPORT "${PROJECT_NAME}Targets"
        FILE
            ${PROJECT_NAME}Targets.cmake
        NAMESPACE
            ${PROJECT_NAME}::
        DESTINATION
            ${ConfigPackageLocation}
    )

    install(
        FILES
            cmake/${PROJECT_NAME}Config.cmake
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION
            ${ConfigPackageLocation}
    )
endif()

