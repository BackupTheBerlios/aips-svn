FILE(GLOB SRC_FILES *.cpp )
FILE(GLOB HDR_FILES *.h )
SET( MOC_INPUT cviewhistogramdialog.h cimagewriterdialog.h cdisplaydialog.h)

IF( ${DEBUG_LEVEL} GREATER 0 )
SET_SOURCE_FILES_PROPERTIES(
  ${SRC_FILES}
  COMPILE_FLAGS -DDL${DEBUG_LEVEL})
ENDIF( ${DEBUG_LEVEL} GREATER 0 )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/sharedclasses 
	${PROJECT_SOURCE_DIR}/src/basictargets )
QT_WRAP_CPP(aipsbasictargets QT_MOC_SRCS ${MOC_INPUT})
ADD_LIBRARY(aipsbasictargets MODULE ${SRC_FILES} ${QT_MOC_SRCS} )
TARGET_LINK_LIBRARIES( aipsbasictargets aipssharedclasses )
INSTALL_TARGETS(/lib aipsbasictargets)
INSTALL_FILES(/include/aipsbasictargets .h ${HDR_FILES} )