FILE(GLOB SRC_FILES *.cpp )
FILE(GLOB HDR_FILES *.h )

IF( ${DEBUG_LEVEL} GREATER 0 )
SET_SOURCE_FILES_PROPERTIES(
  ${SRC_FILES}
  COMPILE_FLAGS -DDL${DEBUG_LEVEL})
ENDIF( ${DEBUG_LEVEL} GREATER 0 )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/sharedclasses 		
	${PROJECT_SOURCE_DIR}/src/histogramfilters )
ADD_LIBRARY( aipshistogramfilters MODULE ${SRC_FILES})
TARGET_LINK_LIBRARIES( aipshistogramfilters aipssharedclasses )
INSTALL_TARGETS(/lib aipshistogramfilters)
INSTALL_FILES(/include/aipshistogramfilters .h ${HDR_FILES} )
