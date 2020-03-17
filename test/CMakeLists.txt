CMAKE_MINIMUM_REQUIRED(VERSION 3.0) # cmake最低版本要求

PROJECT(RTGC)

INCLUDE(CTest)

ADD_DEFINITIONS("-D_CRT_SECURE_NO_DEPRECATE
                 -DALL_INTERIOR_POINTERS -DNO_EXECUTE_PERMISSION")

IF(APPLE)
    IF("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "")
        SET(CMAKE_OSX_ARCHITECTURES "ppc;i386;x86_64" CACHE STRING "Build architectures for Mac OS X" FORCE)
    ENDIF()
ENDIF(APPLE)

#LIBATOMIC #TODO
#ADD_LIBRARY(atomic_ops STATIC )
#SET_TARGET_PROPERTIES(atomic_ops PROPERTIES COMPILE_FLAGS -DNO_DEBUGGING)


#LIBGC

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(libatomic_ops/src)

SET(SRC alloc.c reclaim.c allchblk.c misc.c mach_dep.c os_dep.c
        mark_rts.c headers.c mark.c obj_map.c blacklst.c finalize.c
        new_hblk.c dbg_mlc.c malloc.c dyn_load.c typd_mlc.c ptr_chck.c
        mallocx.c)
SET(LIBS)
OPTION(enable_threads "TODO" NO)
IF(enable_threads)
        FIND_PACKAGE(Threads REQUIRED)
        MESSAGE("Thread Model: ${CMAKE_THREAD_LIBS_INIT}" )
        INCLUDE_DIRECTORIES(${Threads_INCLUDE_DIR})
        SET(LIBS ${LIBS} ${Threads_LIBRARIES})
ENDIF(enable_threads)

OPTION(enable_handle_fork "Attempt to ensure a usable collector after fork()" ON)

OPTION(enable_thread_local_alloc "Turn on thread-local allocation optimization" ON)

OPTION(enable_parallel_mark "Parallelize marking and free list construction" ON)


SET(SRC ${SRC} gc_cpp.cc)

OPTION(enable_threads_discovery "Enable threads discovery in GC" ON)
IF(NOT enable_threads_discovery)
    ADD_DEFINITIONS("-DGC_NO_THREADS_DISCOVERY")
ENDIF()


IF(WIN32)
  ADD_EXECUTABLE(cord cord/cordbscs.c cord/cordxtra.c
                 cord/tests/de.c cord/tests/de_win.c)
  SET_TARGET_PROPERTIES(cord PROPERTIES WIN32_EXECUTABLE TRUE)
  SET_TARGET_PROPERTIES(cord    PROPERTIES
                      COMPILE_DEFINITIONS GC_NOT_DLL)
  TARGET_LINK_LIBRARIES(cord gc-lib)
  TARGET_LINK_LIBRARIES(cord gdi32)
ENDIF(WIN32)

ADD_SUBDIRECTORY(tests)
