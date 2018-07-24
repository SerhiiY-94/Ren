cmake_minimum_required(VERSION 3.2)
project(test_Ren)

IF(WIN32)
	set(LIBS opengl32)
ELSE(WIN32)
    #set(LIBS X11 dl pthread GL glut c)
    IF(APPLE)
        set(LIBS SDL2 dl pthread c)
    ELSE(APPLE)
        set(LIBS X11 SDL2 dl pthread c)
    ENDIF(APPLE)
ENDIF(WIN32)

set(SOURCE_FILES main.cpp
				 membuf.h
                 test_anim.cpp
                 test_common.h
				 test_mat.cpp
                 test_material.cpp
                 test_mesh.cpp
                 test_sparse_array.cpp
                 test_storage.cpp
                 test_texture.cpp
				 test_vec.cpp
                 )

if(${RENDERER} STREQUAL "GL")
    set(SOURCE_FILES ${SOURCE_FILES}
                     test_programGL.cpp)
elseif(${RENDERER} STREQUAL "SW")
    set(SOURCE_FILES ${SOURCE_FILES}
                     test_programSW.cpp)
elseif(${RENDERER} STREQUAL "VK")

endif()

add_executable(test_Ren ${SOURCE_FILES})
target_link_libraries(test_Ren ${LIBS} Ren)
