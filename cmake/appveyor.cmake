## This version strips the newline at the end
##EXEC_PROGRAM(uname  ARGS -m OUTPUT_VARIABLE ARCH)
## There should be a strip option in later versions of cmake (e.g. 2.4.4)
##EXECUTE_PROCESS(COMMAND uname -m OUTPUT_VARIABLE ARCH)
##MESSAGE("ARCH ${ARCH}")
#
#SET (CMAKE_CXX_WARNING_LEVEL 4)
SET (FLEX "C:/cygwin/bin/flex.exe")
SET (BISON "C:/cygwin/bin/bison.exe")

ADD_DEFINITIONS(-DSTATIC_BUILD -D_USE_MATH_DEFINES -DTCL_THREADS)
#Math stuff
# warning C4005: concerning macro redefines in stdint.h
# warning C4244: conversion from 'size_t' to 'const double'
# warning C4267: possible loss of data
# warning C4503: Decorated name length truncated
# warning C4800: casting from point to bool performance warning
# warning C4996: 'isatty': The POSIX name for this item is deprecated
# dll linkage warning from python, define HAVE_ROUND
SET (WARNINGS_IGNORE " /wd4005 /wd4244 /wd4267 /wd4503 /wd4800 /wd4996 /DHAVE_ROUND")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:strict /EHsc ${WARNINGS_IGNORE}")
SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fp:strict ${WARNINGS_IGNORE}")

SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")

#SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
#SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT /NODEFAULTLIB:OLDNAMES")

# windows build

IF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
ELSE (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
# 64 bit
SET (THIRDPARTY ${CMAKE_SOURCE_DIR}/external/devsim_win32_deps)
#SET(MKL_DIR
#"C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2016/windows/mkl/lib/intel64_win"
#)
#SET(INTEL_MKL_LINK
#${MKL_DIR}/mkl_intel_lp64.lib
#${MKL_DIR}/mkl_core.lib
#${MKL_DIR}/mkl_sequential.lib
#)
#SET (BLAS_ARCHIVE
#${INTEL_MKL_LINK}
#)
SET (BLAS_ARCHIVE 
#${THIRDPARTY}/CLAPACK64/lib/lapack.lib
#${THIRDPARTY}/CLAPACK64/lib/blas.lib
#${THIRDPARTY}/CLAPACK64/lib/libf2c.lib
c:/Miniconda-x64/Library/lib/mkl_rt.lib
)

SET (CGNS_ARCHIVE ${THIRDPARTY}/cgnslib64/lib/cgns.lib)
SET (CGNS_INCLUDE ${THIRDPARTY}/cgnslib64/include)
SET (PYTHON_ARCHIVE C:/Miniconda-x64/libs/python27.lib)
SET (PYTHON_INCLUDE C:/Miniconda-x64/include)
SET (SQLITE3_ARCHIVE "${THIRDPARTY}/sqlite-amalgamation-3080600/sqlite3.lib")
SET (SQLITE3_INCLUDE "${THIRDPARTY}/sqlite-amalgamation-3080600")
SET (SUPERLULOCATE  ${THIRDPARTY}/SuperLU_4.3)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/superlu.lib)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SYMDIFF_INCLUDE ${CMAKE_SOURCE_DIR}/external/symdiff/include)
SET (SYMDIFF_ARCHIVE ${CMAKE_SOURCE_DIR}/external/symdiff/win64/src/engine/Release/symdiff_static.lib)
SET (TCL_ARCHIVE "c:/Tcl/lib/tcl85.lib")
SET (TCL_INCLUDE "c:/Tcl/include")
SET (TCL_STUB    "c:/Tcl/lib/tclstub85.lib")
SET (ZLIB_ARCHIVE "${THIRDPARTY}/zlib64/lib/zlibstatic.lib")
SET (ZLIB_INCLUDE "${THIRDPARTY}/zlib64/include")
ENDIF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)

