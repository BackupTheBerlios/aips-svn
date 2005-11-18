/************************************************************************
 * File: aipsnumbertraits.h                                             *
 * Project: AIPS                                                        *
 * Description: Trait classes for common data types                     *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status: Beta                                                         *
 * Created: 2005-11-15                                                  *
 * Changed:                                                             *
 *        2005-11-15 Seperated from aipsdatatraits.h                    *
 *        2005-11-16 Updated naming scheme and documentation            *
 *                   Added TScalarDataType and toScalarType to          *
 *                    non-scalar data types                             *
 *        2005-11-18 Updated documentation                              *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef AIPSNUMBERTRAITS_H
#define AIPSNUMBERTRAITS_H
#include <complex>
#include <aipsvectordefs.h>
#include <aipsnulltype.h>
#include <aipsdistancefunctions.h>

namespace aips {

/**
 * \defgroup datatraits Data traits
 * \brief This group contains all data trait classes
 *
 * Data traits are designed after the following pattern
 * \code
 * struct SDataTraits
 * {		
 *	typedef T TDataType;  <-- This is the data type of a single element
 *	typedef NullType TIncreasedRangeType; <-- This is a type to store element data of increased range
 *	typedef TDataType TScalarDataType; <-- This is the type to convert to after scalarization of more complex types
 *	static const TDataType ONE();  <-- Representation of one value
 *	static const TDataType ZERO(); <-- Representation of zero value
 *	enum { isScalar = false }; <-- Is our data scalar?
 *	enum { isComparable = false }; <-- Is our data comparable?
 *	enum { isNumeric = false }; <-- Is our data numeric?
 * }; 
 * \endcode
 */
 
/** 
 * \brief Unspecialized trait template.
 * \ingroup datatraits
 * \attention These trait classes no longer represent data sets, but give traits for (alpha)numerical types         
 * directly! Make sure that your code is updated accordingsly: 
 * \code dataTraits<CTypedMap<foo> > \endcode becomes 
 * \code SDataTraits<foo> \endcode or \code SDataTraots<CTypedMap<foo>::TDataType> \endcode.
 * \todo Tests for specialised and general data traits
 */
template<typename T, typename TDistance = SDefaultDistFunc<T> > 
struct SDataTraits
{		
	typedef T TDataType; ///< Actual data type
	typedef NullType TIncreasedRangeType; ///< Data type to use for out-of-range computations 
	typedef TDataType TScalarDataType; ///< Data type to use for scalar representation of TDataType
	typedef TDistance TDistanceType; ///< Type of distance measurement
	enum { isScalar = false }; ///< Is the type scalar?
	enum { isComparable = false }; ///< Are comparison operators defined for this type?
	enum { isNumeric = false }; ///< Is this a numeric type
};

/** 
 * \ingroup datatraits
 * \brief Data traits for byte datasets 
 *
 * Specialised from SDataTraits.
 */
