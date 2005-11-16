/************************************************************************
 * File: aipsdatatraits.h                                               *
 * Project: AIPS aipsbase library                                       *
 * Description: Trait classes for commonly used dataset types           *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.4                                                         *
 * Status:  Beta                                                        *
 * Created: 2005-01-14                                                  *
 * Changed: 2005-01-26 Added dataTraits for TComplexImage               *
 *                     Adapted dataTraits<TImage> for new type short    *
 *          2005-07-06 Added dataTraits for TInteger, TDouble, TComplex *
 *                      and TStringField                                *
 *          2005-07-07 Added traits for TSingleString                   *
 *                     Added function checkType                         * 
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#ifndef AIPSDATATRAITS_H
#define AIPSDATATRAITS_H

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


}
#endif
