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

template<typename valueType, bool isScalar>
class CDataRange
{
};

template<typename valueType>
class CDataRange<valueType,true>
{
public:
	CDataRange()
	{
	}
	CDataRange( valueType theMinimum_, valueType theMaximum_ ) 
		: theMinimum( theMinimum_ ), theMaximum( theMaximum_ )
	{ 
	}
	CDataRange<valueType,true> operator=(const CDataRange<valueType,true>& aDataRange)
	{
		theMinimum = aDataRange.theMinimum;
		theMaximum = aDataRange.theMaximum;
		return (*this);
	}
	void setMinimum( valueType theMinimum_ )
	{
		theMinimum = theMinimum_;
	}
	void setMaximum( valueType theMaximum_ )
	{
		theMaximum = theMaximum_;
	}
	valueType getMinimum()
	{
		return theMinimum;
	}
	valueType getMaximum()
	{
		return theMaximum;
	}
	bool isInRange( valueType value )
	{
		return ( value >= theMinimum && value <= theMaximum );
	}
	void updateRange( valueType value )
	{
		if ( value < theMinimum )	
			theMinimum = value;
		if ( value > theMaximum )	
			theMaximum = value;			
	}
private:
	valueType theMinimum;
	valueType theMaximum;
};

template<typename valueType>
class CDataRange<valueType,false>
{
public:
	CDataRange()
	{
	}
	CDataRange( valueType theMinimum_, valueType theMaximum_ ) 
	{ 
		theMinimum = toScalarType( theMinimum_ );
		theMaximum = toScalarType( theMaximum_ );
	}
	CDataRange<valueType,false> operator=(const CDataRange<valueType,false>& aDataRange)
	{
		theMinimum = aDataRange.theMinimum;
		theMaximum = aDataRange.theMaximum;
		return (*this);
	}
	void setMinimum( valueType theMinimum_ )
	{
		theMinimum = dataTraits<valueType>::toScalarType( theMinimum_ );
	}
	void setMaximum( valueType theMaximum_ )
	{
		theMaximum = dataTraits<valueType>::toScalarType( theMaximum_ );
	}
	typename dataTraits<valueType>::scalarDataType getMinimum()
	{
		return theMinimum;
	}
	typename dataTraits<valueType>::scalarDataType getMaximum()
	{
		return theMaximum;
	}
	bool isInRange( valueType value_ )
	{
		typename dataTraits<valueType>::scalarDataType value = dataTraits<valueType>::toScalarType( value_ );
		return ( value >= theMinimum && value <= theMaximum );
	}
	void updateRange( valueType value_ )
	{
		typename dataTraits<valueType>::scalarDataType value = dataTraits<valueType>::toScalarType( value_ );
		if ( value < theMinimum )	
			theMinimum = value;
		if ( value > theMaximum )	
			theMaximum = value;			
	}
private:
	typename dataTraits<valueType>::scalarDataType theMinimum;
	typename dataTraits<valueType>::scalarDataType theMaximum;
};

}
