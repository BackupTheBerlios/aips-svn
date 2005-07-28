PROJECT(aipsfilehandlers)

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
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${GSL_INCLUDES} )
LINK_LIBRARIES( aipsbase ${QT_LIBRARIES} ${GSL_LIBRARY} gslcblas m )
LINK_DIRECTORIES( -L/usr/local/lib/vtk )

SET( DEBUG_LEVEL 0 CACHE STRING "Level of debugging output (This is either 0 (no output) or 1,2,3)")

CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/aipsgui_config.h.in 
	${PROJECT_BINARY_DIR}/aipsgui_config.h)
SUBDIRS( src )

SUBDIRS( src/sharedclasses src/diffusionfilters src/edgedetectors src/fieldfilters
	src/histogramfilters src/imagefilters src/neighbourfilters src/segment src/basicconverters 
	src/basicsources src/basictargets src)