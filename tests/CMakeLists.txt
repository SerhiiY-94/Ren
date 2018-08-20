cmake_minimum_required(VERSION 3.2)
project(test_Ren)

if(WIN32)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(SDL2_INCLUDE_DIRS SDL2/x86_64-w64-mingw32/include)
			set(SDL2_LIBRARIES "SDL2/x86_64-w64-mingw32/lib")
		else(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(SDL2_INCLUDE_DIRS SDL2/i686-w64-mingw32/include)
			set(SDL2_LIBRARIES "SDL2/i686-w64-mingw32/lib")
		endif()
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(SDL2_LIBRARIES "SDL2/lib/x64")
        else(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(SDL2_LIBRARIES "SDL2/lib/x86")
        endif()
        set(SDL2_INCLUDE_DIRS SDL2/include)
    endif()
else(WIN32)
    include(FindPkgConfig)
    pkg_search_module (SDL2 REQUIRED sdl2)

    if(APPLE)
        set(SDL2_INCLUDE_DIRS /usr/local/include)
        set(SDL2_LIBRARIES /usr/local/lib)
    endif(APPLE)
endif(WIN32)

include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARIES})

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
