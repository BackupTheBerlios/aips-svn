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

const short dataTraits<short>::ONE = 1;
const short dataTraits<short>::ZERO = 0;

const TFloatType dataTraits<TFloatType>::ONE = 1.0;
const TFloatType dataTraits<TFloatType>::ZERO = 0.0;

#ifdef USE_DOUBLE
const float dataTraits<float>::ONE = 1.0;
const float dataTraits<float>::ZERO = 0.0;
#else
const double dataTraits<double>::ONE = 1.0;
const double dataTraits<double>::ZERO = 0.0;
#endif

const long dataTraits<long>::ONE = 1;
const long dataTraits<long>::ZERO = 0;

const std::string dataTraits<std::string>::ONE = "";
const std::string dataTraits<std::string>::ZERO = "";

const std::complex<TFloatType> dataTraits<std::complex<TFloatType> >::ONE 
	( 1.414213562373095048763788073031832936977, 1.414213562373095048763788073031832936977 );
const std::complex<TFloatType> dataTraits<std::complex<TFloatType> >::ZERO 
	( 0.0, 0.0 );

const TVector2D dataTraits<TVector2D>::ONE ( 1.414213562373095048763788073031832936977 );
const TVector2D dataTraits<TVector2D>::ZERO ( 0.0 );

const TVector3D dataTraits<TVector3D>::ONE ( 1.732050807568877293573725295594556428114 );
const TVector3D dataTraits<TVector3D>::ZERO ( 0.0 );

const TPoint2D dataTraits<TPoint2D>::ONE ( 1, 1 );
const TPoint2D dataTraits<TPoint2D>::ZERO ( 0, 0 );

const TPoint3D dataTraits<TPoint3D>::ONE ( 1, 1, 1 );
const TPoint3D dataTraits<TPoint3D>::ZERO ( 0, 0, 0 );

