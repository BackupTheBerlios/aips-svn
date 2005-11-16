//
// C++ Interface: cdatarange
//
// Description: 
//
//
// Author: Hendrik Belitz <hendrik@darkon.info>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

namespace aips {

template<typename TValue, bool isScalar>
class CDataRange
{
};

template<typename TValue>
class CDataRange<TValue,true>
{
public:
	CDataRange()
	{
	}
	CDataRange( TValue theMinimum_, TValue theMaximum_ )
		: theMinimum( theMinimum_ ), theMaximum( theMaximum_ )
	{ 
	}
	CDataRange<TValue,true> operator=(const CDataRange<TValue,true>& aDataRange)
	{
		theMinimum = aDataRange.theMinimum;
		theMaximum = aDataRange.theMaximum;
		return (*this);
	}
	void setMinimum( TValue theMinimum_ )
	{
		theMinimum = theMinimum_;
	}
	void setMaximum( TValue theMaximum_ )
	{
		theMaximum = theMaximum_;
	}
	TValue getMinimum()
	{
		return theMinimum;
	}
	TValue getMaximum()
	{
		return theMaximum;
	}
	bool isInRange( TValue value )
	{
		return ( value >= theMinimum && value <= theMaximum );
	}
	void updateRange( TValue value )
	{
		if ( value < theMinimum )	
			theMinimum = value;
		if ( value > theMaximum )	
			theMaximum = value;			
	}
private:
	TValue theMinimum;
	TValue theMaximum;
};

template<typename TValue>
class CDataRange<TValue,false>
{
public:
	CDataRange()
	{
	}
	CDataRange( TValue theMinimum_, TValue theMaximum_ )
	{ 
		theMinimum = toScalarType( theMinimum_ );
		theMaximum = toScalarType( theMaximum_ );
	}
	CDataRange<TValue,false> operator=(const CDataRange<TValue,false>& aDataRange)
	{
		theMinimum = aDataRange.theMinimum;
		theMaximum = aDataRange.theMaximum;
		return (*this);
	}
	void setMinimum( TValue theMinimum_ )
	{
		theMinimum = dataTraits<TValue>::toScalarType( theMinimum_ );
	}
	void setMaximum( TValue theMaximum_ )
	{
		theMaximum = dataTraits<TValue>::toScalarType( theMaximum_ );
	}
	typename dataTraits<TValue>::scalarDataType getMinimum()
	{
		return theMinimum;
	}
	typename dataTraits<TValue>::scalarDataType getMaximum()
	{
		return theMaximum;
	}
	bool isInRange( TValue value_ )
	{
		typename dataTraits<TValue>::scalarDataType value = dataTraits<TValue>::toScalarType( value_ );
		return ( value >= theMinimum && value <= theMaximum );
	}
	void updateRange( TValue value_ )
	{
		typename dataTraits<TValue>::scalarDataType value = dataTraits<TValue>::toScalarType( value_ );
		if ( value < theMinimum )	
			theMinimum = value;
		if ( value > theMaximum )	
			theMaximum = value;			
	}
private:
	typename SDataTraits<TValue>::TScalarDataType theMinimum;
	typename SDataTraits<TValue>::TScalarDataType theMaximum;
};

}
