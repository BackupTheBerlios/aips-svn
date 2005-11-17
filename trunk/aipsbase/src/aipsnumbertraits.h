/************************************************************************
 * File: aipsnumbertraits.h                                             *
 * Project: AIPS                                                        *
 * Description: Trait classes for common data types                     *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status: Beta                                                         *
 * Created: 2005-11-15                                                  *
 * Changed:                                                             *
 *        2005-11-15 Seperated from aipsdatatraits.h                    *
 *        2005-11-16 Updated naming scheme and documentation            *
 *                   Added TScalarDataType and toScalarType to          *
 *                    non-scalar data types                             *
 *        IMPORTANT These trait classes no longer represent data sets,  *
 *                   but give traits for (alpha)numerical types         *
 *                   directly! Make sure that your code is updated      *
 *                   accordingsly ( dataTraits<CTypedMap<bla> > becomes *
 *                   SDataTraits<bla> or                                *
 *                   SDataTraots<CTypedMap<bla>::TDataType> )           *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef AIPSNUMBERTRAITS_H
#define AIPSNUMBERTRAITS_H
#include <aipsvectordefs.h>
#include <aipsnulltype.h>

namespace aips {

/******************************
 * Trait classes for datasets *
 ******************************/

/** 
 * Unspecialized trait template 
 * The following traits should be defined in any specialization
 * TData (typedef) type used to store the value of a single data member
 * TIncreasedRange (typedef) type used to temporarely store the value of a single data member.
 *   This is usually of bigger range the voxelType
 */
template<typename T> 
struct SDataTraits
{		
	typedef T TDataType; ///< This is the data type of a single element
	typedef NullType TIncreasedRangeType; ///< This is a type to store element data of increased range
	typedef TDataType TScalarDataType; ///< This is the type to convert to after scalarization of more complex types
	/// Conversion from a non-scalar to a scalar type (e.g. vector norm)
	static TScalarDataType toScalarType( const TDataType& data );	
	enum { isScalar = false }; ///< Is our data scalar?
	enum { isComparable = false }; ///< Is our data comparable?
	enum { isNumeric = false }; ///< Is our data numeric?
};

