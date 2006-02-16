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

# OPTION(USE_IMAGEMAGICK
# 	"Use the image magick++ library for handling of standard image formats"
# OFF)
# 
# OPTION(USE_VTK
# 	"Use VTK for handling VTK files"
# OFF)
# 
# OPTION(USE_ITK
# 	"Use ITK for medical image IO"
# OFF)

INCLUDE( ${aipsbase_USE_FILE} )



#Look for Magick++ library and includes
IF(aipsbase_USE_IM)
  INCLUDE( FindImageMagickLib.cmake )
  INCLUDE_DIRECTORIES( ${IMAGEMAGICKLIB_INCLUDE_DIR} )
  LINK_LIBRARIES( ${IMAGEMAGICKLIB_CXX_LIBRARY} ${IMAGEMAGICKLIB_C_LIBRARY} ${IMAGEMAGICKLIB_X11_LIBRARY} )
	SET( HAVE_IM 1 )
ENDIF(aipsbase_USE_IM)
#endof Magick++ search

IF(aipsbase_USE_VTK)
# 	INCLUDE(${CMAKE_ROOT}/Modules/FindVTK.cmake)
# 	IF(!VTK_FOUND)
# 		MESSAGE( SEND_ERROR "Cannot find VTK library" )
# 	ENDIF(!VTK_FOUND)	
# 	INCLUDE( ${VTK_USE_FILE} )
	SET( HAVE_VTK 1 )
# 	INCLUDE_DIRECTORIES( ${VTK_INCLUDE_DIRS} )
# 	LINK_DIRECTORIES( ${VTK_LIBRARY_DIRS} )
 	LINK_LIBRARIES( vtkCommon vtkIO )	
ENDIF(aipsbase_USE_VTK)

IF(aipsbase_USE_ITK)
# 	INCLUDE(${CMAKE_ROOT}/Modules/FindITK.cmake)
# 	IF(!ITK_FOUND)
# 		MESSAGE( SEND_ERROR "Cannot find ITK library" )
# 	ENDIF(!ITK_FOUND)	
# 	INCLUDE( ${ITK_USE_FILE} )
	SET( HAVE_ITK 1 )
# 	INCLUDE_DIRECTORIES( ${ITK_INCLUDE_DIRS} )
# 	LINK_DIRECTORIES( ${ITK_LIBRARY_DIRS} )
 	LINK_LIBRARIES( ITKIO )	
ENDIF(aipsbase_USE_ITK)

LINK_LIBRARIES( aipsbase pthread )

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
