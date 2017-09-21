cmake_minimum_required(VERSION 2.8.4)
project(ren)

if(WIN32)
    if(SDL_STATIC)
    else(SDL_STATIC)
        if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            set(LIBS opengl32 SDL2 SDL2_mixer)
        elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
            set(LIBS opengl32 SDL2 SDL2_mixer)
        endif()
    endif(SDL_STATIC)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
else(WIN32)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
if(${RENDERER} STREQUAL "GL")
    set(LIBS GL)
elseif(${RENDERER} STREQUAL "VK")
    set(LIBS GL)
    add_definitions(-DVK_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR)
endif()
ENDIF(WIN32)

set(SOURCE_FILES Anim.h
                 Anim.cpp
				 Camera.h
				 Camera.cpp
                 Context.h
				 Context.cpp
                 Fwd.h
                 Material.h
                 Material.cpp
                 Matrices_old.h
                 Matrices_old.cpp
				 Mesh.h
				 Mesh.cpp
				 Program.h
				 RenderThread.h
				 RenderThread.cpp
                 RingBuffer.h
                 SparseArray.h
				 Storage.h
				 Texture.h
                 Utils.h
                 Utils.cpp)

if(${RENDERER} STREQUAL "GL")
    set(SOURCE_FILES ${SOURCE_FILES}
                     ContextGL.cpp
					 GL.h
					 GLExt.cpp
                     ProgramGL.h
				     ProgramGL.cpp
                     TextureGL.h
				     TextureGL.cpp)

    if(WIN32)
        if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            set(LIBS ${LIBS} SOIL2 opengl32)
        else()
            set(LIBS ${LIBS} SOIL2 opengl32)
        endif()
    else(WIN32)
        set(LIBS ${LIBS} SOIL2)
    ENDIF(WIN32)
elseif(${RENDERER} STREQUAL "SW")
    set(SOURCE_FILES ${SOURCE_FILES}
                     ContextSW.cpp
                     ProgramSW.h
                     ProgramSW.cpp
                     TextureSW.h
                     TextureSW.cpp)
elseif(${RENDERER} STREQUAL "VK")
    set(SOURCE_FILES    ${SOURCE_FILES}
                        RenderStateVK.cpp
                        RenderStateVK.h)
endif()

add_subdirectory(SW)

list(APPEND ALL_SOURCE_FILES ${SOURCE_FILES})
set_source_files_properties(${SOURCE_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("src" FILES ${SOURCE_FILES})

list(APPEND ALL_SOURCE_FILES _ren.cpp)
source_group("src" FILES _ren.cpp)

add_library(ren STATIC ${ALL_SOURCE_FILES})
target_link_libraries(ren ${LIBS} math)

add_subdirectory(tests)
