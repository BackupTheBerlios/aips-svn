/************************************************************************
 * File: aipsdatatraits.h                                               *
 * Project: AIPS aipsbase library                                       *
 * Description: Trait classes for commonly used dataset types           *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Alpha                                                       *
 * Created: 2005-01-14                                                  *
 * Changed: 2005-01-26 Added dataTraits for TComplexImage               *
 *                     Adapted dataTraits<TImage> for new type short    *
 *          2005-07-06 Added dataTraits for TInteger, TDouble, TComplex *
 *                      and TStringField                                *
 *          2005-07-07 Added traits for TSingleString                   *
 *                   Added function checkType                           * 
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#ifndef AIPSDATATRAITS_H
#define AIPSDATATRAITS_H

#include<aipsnumeric.h>

namespace aips {

/***********************************************
 * Some really weird but useful typelist stuff *
 ***********************************************/

/** A "Non-Type", especially useful for our typelists */
class NullType
{
};

/** Generic template representing a typelist */
template<typename T, typename U> struct TypeList
{
	typedef T Head;
	typedef U Tail;
};

/** Template metaprogram to compute the number of elements in a typelist */
template<typename TList> struct Length;
template<> struct Length<NullType>
{
	enum { value = 0 };
};
template<typename T, typename U> struct Length< TypeList<T, U> >
{
	enum { value = 1 + Length<U>::value };
};

/** Template metaprogram to return the type of a typelist at the given index*/
template<typename TList, unsigned int index> struct TypeAt;
template<typename Head, typename Tail> struct TypeAt<TypeList<Head, Tail>, 0>
{
	typedef Head Result;
};
template<typename Head, typename Tail, unsigned int index> struct TypeAt<TypeList<Head, Tail>, index>
{
	typedef typename TypeAt<Tail, index - 1>::Result Result;
};


/** Typelist of all supported field types */
typedef TypeList<TImage, TypeList<TField, TypeList<TField2D, TypeList<TField3D, TypeList<TStringField, NullType> > > > > datasetTL;

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
template<typename T> struct dataTraits
{		
};

/** Data traits for ushort datasets */
struct dataTraits<TImage>
{
	typedef short dataType;
	typedef long increasedRangeType;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** Data traits for double datasets */
struct dataTraits<TField>
{
	typedef double dataType;
	typedef double increasedRangeType;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** Data traits for complex datasets */
struct dataTraits<TComplexImage>
{
	typedef std::complex<double> dataType;
	typedef std::complex<double> increasedRangeType;
	enum { isScalar = true };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for 2D vector datasets */
struct dataTraits<TField2D>
{
	typedef TVector2D dataType;
	typedef TVector2D increasedRangeType;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for 3D vector datasets */
template<>
struct dataTraits<TField3D>
{
	typedef TVector3D dataType;
	typedef TVector3D increasedRangeType;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for string array datasets */
struct dataTraits<TStringField>
{
	typedef std::string dataType;
	typedef std::string increasedRangeType;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = false };
};

/** Data traits for scalar integer data */
struct dataTraits<TInteger>
{
	typedef long dataType;
	typedef long increasedRangeType;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** Data traits for scalar double data */
struct dataTraits<TDouble>
{
	typedef double dataType;
	typedef double increasedRangeType;
	enum { isScalar = true };
	enum { isComparable = true };
	enum { isNumeric = true };
};

/** Data traits for scalar complex data */
struct dataTraits<TComplex>
{
	typedef std::complex<double> dataType;
	typedef std::complex<double> increasedRangeType;
	enum { isScalar = true };
	enum { isComparable = false };
	enum { isNumeric = true };
};

/** Data traits for scalar string data */
struct dataTraits<TSingleString>
{
	typedef std::string dataType;
	typedef std::string increasedRangeType;
	enum { isScalar = false };
	enum { isComparable = false };
	enum { isNumeric = false };
};

/**
 * Function to compare field/scalar types with a given dataset type
 * Call with checkType<FieldType>( data )
 * \param aDataSet dataset to check
 */
template<typename T> inline bool checkType( const CDataSet& aDataSet ) throw()
{
	return( aDataSet.getType() == typeid( dataTraits<T>::dataType ) );
}

/**
 * Function to compare field/scalar types with a given dataset type
 * Call with checkType<FieldType>( data )
 * Overloaded function for pointer types
 * \param aDataSet dataset to check
 */
template<typename T> inline bool checkType( const CDataSet* aDataSet ) throw()
{
	return( aDataSet->getType() == typeid( dataTraits<T>::dataType ) );
}

/**
 * Function to compare field/scalar types with a given dataset type
 * Call with checkType<FieldType>( data )
 * Overloaded function for boost::shared_ptr pointer types
 * \param aDataSet dataset to check
 */
template<typename T> inline bool checkType( const boost::shared_ptr<CDataSet> aDataSet ) throw()
{
	return( aDataSet->getType() == typeid( dataTraits<T>::dataType ) );
}

}
#endif
