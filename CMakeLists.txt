CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(tlx VERSION 1.0.0)

SET(CMAKE_CXX_STANDARD 17)

# cmake needed modules
include(CheckIncludeFiles)
include(CheckLibraryExists)

############################################################################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/lib)
#set(LIBRARY_OUTPUT_PATH             ${PROJECT_SOURCE_DIR}/build)
#set(EXECUTABLE_OUTPUT_PATH          ${CMAKE_CURRENT_SOURCE_DIR}/build)

############################################################################

message("-- Platform/Operating Systems: ${CMAKE_SYSTEM_NAME}")

message("-- Set Environment Variable \$ENV{APP_PREFIX} -> \${_APP_PREFIX_}")
set(_APP_PREFIX_ $ENV{APP_PREFIX})
if (NOT _APP_PREFIX_)
    message("-- Set Environment Variable \$ENV{HOME} -> \${_HOME_}")
    set(_HOME_ $ENV{HOME})
	set(_APP_PREFIX_ "${_HOME_}/_local")
    message("-- Set Environment Variable \${APP_PREFIX}:" ${_APP_PREFIX_})
endif ()

set(PKG_INCLUDE_PATH
    ${_APP_PREFIX_}/include
    ${_APP_PREFIX_}/include/apr-1
	/usr/local/include
)
set(PKG_LIBRARY_PATH
    ${_APP_PREFIX_}/lib
	/usr/local/lib
)

############################################################################

set(SRC_INCLUDE_PATH
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src/hx
    ${PROJECT_SOURCE_DIR}/src/tpl
)

set(SRC_LIBRARY_PATH
    ${PROJECT_SOURCE_DIR}/lib
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

############################################################################

include_directories(
    ${SRC_INCLUDE_PATH}
    ${PKG_INCLUDE_PATH}
)

link_directories(
    ${SRC_LIBRARY_PATH}
    ${PKG_LIBRARY_PATH}
)

############################################################################

set(USE_JEMALLOC    NO)
set(USE_TCMALLOC    NO)
#set(USE_CUDA        NO)
#set(USE_BOOST       NO)

############################################################################

# Boost
#if (USE_BOOST)
#    set(BOOST_ROOT ${_APP_PREFIX_})
#    set(Boost_USE_MULTITHREADED     ON)
#    set(Boost_USE_STATIC_LIBS       OFF)
#    set(Boost_USE_STATIC_RUNTIME    OFF)
#endif ()
#
#if (USE_CUDA)
#    set(CUDA_HOME $ENV{CUDA_HOME})
#    if (NOT CUDA_HOME)
#        set(CUDA_HOME "/usr/local/cuda")
#    endif()
#
#    set(CUDA_TOOLKIT_ROOT_DIR   ${CUDA_HOME})
#    set(CUDA_SDK_ROOT_DIR       ${CUDA_HOME})
#endif ()

############################################################################

set(BUILD_FLAGS "-g -Wall -fPIC")
set(BUILD_FLAGS "${BUILD_FLAGS} -DU_CHARset_IS_UTF8=1 -D_REENTRANT")

set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -m64 -std=c11 ${BUILD_FLAGS}")
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -m64 -std=c++11 ${BUILD_FLAGS}")

if (USE_JEMALLOC)
    # with jemalloc
    set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS} -DUSE_JEMALLOC")
    set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -DUSE_JEMALLOC")
elseif(USE_TCMALLOC)
    # with tcmalloc
    set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS} -DUSE_TCMALLOC")
    set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -DUSE_TCMALLOC")
endif ()

############################################################################

find_library(apr        NAMES "apr-1"       PATHS ${PKG_LIBRARY_PATH} NO_DEFAULT_PATH)
find_library(aprutil    NAMES "aprutil-1"   PATHS ${PKG_LIBRARY_PATH} NO_DEFAULT_PATH)

