FILE(GLOB SRC_FILES *.cpp )

FILE(GLOB INSTALL_HEADERS *.h)

FILE(GLOB INSTALL_TEMPLATES *.tpp)

IF( ${DEBUG_LEVEL} GREATER 0 )
SET_SOURCE_FILES_PROPERTIES(
  ${SRC_FILES}
  COMPILE_FLAGS -DDL${DEBUG_LEVEL})
ENDIF( ${DEBUG_LEVEL} GREATER 0 )

INSTALL_FILES(/include/aipsfilters .h ${INSTALL_HEADERS})
INSTALL_FILES(/include/aipsfilters .tpp ${INSTALL_TEMPLATES})

ADD_LIBRARY(aipsfilters ${SRC_FILES})
INSTALL_TARGETS(/lib aipsfilters)
