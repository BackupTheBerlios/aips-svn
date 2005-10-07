/************************************************************************
 * File: ctypeddata.h                                                   *
 * Project: AIPS                                                        *
 * Description: A dataset representing an array of a specific type      *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.10                                                        *
 * Status : Beta                                                        *
 * Created: 2003-11-25                                                  *
 * Changed: 2003-11-27 Added class CSingleValue for zerodimensional     *
 *                      values like scalars and vectors                 *
 *                     Template methods are now seperated from          *
 *                      declarations                                    *
 *                     CDataSet is now derived from CBase               *
 *          2004-01-16 Corrected an error in CTypedData::operator=      *
 *                      This member also is no longer virtual           *
 *                     Moved CVector classes to aipsnumeric.h           *
 *          2004-01-20 Virtual methods and structors are no longer      *
 *                      inline                                          *
 *                     Made the code simpler and look prettier          *
 *                     Deleted method convert() from CTypedData<>       *
 *                     File splitted up into the three files            *
 *                      cdataset.h csinglevalue.h and ctypeddata.h      *
 *          2004-01-21 Implemented random access iterator               *
 *          2004-02-12 Operator= and copy constructor now also copy     *
 *                     the minimum and maximum value                    *
 *          2004-04-20 Added method swap()                              *
 *                     Added method getDataSize()                       *
 *                     Corrected documentation of getArraySize()        *
 *          2001-05-01 Added new iterator class to CTypedData which     *
 *                     offers some convenience methods for easy access  *
 *                     Tested CTypedData for about 5 hours ...          *
 *          2004-08-12 Added resize(), increaseDataDimension() and      *
 *                     decreaseDataDimension() members                  *
 *          2005-05-23 Added constructor for 1D-datasets                *
 *                     Added methods setDataRange() and                 *
 *                      adjustDataRange()                               *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CTYPEDDATA_H
#define CTYPEDDATA_H

#define CTYPEDDATA_VERSION "0.10"

// Standard includes
#include <algorithm> // std::swap

// AIPS includes
#include "cdataset.h"

namespace aips {

/**
 * A dataset representing a multidimensional array of a specific type
 */
