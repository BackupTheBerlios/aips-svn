/************************************************************************
 * File: cpipelineitem.cpp                                              *
 * Project: AIPS                                                        *
 * Description: An abstract base for all processing pipeline items      *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Created: 17.09.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#include "cpipelineitem.h"

using namespace aips;
using namespace std;
using namespace boost;

set<CPipelineItem*> CPipelineItem::allItemsSet;
priority_queue<CPipelineItem*, std::vector<CPipelineItem*>, CPipelineItem::itemCompareFunctor> CPipelineItem::itemsPriQueue;

/********************************
 * CPipelineItem::CParameterMap *
 ********************************/

/**
 * \param sParamName name of the parameter
 * \param lParamDef default value
 * \param lParamMin minimum value of parameter
 * \param lParamMax maximum value of parameter
 */
void CPipelineItem::CParameterMap::initLong( const std::string& sParamName, 
	const long lParamDef, const long lParamMin, const long lParamMax ) throw()
{
	setLong( sParamName, lParamDef );
	setLong( sParamName+"_Default", lParamDef );
	setLong( sParamName+"_Minimum", lParamMin );
	setLong( sParamName+"_Maximum", lParamMax );
}

/**
 * \param sParamName name of the parameter
 * \param ulParamDef default value
 * \param ulParamMin minimum value of parameter
 * \param ulParamMax maximum value of parameter
 */
void CPipelineItem::CParameterMap::initUnsignedLong( const std::string& sParamName, 
	const ulong ulParamDef, const ulong ulParamMin, const ulong ulParamMax ) throw()
{
	setUnsignedLong( sParamName, ulParamDef );
	setUnsignedLong( sParamName+"_Default", ulParamDef );
	setUnsignedLong( sParamName+"_Minimum", ulParamMin );
	setUnsignedLong( sParamName+"_Maximum", ulParamMax );
}

/**
 * \param sParamName name of the parameter
 * \param dParamDef default value
 * \param dParamMin minimum value of parameter
 * \param dParamMax maximum value of parameter
 */
void CPipelineItem::CParameterMap::initDouble( const std::string& sParamName, 
	const double dParamDef, const double dParamMin, const double dParamMax ) throw()
{
	setDouble( sParamName, dParamDef );
	setDouble( sParamName+"_Default", dParamDef );
	setDouble( sParamName+"_Minimum", dParamMin );
	setDouble( sParamName+"_Maximum", dParamMax );
}

/**
 * \param sParamName name of the parameter
 * \param bParamDef default value
 */
void CPipelineItem::CParameterMap::initBool( const std::string& sParamName, 
	const bool bParamDef ) throw()
{
	setBool( sParamName, bParamDef );
	setBool( sParamName+"_Default", bParamDef );
}

void CPipelineItem::CParameterMap::initString( const std::string& sParamName, 
	const std::string& sParamDef ) throw()
{
	setString( sParamName, sParamDef );
	setString( sParamName+"_Default", sParamDef );
}			

/**********************
 * itemCompareFunctor *
 **********************/
		
bool CPipelineItem::itemCompareFunctor::operator() ( CPipelineItem* a, CPipelineItem* b ) throw()
{
	if ( a->iDepth < b->iDepth ) return true;
	return false;
}
	
/*****************
 * CPipelineItem *
 *****************/

/**
 * \param ulID_ Unique item ID (in processing graph)
 * \param usFIn_ Fanin ( = Number of inputs ) of item (itemtype-specific)
 * \param usFOut_ Fanout ( = Number of outputs ) of item (itemtype-specific)
 * \param sClassName_ name of the class (type information)
 * \param sClassVersion_ version number of the class (type information)
 * \param sDerivedFrom_ name of the classes this class is derived from (type information)
 *
 * In a real module constructor, you should do the following:
 * <ul>
 * <li> call CBase::setInfo(...) with the actual class parameters
 * <li> call setModuleID() 
 * <li> call setModuleName()
 * <li> fill the sDocumentation string. Desribe all IO ports, parameters and module function
 * <li> initialise all parameters: initParameter( ... );
 * <li> set all port types: inputsVec[..].portType = ..; outputsVec[..].portType = ..;
 * <li> declare non-mandatory input ports: inputsVec[..].bMandatory = false;
 * <li> set usNoOfMandatory inputs according to the above.
 * <li> build module dialog (if applicable)
 * </ul>
 */
