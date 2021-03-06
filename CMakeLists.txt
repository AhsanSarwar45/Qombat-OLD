# ===============================================
# QOMBAT BUILD SYSTEM

# This is the root CMakeLists.txt that set ups
# the general environment.
# ===============================================

cmake_minimum_required (VERSION 3.16) #3.16 for support for precompiled headers

project(Qombat)

SET(BUILD_COVERAGE FALSE CACHE BOOL "Specify if coverage report is to be generated")
SET(BUILD_COVERAGE_HTML FALSE CACHE BOOL "Specify if HTML coverage report is to be generated")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

# =======================================================
# Adding Vendor sub-directories
# =======================================================
add_subdirectory("${CMAKE_SOURCE_DIR}/Engine/Vendor/Spdlog")
add_subdirectory("${CMAKE_SOURCE_DIR}/Engine/Vendor/MagicEnum")
add_subdirectory("${CMAKE_SOURCE_DIR}/Engine/Vendor/GLFW")
add_subdirectory("${CMAKE_SOURCE_DIR}/Engine/Vendor/Glad")
add_subdirectory("${CMAKE_SOURCE_DIR}/Engine/Vendor/EASTL")
add_subdirectory("${CMAKE_SOURCE_DIR}/Tests/Vendor/Catch2")

if(BUILD_COVERAGE)
    message("-- Setting flags for coverage")
    include(CodeCoverage)
    append_coverage_compiler_flags()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")      
endif()

# =======================================================
# Adding sub-directories
# =======================================================
add_subdirectory(Editor)
add_subdirectory(Engine)

enable_testing()

add_subdirectory(Tests)

if(BUILD_COVERAGE)
    message("-- Setting up targets for coverage")
    if(BUILD_COVERAGE_HTML)
        setup_target_for_coverage_lcov(
            NAME TestCoverageHTML
            EXECUTABLE Tests
            DEPENDENCIES Engine
            BASE_DIRECTORY "${CMAKE_SOURCE_DIR}"
            EXCLUDE "/usr/include/*" "*/Vendor/*" "${CMAKE_SOURCE_DIR}/Tests/*")
    endif()
    setup_target_for_coverage_gcovr_xml(
        NAME TestCoverageXML
        EXECUTABLE Tests
        DEPENDENCIES Engine
        BASE_DIRECTORY "${CMAKE_SOURCE_DIR}"
        EXCLUDE "/usr/include/*" "*/Vendor/*" "${CMAKE_SOURCE_DIR}/Tests/*")
    # set(GCOV_LINK_FLAGS "-lgcov --coverage")
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCOV_LINK_FLAGS}")
    # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g --coverage -fprofile-arcs -ftest-coverage")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g --coverage -fprofile-arcs -ftest-coverage")       
endif()