template<typename valueType>
class CTypedData : public CDataSet
{
private:
	/// Standard constructor
	CTypedData();
public:
	/// Enumeration types
	enum EDataAlign { DataAlignFront = 0, DataAlignCenter, DataAlignBack };
/* Structors */
  /// Constructor
  CTypedData( const ushort usDimension_, const size_t* extendArr_,
    const size_t dataDimensionSize_ = 1) throw();
  /// Constructor
  CTypedData( const ushort usDimension_, const std::vector<size_t> extendVec_,
    const size_t dataDimensionSize_ = 1) throw();
  /// Constructor
  CTypedData( const size_t extent_, const size_t dataDimensionSize_ = 1) throw();
  /// Copy constructor
  CTypedData( const CTypedData<valueType>& otherDataSet )
    throw();
  /// Destructor
  virtual ~CTypedData()
    throw();
/* Operators (non-access) */
  /// Operator=
  inline CTypedData<valueType>& operator=( const CTypedData<valueType> &otherDataSet )
    throw();
  /// Operator= to set all field values to a new default value
  inline CTypedData<valueType>& operator=( const valueType newDefault )
    throw();
/* Accessors */
  /// Returns the data type of the stored elements
  virtual const std::type_info& getType() const
    throw();
  /// Constant access operator with range checking
  inline const valueType& get( const ushort usX, const ushort usY, const ushort usZ,
    const ushort usW ) const throw(OutOfRangeException);
  /// Returns a typed handle to the data array
  inline valueType* getArray( ushort usChannel = 0 )
    throw( OutOfRangeException );
  /// Returns a void handle to the data array
  virtual void* getVoidArray() throw();
  /// Returns the size of the internal Array (no. of elements)
  inline ulong getArraySize() const
    throw();
	/// Returns the size of the data block (in bytes)
	inline ulong getDataSize() const
		throw();
  /// Returns the field minimum value
  inline const valueType& getMinimum() const
    throw();
  /// Returns the field maximum value
  inline const valueType& getMaximum() const
    throw();
/* Mutators */
  /// Access operator with range checking and automatic min/max assignment
  inline valueType& set( const ushort usX, const ushort usY, const ushort usZ, const ushort usW,
    const valueType newValue ) throw(OutOfRangeException);
  /// Sets a new minimum value for the field
  inline void setMinimum( const valueType newMinimum )
    throw();
  /// Sets a new maximum value for the field
  inline void setMaximum( const valueType newMaximum )
    throw();
	/// Set a new minimum and maximum at once
  inline void setDataRange( const valueType newMinimum, const valueType newMaximum )
    throw();
  /// Adjust range so that the given value lies definitely in data range
  inline void adjustDataRange( const valueType theValue )
    throw();  
	/// Resizes the data set. This won't change the array dimension nor the data dimension
	inline void resize( const size_t* extendArr_, const EDataAlign alignment = DataAlignFront ) 
		throw();
	/// Resizes the data set. This won't change the array dimension nor the data dimension
  inline void resize( const std::vector<size_t> extendVec_,
		const EDataAlign alignment = DataAlignFront )	throw();
	/// Increases the data dimension by the given amount
	inline void increaseDataDimension( const size_t addToDataDimension ) 
		throw();
	/// Decreases the data dimension by the given amount
	inline void decreaseDataDimension( const size_t subFromDataDimension ) 
		throw();
/* Access operators */
  /// Access operator without range checking (1D)
  inline valueType& operator()( const ushort usX )
    throw();
  /// Access operator without range checking (2D)
  inline valueType& operator()( const ushort usX, const ushort usY )
    throw();
  /// Access operator without range checking (3D)
  inline valueType& operator()( const ushort usX, const ushort usY, const ushort usZ )
    throw();
  /// Access operator without range checking (4D)
  inline valueType& operator()( const ushort usX, const ushort usY, const ushort usZ,
    const ushort usW ) throw();
  /// Constant access operator without range checking (1D)
  inline const valueType& operator()( const ushort usX ) const
    throw();
  /// Constant access operator without range checking (2D)
  inline const valueType& operator()( const ushort usX, const ushort usY ) const
    throw();
  /// Constant access operator without range checking (3D)
  inline const valueType& operator()( const ushort usX, const ushort usY, const ushort usZ )
    const throw();
  /// Constant access operator without range checking (4D)
  inline const valueType& operator()( const ushort usX, const ushort usY,
    const ushort usZ, const ushort usW ) const
    throw();
	/// Operator[] for C-Style array access to the dataset
  valueType& operator[]( const ulong ulIndex ) throw();
  /// Const access operator[] for C-Style array access to the dataset 
  const valueType& operator[]( const ulong ulIndex ) const throw();
/* Iterators */
	/** An random iterator for CTypedData */
	template<typename T, typename U > class TypedDataIterator 
		: public std::iterator<std::random_access_iterator_tag, T, U>
	{
	public:
	/* Structors */
		/** 
		 * Constructor for internal usage 
		 * \param initialPositionPtr element to point to
		 * \param parentPtr_ data structure of iterator
		 */
		TypedDataIterator( T* initialPositionPtr, CTypedData<T>* parentPtr_ ) throw()
		{ 
			positionPtr = initialPositionPtr; 
			parentPtr = parentPtr_; 
		}
		/** Constructor */
		TypedDataIterator() throw()
		{ 
			positionPtr = NULL; 
			parentPtr = NULL; 
		}
		/** Destructor */
		~TypedDataIterator() throw()
		{
		}
		/** 
		 * Copy constructor 
		 * \param otherIterator iterator to copy from
		 */
		TypedDataIterator( const TypedDataIterator& otherIterator ) throw()
		{ 
			positionPtr = otherIterator.positionPtr; 
			parentPtr = otherIterator.parentPtr; 
		}
	/* Operators / Assignment and pointer-like behaviour */
		/** 
		 * Assignment operator 
 		 * \param otherIterator iterator to copy from
		 */
		TypedDataIterator& operator=( const TypedDataIterator& otherIterator ) throw()
		{ 
			if ( &otherIterator == this )
				return *this;
			positionPtr = otherIterator.positionPtr; 
			parentPtr = otherIterator.parentPtr; 
			return *this; 
		}
		/** Derefercing operator */
		T& operator*() throw()
		{ 
			return (*positionPtr); 
		}
		/** Member operator */
		T* operator->() throw()
		{ 
			return positionPtr; 
		}
	/* Operators / Comparison */
		/** 
		 * Equality operator 
		 * \param otherIterator iterator to compare with
		 */
		bool operator==( const TypedDataIterator<T,U>& otherIterator ) throw()
		{ 
			return (positionPtr == otherIterator.positionPtr); 
		}
		/** 
		 * Non-Equality operator 
		 * \param otherIterator iterator to compare with
		 */
		bool operator!=( const TypedDataIterator<T,U>& otherIterator ) throw()
		{ 
			return (positionPtr != otherIterator.positionPtr); 
		}
		/** 
		 * Less than operator 
		 * \param otherIterator iterator to compare with
		 */
		bool operator<( const TypedDataIterator<T,U>& otherIterator ) throw()
		{ 
			return (positionPtr < otherIterator.positionPtr); 
		}
		/** 
		 * Greater than operator 
		 * \param otherIterator iterator to compare with
		 */
		bool operator>( const TypedDataIterator<T,U>& otherIterator ) throw()
		{ 
			return (positionPtr > otherIterator.positionPtr); 
		}
		/** 
		 * Less or equal than operator 
		 * \param otherIterator iterator to compare with
		 */
		bool operator<=( const TypedDataIterator<T,U>& otherIterator ) throw()
		{ 
			return (positionPtr <= otherIterator.positionPtr); 
		}
		/** 
		 * Greater or equal than operator 
		 * \param otherIterator iterator to compare with
		 */
		bool operator>=( const TypedDataIterator<T,U>& otherIterator ) throw()
		{ 
			return (positionPtr >= otherIterator.positionPtr); 
		}
	/* Operators to mimic pointer arithmetic */
		/** Increment operator (prefix) */
		TypedDataIterator<T,U>& operator++() throw()
		{ 
			++positionPtr; 
			return *this; 
		}
		/** Increment operator (postfix) */
		TypedDataIterator<T,U> operator++( int ) throw()
		{ 
			return TypedDataIterator<T,U>( positionPtr++, parentPtr ); 
		}
		/** Decrement operator (prefix) */
		TypedDataIterator<T,U>& operator--() throw()
		{ 
			--positionPtr; 
			return *this; 
		}
		/** Decrement operator (postfix) */
		TypedDataIterator<T,U> operator--( int ) throw()
		{ 
			return TypedDataIterator<T,U>( positionPtr--, parentPtr ); 
		}
		/** 
		 * Operator += 
		 * \param distance number of steps to move
		 */
		TypedDataIterator<T,U>& operator+=( ptrdiff_t distance ) throw()
		{ 
			positionPtr += distance; 
			return *this;
		}
		/** 
		 * Operator -= 
		 * \param distance number of steps to move
		 */		
		TypedDataIterator<T,U>& operator-=( ptrdiff_t distance ) throw()
		{ 
			positionPtr -= distance; 
			return *this;
		}
		/**
		 * Moves the iterator relative to its actual position
		 * \param sX steps to move in X direction
		 */
		void moveRel( const short sX ) throw()
		{ 
			positionPtr += sX; 
		}
		/**
		 * Moves the iterator relative to its actual position
		 * \param sX steps to move in X direction
		 * \param sY steps to move in Y direction
		 */		
		void moveRel( const short sX, const short sY ) throw()
		{ 
			positionPtr += sX + parentPtr->getExtent(0) * sY; 
		}
		/**
		 * Moves the iterator relative to its actual position
		 * \param sX steps to move in X direction
		 * \param sY steps to move in Y direction
		 * \param sZ steps to move in Z direction
		 */
		void moveRel( const short sX, const short sY,	const short sZ ) throw()
		{ 
			positionPtr += sX + parentPtr->getExtent(0) * sY 
				+ parentPtr->getExtent(0) * parentPtr->getExtent(1) * sZ; 
		}
		/**
		 * Moves the iterator relative to its actual position
		 * \param sX steps to move in X direction
		 * \param sY steps to move in Y direction
		 * \param sZ steps to move in Z direction
		 * \param sW steps to move in W direction
		 */
		void moveRel( const short sX, const short sY,	const short sZ, const short sW ) throw()
		{ 
			positionPtr += sX + parentPtr->getExtent(0) * sY 
				+ parentPtr->getExtent(0) * parentPtr->getExtent(1) * sZ
				+ parentPtr->getExtent(0) * parentPtr->getExtent(1) * parentPtr->getExtent(2) * sW; 
		}
		/**
		 * Returns the actual iterator position in image coordinates
		 * \returns the actual iterator position in image coordinates
		 * \param usX X position
		 */
		void getPos( ushort& usX ) const throw()
		{ 
			usX = ( positionPtr - parentPtr->begin().pos ); 
		}
		/**
		 * Returns the actual iterator position in image coordinates
		 * \returns the actual iterator position in image coordinates
		 * \param usX X position
		 * \param usY Y position
		 */
		void getPos( ushort& usX, ushort& usY ) const throw()
		{ 
			ptrdiff_t diff = ( positionPtr - parentPtr->begin().positionPtr );
			usY = diff / parentPtr->getExtent(0);
			usX = diff % parentPtr->getExtent(0); 
		}
		/**
		 * Returns the actual iterator position in image coordinates
		 * \returns the actual iterator position in image coordinates
		 * \param usX X position
		 * \param usY Y position
		 * \param usZ Z position
		 */
		void getPos( ushort& usX, ushort& usY, ushort& usZ ) const throw()
		{
			ptrdiff_t diff = ( positionPtr - parentPtr->begin().positionPtr );
			usZ = diff / ( parentPtr->getExtent(0) * parentPtr->getExtent(1) );
			usY = ( diff % ( parentPtr->getExtent(0) * parentPtr->getExtent(1) ) ) 
				/ ( parentPtr->getExtent(0) );
			usX = diff - ( parentPtr->getExtent(0) * parentPtr->getExtent(1) * usZ )
				- ( parentPtr->getExtent(0) * usY );
		}
		/**
		 * Returns the actual iterator position in image coordinates
		 * \returns the actual iterator position in image coordinates
		 * \param usX X position
		 * \param usY Y position
		 * \param usZ Z position
		 * \param usW W position
		 */
		void getPos( ushort& usX, ushort& usY, ushort& usZ, ushort& usW ) const throw()
		{
			ptrdiff_t diff = ( positionPtr - parentPtr->begin().positionPtr );
			usW = diff / ( parentPtr->getExtent(0) * parentPtr->getExtent(1) * parentPtr->getExtent(2) );
			usZ = ( diff % ( parentPtr->getExtent(0) * parentPtr->getExtent(1) * parentPtr->getExtent(2) ) )
				/ ( parentPtr->getExtent(0) * parentPtr->getExtent(1) ) ;
			usY = ( diff - ( parentPtr->getExtent(0) * parentPtr->getExtent(1) * parentPtr->getExtent(2) 
				* usW )	- ( parentPtr->getExtent(0) * parentPtr->getExtent(1) * usZ ) ) 
				/ parentPtr->getExtent(0);
			usX = diff - ( parentPtr->getExtent(0) * parentPtr->getExtent(1) * parentPtr->getExtent(2) 
				* usW )	- ( parentPtr->getExtent(0) * parentPtr->getExtent(1) * usZ ) 
				- ( parentPtr->getExtent(0) * usY );
		}
		/**
		 * Returns a pointer to the iterator parent data structure
		 * \returns pointer to parent
		 */
		CTypedData<T>* getParent() throw()
		{ 
			return parentPtr; 
		}
/// Move the TypedDataIterator forward by an specific amount
	TypedDataIterator<T,U> operator+( ptrdiff_t amount ){ TypedDataIterator<T,U> t; t += amount; return t; }

/// Move the TypedDataIterator backward by an specific amount
	TypedDataIterator<T,U> operator-( ptrdiff_t amount ){ TypedDataIterator<T,U> t; t -= amount; return t; }
		
	private:
		U positionPtr; 					///< Pointer to actual position
		CTypedData<T>* parentPtr; ///< Pointer to parent CTypedData
	};
	
