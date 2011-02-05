CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

project(libtpl VERSION 1.0.0)

SET(CMAKE_CXX_STANDARD 17)

set(LIB_TPL_SOURCE_FILES
    vx_type.h
    vtx.cc vtx.h
    vx_normlzr.cc vx_normlzr.h
    vx_str.cc vx_str.h
    )

# libtpl
add_library(
    libtpl_objs
    OBJECT
    ${LIB_TPL_SOURCE_FILES}
)

if (BUILD_LIB_TPL_SHARED)
    # shared libraries need PIC
    set_property(TARGET ${libtpl_objs} PROPERTY POSITION_INDEPENDENT_CODE 1)

    add_library(
        ${LIB_TPL_shared}
        SHARED
        $<TARGET_OBJECTS:libtpl_objs>
    )
    target_link_libraries(${LIB_TPL_shared} ${LIBS_COMMON})
    set_target_properties(${LIB_TPL_shared} PROPERTIES OUTPUT_NAME ${LIB_TPL} CLEAN_DIRECT_OUTPUT 1)
endif ()

if (BUILD_LIB_TPL_STATIC)
    add_library(
        ${LIB_TPL_static}
        STATIC
        $<TARGET_OBJECTS:libtpl_objs>
    )
    set_target_properties(${LIB_TPL_static} PROPERTIES OUTPUT_NAME ${LIB_TPL} CLEAN_DIRECT_OUTPUT 1)
    set_target_properties(${LIB_TPL_static} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
endif ()

