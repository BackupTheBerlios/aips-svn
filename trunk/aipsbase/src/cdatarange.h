//
// C++ Interface: cdatarange
//
// Description: 
//
//
// Author: Hendrik Belitz <hbelitz@users.berlios.de>, (C) 2005
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
	TValue getMinimum() const
	{
		return theMinimum;
	}
	TValue getMaximum() const
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
		theMinimum = SDataTraits<TValue>::TDistanceType::distanceFromZero( theMinimum_ );
		theMaximum = SDataTraits<TValue>::TDistanceType::distanceFromZero( theMaximum_ );
	}
	CDataRange<TValue,false> operator=(const CDataRange<TValue,false>& aDataRange)
	{
		theMinimum = aDataRange.theMinimum;
		theMaximum = aDataRange.theMaximum;
		return (*this);
	}
	void setMinimum( TValue theMinimum_ )
	{
		theMinimum = SDataTraits<TValue>::TDistanceType::distanceFromZero( theMinimum_ );
	}
	void setMaximum( TValue theMaximum_ )
	{
		theMaximum = SDataTraits<TValue>::TDistanceType::distanceFromZero( theMaximum_ );
	}
	typename SDataTraits<TValue>::TScalarDataType getMinimum() const
	{
		return theMinimum;
	}
	typename SDataTraits<TValue>::TScalarDataType getMaximum() const
	{
		return theMaximum;
	}
	bool isInRange( TValue value_ )
	{
		typename SDataTraits<TValue>::TScalarDataType value = 
			SDataTraits<TValue>::TDistanceType::distanceFromZero( value_ );
		return ( value >= theMinimum && value <= theMaximum );
	}
	void updateRange( TValue value_ )
	{
		typename SDataTraits<TValue>::TScalarDataType value = 
			SDataTraits<TValue>::TDistanceType::distanceFromZero( value_ );
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
