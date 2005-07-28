/************************************************************************
 * File: cdragpipeitem.cpp                                              *
 * Project: AIPS                                                        *
 * Description: A drag and drop item storing pipeline items             *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 10.09.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cdragpipeitem.h"

/**
 * Constructor
 * \param itemPtr_ corresponding pipeline module
 * \param parentPtr Qt parent widget
 * \param sName Qt object name
 */
CDragPipeItem::CDragPipeItem( CPipelineItem* itemPtr_, ulong moduleID, ulong libID, ulong typeID,
	QWidget* parentPtr, const char *sName )
  throw() : QStoredDrag( "obj/pipeitem", parentPtr, sName), itemPtr( itemPtr_ ), 
		moduleid( moduleID ), libid( libID ), typeId( typeID )
{  
}

/** Destructor */
CDragPipeItem::~CDragPipeItem() throw()
{
}

/**
 * Reimplemented from QStoredDrag
 * Decodes a given mime source back into the address of a pipeline module
 * \param mimeSourcePtr source to decode
 * \param ulAddress memory address of retrieved structure
 */
bool CDragPipeItem::decode ( const QMimeSource* mineSourcePtr, ulong &ulAddress, ulong &moduleID,
ulong &libID, ulong &typeID )
{
  if ( QString( mineSourcePtr->format() ) == "obj/pipeitem" )
  {
    QByteArray tmpArr = mineSourcePtr->encodedData( "obj/pipeitem" );
    QDataStream is ( tmpArr, IO_ReadOnly );
    is >> ulAddress >> moduleID >> libID >> typeID;
    return true;
  }
  return false;
}

/**
 * Reimplemented from QStoredDrag
 * \param int not used
 * \returns the mime source format of interpretable objects
 */
const char* CDragPipeItem::format ( int ) const
{
  return "obj/pipeitem";
}

/**
 * Reimplemented from QStoredDrag
 * \param const char* not used
 * \returns byte array which contains the adress of the corresponding module
 */
QByteArray CDragPipeItem::encodedData( const char* ) const
{
  QByteArray tmpArr;
  QDataStream os( tmpArr, IO_WriteOnly );  
  os << reinterpret_cast<ulong>( itemPtr ) << moduleid << libid << typeId;
  return tmpArr;
}
