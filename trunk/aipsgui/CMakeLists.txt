PROJECT(aipsgui)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc. 
ADD_DEFINITIONS(-Wall -DQT_THREAD_SUPPORT)

SET( DEBUG_LEVEL 0 CACHE STRING "Level of debugging output (This is either 0 (no output) or 1,2,3)")
SET( USE_RENDERING ON "Use VTK-based rendering for display")

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

#Look for gsl library and includes
FIND_LIBRARY(GSL_LIBRARY
	NAMES gsl
	PATHS /usr/lib /usr/local/lib
	DOC "Path to the gsl library"
)
FIND_PATH(gsl_INCLUDES
  	gsl/gsl_math.h
		/usr/include /usr/local/include
  	DOC "Path where gsl include directory can be found"
	)
IF( NOT GSL_LIBRARY AND GSL_INCLUDES )
	MESSAGE( SEND_ERROR "libgsl and include files are needed" )
ENDIF( NOT GSL_LIBRARY AND GSL_INCLUDES )
#endof gsl search

INCLUDE( ${aipsbase_USE_FILE} )
INCLUDE( ${aipsfilehandlers_USE_FILE} )
INCLUDE_DIRECTORIES( ${VTK_INCLUDE_DIRS} ${ITK_INCLUDE_DIRS} ${QT_INCLUDE_DIR} ${GSL_INCLUDES} )
LINK_LIBRARIES( aipsbase ${QT_LIBRARIES} ${GSL_LIBRARY} pthread gslcblas m vtkCommon vtkIO vtkRendering)
LINK_DIRECTORIES( ${VTK_LIBRARY_DIRS} ${ITK_LIBRARY_DIRS} ${aipsbase_LIBRARY_DIR} ${aipsfilehandlers_LIBRARY_DIR} ) 

SET( DEBUG_LEVEL 0 CACHE STRING "Level of debugging output (This is either 0 (no output) or 1,2,3)")

#CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/aipsgui_config.h.in 
#	${PROJECT_BINARY_DIR}/aipsgui_config.h)
#SUBDIRS( src )

SUBDIRS( src/sharedclasses src/diffusionfilters src/edgedetectors src/fieldfilters
	src/histogramfilters src/imagefilters src/neighbourfilters src/segment src/basicconverters 
	src/basicsources src/basictargets src)
