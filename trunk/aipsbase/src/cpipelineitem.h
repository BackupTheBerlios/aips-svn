/************************************************************************
 * File: cpipelineitem.h                                                *
 * Project: AIPS                                                        *
 * Description: An abstract base for all processing pipeline items      *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.9.3                                                       *
 * Status : Beta                                                        *
 * Created: 17.09.03                                                    *
 * Changed: 18.09.03 Added dump() method                                *
 *          22.09.03 All members now use string instead of QString for  *
 *                    parameters and return values                      *
 *          23.09.03 Added private standard constructor                 *
 *          30.09.03 Reorganized file structure, moved documentation    *
 *                   to cpipelineitem.cpp                               *
 *          17.10.03 Added i/o member functions and variables.          *
 *                   Added pure virtual member function apply.          *
 *          27.10.03 Added parameter handling and dataChanged() signal  *
 *          06.11.03 Added additional runtype type id mechanism         *
 *          27.11.03 Updated documentation                              *
 *          17.12.03 Added simple online documentation support          *
 *          08.01.04 Iteration loops now use ++it instead of it++       *
 *          21.01.04 Made source code look prettier.                    *
 *                   Corrected an error in getInput()                   *
 *                   Moved some inline functions to cpp file            *
 *                   Added constructors to struct SParameter            *
 *          26.01.04 Updated parameter type names                       *
 *          20.04.04 Added graph traversal marker and output cache flag *
 *                    ( with accessors )                                *
 *          29.04.04 Class now uses CTypedMap to store parameters       *
 *          30.04.04 Updated documentation                              *
 *                   Deleted all the stuff we don't need anymore        *
 *        2004-09-17 Moved dialog-specific stuff to CModuleDialog       *
 *        2004-11-22 Removed setinfo. Updated constructor accordingly   *
 *                   Class now uses boost::shared_ptr                   *
 *        2004-12-21 Added new function template checkInput()           *
 *        2005-06-10 Complete update to new pipelining architecture     *
 *        2005-07-07 Added new IO types                                 *
 *                   Added runtime type checking for io ports           *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CPIPELINEITEM_H
#define CPIPELINEITEM_H

// Standard library includes
#include <queue> // std::queue
#include <set> // std::set

// AIPS includes
#include <boost/weak_ptr.hpp>
#include "aipsdatatraits.h"
#include "ctypeddata.h"
#include "ctypedmap.h"
#include "cmoduledialog.h"

#ifdef BENCHMARK
#include <boost/timer.hpp>
#endif

namespace aips {

class CPipelineItem;

typedef boost::shared_ptr<CPipelineItem> TPipelineItemPtr;

/** An abstract base for all processing pipeline items */
class CPipelineItem : public CSubject
{
public:
/* Data Types */
  /** Item types */
  enum EItemType {	ITypeFilter = 1,///< Item type filter
                           	ITypeConverter, ///< Item type converter
                           	ITypeSource,    ///< Item type source
                           	ITypeTarget,    ///< Item type target
                           	ITypeStub,      ///< Item type stub (scripting or external processes)
                           	ITypeUser };    ///< Item type user (anything else)
  /**
   * IO port data types. 
   * A simple hierarchy is obtained through the numerical values of the type
   * members. As you will notice, each parent is a divisor of all derived
   * values. E.g. IO2DFloat is a child of IOFloat (15%3==0) which is a
   * child of IOOther (3%1==0)
   */
  enum EIOTypes {	IOPoint = 11,     ///< A single value of any type
  								IOString = 17,    ///< A string field
	                IO1DInteger = 4,  ///< 1D field of integers
                  IO3DInteger = 10, ///< 3D field of integers
                  IO2DInteger = 26, ///< 2D field of integers
                  IO1DFloat = 6,    ///< 1D field of floating point numbers
                  IO2DFloat = 15,   ///< 2D field of floating point numbers
                  IO3DFloat = 39,   ///< 3D field of floating point numbers
                  IO1DVector = 21,  ///< 1D field of vectors
                  IO2DVector = 49,  ///< 2D field of vectors
                  IO3DVector = 77,  ///< 3D field of vectors
                  IOInteger = 2,    ///< Arbitrary dimension field of integers
                  IOFloat = 3,      ///< Arbitrary dimension field of floating point numbers
                  IOVector = 7,     ///< Arbitrary dimension field of vectors
                  IOOther = 1     ///< Any other dataset
								};
protected:
/* Data types */
	/** A map to store all module parameters */
	class CParameterMap : public CTypedMap
	{
	public:
		/// Method for easy initialisation of a long parameter
		void initLong( const std::string& sParamName, const long lParamDef,
      const long lParamMin, const long lParamMax ) throw();
		/// Method for easy initialisation of a ulong parameter
		void initUnsignedLong( const std::string& sParamName, const ulong ulParamDef,
      const ulong ulParamMin, const ulong ulParamMax ) throw();
		/// Method for easy initialisation of a double parameter
		void initDouble( const std::string& sParamName, const double dParamDef,
      const double dParamMin, const double dParamMax ) throw();
		/// Method for easy initialisation of a bool parameter
		void initBool( const std::string& sParamName, const bool bParamDef ) 
			throw();
		/// Method for easy initialisation of a string parameter
		void initString( const std::string& sParamName, const std::string& sParamDef )
      throw();		
	};
  /** Port structure */
  struct SIPort
  {
    EIOTypes portType;      ///< Port io type
    boost::weak_ptr<CDataSet> portData;     ///< Data on port
  };
  struct SOPort
  {
    EIOTypes portType;      ///< Port io type
    TDataSetPtr portData;     ///< Data on port
  };
  /** Connection structure */
  struct SConnection
  {
  	boost::weak_ptr<CPipelineItem> outputItem;
  	uint outputPort; // Imported port
  	SConnection() : outputPort(0)  { outputItem.reset(); }
  };
private:
  /// Standard constructor
  CPipelineItem();
  /// Copy constructor
  CPipelineItem( CPipelineItem& );
public:
/* Structors */
  /// Constructor
  CPipelineItem( ulong ulID_, ushort usFanIn_, ushort usFanOut_, const std::string &sClassName_, 
		const std::string &sClassVersion_, const std::string &sDerivedFrom_ )
    throw();
  /// Destructor 
  virtual ~CPipelineItem()
    throw();    
/* Accessors */  
  /// Returns the fanin of the item
  ushort getFanIn() const
    throw();
  /// Returns the fanout of the item
  ushort getFanOut() const
    throw();
  /// Return the module name
  const std::string getModuleName() const
    throw();
  /// Returns the module documentation
  const std::string getDocumentation() const 
		throw();
  /// Get the item ID
  ulong getID() const
    throw();
  /// Get the item type
  ushort getType() const
    throw();
  /// Returns the number of filter parameters = size of the parameter array
  size_t getNoOfParameters() const
    throw();
	/// Returns a handle to the map containing all parameters
	CTypedMap* getParameters() 
		throw();
  /// Returns a unique module id
  const std::string getModuleID() const
    throw();
/* Mutators */
  /// Set the module name
  void setModuleName( const std::string &sNewName )
    throw();
  /// Set the item type
  void setType( const ushort itemType_ )
    throw();    
  /// Sets the module ID
  void setModuleID( const std::string sModuleID )
    throw();
/* Dataset IO functions */
  /// Return the input dataset
  TDataSetPtr getInput( ushort usInputNumber = 0 ) const
    throw( OutOfRangeException );
  /// Return the output dataset
  TDataSetPtr getOutput( ushort usOutputNumber = 0 ) const
    throw( OutOfRangeException );
  /// Set the input dataset
  void setInput( TDataSetPtr anInputPtr, ushort usInputNumber = 0 )
    throw( OutOfRangeException );
  /// Set the output dataset
  void setOutput( TDataSetPtr anOutputPtr, ushort usOutputNumber = 0 )
    throw( OutOfRangeException );
  /// Get the dataset type of an input port
  EIOTypes getInputType( uint uiPort ) const
    throw( OutOfRangeException );
  /// Get the dataset type of an output port
  EIOTypes getOutputType( uint uiPort ) const
    throw( OutOfRangeException );
/* Graph edge access methods  */
  /// Add conncection
  void addConnection( TPipelineItemPtr anInputPtr, uint uiLocalPort = 0, uint uiImportedPort = 0 )
    throw( OutOfRangeException, RunTimeTypeException, NullException );
  /// Delete connection
  void deleteConnection( uint uiLocalPort )
    throw( OutOfRangeException );
/* Other methods */
  /**
   * Apply the module to the input data. Here you should do the following:
	 * <ol>
	 * <li> set <b>bModuleReady</b> to <b>false</b> </li>
   * <li> Retrieve and check all input ports ( normally this is done by dynamic casting the input
   *   data sets to the needed CTypedData<> instantiations and checking for invalid pointers or datasets)</li>
	 * <li> After checking all required input ports, set <b>bModuleReady</b> to <b>true</b></li>
	 * <li> Retrieve all optional input ports (s.a.)</li>
   * <li> Call deleteOldOutput() for each output port</li>
	 * <li> Create new output ports.<br>
	 *      Alternately to the latter two you could also overwrite the old output data sets</li>
	 * <li> Set the output ports ( via setOutput() )</li>
   * <li> Update all internal parameters ( read out all parameters and dialog inputs )</li>
   * <li> Process ( this is where your algorithms will reside )</li>
   * <li> Update all internal parameters ( update dialog and parameter array )</li>   
	 * </ol>
   */
  virtual void apply() throw() =0;
  /// Returns if module is ready for computation
	bool isReady() const
		throw();
	/// Enforce the recomputation of the modules outputs
	void forceRecomputation()
		throw();	
  /// Update function to get the most actual data from the pipeline
  void update( int iDepth_ = 0 )
  	throw();
  /// Reimplemented from CBase
  virtual const std::string dump() const
    throw();
  /**
   * Returns a new instance of the class.
   * Each child of CPipelineItem MUST overwrite this if it 
	 * is not an abstract class.
   * \returns a new instance (not a copy!) of the item
   */
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw() =0;
	/// Checks one input pointer for existence and correctness
	template<typename U, typename T> bool checkInput( T inputPtr, ushort usMinDim = 0, ushort usMaxDim = 0, ushort usMinDataDim = 0, ushort usMaxDataDim = 0) throw();
	bool isOutputCached() const throw();
	void cacheOutput( bool bCacheOutputs_ = true ) throw();
/* Dialog member functions */
	/// Returns the module Dialog
  boost::shared_ptr<CModuleDialog> getModuleDialog() const
		throw();
	/// Sets the module dialog
	void setModuleDialog( const boost::shared_ptr<CModuleDialog> newItemDialog )
		throw();	
protected:
  /// Deletes an output port
  void deleteOldOutput( ushort usOutputNumber = 0 )
    throw( OutOfRangeException );
protected:
  std::vector<SIPort> inputsVec;     	 ///< Input dataset
  std::vector<SOPort> outputsVec; 			 ///< Output dataset
  CParameterMap parameters; 					 ///< Array of parameter names
  std::string sModuleID;               ///< Unique module ID
  std::string sDocumentation;          ///< Module documentation
	bool bModuleReady;
	ulong ownTimeStamp; ///< Timestamp
private:
	bool bCacheOutputs; ///< Should we cache our outputs or delete them after read-out?
	int iDepth;                                 ///< Marker for graph traversal	
  ulong ulID;                                   ///< Unique processing ID
  ushort itemType;                              ///< Item type 
  ushort usFanIn;                               ///< Item fanin
  ushort usFanOut;                              ///< Item fanout
  std::string sName;                            ///< Name of the pipeline module 
  std::vector<SConnection> connectionsPtrVec;   ///< Input connections of the module
  std::vector<ulong> connectionsTimeStampsVec; ///< Time stamps of all connections
  boost::shared_ptr<CModuleDialog> itemDialog;  ///< Item dialog
	bool bRecompute; ///< Do we enforce a recomputation of all outputs?  
  struct itemCompareFunctor ///< Functor to compare two pipeline items
	{
		bool operator() ( CPipelineItem* a, CPipelineItem* b ) throw();
	};
	static std::set<CPipelineItem*> allItemsSet; ///< All items of the pipeline we're actually working on	
	static std::priority_queue<CPipelineItem*, std::vector<CPipelineItem*>, itemCompareFunctor>
		itemsPriQueue; ///< Priority queue to execute all pipeline items in correct order
private:	
	/// Actually execute the pipeline item
  void execute() throw();
  /// Clear all outputs
  void clearCache() throw();
  // Static members
  /// Iterate through pipeline hierarchy
  static void iterate() throw();  	
};

#include "cpipelineiteminlines.tpp"

}
#endif
