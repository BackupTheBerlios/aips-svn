PROJECT(aipsbase)

# Include some usefil CMake modules 
INCLUDE(${CMAKE_ROOT}/Modules/CMakeDetermineCXXCompiler.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CMakeDetermineSystem.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckTypeSize.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CMakeSystemSpecificInformation.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CMakeTestCXXCompiler.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/TestForSTDNamespace.cmake)
# Include FindZLIB, since we need this for the GZStream class
INCLUDE(${CMAKE_ROOT}/Modules/FindZLIB.cmake)

# Always give as much information as possible when compiling
ADD_DEFINITIONS(-Wall)
SET(CMAKE_VERBOSE_MAKEFILE ON)

# Some basic options for the user

OPTION(BENCHMARK "Allow timers for benchmarking" OFF)

OPTION(USE_DOUBLE "Wether to use double or float as the standard floating point type (double should be preferred)" ON )

OPTION(USE_BLITZ "Use the blitz++ library for numerical computations (recommended)" ON )

OPTION(USE_XMLPP "Support loading of XML files (strongly recommended)" ON )

OPTION(USE_ITK "Compile ITK adapter" OFF )

SET( DEBUG_LEVEL 0 CACHE STRING "Level of debugging output (This is either 0 (no output) or 1,2,3)" )

OPTION(BUILD_SHARED_LIBS "Build with shared libraries" ON )

OPTION(BUILD_DOC "Build documentation with Doxygen (takes very long!)"	OFF )

# --- Checking for mandatory libs ---

# We need to link with libz.so and set the appropiate include path
LINK_LIBRARIES( ${ZLIB_LIBRARIES} )
INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )

#BEGIN Checking for components of the boost libarries
FIND_PATH(BOOST_LEXICAL_CAST boost/lexical_cast.hpp
  /usr/include
  /usr/local/include
        DOC "Directory containing lexical_cast.hpp")
IF( NOT BOOST_LEXICAL_CAST )
 MESSAGE( SEND_ERROR "boost::lexical_cast<> is mandatory for installation" )
ENDIF( NOT BOOST_LEXICAL_CAST )

FIND_PATH(BOOST_SHARED_PTR boost/shared_ptr.hpp
  /usr/include
  /usr/local/include
        DOC "Directory containing shared_ptr.hpp")
IF( NOT BOOST_SHARED_PTR )
 MESSAGE( SEND_ERROR "boost::shared_ptr<> is mandatory for installation" )
ENDIF( NOT BOOST_SHARED_PTR )

FIND_PATH(BOOST_WEAK_PTR boost/weak_ptr.hpp
  /usr/include
  /usr/local/include
        DOC "Directory containing weak_ptr.hpp")
IF( NOT BOOST_WEAK_PTR )
 MESSAGE( SEND_ERROR "boost::weak_ptr<> is mandatory for installation" )
ENDIF( NOT BOOST_WEAK_PTR )

FIND_PATH(BOOST_TIMER boost/timer.hpp
  /usr/include
  /usr/local/include
        DOC "Directory containing timer.hpp")
IF( NOT BOOST_TIMER )
 MESSAGE( SEND_ERROR "boost::timer is mandatory for installation" )
ENDIF( NOT BOOST_TIMER )

FIND_PATH(BOOST_OPTIONAL boost/optional/optional.hpp
  /usr/include
  /usr/local/include
        DOC "Directory containing optional.hpp")
IF( NOT BOOST_OPTIONAL )
 MESSAGE( SEND_ERROR "boost::optional<> is mandatory for installation" )
ENDIF( NOT BOOST_OPTIONAL )

INCLUDE_DIRECTORIES( ${BOOST_LEXICAL_CAST} ${BOOST_SHARED_PTR} ${BOOST_WEAK_PTR} ${BOOST_TIMER} ${BOOST_OPTIONAL} )
SET( AIPSBASE_EXTRA_INCLUDES ${BOOST_LEXICAL_CAST} ${BOOST_SHARED_PTR} ${BOOST_WEAK_PTR} ${BOOST_TIMER} ${BOOST_OPTIONAL} )
#END Checking for components of the boost libarries

