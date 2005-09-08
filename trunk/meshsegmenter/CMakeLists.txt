
PROJECT(meshsegmenter)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc. 
ADD_DEFINITIONS(-Wall -O0 -g3)

SET( DEBUG_LEVEL 0 CACHE STRING "Level of debugging output (This is either 0 (no output) or 1,2,3)")

INCLUDE(${CMAKE_ROOT}/Modules/FindQt.cmake)
IF(NOT QT_FOUND)
	MESSAGE( SEND_ERROR "Qt library is needed for compilation" )
ENDIF(NOT QT_FOUND)	

FIND_PACKAGE(aipsbase)
IF(NOT aipsbase_FOUND)
	MESSAGE( SEND_ERROR "aipsbase library is needed for compilation" )
ENDIF(NOT aipsbase_FOUND)	

FIND_PACKAGE(aipsfilehandlers)
IF(NOT aipsfilehandlers_FOUND)
	MESSAGE( SEND_ERROR "aipsfilehandlers library is needed for compilation" )
ENDIF(NOT aipsfilehandlers_FOUND)	

INCLUDE(${CMAKE_ROOT}/Modules/FindVTK.cmake)
INCLUDE( ${VTK_USE_FILE} )
INCLUDE( ${aipsbase_USE_FILE} )
INCLUDE( ${aipsfilehandlers_USE_FILE} )
INCLUDE_DIRECTORIES( ${VTK_INCLUDE_DIRS} /home/belitz/include )
LINK_DIRECTORIES( ${VTK_LIBRARY_DIRS} )
LINK_LIBRARIES( vtkIO vtkCommon vtkRendering vtkGraphics aipsfilehandlers aipsbase )
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
LINK_LIBRARIES( aipsbase ${QT_LIBRARIES} )
LINK_DIRECTORIES( -L/usr/local/lib/vtk )

SET( DEBUG_LEVEL 0 CACHE STRING "Level of debugging output (This is either 0 (no output) or 1,2,3)")

#CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/aipsgui_config.h.in 
#	${PROJECT_BINARY_DIR}/aipsgui_config.h)
#SUBDIRS( src )

SUBDIRS( src )