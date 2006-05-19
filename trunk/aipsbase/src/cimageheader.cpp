/************************************************************************
 * File: cimageheader.cpp                                               *
 * Project: AIPS aipsbase library                                       *
 * Description: A header class for image file headers                   *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Created: 2004-04-27                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#include "cimageheader.h"

namespace aips {

/*************
 * Structors *
 *************/
 
CImageHeader::CImageHeader() throw() : CTypedMap()
{
	// Initialize a minimum of header entries
	setBool( "FileEndianess", false );
	setString( "VoxelType", "uint8" );
	setUnsignedLong( "ExtentX", 1 );
	setDouble( "OriginX", 0.0 );
	setDouble( "VoxelDimensionX", 1.0 );
}


CImageHeader::~CImageHeader() throw()
{
}

/*************
 * Accessors *
 *************/

/**
 * \returns "ExtentX", "ExtentY", "ExtentZ" and "ExtentT", if defined
 * \throws NotPresentException if key "ExtentX" doesn't exist
 */
std::vector<size_t> CImageHeader::getExtents() const throw()
{
	std::vector<size_t> extentVec;
	extentVec.push_back( getUnsignedLong( "ExtentX" ) );
	if ( isDefined( "ExtentY" ) )
		extentVec.push_back( getUnsignedLong( "ExtentY" ) );
	if ( isDefined( "ExtentZ" ) )
		extentVec.push_back( getUnsignedLong( "ExtentZ" ) );
	if ( isDefined( "ExtentT" ) )
		extentVec.push_back( getUnsignedLong( "ExtentT" ) );
	return extentVec;
}

/**
 * \returns "FileEndianess"  
 * \throws NotPresentException if key "FileEndianess" doesn't exist
 */
bool CImageHeader::getEndianess() const throw() 
{
	return getBool( "FileEndianess" );
}

/**
 * \returns "VoxelType"  
 * \throws NotPresentException if key "VoxelType"  doesn't exist
 */
const std::string CImageHeader::getVoxelType() const throw()
{
	return getString( "VoxelType" );
}

/**
 * \returns "Origin"  
 * \throws NotPresentException if key "VoxelType"  doesn't exist
 */
const std::vector<double> CImageHeader::getOrigin() const	throw()
{
	std::vector<double> ret;
	if ( isDefined( "OriginX" ) )
		ret.push_back( getDouble( "OriginX" ) );
	if ( isDefined( "OriginY" ) )
		ret.push_back( getDouble( "OriginY" ) );
	if ( isDefined( "OriginZ" ) )
		ret.push_back( getDouble( "OriginZ" ) );
	if ( isDefined( "OriginT" ) )
		ret.push_back( getDouble( "OriginT" ) );
	return ret;
}

/**
 * \returns "VoxelDimensions"  
 * \throws NotPresentException if key "VoxelType"  doesn't exist
 */
const std::vector<double> CImageHeader::getVoxelDimensions() const throw()
{
	std::vector<double> ret;
	if ( isDefined( "VoxelDimensionX" ) )
		ret.push_back( getDouble( "VoxelDimensionX" ) );
	if ( isDefined( "VoxelDimensionY" ) )
		ret.push_back( getDouble( "VoxelDimensionY" ) );
	if ( isDefined( "VoxelDimensionZ" ) )
		ret.push_back( getDouble( "VoxelDimensionZ" ) );
	if ( isDefined( "VoxelDimensionT" ) )
		ret.push_back( getDouble( "VoxelDimensionT" ) );
	return ret;	
}

/************
 * Mutators *
 ************/
 
/**
 * \param extentVec vector of up to four size_t which defines the image dimensions
 */
void CImageHeader::setExtents( const std::vector<size_t>& extentVec ) throw()
{
	if ( extentVec.empty() )
		return;
	setUnsignedLong( "ExtentX", extentVec[0] );
	if ( extentVec.size() > 1 )
	{
		setUnsignedLong( "ExtentY", extentVec[1] );
		if ( extentVec.size() > 2 )
		{
			setUnsignedLong( "ExtentZ", extentVec[2] );
			if ( extentVec.size() > 3 )
			{
				setUnsignedLong( "ExtentT", extentVec[3] );				
				if ( extentVec.size() > 4 )
					alog << LWARN << "Currently we only support images with up to four dimensions!\n" 
						<< " Higher dimensions will be ignored" << std::endl;
			}
		}
	}
}

/**
 * \param bEndianess value to store
 */
void CImageHeader::setEndianess( bool bEndianess ) throw()
{
	setBool( "FileEndianess", bEndianess );
}
	
/**
 * \param sVoxelType voxel data type. 
 * This could be either "Intx", "UIntx" with x=8,16,32 or "Floaty" with y=16,32,64
 */
void CImageHeader::setVoxelType( const std::string& sVoxelType ) throw()
{
	setString( "VoxelType", sVoxelType );
}

/**
 * \param dX x-coordinate of origin
 * \param dY y-coordinate of origin
 * \param dZ z-coordinate of origin
 */
void CImageHeader::setOrigin( double dX, double dY, double dZ, double dT ) throw()
{
	setDouble( "OriginX", dX );
	if ( isDefined( "ExtentY" ) )
		setDouble( "OriginY", dY );
	if ( isDefined( "ExtentZ" ) )
		setDouble( "OriginZ", dZ );
	if ( isDefined( "ExtentT" ) )
		setDouble( "OriginT", dT );
}

/**
 * \param dX x-extent of a voxel
 * \param dY y-extent of a voxel
 * \param dZ z-extent of a voxel
 */
void CImageHeader::setVoxelDimensions( double dX, double dY, double dZ, double dT ) throw()
{
	setDouble( "VoxelDimensionX", dX );
	if ( isDefined( "ExtentY" ) )
		setDouble( "VoxelDimensionY", dY );
	if ( isDefined( "ExtentZ" ) )
		setDouble( "VoxelDimensionZ", dZ );
	if ( isDefined( "ExtentT" ) )
		setDouble( "VoxelDimensionT", dT );
}

};
