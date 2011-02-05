CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

SET(TARGET_NAME text_sgmt)
PROJECT(${TARGET_NAME})

SET(CMAKE_CXX_STANDARD 17)

# text_segmenter
SET(SOURCE_FILES
    text_sgmt.cc text_sgmt.h
    text_sgmt_opt.cc text_sgmt_opt.h
    )

ADD_EXECUTABLE(${TARGET_NAME} ${SOURCE_FILES})

if (BUILD_LIB_HX_SHARED)
    add_dependencies(${TARGET_NAME} ${LIB_HX_shared})
    add_dependencies(${TARGET_NAME} ${LIB_TPL_shared})
endif ()
if (BUILD_LIB_HX_STATIC)
    add_dependencies(${TARGET_NAME} ${LIB_HX_static})
    add_dependencies(${TARGET_NAME} ${LIB_TPL_static})
endif ()

TARGET_LINK_LIBRARIES(${TARGET_NAME} ${LIB_HX})
TARGET_LINK_LIBRARIES(${TARGET_NAME} ${LIB_TPL})
TARGET_LINK_LIBRARIES(${TARGET_NAME} ${LIBS_COMMON})