/** Data traits for byte datasets */
template<>
struct SDataTraits<int8_t>
{
	typedef int8_t TDataType; 
	typedef int16_t TIncreasedRangeType;
	static const TDataType ONE;  ///< Representation of one value
	static const TDataType ZERO; ///< Representation of zero value
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** Data traits for short datasets */
template<>
struct SDataTraits<int16_t>
{
	typedef int16_t TDataType; 
	typedef int32_t TIncreasedRangeType;
	static const TDataType ONE;  ///< Representation of one value
	static const TDataType ZERO; ///< Representation of zero value
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** Data traits for scalar integer data */
template<>
struct SDataTraits<int32_t>
{
	typedef int32_t TDataType; 
	typedef int32_t TIncreasedRangeType;
	static const TDataType ONE;  ///< Representation of one value
	static const TDataType ZERO; ///< Representation of zero value
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** Data traits for double datasets */
template<>
struct SDataTraits<TFloatType>
{
	typedef TFloatType TDataType; 
	typedef TFloatType TIncreasedRangeType;
	static const TDataType ONE;  ///< Representation of one value
	static const TDataType ZERO; ///< Representation of zero value
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

#ifdef USE_DOUBLE
template<>
struct SDataTraits<float>
{
	typedef float TDataType; 
	typedef float TIncreasedRangeType;
	static const TDataType ONE;  ///< Representation of one value
	static const TDataType ZERO; ///< Representation of zero value
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};
#else
template<>
struct SDataTraits<double>
{
	typedef double TDataTypeType; 
	typedef double TIncreasedRangeType;
	static const TDataTypeType ONE;  ///< Representation of one value
	static const TDataTypeType ZERO; ///< Representation of zero value
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};
#endif

/** Data traits for complex datasets */
template<>
struct SDataTraits< std::complex<TFloatType> >
{
	typedef std::complex<TFloatType> TDataTypeType; 
	typedef std::complex<TFloatType> TIncreasedRangeType;
	typedef TFloatType TScalarDataType;
	static TScalarDataType toScalarType( const std::complex<TFloatType>& data )
		{ return static_cast<TFloatType>( ( data.real() + data.imag() ) / 2.0 ); }
	static const TDataTypeType ONE;  ///< Representation of one value (number with a modulus == 1.0)
	static const TDataTypeType ZERO; ///< Representation of zero value (norm/modulus of complex number)
	enum { isScalar = true };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for 2D vector datasets */
template<>
struct SDataTraits<TVector2D>
{
	typedef TVector2D TDataTypeType; 
	typedef TVector2D TIncreasedRangeType;
	typedef TFloatType TScalarDataType;
	static TScalarDataType toScalarType( const TVector2D& data ) { return norm( data ); }
	static const TDataTypeType ONE;  ///< Representation of one value (unit vector)
	static const TDataTypeType ZERO; ///< Representation of zero value (zero vector)
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

// template<typename T>
// struct CheckerBoardDistanceFunctor
// {
// 	typename T::T_numtype operator() ( const T& a, const T& b )
// 	{
// 		typename T::T_numtype theResult = static_cast<typename T::T_numtype>( 0.0 );
// 		typename T::const_iterator aend = a.end();
// 		typename T::const_iterator bit = b.begin();
// 		for( typename T::const_iterator ait = a.begin(); ait != aend; ++ait, ++bit )
// 		{
// 			theResult += abs( *ait - *bit );
// 		}
// 		return theResult;
// 	}
// };

/** 
 * Data traits for 2D point datasets 
 * Mind that the scalar type is also an integer value (ceiling of the corresponding Euclidan norm)
 */
template<>
struct SDataTraits<TPoint2D>
{
	typedef TPoint2D TDataTypeType; 
	typedef TPoint2D TIncreasedRangeType;
	typedef TPoint2D::T_numtype TScalarDataType;
	static const TDataTypeType ONE;  ///< Representation of one value (unit vector)
	static const TDataTypeType ZERO; ///< Representation of zero value (zero vector)
	static TScalarDataType toScalarType( const TPoint2D& data )
	{ 
		return static_cast<TScalarDataType>( ceil( norm( data ) ) ); 
	}
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for 3D vector datasets */
template<>
struct SDataTraits<TVector3D>
{
	typedef TVector3D TDataTypeType; 
	typedef TVector3D TIncreasedRangeType;
	typedef TFloatType TScalarDataType;
	static TScalarDataType toScalarType( const TVector3D& data ) 
	{ 
		return static_cast<TScalarDataType>( norm( data ) ); 
	}
	static const TDataTypeType ONE;  ///< Representation of one value (unit vector)
	static const TDataTypeType ZERO; ///< Representation of zero value (zero vector)
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** 
 * Data traits for 3D point datasets 
 * Mind that the scalar type is also an integer value (ceiling of the corresponding Euclidan norm)
 */
template<>
struct SDataTraits<TPoint3D>
{
	typedef TPoint3D TDataTypeType; 
	typedef TPoint3D TIncreasedRangeType;
	typedef TPoint3D::T_numtype TScalarDataType;
	static TScalarDataType toScalarType( const TPoint3D& data ) 
	{ 
		return static_cast<TScalarDataType>( ceil( norm( data ) ) ); 
	}
	static const TDataTypeType ONE;  ///< Representation of one value (unit vector)
	static const TDataTypeType ZERO; ///< Representation of zero value (zero vector)
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for string array datasets */
template<>
struct SDataTraits<std::string>
{
	typedef std::string TDataTypeType; 
	typedef std::string TIncreasedRangeType;
	static const TDataTypeType ONE;  ///< Representation of one value (empty string)
	static const TDataTypeType ZERO; ///< Representation of zero value (empty string)
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = false };
};

}
#endif
