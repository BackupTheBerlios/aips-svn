PROJECT(evaltool)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc. 
ADD_DEFINITIONS(-Wall)

FIND_PACKAGE(aipsbase)
IF(!aipsbase_FOUND)
	MESSAGE( SEND_ERROR "aipsbase library is needed for compilation" )
ENDIF(!aipsbase_FOUND)	

FIND_PACKAGE(aipsfilehandlers)
IF(!aipsfilehandlers_FOUND)
	MESSAGE( SEND_ERROR "aipsfilehandlers library is needed for compilation" )
ENDIF(!aipsfilehandlers_FOUND)	

FIND_PATH(BOOST_PROGRAM_OPTIONS boost/program_options.hpp
  /usr/include
  /usr/local/include
        DOC "Directory containing boost/program_options.hpp")
IF( NOT BOOST_PROGRAM_OPTIONS )
 MESSAGE( SEND_ERROR "boost::program_options namespace is mandatory for compilation" )
ENDIF( NOT BOOST_PROGRAM_OPTIONS )
FIND_LIBRARY( BOOST_PROGRAM_OPTIONS_LIB
    NAMES boost_program_options boost_program_options-gcc
    PATHS /usr/lib/ /usr/local/lib/
    DOC "Path to the boost::program_options library")
IF( NOT BOOST_PROGRAM_OPTIONS_LIB )
 MESSAGE( SEND_ERROR "boost::program_options library is mandatory for compilation" )
ENDIF( NOT BOOST_PROGRAM_OPTIONS_LIB )
    
INCLUDE( ${aipsbase_USE_FILE} )
INCLUDE( ${aipsfilehandlers_USE_FILE} )
LINK_LIBRARIES( -laipsbase -laipsfilehandlers ${BOOST_PROGRAM_OPTIONS_LIB} )
LINK_DIRECTORIES( -L/usr/local/lib/vtk -L/usr/X11R6/lib )

SET( DEBUG_LEVEL 0 CACHE STRING "Level of debugging output (This is either 0 (no output) or 1,2,3)")

SUBDIRS( src )
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "evaltool - evaluation metrics for quantitative assessment of ip algorithms")
SET(CPACK_PACKAGE_VENDOR "Forschungszentrum Jülich")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "9")
SET(CPACK_PACKAGE_VERSION_PATCH "1")
SET(CPACK_PACKAGE_EXECUTABLES "evaltool" "evaltool")
INCLUDE(CPack)