CPipelineItem::CPipelineItem( ulong ulID_, ushort usFIn_, ushort usFOut_, const string &sClassName_, 
		const string &sClassVersion_, const string &sDerivedFrom_ ) throw()
  : CSubject( sClassName_, sClassVersion_, sDerivedFrom_ ),
  sModuleID(""), sDocumentation("No documentation available"), bModuleReady( false ), ownTimeStamp ( 0 ), bCacheOutputs( true ),iDepth( -1 ), ulID( ulID_ ), usFanIn( usFIn_ ), usFanOut( usFOut_ ), connectionsPtrVec( usFanIn ),
	connectionsTimeStampsVec( usFanIn ), bRecompute( false )
{
  inputsVec.resize( usFanIn );
  for( int i = 0; i < usFanIn; i++ )
  {
    inputsVec[i].portData.reset();
    inputsVec[i].portType = IOPoint;
    connectionsTimeStampsVec[i] = 0;
  }
  outputsVec.resize( usFanOut );
  for( int i = 0; i < usFanOut; i++ )
  {
    outputsVec[i].portData.reset();
    outputsVec[i].portType = IOPoint;
  }
}

/**
 * Remember to delete your dialog window here if you have one
 */
CPipelineItem::~CPipelineItem() throw()
{
	inputsVec.clear();
	outputsVec.clear();
}

/************************
 * Dataset IO functions *
 ************************/

/** 
 * \param usInputNumber requested input port
 * \returns dataset belonging to the given input port
 */
TDataSetPtr CPipelineItem::getInput( ushort usInputNumber ) const throw( OutOfRangeException )
{
  if ( usInputNumber >= usFanIn )
    throw( OutOfRangeException( SERROR("Input port number too large"), CException::FATAL, ERR_RANGE ) );
  return inputsVec[usInputNumber].portData.lock();
}

/** 
 * \param usOutputNumber requested output port
 * \returns dataset belonging to the given output port
 */
TDataSetPtr CPipelineItem::getOutput( ushort usOutputNumber ) const throw( OutOfRangeException )
{
  if ( usOutputNumber >= usFanOut )
    throw( OutOfRangeException( SERROR("Output port number too large"), CException::FATAL, ERR_RANGE ) );
  return outputsVec[usOutputNumber].portData;
}

/**
 * \param anInputPtr the input dataset
 * \param usInputNumber port number the dataset should be send to
 * \exception NullException if given parameter is NULL
 */
void CPipelineItem::setInput( TDataSetPtr anInputPtr, ushort usInputNumber ) throw( OutOfRangeException )
{
  if ( usInputNumber >= usFanIn )
	  throw( OutOfRangeException( SERROR("Input port number too large"), CException::FATAL, ERR_RANGE ) );
  inputsVec[usInputNumber].portData = anInputPtr;
}

/**
 * This may override any output generated by the module
 * \param anOutputPtr the output dataset
 * \param usOutputNumber port number the dataset should be send to 
 * \exception NullException if given parameter is NULL
 */
void CPipelineItem::setOutput( TDataSetPtr anOutputPtr, ushort usOutputNumber ) throw( OutOfRangeException )
{
  if ( usOutputNumber >= usFanOut )
	  throw( OutOfRangeException( SERROR("Output port number too large"), CException::FATAL, ERR_RANGE ) );
  outputsVec[usOutputNumber].portData = anOutputPtr;
}

/*****************
 * Other methods *
 *****************/

/**
 * \returns number of parameters
 */
