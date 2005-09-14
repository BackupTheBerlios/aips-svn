
PROJECT(meshsegmenter)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc. 
ADD_DEFINITIONS(-Wall)

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
IF(NOT VTK_FOUND)
	MESSAGE( SEND_ERROR "VTK 4.x library is needed for compilation" )
ENDIF(NOT VTK_FOUND)	

INCLUDE(${CMAKE_ROOT}/Modules/FindITK.cmake)
IF(NOT ITK_FOUND)
	MESSAGE( SEND_ERROR "ITK 2.x library is needed for compilation" )
ENDIF(NOT ITK_FOUND)	

INCLUDE( ${VTK_USE_FILE} )
INCLUDE( ${ITK_USE_FILE} )
INCLUDE( ${aipsbase_USE_FILE} )
INCLUDE( ${aipsfilehandlers_USE_FILE} )
INCLUDE_DIRECTORIES( ${VTK_INCLUDE_DIRS} ${ITK_INCLUDE_DIRS} ${aipsbase_INCLUDE_DIRS} ${aipsfilehandlers_INCLUDE_DIRS} )

LINK_DIRECTORIES( ${VTK_LIBRARY_DIRS} ${ITK_LIBRARY_DIRS} ${aipsbase_LIBRARY_DIRS} ${aipsfilehandlers_LIBRARY_DIRS} )
LINK_LIBRARIES( vtkIO vtkCommon vtkRendering vtkGraphics vtkftgl vtkPatented vtkpng vtkDICOMParser vtkexpat vtkjpeg vtkHybrid vtkFiltering vtkParallel vtktiff vtkfreetype vtkImaging vtkzlib aipsfilehandlers aipsbase )

INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
LINK_LIBRARIES( aipsbase ${QT_LIBRARIES} )

SET( DEBUG_LEVEL 0 CACHE STRING "Level of debugging output (This is either 0 (no output) or 1,2,3)")

SUBDIRS( src )
