cmake_minimum_required(VERSION 3.2)
project(TestRen)

IF(WIN32)
    IF(SDL_STATIC)
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            set(LIBS :libSDL2main.a :libSDL2.a :libSDL2_mixer.a winmm imm32 version)
        elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
            set(LIBS SDL2main.lib SDL2.lib winmm imm32 version)
        endif()
    ELSE(SDL_STATIC)
        set(LIBS SDL2)
    ENDIF(SDL_STATIC)
ELSE(WIN32)
    #set(LIBS X11 :libSDL2.a dl pthread GL glut c)
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

add_executable(TestRen ${SOURCE_FILES})
target_link_libraries(TestRen ${LIBS} Ren)
