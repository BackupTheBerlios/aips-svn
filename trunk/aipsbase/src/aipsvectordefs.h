/************************************************************************
 * File: aipsvectordefs.h                                               *
 * Project: aipsbase                                                    *
 * Description: Vector type definitions                                 *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status: Beta                                                         *
 * Created: 2005-11-15                                                  *
 * Changed:                                                             *
 *          2005-11-15 Seperated from aipsnumeric.h                     *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#ifndef AIPSVECTORDEF_H
#define AIPSVECTORDEF_H

#include <aipsbase_config.h>
#ifdef USE_BLITZ
// Blitz includes
#include <blitz/tinyvec.h>       // blitz::TinyVector
#include <blitz/tinyvec-et.h>    // blitz::TinyVector mathematical operators
#include <blitz/array/storage.h> // blitz::GeneralArrayStorage
#else
// Use standalone vector class 
#include "cvector.h"
#endif

#ifdef USE_DOUBLE
typedef double TFloatType;
#else
typedef float TFloatType;
#endif

namespace aips {

#ifdef USE_BLITZ
/* Typedefs for vectors if AIPS get compiled with Blitz++ support */
typedef blitz::TinyVector<TFloatType,2> TVector2D; ///< Typedef for a mathematical 2D vector
typedef blitz::TinyVector<TFloatType,3> TVector3D; ///< Typedef for a mathematical 3D vector
typedef blitz::TinyVector<int,2>    		TPoint2D;  ///< Typedef for a discrete 2D point
typedef blitz::TinyVector<int,3>    		TPoint3D;  ///< Typedef for a discrete 3D point
#else
/* Typedefs for vectors if AIPS get compiled without Blitz++ support */
typedef CVector<TFloatType,2> TVector2D; ///< Typedef for a mathematical 2D vector
typedef CVector<TFloatType,3> TVector3D; ///< Typedef for a mathematical 3D vector
typedef CVector<int,2>    		TPoint2D;  ///< Typedef for discrete 2D point
typedef CVector<int,3>	   		TPoint3D;  ///< Typedef for discrete 3D point

/// Norm function for data types with defined dot() function
template<typename T> inline double norm( const T& aVector ) 
	throw();
#endif
	
/// Quadratic norm function for data types with defined dot() function.
template<typename T> inline double quadNorm( const T& aVector ) 
	throw();
	
/// Outer product for 3D vectors
inline TVector3D vecProd( const TVector3D& leftVector, const TVector3D& rightVector ) 
	throw();

#include <aipsvectordefsinlines.tpp>

}
#endif