#BEGIN Look for blitz++ library and includes
IF(USE_BLITZ)
	FIND_LIBRARY(BLITZ_LIBRARY
		NAMES blitz
		PATHS /usr/lib/ /usr/local/lib/
		DOC "Path to the blitz++ library"
	)
	FIND_PATH(BLITZ_INCLUDES 
  	blitz.h 
		/usr/include/blitz /usr/local/include/blitz
  	DOC "Path where blitz++ include files can be found"
	)
	IF( NOT BLITZ_LIBRARY AND BLITZ_INCLUDES )
 		MESSAGE( SEND_ERROR "libblitz and include files are needed" )
	ENDIF( NOT BLITZ_LIBRARY AND BLITZ_INCLUDES )
	LINK_LIBRARIES( ${BLITZ_LIBRARY} -lm )
	INCLUDE_DIRECTORIES( ${BLITZ_INCLUDES} )
	IF ( BLITZ_INCLUDES )
		SET( AIPSBASE_EXTRA_INCLUDES ${AIPSBASE_EXTRA_INCLUDES} ${BLITZ_INCLUDES} )
	ENDIF( BLITZ_INCLUDES )
ELSE(USE_BLITZ)
  SET( USE_STANDALONE ON )
ENDIF(USE_BLITZ)
#END Look for blitz++ library and includes

#BEGIN Look for the libxml++ library, its dependencies and includes
IF(USE_XMLPP)
	FIND_LIBRARY(XMLPP_LIBRARY
		NAMES xml++-2.6 xml++
		PATHS /usr/lib /usr/local/lib
		DOC "Path to the libxml++-2.6 library"
	)
	FIND_LIBRARY(XML2_LIBRARY
		NAMES xml2
		PATHS /usr/lib /usr/local/lib
		DOC "Path to the libxml2 library"
	)
	FIND_LIBRARY(GLIBMM_LIBRARY
		NAMES glibmm-2.4 glibmm 
		PATHS /usr/lib /usr/local/lib
		DOC "Path to the glibmm-2.4 library"
	)
	FIND_PATH(XML2_INCLUDES 
  	SAX.h 
		/usr/include/libxml2/libxml /usr/local/include/libxml2/libxml
  	DOC "Path where libxml2 include files can be found"
	)
	FIND_PATH(XMLPP_INCLUDES 
  	libxml++/libxml++.h 
		/usr/include/libxml++-2.6 /usr/local/include/libxml++-2.6
  	DOC "Path where libxml++-2.6 include files can be found"
	)
	FIND_PATH(GLIBMM_INCLUDES 
  	glibmm/arrayhandle.h
		/usr/include/glibmm-2.4 /usr/local/include/glibmm-2.4
  	DOC "Path where glibmm-2.4 include files can be found"
	)
	FIND_PATH(GLIBMM_CONFIG_INCLUDES 
  	glibmmconfig.h   	
		/usr/lib/glibmm-2.4/include /usr/local/lib/glibmm-2.4/include
	)	
	FIND_PATH(GLIB_INCLUDES
		glib/gmacros.h
		/usr/include/glib-2.0 /usr/local/include/glib-2.0
	)
	FIND_PATH(GLIB_CONFIG_INCLUDES
		glibconfig.h
		/usr/lib/glib-2.0/include /usr/local/lib/glib-2.0/include
	)
	IF( NOT GLIB_INCLUDES AND GLIB_CONFIG_INCLUDES )
		MESSAGE( SEND_ERROR "Could not find glib include files" )
	ENDIF( NOT GLIB_INCLUDES AND GLIB_CONFIG_INCLUDES )
	IF( NOT GLIBMM_LIBRARY AND GLIBMM_CONFIG_INCLUDES AND GLIBMM_INCLUDES )
 		MESSAGE( SEND_ERROR "Could not find glibmm-2.4 or its include files" )
	ENDIF( NOT GLIBMM_LIBRARY AND GLIBMM_CONFIG_INCLUDES AND GLIBMM_INCLUDES )
	IF( NOT XMLPP_INCLUDES AND XMLPP_LIBRARY )
 		MESSAGE( SEND_ERROR "Could not find libxml++-2.6 or its include files" )
	ENDIF( NOT XMLPP_INCLUDES AND XMLPP_LIBRARY )
	IF( NOT XML2_INCLUDES AND XML2_LIBRARY )
 		MESSAGE( SEND_ERROR "Could not find libxml2 or its include files" )
	ENDIF( NOT XML2_INCLUDES AND XML2_LIBRARY )
	LINK_LIBRARIES(	${XMLPP_LIBRARY} ${XML2_LIBRARY} ${GLIBMM_LIBRARY} -lm -lpthread )
	INCLUDE_DIRECTORIES( ${XMLPP_INCLUDES} ${XML2_INCLUDES} ${GLIBMM_INCLUDES}
		${GLIBMM_CONFIG_INCLUDES} ${GLIB_INCLUDES} ${GLIB_CONFIG_INCLUDES} )
	IF ( XMLPP_INCLUDES )
		SET( AIPSBASE_EXTRA_INCLUDES ${AIPSBASE_EXTRA_INCLUDES} ${XMLPP_INCLUDES} )
	ENDIF( XMLPP_INCLUDES )
	IF ( XML2_INCLUDES )
		SET( AIPSBASE_EXTRA_INCLUDES ${AIPSBASE_EXTRA_INCLUDES} ${XML2_INCLUDES} )
	ENDIF( XML2_INCLUDES )
	IF ( GLIBMM_INCLUDES )
		SET( AIPSBASE_EXTRA_INCLUDES ${AIPSBASE_EXTRA_INCLUDES} ${GLIBMM_INCLUDES} )
	ENDIF( GLIBMM_INCLUDES )
	IF ( GLIBMM_CONFIG_INCLUDES )
		SET( AIPSBASE_EXTRA_INCLUDES ${AIPSBASE_EXTRA_INCLUDES} ${GLIBMM_CONFIG_INCLUDES} )
	ENDIF( GLIBMM_CONFIG_INCLUDES )
	IF ( GLIB_INCLUDES )
		SET( AIPSBASE_EXTRA_INCLUDES ${AIPSBASE_EXTRA_INCLUDES} ${GLIB_INCLUDES} )
	ENDIF( GLIB_INCLUDES )
	IF ( GLIB_CONFIG_INCLUDES )
		SET( AIPSBASE_EXTRA_INCLUDES ${AIPSBASE_EXTRA_INCLUDES} ${GLIB_CONFIG_INCLUDES} )
	ENDIF( GLIB_CONFIG_INCLUDES )
