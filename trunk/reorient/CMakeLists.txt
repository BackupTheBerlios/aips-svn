PROJECT(reorient)

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

INCLUDE(${CMAKE_ROOT}/Modules/FindITK.cmake)
IF(!ITK_FOUND)
  MESSAGE( SEND_ERROR "Cannot find ITK library" )
ENDIF(!ITK_FOUND) 
INCLUDE( ${ITK_USE_FILE} )
INCLUDE_DIRECTORIES( ${ITK_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${ITK_LIBRARY_DIRS} )
LINK_LIBRARIES( ITKCommon ITKIO )

FILE(GLOB SRC_FILES *.cpp )
ADD_EXECUTABLE(reorient ${SRC_FILES})
INSTALL_TARGETS(/bin reorient)
