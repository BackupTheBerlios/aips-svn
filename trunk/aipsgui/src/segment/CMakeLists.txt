FILE(GLOB SRC_FILES *.cpp )
FILE(GLOB HDR_FILES *.h )
SET( MOC_INPUT csynergeticmodeldialog.h cdrawerdialog.h csnakedialog.h )
IF( ${DEBUG_LEVEL} GREATER 0 )
SET_SOURCE_FILES_PROPERTIES(
  ${SRC_FILES}
  COMPILE_FLAGS -DDL${DEBUG_LEVEL})
ENDIF( ${DEBUG_LEVEL} GREATER 0 )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/sharedclasses 		
	${PROJECT_SOURCE_DIR}/src/segment )
QT_WRAP_CPP( aipssegmentation QT_MOC_SRCS ${MOC_INPUT})
ADD_LIBRARY( aipssegmentation MODULE ${QT_MOC_SRCS} ${SRC_FILES})
TARGET_LINK_LIBRARIES( aipssegmentation aipssharedclasses )
INSTALL_TARGETS( /lib aipssegmentation)
INSTALL_FILES( /include/aipssegmentation .h ${HDR_FILES} )
