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
 * dataType (typedef) type used to store the value of a single data member
 * increasedRangeType (typedef) type used to temporarely store the value of a single data member.
 *   This is usually of bigger range the voxelType
 */
template<typename T> 
struct dataTraits
{		
	typedef T dataType; //< This is the data type of a single element
	typedef NullType increasedRangeType; //< This is a type to store element data of increased range
	typedef dataType scalarDataType; //< This is the type to convert to after scalarization of more complex types
	static scalarDataType toScalarType( const dataType& data );
/*	const T One;
	const T Zero;*/
	enum { isScalar = false }; //< Is our data scalar
	enum { isComparable = false }; //< Is our data comparable
	enum { isNumeric = false }; //< Is our data numeric
};

/** Data traits for ushort datasets */
template<>
struct dataTraits<short>
{
	typedef short dataType; //< This is the data type of a single element
	typedef long increasedRangeType;
/*	typedef dataType scalarDataType; //< This is the type to convert to after scalarization of more complex types
	static scalarDataType toScalarType( const dataType& data ) { return data; }*/
	static const dataType ONE;
	static const dataType ZERO;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** Data traits for scalar integer data */
template<>
struct dataTraits<long>
{
	typedef long dataType; //< This is the data type of a single element
	typedef long increasedRangeType;
// 	typedef dataType scalarDataType; //< This is the type to convert to after scalarization of more complex types
// 	static scalarDataType toScalarType( const dataType& data ) { return data; }
	static const dataType ONE;
	static const dataType ZERO;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** Data traits for double datasets */
template<>
struct dataTraits<TFloatType>
{
	typedef TFloatType dataType; //< This is the data type of a single element
	typedef TFloatType increasedRangeType;
/*	typedef dataType scalarDataType; //< This is the type to convert to after scalarization of more complex types
	static scalarDataType toScalarType( const dataType& data ) { return data; }*/
	static const dataType ONE;
	static const dataType ZERO;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

#ifdef USE_DOUBLE
template<>
struct dataTraits<float>
{
	typedef float dataType; //< This is the data type of a single element
	typedef float increasedRangeType;
	static const dataType ONE;
	static const dataType ZERO;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};
#else
template<>
struct dataTraits<double>
{
	typedef double dataType; //< This is the data type of a single element
	typedef double increasedRangeType;
	static const dataType ONE;
	static const dataType ZERO;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};
#endif

/** Data traits for complex datasets */
template<>
struct dataTraits< std::complex<TFloatType> >
{
	typedef std::complex<TFloatType> dataType; //< This is the data type of a single element
	typedef std::complex<TFloatType> increasedRangeType;
	typedef TFloatType scalarDataType;
	static scalarDataType toScalarType( const std::complex<TFloatType>& data ) 
		{ return static_cast<TFloatType>( ( data.real() + data.imag() ) / 2.0 ); }
	static const dataType ONE;
	static const dataType ZERO;
	enum { isScalar = true };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for 2D vector datasets */
template<>
struct dataTraits<TVector2D>
{
	typedef TVector2D dataType; //< This is the data type of a single element
	typedef TVector2D increasedRangeType;
	typedef TFloatType scalarDataType;
	static scalarDataType toScalarType( const TVector2D& data ) { return norm( data ); }
	static const dataType ONE;
	static const dataType ZERO;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for 2D point datasets */
template<>
struct dataTraits<TPoint2D>
{
	typedef TPoint2D dataType; //< This is the data type of a single element
	typedef TPoint2D increasedRangeType;
	typedef TFloatType scalarDataType;
	static scalarDataType toScalarType( const TPoint2D& data ) { return norm( data ); }
	static const dataType ONE;
	static const dataType ZERO;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for 3D vector datasets */
template<>
struct dataTraits<TVector3D>
{
	typedef TVector3D dataType; //< This is the data type of a single element
	typedef TVector3D increasedRangeType;
	typedef TFloatType scalarDataType;
	static scalarDataType toScalarType( const TVector3D& data ) { return norm( data ); }
	static const dataType ONE;
	static const dataType ZERO;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for 3D point datasets */
template<>
struct dataTraits<TPoint3D>
{
	typedef TPoint3D dataType; //< This is the data type of a single element
	typedef TPoint3D increasedRangeType;
	typedef TFloatType scalarDataType;
	static scalarDataType toScalarType( const TPoint3D& data ) { return norm( data ); }
	static const dataType ONE;
	static const dataType ZERO;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for string array datasets */
template<>
struct dataTraits<std::string>
{
	typedef std::string dataType; //< This is the data type of a single element
	typedef std::string increasedRangeType;
// 	typedef NullType scalarDataType;
	static const dataType ONE;
	static const dataType ZERO;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = false };
};

}
#endif
