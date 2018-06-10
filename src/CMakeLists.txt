

FILE(GLOB FORMAT_SRC "format/*.cc")
FILE(GLOB STRINGS_SRC "strings/*.cc")
FILE(GLOB TIME_SRC "time/*.cc")
FILE(GLOB SYS_SRC "sys/*.cc")
FILE(GLOB COMMON_SRC "common/*.cc")
FILE(GLOB LOG_SRC "log/*.cc")
FILE(GLOB MEMORY_SRC "memory/*.cc")
FILE(GLOB MATH_SRC "math/*.cc")

set(WALLE_SRC 
    ${FORMAT_SRC}
    ${STRINGS_SRC}
    ${TIME_SRC}
    ${SYS_SRC}
    ${COMMON_SRC}
    ${LOG_SRC}
    ${MEMORY_SRC}
    ${MATH_SRC}
    )

add_library(walleStatic STATIC ${WALLE_SRC} )
#add_library(walleShared SHARED ${WALLE_SRC} )
target_link_libraries(walleStatic pthread )
#target_link_libraries(walleShared pthread )
set_target_properties(walleStatic PROPERTIES OUTPUT_NAME 
        walle_static)
#set_target_properties(walleShared PROPERTIES OUTPUT_NAME  walle)
    
    
#install(TARGETS walleStatic walleShared 
#        RUNTIME DESTINATION bin
#        LIBRARY DESTINATION lib 
#        ARCHIVE DESTINATION lib
#        )
