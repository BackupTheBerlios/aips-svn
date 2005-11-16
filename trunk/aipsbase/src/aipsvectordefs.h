//
// C++ Interface: aipsvectordefs
//
// Description: 
//
//
// Author: Hendrik Belitz <hendrik@darkon.info>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
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

/**
 * Definition of our own storage format. Using x as the first index AND the fastest
 * varying index is more useful when working with images. Except for the base flag
 * this is identical to a FortranArray of Blitz++
 */
template<int N_rank>
class AIPSArray : public blitz::GeneralArrayStorage<N_rank>
{
private:
  typedef blitz::GeneralArrayStorage<N_rank> T_base;
  typedef _bz_typename T_base::noInitializeFlag noInitializeFlag;
  using T_base::ordering_;
  using T_base::ascendingFlag_;
  using T_base::base_;
public:
  AIPSArray()
    : blitz::GeneralArrayStorage<N_rank>( noInitializeFlag() )
  {
    for (int i=0; i < N_rank; ++i)
      ordering_(i) = i;
    ascendingFlag_ = true;
    base_ = 0;
  }
};
#else
/* Typedefs for vectors if AIPS get compiled without Blitz++ support */
typedef CVector<TFloatType,2> TVector2D; ///< Typedef for a mathematical 2D vector
typedef CVector<TFloatType,3> TVector3D; ///< Typedef for a mathematical 3D vector
typedef CVector<int,2>    		TPoint2D;  ///< Typedef for discrete 2D point
typedef CVector<int,3>	   		TPoint3D;  ///< Typedef for discrete 3D point
#endif

#ifdef USE_STANDALONE
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

#include <aipsvectordefs.tpp>

}
#endif