size_t CPipelineItem::getNoOfParameters() const throw()
{	
	size_t parametersSize = 0;
	vector<string> keyList = parameters.getKeyList();
	for( vector<string>::const_iterator it = keyList.begin(); it != keyList.end(); ++it )
		if ( (*it).rfind( "_Default" ) == string::npos
			&& (*it).rfind( "_Minimum" ) == string::npos
			&& (*it).rfind( "_Maximum" ) == string::npos )
			parametersSize++;
  return ( parametersSize );
}

const string CPipelineItem::dump() const throw()
{
  std::ostringstream os;
  os << "usFanIn " << usFanIn << ", usFanOut " << usFanOut << ",\n";
  os << ", inputsVec " << inputsVec.size() << ", outputsVec " << outputsVec.size() <<",\n";
  os << "parameters " << parameters.getNumberOfEntries() 
		<< "\n";
  os << "sModuleID " << sModuleID << ", sDocumentation " << sDocumentation << ", ulID " << ulID
    << ", itemType " << itemType << ",\n";
  os << "usFanIn " << usFanIn << ", usFanOut " << usFanOut << ", sName " << sName << "\n";

  return CBase::dump() + os.str();
}

void CPipelineItem::forceRecomputation() throw()
{
	bRecompute = true;
	execute();
}

void CPipelineItem::update( int iDepth_ ) throw()
{
DBG1( "+++ CPipelineItem::update " << sName );
	allItemsSet.insert( this );
	if ( iDepth_ > iDepth )
		iDepth = iDepth_;
	for( uint i = 0; i < connectionsPtrVec.size(); ++i )
		if ( TPipelineItemPtr tmpPtr = connectionsPtrVec[i].outputItem.lock() )
			tmpPtr->update( iDepth + 1 );
	if ( iDepth_ == 0 )
			iterate();
DS( "--- CPipelineItem::update " << sName );
}

/*********************
 * protected members *
 *********************/

/**
 * \param usPortNumber number of output port to delete
 */
void CPipelineItem::deleteOldOutput( ushort usPortNumber ) throw( OutOfRangeException )
{
	if ( usPortNumber >= usFanOut )
		throw( OutOfRangeException( "Output port number too high", CException::FATAL, ERR_RANGE ) );
  setOutput( TDataSetPtr(), usPortNumber );
}

/*******************
 * private members *
 *******************/

void CPipelineItem::execute() throw()
{
DBG1( "+++ CPipelineItem::execute" << sName );
	bool bUpdate = false;
	if ( !bRecompute && ownTimeStamp != 0 )
	{
		for( uint i = 0; i < connectionsPtrVec.size(); ++i )
		{
			if ( TPipelineItemPtr tmpPtr = connectionsPtrVec[i].outputItem.lock() )
				if ( tmpPtr->ownTimeStamp != connectionsTimeStampsVec[i] )
			{
				bUpdate = true; break;
			}
		}
	}
	else
		bUpdate = true;
DS("Upd: " << bUpdate);
	// We need to update
	if ( bUpdate )
	{
DS( "Updating " << ulID << " ... " );
		for( uint i = 0; i < connectionsPtrVec.size(); ++i )
		{
			if ( TPipelineItemPtr tmpPtr = connectionsPtrVec[i].outputItem.lock() )			
			{
				connectionsTimeStampsVec[i] = tmpPtr->ownTimeStamp;
				setInput( tmpPtr->getOutput( connectionsPtrVec[i].outputPort ), i );				
			}
		}
		this->apply();
		for( uint i = 0; i < connectionsPtrVec.size(); ++i )
		{
			if ( TPipelineItemPtr tmpPtr = connectionsPtrVec[i].outputItem.lock() )
			{
				tmpPtr->clearCache();
				cerr << "<" << inputsVec[i].portData.use_count() << ">" << endl;
				inputsVec[i].portData.reset();
			}
		}		
		iDepth = -1;
		ownTimeStamp++;
DS( "done" );
		bRecompute = false;
	}
	else // We do not need to update
	{
DS( "No update needed: " << ulID );
		iDepth = -1;
	}
DS( "--- CPipelineItem::execute " << sName );
}
 