	/* Iterator definitions */
	/// Normal iterator
	typedef TypedDataIterator<valueType, valueType*> iterator;
	/// Reverse iterator
  typedef std::reverse_iterator<TypedDataIterator<valueType, valueType*> > reverse_iterator;

/* Iterator generating methods */
  iterator begin() throw();
  /// Returns iterator for end of array
  iterator end() throw();
  /// Returns reverse_iterator for begin of array
  reverse_iterator rbegin() throw();
  /// Returns reverse_iterator for end of array
  reverse_iterator rend() throw();	
	/// Returns an iterator to the given position 
	iterator moveTo( const ushort usX ) throw();
	/// Returns an iterator to the given position 
	iterator moveTo( const ushort usX, const ushort usY ) throw();
	/// Returns an iterator to the given position 
	iterator moveTo( const ushort usX, const ushort usY, const ushort usZ ) throw();
	/// Returns an iterator to the given position 
	iterator moveTo( const ushort usX, const ushort usY, const ushort usZ, const ushort usW ) throw();
  
/* Other Methods */
  /// Reimplemented from CDataSet
  virtual const std::string dump() const
    throw();
	/// Swaps the data with another data set of the same type
	void swap( CTypedData<valueType>& aDataSet ) 
		throw();
private:
  size_t arraySize;               ///< Size of the data array (no. of elements)
  std::vector<valueType> dataVec; ///< The data array
  valueType theMinimum;           ///< Dataset minimum value
  valueType theMaximum;           ///< Dataset maximum value
};


#include "ctypeddatainlines.tpp"

}

#endif
