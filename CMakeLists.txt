PROJECT (DEVSIM)
#cmake_minimum_required(VERSION 2.6)
OPTION(GENIUSREADER "Build with Genius Reader" ON)
OPTION(VTKWRITER    "Build with VTK Writer" ON)
OPTION(TCLMAIN      "Build with TCL Interpreter" ON)
OPTION(DEVSIM_EXTENDED_PRECISION "Build with extended precision" OFF)


set (CMAKE_CXX_STANDARD 11)

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/${DEVSIM_CONFIG}.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/local.cmake OPTIONAL)

ADD_DEFINITIONS(-DDEVSIM_VERSION_STRING="\\"Beta 0.01\\"")
ADD_DEFINITIONS(-DDEVSIM_COPYRIGHT_YEAR="\\"2009-2017\\"")

IF (DEVSIM_EXTENDED_PRECISION)
INCLUDE_DIRECTORIES(${BOOST_INCLUDE})
ADD_DEFINITIONS(-DDEVSIM_EXTENDED_PRECISION)
ENDIF (DEVSIM_EXTENDED_PRECISION)

SET (SUBDIRS
    src
    testing
)

SITE_NAME(HOSTNAME)
MESSAGE("Hostname: ${HOSTNAME}")

ENABLE_TESTING()
FOREACH (SUB ${SUBDIRS})
    ADD_SUBDIRECTORY(${SUB})
ENDFOREACH (SUB ${SUBDIRS})

