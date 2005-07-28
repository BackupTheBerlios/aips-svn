PROJECT(aipsfilehandlers)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc. 
ADD_DEFINITIONS(-Wall)

FIND_PACKAGE(aipsbase)
IF(!aipsbase_FOUND)
	MESSAGE( SEND_ERROR "aipsbase library is needed for compilation" )
ENDIF(!aipsbase_FOUND)	
	
OPTION(BUILD_SHARED_LIBS 
	"Build with shared libraries." 
ON)

OPTION(BUILD_DOC
	"Build documentation with Doxygen"
OFF)

SET( DEBUG_LEVEL 0 CACHE STRING "Level of debugging output (This is either 0 (no output) or 1,2,3)")

OPTION(USE_IMAGEMAGICK
	"Use the image magick++ library for handling of standard image formats"
OFF)

OPTION(USE_VTK
	"Use VTK for handling VTK files"
OFF)

INCLUDE( ${aipsbase_USE_FILE} )

#Look for Magick++ library and includes
IF(USE_IMAGEMAGICK)
	FIND_PROGRAM( MAGICKCONFIG_PATH Magick++-config /usr/bin /usr/local/bin )
	IF( !MAGICKCONFIG_PATH )
 		MESSAGE( SEND_ERROR "Magick++-config executable is needed" )
	ENDIF( !MAGICKCONFIG_PATH )
	EXEC_PROGRAM( ${MAGICKCONFIG_PATH} 
		ARGS --cxxflags
		OUTPUT_VARIABLE IM_CXXFLAGS )
	EXEC_PROGRAM( ${MAGICKCONFIG_PATH} 
		ARGS --libs
		OUTPUT_VARIABLE IM_LIBFLAGS )
	EXEC_PROGRAM( ${MAGICKCONFIG_PATH} 
		ARGS --ldflags
		OUTPUT_VARIABLE IM_LDFLAGS )	
	STRING(REGEX REPLACE "-l[a-zA-Z0-9]+ " " " IM_LD ${IM_LDFLAGS} )
	STRING(REGEX REPLACE "-L" " " IM_LD2 ${IM_LD} )
	STRING(REGEX REPLACE "  " " " IM_LD3 ${IM_LD2} )
	
	LINK_DIRECTORIES( /usr/lib /usr/X11R6/lib /usr/lib/graphviz /usr/lib )
	MESSAGE( "<${IM_LD3}>" )
#	STRING(REGEX REPLACE "-l" " " IM_LIB ${IM_LIBFLAGS} )
	LINK_LIBRARIES( ${IM_LIBFLAGS} )	
	SET( CMAKE_CXX_FLAGS "${IM_CXXFLAGS} ${CMAKE_CXX_FLAGS}" )
	SET( HAVE_IM 1 )
ENDIF(USE_IMAGEMAGICK)
#endof Magick++ search

IF(USE_VTK)
	INCLUDE(${CMAKE_ROOT}/Modules/FindVTK.cmake)
	IF(!VTK_FOUND)
		MESSAGE( SEND_ERROR "Cannot find VTK library" )
	ENDIF(!VTK_FOUND)	
	INCLUDE( ${VTK_USE_FILE} )
	SET( HAVE_VTK 1 )
	INCLUDE_DIRECTORIES( ${VTK_INCLUDE_DIRS} )
	LINK_DIRECTORIES( ${VTK_LIBRARY_DIRS} )
	MESSAGE( "<${VTK_LIBRARY_DIRS}>" )
 	LINK_LIBRARIES( vtkIO )	
ENDIF(USE_VTK)

INCLUDE_DIRECTORIES( ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src ) 
CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/aipsfilehandlers_config.h.in 
	${PROJECT_BINARY_DIR}/aipsfilehandlers_config.h)
SUBDIRS( src )

INSTALL_FILES(/include/aipsfilehandlers .h aipsfilehandlers_config.h)

CONFIGURE_FILE( ${aipsfilehandlers_SOURCE_DIR}/aipsfilehandlersConfig.cmake.in 
	${aipsfilehandlers_BINARY_DIR}/aipsfilehandlersConfig.cmake)
CONFIGURE_FILE( ${aipsfilehandlers_SOURCE_DIR}/Doxyfile.in 
	${aipsfilehandlers_SOURCE_DIR}/Doxyfile)
	

INCLUDE(CMakeExportBuildSettings)

CMAKE_EXPORT_BUILD_SETTINGS(
	${aipsfilehandlers_BINARY_DIR}/aipsfilehandlersBuildSettings.cmake
)

EXPORT_LIBRARY_DEPENDENCIES(
	${aipsfilehandlers_BINARY_DIR}/aipsfilehandlersLibraryDepends.cmake
)

INSTALL_FILES(/lib/aipsfilehandlers FILES
	${aipsfilehandlers_SOURCE_DIR}/aipsfilehandlersUse.cmake	
	${aipsfilehandlers_BINARY_DIR}/aipsfilehandlersConfig.cmake
	${aipsfilehandlers_BINARY_DIR}/aipsfilehandlersBuildSettings.cmake
	${aipsfilehandlers_BINARY_DIR}/aipsfilehandlersLibraryDepends.cmake
)

IF(BUILD_DOC)
INCLUDE( ${CMAKE_ROOT}/Modules/FindDoxygen.cmake)
    ADD_CUSTOM_TARGET(api-docs ALL ${DOXYGEN}
	${aipsfilehandlers_SOURCE_DIR}/Doxyfile )
ENDIF(BUILD_DOC)