ENDIF(USE_XMLPP)
#END Look for the libxml++ library, its dependencies and includes

IF(USE_ITK)
  INCLUDE(${CMAKE_ROOT}/Modules/FindITK.cmake)
  IF(!ITK_FOUND)
    MESSAGE( SEND_ERROR "Cannot find ITK library" )
  ENDIF(!ITK_FOUND) 
  INCLUDE( ${ITK_USE_FILE} )
  SET( HAVE_ITK 1 )
  INCLUDE_DIRECTORIES( ${ITK_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${ITK_LIBRARY_DIRS} )
  LINK_LIBRARIES( ITKCommon )
ENDIF(USE_ITK)

INCLUDE_DIRECTORIES( ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src )

INSTALL_FILES(/include/aipsbase .h aipsbase_config.h)

CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/aipsbase_config.h.in 
	${PROJECT_BINARY_DIR}/aipsbase_config.h)

CONFIGURE_FILE( ${aipsbase_SOURCE_DIR}/aipsbaseConfig.cmake.in 
	${aipsbase_BINARY_DIR}/aipsbaseConfig.cmake)

CONFIGURE_FILE( ${aipsbase_SOURCE_DIR}/Doxyfile.in 
	${aipsbase_SOURCE_DIR}/Doxyfile)
	
INCLUDE(CMakeExportBuildSettings)

CMAKE_EXPORT_BUILD_SETTINGS(
	${aipsbase_BINARY_DIR}/aipsbaseBuildSettings.cmake
)

EXPORT_LIBRARY_DEPENDENCIES(
	${aipsbase_BINARY_DIR}/aipsbaseLibraryDepends.cmake
)

INSTALL_FILES(/lib/aipsbase FILES
	${aipsbase_SOURCE_DIR}/aipsbaseUse.cmake	
	${aipsbase_BINARY_DIR}/aipsbaseConfig.cmake
	${aipsbase_BINARY_DIR}/aipsbaseBuildSettings.cmake
	${aipsbase_BINARY_DIR}/aipsbaseLibraryDepends.cmake
)

IF(BUILD_DOC)
INCLUDE( ${CMAKE_ROOT}/Modules/FindDoxygen.cmake)
    ADD_CUSTOM_TARGET(api-docs ALL ${DOXYGEN}
	${aipsbase_SOURCE_DIR}/Doxyfile )
ENDIF(BUILD_DOC)

SUBDIRS( src )
