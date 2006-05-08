PROJECT(aipsfilters)

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
	SET( HAVE_VTK 1 )
 	LINK_LIBRARIES( vtkCommon vtkIO )	
ENDIF(aipsbase_USE_VTK)

IF(aipsbase_USE_ITK)
	SET( HAVE_ITK 1 )
 	LINK_LIBRARIES( ITKIO )	
ENDIF(aipsbase_USE_ITK)

LINK_LIBRARIES( aipsbase pthread )

INCLUDE_DIRECTORIES( ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src ) 
CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/aipsfilters_config.h.in 
	${PROJECT_BINARY_DIR}/aipsfilters_config.h)
SUBDIRS( src )

INSTALL_FILES(/include/aipsfilters .h aipsfilters_config.h)

CONFIGURE_FILE( ${aipsfilters_SOURCE_DIR}/aipsfiltersConfig.cmake.in 
	${aipsfilters_BINARY_DIR}/aipsfiltersConfig.cmake)
CONFIGURE_FILE( ${aipsfilters_SOURCE_DIR}/Doxyfile.in 
	${aipsfilters_SOURCE_DIR}/Doxyfile)
	
INCLUDE(CMakeExportBuildSettings)

CMAKE_EXPORT_BUILD_SETTINGS(
	${aipsfilters_BINARY_DIR}/aipsfiltersBuildSettings.cmake
)

EXPORT_LIBRARY_DEPENDENCIES(
	${aipsfilters_BINARY_DIR}/aipsfiltersLibraryDepends.cmake
)

INSTALL_FILES(/lib/aipsfilters FILES
	${aipsfilters_SOURCE_DIR}/aipsfiltersUse.cmake	
	${aipsfilters_BINARY_DIR}/aipsfiltersConfig.cmake
	${aipsfilters_BINARY_DIR}/aipsfiltersBuildSettings.cmake
	${aipsfilters_BINARY_DIR}/aipsfiltersLibraryDepends.cmake
)

IF(BUILD_DOC)
INCLUDE( ${CMAKE_ROOT}/Modules/FindDoxygen.cmake)
    ADD_CUSTOM_TARGET(api-docs ALL ${DOXYGEN}
	${aipsfilters_SOURCE_DIR}/Doxyfile )
ENDIF(BUILD_DOC)
