cmake_minimum_required(VERSION 3.21)

project(
    ExampleMod
    VERSION 0.0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Auto-init submodules if inside a git repo (self-heals if user skipped init.sh)
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE _submod_result)
    if(NOT _submod_result EQUAL 0)
        message(FATAL_ERROR "git submodule update failed (exit code ${_submod_result})")
    endif()
endif()

# Dependencies

find_package(spdlog CONFIG REQUIRED)
# directxtk must be found before add_commonlibsse_plugin (CommonLibSSE-NG links
# Microsoft::DirectXTK transitively).
find_package(directxtk CONFIG REQUIRED)
# rapidcsv must be in vcpkg.json (CommonLibSSE-NG unconditionally adds it as an include dir)

# CommonLibSSE-NG

set(ENABLE_SKYRIM_SE
    ON
    CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_AE
    ON
    CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_VR
    OFF
    CACHE BOOL "" FORCE)
set(BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(SKSE_SUPPORT_XBYAK
    OFF
    CACHE BOOL "" FORCE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/commonlibsse-ng" "${CMAKE_CURRENT_BINARY_DIR}/commonlibsse-ng"
                 EXCLUDE_FROM_ALL)

include("${CMAKE_CURRENT_SOURCE_DIR}/lib/commonlibsse-ng/cmake/CommonLibSSE.cmake")

# Plugin — auto-generates SKSEPlugin_Version; Plugin.cpp only needs SKSEPlugin_Load
add_commonlibsse_plugin(
    ${PROJECT_NAME}
    NAME
    "ExampleMod"
    AUTHOR
    "Author"
    SOURCES
    src/Plugin.cpp
    USE_ADDRESS_LIBRARY)

target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_BINARY_DIR}/src")

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES OUTPUT_NAME ExampleMod
               PREFIX ""
               SUFFIX ".dll")

# Compiler/linker flags: /Zi emits PDB in all configs; Release gets full optimization + /DEBUG:FULL
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /Zi "$<$<CONFIG:Release>:/Zc:inline;/JMC->")
    target_link_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Debug>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
                        "$<$<CONFIG:Release>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>")
endif()

# Deploy: copies DLL + PDB to SKYRIM_MODS_FOLDER, SKYRIM_FOLDER/Data, or build/deploy/ as fallback

if(DEFINED ENV{SKYRIM_MODS_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_MODS_FOLDER}")
    set(_default_deploy "$ENV{SKYRIM_MODS_FOLDER}/${PROJECT_NAME}")
elseif(DEFINED ENV{SKYRIM_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_FOLDER}/Data")
    set(_default_deploy "$ENV{SKYRIM_FOLDER}/Data")
else()
    set(_default_deploy "${CMAKE_BINARY_DIR}/deploy/${PROJECT_NAME}")
endif()

set(DEPLOY_DIR
    "${_default_deploy}"
    CACHE PATH "Deploy destination (set SKYRIM_MODS_FOLDER or SKYRIM_FOLDER in .env)")

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "${DEPLOY_DIR}/SKSE/Plugins")

install(
    FILES "$<TARGET_PDB_FILE:${PROJECT_NAME}>"
    DESTINATION "${DEPLOY_DIR}/SKSE/Plugins"
    OPTIONAL)

# `cmake --build --preset deploy`  (or  cmake --workflow --preset deploy)
add_custom_target(
    deploy
    COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}"
    DEPENDS ${PROJECT_NAME}
    COMMENT "Deploying to ${DEPLOY_DIR}/SKSE/Plugins")
