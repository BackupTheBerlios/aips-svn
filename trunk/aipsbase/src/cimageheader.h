/************************************************************************
 * File: cimageheader.h                                                 *
 * Project: AIPS aipsbase library                                       *
 * Description: A header class for image file headers                   *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Version: 0.5                                                         *
 * Status:  Alpha                                                       *
 * Created: 2004-04-27                                                  *
 * Changed: 2004-04-28 Updated documentation                            *
 *          2004-11-23 Removed getVoxelSize. Added getVoxelType         *
 *          2004-12-07 Added abstract load and save methods             *
 *          2005-07-07 Added functions for VoxelDimensions and Origin   *
 * TODO: Support for images with 5D+ dimensions                         *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef AIPSCIMAGEHEADER_H
#define AIPSCIMAGEHEADER_H

// Standard includes
#include <vector>  // std::vector
#include <istream> // std::istream
#include <ostream> // std::ostream

// AIPS includes
#include <ctypedmap.h>

namespace aips {

/**
 * A header class for image file headers
 * This class just provides some convenience methods for data members
 * which are important for nearly all image headers
 */
class CImageHeader : public CTypedMap
{
public:
/* Structors */
	/// Constructor
  CImageHeader() 
		throw();
	/// Destructor
	~CImageHeader() 
		throw();
/* Accessors */
	/// Return "ExtendX" and, if applicable, "ExtendY" "ExtendZ" and "ExtendT"
	std::vector<size_t> getExtents() const
		throw();
	/// Return "FileEndianess"
	bool getEndianess() const
		throw();
	/// Return "VoxelType"
	const std::string getVoxelType() const
		throw();
	/// Return "OriginX", and, if applicable, values for the other coordinates
	const std::vector<double> getOrigin() const
		throw();
	/// Return "VoxelDimensionX", and, if applicable, values for the other coordinates
	const std::vector<double> getVoxelDimensions() const
		throw();
/* Mutators */		
	/// Set "ExtendX", and ,if applicable, "ExtendY" "ExtentZ" and "ExtendT" from the given vector
	void setExtents( const std::vector<size_t>& extentVec ) 
		throw();
	/// Set "FileEndianess"
	void setEndianess( bool bEndianess ) 
		throw();
	/// Set "VoxelType"
	void setVoxelType( const std::string& sVoxelType )		
		throw();
	/// Set "Origin[XYZT]" of image space. Image extents need to be set before calling this function.
	void setOrigin( double dX = 0.0, double dY = 0.0, double dZ = 0.0, double dT = 0.0 )
		throw();
	/// Set "VoxelDimension[XYZT]" (in mm/s). Image extents need to be set before calling this function.
	void setVoxelDimensions( double dX = 1.0, double dY = 1.0, double dZ = 1.0, double dT = 1.0 )
		throw();		
/* Other methods */		
	/** 
	 * Loads the header from an open XML stream
	 * \param theFile an open istream
   * \throws FileException on any file error
   */
	virtual void loadHeader( std::istream& theFile )
		throw( FileException ) {}    
	/** 
	 * Saves the header to an open XML stream
   * \param theFile an open ostream
   * \throws FileException on any file error
   */	
	virtual void saveHeader( std::ostream& theFile ) 
		throw( FileException ) {};
};

};

#endif
