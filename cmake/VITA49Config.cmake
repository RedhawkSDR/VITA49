
#include(CMakeFindDependencyMacro)

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

include("${CMAKE_CURRENT_LIST_DIR}/VITA49Targets.cmake")