template<typename TDistance>
struct SDataTraits<int8_t, TDistance>
{
	typedef int8_t TDataType; 
	typedef int16_t TIncreasedRangeType;
	typedef TDistance TDistanceType;
	static const TDataType ONE(){ return 1; }
	static const TDataType ZERO(){ return 0; }
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** 
 * \ingroup datatraits
 * \brief Data traits for short datasets 
 *
 * Specialised from SDataTraits.
 */
template<typename TDistance>
struct SDataTraits<int16_t, TDistance>
{
	typedef int16_t TDataType; 
	typedef int32_t TIncreasedRangeType;
	typedef TDistance TDistanceType;
	static const TDataType ONE(){ return 1; }
	static const TDataType ZERO(){ return 0; }
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** 
 * \ingroup datatraits
 * \brief Data traits for scalar integer data 
 *
 * Specialised from SDataTraits.
 */
template<typename TDistance>
struct SDataTraits<int32_t, TDistance >
{
	typedef int32_t TDataType; 
	typedef int32_t TIncreasedRangeType;
	typedef TDistance TDistanceType;
	static const TDataType ONE(){ return 1; }
	static const TDataType ZERO(){ return 0; }
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** 
 * \ingroup datatraits
 * \brief Data traits for double datasets 
 *
 * Specialised from SDataTraits.
 */
template<typename TDistance>
struct SDataTraits<TFloatType, TDistance >
{
	typedef TFloatType TDataType; 
	typedef TFloatType TIncreasedRangeType;
	typedef TDistance TDistanceType;
	static const TDataType ONE(){ return 1.0; }  
	static const TDataType ZERO(){ return 0.0; } 
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

#ifdef USE_DOUBLE
/** 
 * \ingroup datatraits
 * \brief Data traits for float datasets (used if TFloatType equals double) 
 *
 * Specialised from SDataTraits.
 */
template<typename TDistance>
struct SDataTraits<float, TDistance >
{
	typedef float TDataType; 
	typedef float TIncreasedRangeType;
	typedef TDistance TDistanceType;
	static const TDataType ONE(){ return 1.0; }  
	static const TDataType ZERO(){ return 0.0; } 
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};
#else
/** 
 * \ingroup datatraits
 * \brief Data traits for double datasets (used if TFloatType equals float) 
 *
 * Specialised from SDataTraits.
 */
template<typename TDistance>
struct SDataTraits<double, TDistance >
{
	typedef double TDataType; 
	typedef double TIncreasedRangeType;
	typedef TDistance TDistanceType;
	static const TDataType ONE(){ return 1.0; }  
	static const TDataType ZERO(){ return 0.0; } 
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};
#endif

/** 
 * \ingroup datatraits
 * \brief Data traits for complex datasets 
 *
 * Specialised from SDataTraits.
 * \todo Remove these nasty numerical constants in ONE()
 */
template<typename TDistance>
struct SDataTraits< std::complex<TFloatType>, TDistance >
{
	typedef std::complex<TFloatType> TDataType; 
	typedef std::complex<TFloatType> TIncreasedRangeType;
	typedef TFloatType TScalarDataType;
	typedef TDistance TDistanceType;
	/// Representation of one value (number with a modulus == 1.0)
	static const TDataType ONE(){ return TDataType( 1.414213562373095048763788073031832936977, 1.414213562373095048763788073031832936977 ); }
	/// Representation of zero value (norm/modulus of complex number)
	static const TDataType ZERO(){ return TDataType( 0.0, 0.0 ); }
	enum { isScalar = true };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** 
 * \ingroup datatraits
 * \brief Data traits for 2D vector datasets 
 *
 * Specialised from SDataTraits.
 * \todo Remove these nasty numerical constants in ONE()
 */
template<typename TDistance>
struct SDataTraits<TVector2D, TDistance>
{
	typedef TVector2D TDataType; 
	typedef TVector2D TIncreasedRangeType;
	typedef TFloatType TScalarDataType;
	typedef TDistance TDistanceType;
	/// Representation of one value (unit vector)
	static const TDataType ONE(){ return TVector2D( 1.414213562373095048763788073031832936977 ); };  
	/// Representation of zero value (zero vector)
	static const TDataType ZERO(){ return TVector2D( 0.0 ); }; 
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** 
 * \ingroup datatraits
 * \brief Data traits for 2D point datasets. 
 *
 * Specialised from SDataTraits.
 * Mind that the TScalarType is also an integer value (distances are given using the chessboard distance).
 */
template<typename TDistance>
struct SDataTraits<TPoint2D, TDistance>
{
	typedef TPoint2D TDataType; 
	typedef TPoint2D TIncreasedRangeType;
	typedef TPoint2D::T_numtype TScalarDataType;
	typedef TDistance TDistanceType;
	/// Representation of one value (unit vector)
	static const TDataType ONE(){ return TPoint2D( 1, 0 ); }  
	/// Representation of zero value (zero vector)
	static const TDataType ZERO(){ return TPoint2D( 0, 0 ); } 
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** 
 * \ingroup datatraits 
 * \brief Data traits for 3D vector datasets 
 *
 * Specialised from SDataTraits.
 * \todo Remove these nasty numerical constants in ONE()
 */
template<typename TDistance>
struct SDataTraits<TVector3D, TDistance>
{
	typedef TVector3D TDataTypeType; 
	typedef TVector3D TIncreasedRangeType;
	typedef TFloatType TScalarDataType;
	typedef TDistance TDistanceType;
	/// Representation of one value (unit vector)
	static const TDataTypeType ONE(){ return TVector3D( 1.732050807568877293573725295594556428114 ); }
	/// Representation of zero value (zero vector)
	static const TDataTypeType ZERO(){ return TVector3D( 0.0 ); }
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** 
 * \ingroup datatraits
 * \brief Data traits for 3D point datasets 
 *
 * Specialised from SDataTraits.
 * Mind that the TScalarType is also an integer value (distances are given using the chessboard distance).
 */
template<typename TDistance>
struct SDataTraits<TPoint3D, TDistance>
{
	typedef TPoint3D TDataType; 
	typedef TPoint3D TIncreasedRangeType;
	typedef TPoint3D::T_numtype TScalarDataType;
	typedef TDistance TDistanceType;
	/// Representation of one value (unit vector)
	static const TDataType ONE(){ return TPoint3D( 1, 0, 0 ); } 
	/// Representation of zero value (zero vector)
	static const TDataType ZERO(){ return TPoint3D( 0, 0, 0 ); } 	
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** 
 * \ingroup datatraits
 * \brief Data traits for string array datasets 
 *
 * Specialised from SDataTraits
 */
template<typename TDistance>
struct SDataTraits<std::string, TDistance>
{
	typedef std::string TDataType; 
	typedef std::string TIncreasedRangeType;
	typedef TDistance TDistanceType;
	/// Representation of one value (empty string)
	static const TDataType ONE(){ return ""; };  
	/// Representation of zero value (empty string)	
	static const TDataType ZERO(){ return ""; }; 
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = false };
};

}
#endif