find_library(icuuc      NAMES "icuuc"       PATHS ${PKG_LIBRARY_PATH} NO_DEFAULT_PATH)
find_library(icui18n    NAMES "icui18n"     PATHS ${PKG_LIBRARY_PATH} NO_DEFAULT_PATH)
#find_library(icudata    NAMES "icudata"     PATHS ${PKG_LIBRARY_PATH} NO_DEFAULT_PATH)
#find_library(icule      NAMES "icule"       PATHS ${PKG_LIBRARY_PATH} NO_DEFAULT_PATH)
#find_library(iculx      NAMES "iculx"       PATHS ${PKG_LIBRARY_PATH} NO_DEFAULT_PATH)
#find_library(icuio      NAMES "icuio"       PATHS ${PKG_LIBRARY_PATH} NO_DEFAULT_PATH)

find_library(pcre2      NAMES "pcre2-8"     PATHS ${PKG_LIBRARY_PATH} NO_DEFAULT_PATH)
find_library(jansson    NAMES "jansson"     PATHS ${PKG_LIBRARY_PATH} NO_DEFAULT_PATH)

if (USE_JEMALLOC)
    find_library(JEMALLOC_LIB jemalloc)
    if (JEMALLOC_LIB)
        message(STATUS "Using jemalloc memory allocator at ${JEMALLOC_LIB}")
    else()
        message(STATUS "Can't find jemalloc")
    endif ()
elseif(USE_TCMALLOC)
    find_library(TCMALLOC_LIB tcmalloc)
    if (TCMALLOC_LIB)
        message(STATUS "Using tcmalloc memory allocator at ${TCMALLOC_LIB}")
    else()
        message(STATUS "Can't find tcmalloc")
    endif()
endif ()

#if (USE_CUDA)
#    find_package(CUDA REQUIRED)
#    if (${CUDA_FOUND})
#        message("CUDA_TOOLKIT_ROOT_DIR : " ${CUDA_TOOLKIT_ROOT_DIR})
#        message("CUDA_VERSION          : " ${CUDA_VERSION})
#        message("CUDA_INCLUDE_DIRS     : " ${CUDA_INCLUDE_DIRS})
#        message("CUDA_LIBRARIES        : " ${CUDA_LIBRARIES})
#    endif ()
#endif ()
#
#if (USE_BOOST)
#    find_package(Boost REQUIRED COMPONENTS filesystem system)
#    if (Boost_FOUND)
#        message("Boost_INCLUDE_DIRS : " ${Boost_INCLUDE_DIRS})
#        message("Boost_LIBRARY_DIRS : " ${Boost_LIBRARY_DIRS})
#        message("Boost_LIBRARIES    : " ${Boost_LIBRARIES})
#    endif ()
#endif ()

############################################################################

set(LIBS_COMMON
    ${apr} ${aprutil}
    ${icuuc} ${icui18n}
#    ${icudata} ${icule} ${iculx} ${icuio}
    ${pcre2}
    ${jansson}
)

if (USE_JEMALLOC)
    # with jemalloc
    list(APPEND LIBS ${JEMALLOC_LIB})
elseif(USE_TCMALLOC)
    # with tcmalloc
    list(APPEND LIBS ${TCMALLOC_LIB})
endif ()

############################################################################

set(LIB_HX hx-1)
set(LIB_HX_shared ${LIB_HX}_shared)
set(LIB_HX_static ${LIB_HX}_static)
set(BUILD_LIB_HX_SHARED NO)
set(BUILD_LIB_HX_STATIC YES)

set(LIB_TPL tpl-1)
set(LIB_TPL_shared ${LIB_TPL}_shared)
set(LIB_TPL_static ${LIB_TPL}_static)
set(BUILD_LIB_TPL_SHARED NO)
set(BUILD_LIB_TPL_STATIC YES)

############################################################################

add_subdirectory(src/hx)
add_subdirectory(src/tpl)

add_subdirectory(tools/db_tools)
add_subdirectory(tools/db2shm)
add_subdirectory(tools/text_sgmt)

