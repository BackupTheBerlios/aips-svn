PROJECT(aipsgui)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc. 
ADD_DEFINITIONS(-Wall -DQT_THREAD_SUPPORT -pthread)

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



	INCLUDE(${CMAKE_ROOT}/Modules/FindVTK.cmake)
	IF(!VTK_FOUND)
		MESSAGE( SEND_ERROR "Cannot find VTK library" )
	ENDIF(!VTK_FOUND)	
	INCLUDE( ${VTK_USE_FILE} )
	SET( HAVE_VTK 1 )
	INCLUDE_DIRECTORIES( ${VTK_INCLUDE_DIRS} )
	LINK_DIRECTORIES( ${VTK_LIBRARY_DIRS} )
 	LINK_LIBRARIES( vtkIO )	



	INCLUDE(${CMAKE_ROOT}/Modules/FindITK.cmake)
	IF(!ITK_FOUND)
		MESSAGE( SEND_ERROR "Cannot find ITK library" )
	ENDIF(!ITK_FOUND)	
	INCLUDE( ${ITK_USE_FILE} )
	SET( HAVE_ITK 1 )
	INCLUDE_DIRECTORIES( ${ITK_INCLUDE_DIRS} )
	LINK_DIRECTORIES( ${ITK_LIBRARY_DIRS} )
 	LINK_LIBRARIES( ITKIO )	


INCLUDE( ${aipsbase_USE_FILE} )
INCLUDE( ${aipsfilehandlers_USE_FILE} )
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${GSL_INCLUDES} )
LINK_LIBRARIES( vtkIO ITKIO aipsbase ${QT_LIBRARIES} ${GSL_LIBRARY} gslcblas m )
LINK_DIRECTORIES( ${aipsbase_LIBRARY_DIR} ${aipsfilehandlers_LIBRARY_DIR} ${VTK_LIBRARY_DIRS} ${ITK_LIBRARY_DIRS} )

SET( DEBUG_LEVEL 0 CACHE STRING "Level of debugging output (This is either 0 (no output) or 1,2,3)")

#CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/aipsgui_config.h.in 
#	${PROJECT_BINARY_DIR}/aipsgui_config.h)
#SUBDIRS( src )

SUBDIRS( src/sharedclasses src/diffusionfilters src/edgedetectors src/fieldfilters
	src/histogramfilters src/imagefilters src/neighbourfilters src/segment src/basicconverters 
	src/basicsources src/basictargets src)
