/************************************************************************
 * File: cpipelineiteminlines.h                                         *
 * Project: AIPS                                                        *
 * Description: Definition of inline members of CPipelineitem           *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Created: 30.10.03                                                    *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

/*************
 * Accessors *
 *************/

/**
 * \returns Fanin of item
 */
inline ushort CPipelineItem::getFanIn() const throw()
{
  return usFanIn;
}

/**
 * \returns Fanout of item
 */
inline ushort CPipelineItem::getFanOut() const throw()
{
  return usFanOut;
}

/**
 * \returns the module name
 */
inline const std::string CPipelineItem::getModuleName() const throw()
{
  return sName;
}

/** \returns the documentation string of the item */
inline const std::string CPipelineItem::getDocumentation() const throw()
{
  return sDocumentation;
}

/**
 * \returns the unique processing id of the item
 */
inline ulong CPipelineItem::getID() const throw()
{
  return ulID;
}

/**
 * \returns the numerical represantation of the item type
 */
inline ushort CPipelineItem::getType() const throw()
{
  return itemType;
}

/**
 * \returns pointer to parameter map
 */
inline CTypedMap* CPipelineItem::getParameters() throw()
{
	return &parameters;
}

/** \returns a unique module id */
inline const std::string CPipelineItem::getModuleID() const throw()
{
  return sModuleID;
}

/************
 * Mutators *
 ************/

/**
 * Set the module name
 * \param sNewName the new module name
 */
inline void CPipelineItem::setModuleName( const std::string &sNewName ) throw()
{
  sName = sNewName;
}

/**
 * \param itemType_ the item type
 */
inline void CPipelineItem::setType( const ushort itemType_ ) throw()
{
  itemType = itemType_;
}

/**
 * (call this in the constructor of your derived class)
 * \param sLibID ID string of the plugin library the module resides in
 */
inline void CPipelineItem::setModuleID( const std::string sLibID ) throw()
{
  sModuleID = getClassName() + "/" + getClassVersion() + "/" + sLibID;
}

/************************
 * Dataset IO functions *
 ************************/

/**
 * \param uiPort input port number
 * \returns Input port data set type
 */
inline CPipelineItem::EIOTypes CPipelineItem::getInputType( uint uiPort ) const throw( OutOfRangeException )
{
	if ( uiPort > usFanIn )
		throw( OutOfRangeException( SERROR("Input port number too high"), CException::FATAL, ERR_RANGE ) );
  return inputsVec[uiPort].portType;
}

/**
 * \param uiPort output port number
 * \returns Output port data set type
 */
inline CPipelineItem::EIOTypes CPipelineItem::getOutputType( uint uiPort ) const throw( OutOfRangeException )
{
	if ( uiPort > usFanOut )
		throw( OutOfRangeException( SERROR("Output port number too high"), CException::FATAL, ERR_RANGE ) );
  return outputsVec[uiPort].portType;
}


/*****************
 * Other methods *
 *****************/  

/**
 * \return true if item is ready to transfer
 */
inline bool CPipelineItem::isReady() const throw()
{
  return( bModuleReady );
}

/**
 * \param inputPtr Input to check. This must be of a CDataSet or a type derived from CDataSet
 * \param usMinDim Minimum required dimension for dataset ( 0 == no minimum required )
 * \param usMaxDim Maximum required dimension for dataset ( 0 == no maximum required )
 * \param usMinDataDim Minimum required data dimension for dataset ( 0 == no minimum required )
 * \param usMaxDataDim Maximum required data dimension for dataset ( 0 == no maximum required )
 */
template<typename U, typename T> 
inline bool CPipelineItem::checkInput( T inputPtr, ushort usMinDim, 
	ushort usMaxDim, ushort usMinDataDim, ushort usMaxDataDim ) throw()
{
	if ( !inputPtr )
	{
		alog << LWARN << "Input set error: <No input>" << std::endl; 
		return false;
	}	
	if ( inputPtr->getType() != typeid( typename T::TDataType ) )
	{
		alog << LWARN << "Input set error: <Type mismatch>" << std::endl;
		return false;
	}	
	if ( ( usMinDim && inputPtr->getDimension() < usMinDim ) 
		|| ( usMaxDim && inputPtr->getDimension() > usMaxDim ) )
	{
		alog << LWARN << "Input data has wrong dimension " << inputPtr->getDimension() << ". Legal range is "
			<< usMinDim << " - " << usMaxDim << std::endl; 
		return false;
	}
	if ( ( usMinDataDim && inputPtr->getDataDimension() < usMinDataDim )
		|| ( usMaxDataDim && inputPtr->getDataDimension() > usMaxDataDim ) )
	{
		alog << LWARN << "Input data has wrong dimension " << inputPtr->getDataDimension() << ". Legal range is "
			<< usMinDataDim << " - " << usMaxDataDim << std::endl; 
		return false;
	}
	return true;
}
