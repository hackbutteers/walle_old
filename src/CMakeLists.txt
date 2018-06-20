
FILE(GLOB WSL_SRC "wsl/*.cc")
FILE(GLOB MATH_SRC "math/*.cc")

set(WALLE_SRC 
    ${WSL_SRC}
    ${MATH_SRC}
    )

add_library(walleStatic STATIC ${WALLE_SRC} )
#add_library(walleShared SHARED ${WALLE_SRC} )
target_link_libraries(walleStatic pthread dl)
#target_link_libraries(walleShared pthread )
set_target_properties(walleStatic PROPERTIES OUTPUT_NAME 
        walle_static)
#set_target_properties(walleShared PROPERTIES OUTPUT_NAME  walle)
    
    
#install(TARGETS walleStatic walleShared 
#        RUNTIME DESTINATION bin
#        LIBRARY DESTINATION lib 
#        ARCHIVE DESTINATION lib
#        )
