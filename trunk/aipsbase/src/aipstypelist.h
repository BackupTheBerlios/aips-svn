/************************************************************************
 * File: aipstypelist.h                                                 *
 * Project: aipsbase library                                            *
 * Description: Type list definitions to ease templated module dev.     *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Version: 0.5                                                         *
 * Status:  Beta                                                        *
 * Created: 2005-01-14                                                  *
 * Changed: 2005-01-26 Added dataTraits for TComplexImage               *
 *                     Adapted dataTraits<TImage> for new type short    *
 *          2005-07-06 Added dataTraits for TInteger, TDouble, TComplex *
 *                      and TStringField                                *
 *          2005-07-07 Added traits for TSingleString                   *
 *                     Added function checkType                         * 
 *          2005-11-15 Moved traits to seperate file                    *
 *          2005-11-17 Renamed file to aipstypelist.h                   *
 *          2006-04-04 Added some image/field-specific type lists       *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#ifndef AIPSTYPELIST_H
#define AIPSTYPELIST_H

#include<aipsnulltype.h>
#include<aipsnumeric.h>

namespace aips {

/***********************************************
 * Some really weird but useful typelist stuff *
 ***********************************************/

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
typedef TypeList<TImage, TypeList<TComplexImage, TypeList<TField, TypeList<TField2D, TypeList<TField3D, TypeList<TStringField, NullType> > > > > > datasetTL;

/** Typelist of all scalar integer field types (= integer images) */
typedef TypeList<TImage, TypeList<TSmallImage, NullType> > intimageTL;

/** Typelist of all scalar field types (= images) */
typedef TypeList<TImage, TypeList<TField, TypeList<TSmallImage, NullType> > > imageTL;

/** Typelist of all vector field types (= vector fields) */
typedef TypeList<TField2D, TypeList<TField3D, NullType> > vectorfieldTL;

}
#endif
