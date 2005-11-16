//
// C++ Interface: aipsnumbertraits
//
// Description: 
//
//
// Author: Hendrik Belitz <hendrik@darkon.info>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
	typedef T TDataType; //< This is the data type of a single element
	typedef NullType TIncreasedRangeType; //< This is a type to store element data of increased range
	typedef TDataType TScalarDataType; //< This is the type to convert to after scalarization of more complex types
	static TScalarDataType toScalarType( const TDataType& data );
/*	const T One;
	const T Zero;*/
	enum { isScalar = false }; //< Is our data scalar
	enum { isComparable = false }; //< Is our data comparable
	enum { isNumeric = false }; //< Is our data numeric
};

/** Data traits for ushort datasets */
template<>
struct SDataTraits<short>
{
	typedef short TDataType; //< This is the data type of a single element
	typedef long TIncreasedRangeType;
/*	typedef TDataType TScalarDataType; //< This is the type to convert to after scalarization of more complex types
	static TScalarDataType toScalarType( const TDataType& data ) { return data; }*/
	static const TDataType ONE;
	static const TDataType ZERO;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** Data traits for scalar integer data */
template<>
struct SDataTraits<long>
{
	typedef long TDataType; //< This is the data type of a single element
	typedef long TIncreasedRangeType;
// 	typedef TDataType TScalarDataType; //< This is the type to convert to after scalarization of more complex types
// 	static TScalarDataType toScalarType( const TDataType& data ) { return data; }
	static const TDataType ONE;
	static const TDataType ZERO;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** Data traits for double datasets */
template<>
struct SDataTraits<TFloatType>
{
	typedef TFloatType TDataType; //< This is the data type of a single element
	typedef TFloatType TIncreasedRangeType;
/*	typedef TDataType TScalarDataType; //< This is the type to convert to after scalarization of more complex types
	static TScalarDataType toScalarType( const TDataType& data ) { return data; }*/
	static const TDataType ONE;
	static const TDataType ZERO;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

#ifdef USE_DOUBLE
template<>
struct SDataTraits<float>
{
	typedef float TDataType; //< This is the data type of a single element
	typedef float TIncreasedRangeType;
	static const TDataType ONE;
	static const TDataType ZERO;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};
#else
template<>
struct SDataTraits<double>
{
	typedef double TDataTypeType; //< This is the data type of a single element
	typedef double TIncreasedRangeType;
	static const TDataTypeType ONE;
	static const TDataTypeType ZERO;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};
#endif

/** Data traits for complex datasets */
template<>
struct SDataTraits< std::complex<TFloatType> >
{
	typedef std::complex<TFloatType> TDataTypeType; //< This is the data type of a single element
	typedef std::complex<TFloatType> TIncreasedRangeType;
	typedef TFloatType TScalarDataType;
	static TScalarDataType toScalarType( const std::complex<TFloatType>& data )
		{ return static_cast<TFloatType>( ( data.real() + data.imag() ) / 2.0 ); }
	static const TDataTypeType ONE;
	static const TDataTypeType ZERO;
	enum { isScalar = true };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for 2D vector datasets */
template<>
struct SDataTraits<TVector2D>
{
	typedef TVector2D TDataTypeType; //< This is the data type of a single element
	typedef TVector2D TIncreasedRangeType;
	typedef TFloatType TScalarDataType;
	static TScalarDataType toScalarType( const TVector2D& data ) { return norm( data ); }
	static const TDataTypeType ONE;
	static const TDataTypeType ZERO;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for 2D point datasets */
template<>
struct SDataTraits<TPoint2D>
{
	typedef TPoint2D TDataTypeType; //< This is the data type of a single element
	typedef TPoint2D TIncreasedRangeType;
	typedef TFloatType TScalarDataType;
	static TScalarDataType toScalarType( const TPoint2D& data ) { return norm( data ); }
	static const TDataTypeType ONE;
	static const TDataTypeType ZERO;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for 3D vector datasets */
template<>
struct SDataTraits<TVector3D>
{
	typedef TVector3D TDataTypeType; //< This is the data type of a single element
	typedef TVector3D TIncreasedRangeType;
	typedef TFloatType TScalarDataType;
	static TScalarDataType toScalarType( const TVector3D& data ) { return norm( data ); }
	static const TDataTypeType ONE;
	static const TDataTypeType ZERO;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for 3D point datasets */
template<>
struct SDataTraits<TPoint3D>
{
	typedef TPoint3D TDataTypeType; //< This is the data type of a single element
	typedef TPoint3D TIncreasedRangeType;
	typedef TFloatType TScalarDataType;
	static TScalarDataType toScalarType( const TPoint3D& data ) { return norm( data ); }
	static const TDataTypeType ONE;
	static const TDataTypeType ZERO;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for string array datasets */
template<>
struct SDataTraits<std::string>
{
	typedef std::string TDataTypeType; //< This is the data type of a single element
	typedef std::string TIncreasedRangeType;
// 	typedef NullType TScalarDataType;
	static const TDataTypeType ONE;
	static const TDataTypeType ZERO;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = false };
};

}
#endif
