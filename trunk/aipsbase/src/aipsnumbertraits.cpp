//
// C++ Implementation: aipsnumbertraits
//
// Description: 
//
//
// Author: Hendrik Belitz <hendrik@darkon.info>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <aipsnumbertraits.h>

using namespace aips;

const short SDataTraits<short>::ONE = 1;
const short SDataTraits<short>::ZERO = 0;

const TFloatType SDataTraits<TFloatType>::ONE = 1.0;
const TFloatType SDataTraits<TFloatType>::ZERO = 0.0;

#ifdef USE_DOUBLE
const float SDataTraits<float>::ONE = 1.0;
const float SDataTraits<float>::ZERO = 0.0;
#else
const double SDataTraits<double>::ONE = 1.0;
const double SDataTraits<double>::ZERO = 0.0;
#endif

const long SDataTraits<long>::ONE = 1;
const long SDataTraits<long>::ZERO = 0;

const std::string SDataTraits<std::string>::ONE = "";
const std::string SDataTraits<std::string>::ZERO = "";

const std::complex<TFloatType> SDataTraits<std::complex<TFloatType> >::ONE
	( 1.414213562373095048763788073031832936977, 1.414213562373095048763788073031832936977 );
const std::complex<TFloatType> SDataTraits<std::complex<TFloatType> >::ZERO
	( 0.0, 0.0 );

const TVector2D SDataTraits<TVector2D>::ONE ( 1.414213562373095048763788073031832936977 );
const TVector2D SDataTraits<TVector2D>::ZERO ( 0.0 );

const TVector3D SDataTraits<TVector3D>::ONE ( 1.732050807568877293573725295594556428114 );
const TVector3D SDataTraits<TVector3D>::ZERO ( 0.0 );

const TPoint2D SDataTraits<TPoint2D>::ONE ( 1, 1 );
const TPoint2D SDataTraits<TPoint2D>::ZERO ( 0, 0 );

const TPoint3D SDataTraits<TPoint3D>::ONE ( 1, 1, 1 );
const TPoint3D SDataTraits<TPoint3D>::ZERO ( 0, 0, 0 );

