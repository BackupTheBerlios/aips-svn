FILE(GLOB SRC_FILES *.cpp )
FILE(GLOB HDR_FILES *.h )
SET( MOC_INPUT cimagedisplay.h vtkQtRenderWindow.h vtkQtRenderWindowInteractor.h )

IF( ${DEBUG_LEVEL} GREATER 0 )
SET_SOURCE_FILES_PROPERTIES(
  ${SRC_FILES}
  COMPILE_FLAGS -DDL${DEBUG_LEVEL})
ENDIF( ${DEBUG_LEVEL} GREATER 0 )

#INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/src/sharedclasses )
QT_WRAP_CPP(aipsbasictargets QT_MOC_SRCS ${MOC_INPUT})
ADD_LIBRARY(aipssharedclasses STATIC ${SRC_FILES} ${QT_MOC_SRCS})
INSTALL_FILES(/lib .a libaipssharedclasses)
INSTALL_FILES(/include/aipssharedclasses .h ${HDR_FILES})