void CPipelineItem::iterate() throw()
{
DBG1( "+++ CPipelineItem::iterate " );
	for( set<CPipelineItem*>::iterator it = allItemsSet.begin(); it != allItemsSet.end(); ++it )
	{			
		if ( (*it)->iDepth != -1 )
		{
DS( (*it)->ulID << " : " << (*it)->iDepth );
			itemsPriQueue.push( (*it) );
		}
	}	
	while( !itemsPriQueue.empty() )
	{
		itemsPriQueue.top()->execute();
		itemsPriQueue.pop();
	}
	allItemsSet.clear();
DS( "--- CPipelineItem::iterate " );
}

/**
 * \param anInputPtr Pipeline item to connect to
 * \param uiLocalPort number of local input port
 * \param uiImportedPort number of imported output port
 * \throws OutOfRangeException if input or output port numbers are too large
 */
void CPipelineItem::addConnection( TPipelineItemPtr anInputPtr, uint uiLocalPort, uint uiImportedPort )
	throw( OutOfRangeException, RunTimeTypeException, NullException )
{
	if ( uiLocalPort > usFanIn )
		throw( OutOfRangeException( SERROR("Input port number too large"), CException::FATAL, ERR_RANGE ) );
	else if ( uiImportedPort > anInputPtr->usFanIn )
		throw( OutOfRangeException( SERROR("Output port number too large"), CException::FATAL, ERR_RANGE ) );
	if ( !anInputPtr )
		throw( NullException( SERROR("Input is NULL"), CException::FATAL, ERR_CALLERNULL ) );
	if ( anInputPtr->outputsVec[uiImportedPort].portType % inputsVec[uiLocalPort].portType != 0
		&& inputsVec[uiLocalPort].portType % anInputPtr->outputsVec[uiImportedPort].portType != 0 )
		throw( RunTimeTypeException( SERROR("Port types do not match"), CException::WARNING, ERR_PORTTYPE ) );
	SConnection c;
	c.outputItem = anInputPtr;
	c.outputPort = uiImportedPort;
DS( "Connecting output " << uiImportedPort << " of " << anInputPtr->ulID
<< " to " << uiLocalPort << " of " << this->ulID );
	connectionsPtrVec[uiLocalPort] = c;
}

/**
 * \param uiLocalPort port connection to delete
 * \throws OutOfRangeException if port number is too large
 */
void CPipelineItem::deleteConnection( uint uiLocalPort ) throw( OutOfRangeException )
{
	if ( uiLocalPort > usFanIn )
		throw( OutOfRangeException( SERROR("Port number too large"), CException::FATAL, ERR_RANGE ) );
	connectionsPtrVec[uiLocalPort].outputItem.reset();
	connectionsPtrVec[uiLocalPort].outputPort = 0;
}

/** \returns true if output is cached */
bool CPipelineItem::isOutputCached() const throw()
{
	return bCacheOutputs;
}

/** \param cacheOutput true == cache output, false == delete output after first readout */
void CPipelineItem::cacheOutput( bool bCacheOutputs_ ) throw()
{
	bCacheOutputs = bCacheOutputs_;
}

void CPipelineItem::clearCache() throw()
{
	if ( !bCacheOutputs )
		for( uint i = 0; i < outputsVec.size(); ++i )
		{
			DBG3( "-- Clearing data of " << this->ulID << " " << outputsVec[i].portData.use_count() );
			outputsVec[i].portData.reset();
		}
}

/** \returns the module Dialog */
boost::shared_ptr<CModuleDialog> CPipelineItem::getModuleDialog() const
	throw()
{
	return itemDialog;
}

/** \param newItemDialog sets the module dialog to newItemDialog */
void CPipelineItem::setModuleDialog( const shared_ptr<CModuleDialog> newItemDialog )
	throw()
{
	itemDialog = newItemDialog;
}
