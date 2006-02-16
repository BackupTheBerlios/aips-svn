
PROJECT(meshsegmenter)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc. 
ADD_DEFINITIONS(-Wall -DQT_THREAD_SUPPORT)

SET( DEBUG_LEVEL 0 CACHE STRING "Level of debugging output (This is either 0 (no output) or 1,2,3)")

#FIND_PATH(OPEN_MESH_INCLUDES OpenMesh/Core/Mesh/Types/TriMesh_ArrayKernelT.hh
#	${HOME}/include
#  /usr/include
#  /usr/local/include
#  DOC "Directory containing OpenMesh/Core/Mesh/Types/TriMesh_ArrayKernelT.hh")
#IF( NOT OPEN_MESH_INCLUDES )
# MESSAGE( SEND_ERROR "OpenMesh is needed for compilation" )
#ENDIF( NOT OPEN_MESH_INCLUDES )

# FIND_LIBRARY(OPEN_MESH_LIBRARY
# 	NAMES OpenMesh_Core
# 	PATHS /usr/lib /usr/local/lib ${HOME}/lib
# 	DOC "Path to the OpenMesh library"
# )
# IF( NOT OPEN_MESH_LIBRARY )
#  MESSAGE( SEND_ERROR "OpenMesh is needed for compilation" )
# ENDIF( NOT OPEN_MESH_LIBRARY )

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
INCLUDE_DIRECTORIES( ${VTK_INCLUDE_DIRS} ${ITK_INCLUDE_DIRS} ${aipsbase_INCLUDE_DIRS} ${aipsfilehandlers_INCLUDE_DIRS} ${QT_INCLUDE_DIR} )

LINK_DIRECTORIES( ${VTK_LIBRARY_DIRS} ${ITK_LIBRARY_DIRS} ${aipsbase_LIBRARY_DIRS} ${aipsfilehandlers_LIBRARY_DIRS} )
LINK_LIBRARIES( ${QT_LIBRARIES} vtkCommon vtkIO vtkRendering ITKIO aipsbase aipsfilehandlers )

SET( DEBUG_LEVEL 0 CACHE STRING "Level of debugging output (This is either 0 (no output) or 1,2,3)")

SUBDIRS( src )
