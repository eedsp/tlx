CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

PROJECT(libhx VERSION 1.0.0)

SET(CMAKE_CXX_STANDARD 17)

set(LIB_HX_SOURCE_FILES
    ../../include/tlx_types.h
    hx_util.cc      hx_util.h
    hx_log.cc       hx_log.h
)

# libhx-2
add_library(
    libhx_objs
    OBJECT
    ${LIB_HX_SOURCE_FILES}
)

if (BUILD_LIB_HX_SHARED)
#    set(LIB_HX_shared lib${LIB_HX}_shared)
    # shared libraries need PIC
    set_property(TARGET ${libhx_objs} PROPERTY POSITION_INDEPENDENT_CODE 1)

    add_library(
        ${LIB_HX_shared}
        SHARED
        $<TARGET_OBJECTS:libhx_objs>
    )
    target_link_libraries(${LIB_HX_shared} ${LIBS_COMMON})
    target_link_libraries(${LIB_HX_shared} ${hiredis})
    set_target_properties(${LIB_HX_shared} PROPERTIES OUTPUT_NAME ${LIB_HX} CLEAN_DIRECT_OUTPUT 1)
endif ()

if (BUILD_LIB_HX_STATIC)
#    set(LIB_HX_static lib${LIB_HX}_static)
    add_library(
        ${LIB_HX_static}
        STATIC
        $<TARGET_OBJECTS:libhx_objs>
    )
    set_target_properties(${LIB_HX_static} PROPERTIES OUTPUT_NAME ${LIB_HX} CLEAN_DIRECT_OUTPUT 1)
    set_target_properties(${LIB_HX_static} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
endif ()

