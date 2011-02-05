CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

SET(TARGET_NAME db_tools)
PROJECT(${TARGET_NAME})

SET(CMAKE_CXX_STANDARD 17)

# db_tools
SET(SOURCE_FILES
    db_tools.cc db_tools.h
    t_opt.cc t_opt.h
    )

ADD_EXECUTABLE(${TARGET_NAME} ${SOURCE_FILES})

set(LIBS_NLTK_TOOLS
    ${apr} ${aprutil}
    ${pcre2}
    ${jansson}
    ${icuuc} ${icui18n}
)

TARGET_LINK_LIBRARIES(${TARGET_NAME} ${LIBS_NLTK_TOOLS})
