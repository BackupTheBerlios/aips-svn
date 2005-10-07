/***********************************************************************
 * File: cmainwin.cpp                                                  *
 * Project: AIPS                                                       *
 * Description: The main window of the AIPS application                *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 25.08.03                                                   *
 ***********************************************************************/

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "cmainwin.h"
#include <cglobalconfig.h>

using namespace std;
using namespace boost;

typedef boost::shared_ptr<CPipelineItem> pipelineItemPtr;
typedef boost::shared_ptr<CFileHandler> fileHandlerPtr;

/**
 * Constructor
 * \param parentPtr QT parent widget (should be NULL)
 * \paran sName window name (should be AIPS)
 */
CMainWin::CMainWin( QWidget* parentPtr, const char* sName ) throw()
  : QMainWindow( parentPtr, sName ),
		CObserver ( "CMainWin", "0.5", "CBase, QMainWindow" ),
		CSubject ( "CMainWin", "0.5", "CBase, QMainWindow" ),
		currentlyRunning(false)
{
	logWidget = new QTextEdit();
	logWidget->setCaption( "AIPS logging window" );
	logWidget->setTextFormat( Qt::LogText );
	logWidget->setBaseSize( 300, 200 );
	logWidget->show();
		
	// Get some environment variables or set paths to standard values
	bPipelineChanged = false;
	if ( getenv( "AIPS_PLUGINS" ) )
		sPluginDir = getenv( "AIPS_PLUGINS" );
	else
		sPluginDir = ".";
	if ( getenv( "AIPS_PIPELINES" ) )
		sPipelineDir = getenv( "AIPS_PIPELINES" );
	else
		sPipelineDir = ".";
	if ( getenv( "AIPS_DATA" ) )
		sDataDir = getenv( "AIPS_DATA" );
	else
		sDataDir = ".";
	if ( getenv( "AIPS_CONFIG" ) )
		sConfigDir = getenv( "AIPS_CONFIG" );
	else
		sConfigDir = getenv( "HOME" );
  std::string sLutDir;
	if ( getenv( "AIPS_LUT" ) )
		sLutDir = getenv( "AIPS_LUT" );
	else
	{
		sLutDir = getenv( "HOME" );
		sLutDir += "/bin/mricro";
	}
		
	getGlobalConfiguration().setString( "AIPS_PLUGINS", sPluginDir );
	getGlobalConfiguration().setString( "AIPS_PIPELINES", sPipelineDir );
	getGlobalConfiguration().setString( "AIPS_DATA", sDataDir );
	getGlobalConfiguration().setString( "AIPS_CONFIG", sConfigDir );
	getGlobalConfiguration().setString( "AIPS_LUT", sLutDir );

  setCaption("AIPS - Automatic image processing system");
  // Setup window geometry and layout management
  resize( 640, 480 );

  QVBox* aColumnPtr;
  aColumnPtr = new QVBox( this );
  setCentralWidget( aColumnPtr );
  QHBox* firstRowPtr = new QHBox ( aColumnPtr );
  QHBox* secondRowPtr = new QHBox ( aColumnPtr );
  firstRowPtr->setSpacing( 20 );
  firstRowPtr->setMargin( 10 );
  secondRowPtr->setSpacing( 20 );
  secondRowPtr->setMargin( 10 );

  // Create menubar
  fileMenuPtr = new QPopupMenu ( this );
	fileMenuPtr->insertItem( "&New pipeline", 
		this, SLOT( newPipeline() ), CTRL+Key_N );
  fileMenuPtr->insertItem( "&Open pipeline", 
		this, SLOT( openPipeline() ), CTRL+Key_O );
  fileMenuPtr->insertItem( "&Save pipeline", 
		this, SLOT( savePipeline() ), CTRL+Key_S  );
  fileMenuPtr->insertItem( "&Exit", 
		this, SLOT( closeAips() ), CTRL+Key_X  );
	windowMenuPtr = new QPopupMenu ( this );
  windowMenuPtr->setCheckable( true );
  windowMenuPtr->insertItem( "&Hide all client windows", 
		this, SLOT( closeAllClientWindows() ), CTRL+Key_H  );
	windowMenuPtr->insertItem( "&Show all client windows", 
		this, SLOT( openAllClientWindows() ), CTRL+Key_C  );
  windowMenuPtr->insertItem( "&Toggle log window", 
		this, SLOT( toggleLogWindow() ), CTRL+Key_T, 9999  );
  windowMenuPtr->insertSeparator();
	moduleMenuPtr = new QPopupMenu ( this );
  moduleMenuPtr->insertItem( "&Plugin configuration", 
		this, SLOT( showplugindialog() ),	CTRL+Key_P  );
  helpMenuPtr = new QPopupMenu ( this );
  helpMenuPtr->insertItem( "&About", 
		this, SLOT( about() ) );
  helpMenuPtr->insertItem( "About &Qt",
		this, SLOT( aboutQt() ) );
  menuBar()->insertItem( "&File", fileMenuPtr );
  menuBar()->insertItem( "&Window", windowMenuPtr );
  menuBar()->insertItem( "&Modules", moduleMenuPtr );
  menuBar()->insertItem( "&Help", helpMenuPtr );
  
  // Setup the other widgets
  executeCheckPtr = new QCheckBox( "Execute processing pipeline", firstRowPtr );
  executeCheckPtr->setChecked( true );
//	currentProgress = new QProgressBar( firstRowPtr );
  tabBoxPtr = new QTabWidget( secondRowPtr );
  tabBoxPtr->setFixedWidth(250);
  availFiltersPtr = new CDragListBox( 0, 0 );
  availConvertersPtr = new CDragListBox( 1, 0 );
  availSourcesPtr = new CDragListBox( 2, 0 );
  availTargetsPtr = new CDragListBox( 3, 0 );
  otherModulesPtr = new CDragListBox( 4, 0 );

	filtersToolBoxPtr = new QToolBox( tabBoxPtr );
	convertersToolBoxPtr = new QToolBox( tabBoxPtr );
	sourcesToolBoxPtr = new QToolBox( tabBoxPtr );
	targetsToolBoxPtr = new QToolBox( tabBoxPtr );
	othersToolBoxPtr = new QToolBox( tabBoxPtr );
	tabBoxPtr->addTab( filtersToolBoxPtr, "Filters" );
  tabBoxPtr->addTab( convertersToolBoxPtr, "Converters" );
  tabBoxPtr->addTab( sourcesToolBoxPtr, "Sources" );
  tabBoxPtr->addTab( targetsToolBoxPtr, "Targets" );
  tabBoxPtr->addTab( othersToolBoxPtr, "Other Modules" );
	
  pipelineViewPtr = new CPipeScroller( secondRowPtr );

  // Load the plugin libraries
  try
  {
    loadConfigFile();
  }
  catch ( CException &e )
  { alog << LFATAL << e.what() << endl; terminate(); }

	dialog = new CPluginDialog( this );
	dialog->plugInList( loadedPlugIns );
	
  connect( pipelineViewPtr, SIGNAL( connectionAdded( int, int, int, int ) ),
    this, SLOT( addGraphEdge( int, int, int, int ) ) );
  connect( pipelineViewPtr, SIGNAL( connectionDeleted( int, int, int, int ) ),
    this, SLOT( deleteGraphEdge( int, int, int, int ) ) );
  connect( pipelineViewPtr, SIGNAL( itemAdded( CPipeItemWidget* ) ),
    this, SLOT( addGraphNode( CPipeItemWidget* ) ) );
  connect( pipelineViewPtr, SIGNAL( itemDeleted( ulong ) ),
    this, SLOT( deleteGraphNode( ulong ) ) );
  connect( pipelineViewPtr, SIGNAL( itemRenamed( ulong, std::string ) ),
    this, SLOT( moduleRenamed( ulong, std::string ) ) );
  connect( pipelineViewPtr, SIGNAL( toggleDialog( int ) ),
    this, SLOT( toggleWindow( int ) ) );
  connect( pipelineViewPtr, SIGNAL( showItem( ulong ) ),
    this, SLOT( processModule( ulong ) ) );
	connect( dialog, SIGNAL( newItem( const std::string& ) ), 
		this, SLOT( loadRuntimePlugin( const std::string& ) ) );
  connect( dialog, SIGNAL( removeItem( const SLibItem& ) ),
		this, SLOT( removeRuntimePlugin( const SLibItem& ) ) );
	curFont = false;
	showLog = true;
	windowMenuPtr->setItemChecked(9999,true);
	actualInstance = this;
	//newLog.connect( &logStringInWindow );
} 


string CMainWin::encodeString( const string &s )
{
	string re;
	if ( s.find("INFO") != string::npos )
	{ re += "<font color=darkblue>"; curFont = true; }
	else if ( s.find("WARNING") != string::npos )
		{re += "<font color=darkgreen>";curFont = true; }
	else if ( s.find("ERROR") != string::npos )
		{re += "<font color=red>";curFont = true; }
	else if ( s.find("FATAL") != string::npos )
		{re += "<font color=magenta>";curFont = true; }
	else if ( s.find("UNKNOWN") != string::npos )
		{re += "<font color=blue>";curFont = true; }
	for( uint i = 0; i < s.length(); ++i )
	{
		if ( s[i] == '<' )
			re += "&lt;";
		else if ( s[i] == '>' )
			re += "&gt;";
		else if ( s[i] == '&' )
			re += "&amp;";
		else
			re += s[i];
	}
	return re;
}

void CMainWin::logString( const char* text, size_t len )
{
	static ostringstream os;	
	if ( text == NULL || len == 0 )
	{
		string s = encodeString(os.str());
		if ( curFont ) { s += "</font>"; curFont = false; }
		logWidget->append( s.c_str() );		
		os.str("");
		return;
	}	
	string s ( text, len );
	os << s;
}

/** Class destructor */
CMainWin::~CMainWin() throw()
{
  for ( uint i = 0; i < 4; i ++ )
  {
    modules[i].clear();
  }	
	delete logWidget;
	delete dialog;	
}

/**
 * Process the item of the given id
 * \param ulID module id
 */
void CMainWin::processModule( ulong ulID ) throw()
{
	if ( !currentlyRunning )
  	process( ulID );
}

/**
 * Create a new drag item from the selected list box entry
 * \param iWhichTab Selected list box
 * \param iWhichModule Selected list box entry
 */
void CMainWin::newDragItem( int iWhichType, int iWhichLib, int iWhichModule ) throw()
{
DS("Dragging " << iWhichType << "/" << iWhichLib << "/" << iWhichModule );
  CDragPipeItem item ( modules[iWhichType][iWhichLib][iWhichModule].get(), iWhichModule, iWhichLib,
iWhichType);
  QStoredDrag *drag = new QStoredDrag ( "obj/pipeitem", this );
  drag->setEncodedData( item.encodedData( "" ) );
  drag->dragCopy();
}

/**
 * Shows or hides a module dialog
 * \param ulID Window id
 */
void CMainWin::toggleWindow( int iID ) throw()
{
  if ( windowMenuPtr->isItemChecked ( iID ) )
  {
    windowMenuPtr->setItemChecked( iID, false );
    windowItemPtrMap[iID]->getModuleDialog()->hideDialog();
  }
  else
  {
    windowMenuPtr->setItemChecked( iID, true );
    windowItemPtrMap[iID]->getModuleDialog()->showDialog();
    windowItemPtrMap[iID]->getModuleDialog()->activateDialog();
  }
}

void CMainWin::toggleLogWindow() throw()
{
	if ( showLog )
	{
		logWidget->hide();
		showLog = false;
		windowMenuPtr->setItemChecked(9999,false);
	}
	else
	{
		logWidget->show();
		showLog = true;
		windowMenuPtr->setItemChecked(9999,true);
	}
}

void CMainWin::closeAllClientWindows() throw()
{
  for ( map<int,boost::shared_ptr<CPipelineItem> >::iterator it = windowItemPtrMap.begin();
    it != windowItemPtrMap.end(); ++it )
  {
    (*it).second->getModuleDialog()->hideDialog();
    windowMenuPtr->setItemChecked( (*it).first, false );
  }
}

void CMainWin::openAllClientWindows() throw()
{
FBEGIN;
  for ( map<int,boost::shared_ptr<CPipelineItem> >::iterator it = windowItemPtrMap.begin();
    it != windowItemPtrMap.end(); ++it )
  {
    (*it).second->getModuleDialog()->showDialog();
    windowMenuPtr->setItemChecked( (*it).first, true );
  }
FEND;
}

/**
 * Adds a new edge to the processing graph
 * \param iSourceID id of source module
 * \param iTargetID id of target module
 * \param iInputPort input port to use
 * \param iOutputPort output port to use
 */
void CMainWin::addGraphEdge( int iSourceID, int iTargetID, int iInputPort, int iOutputPort )
  throw()
{
DBG("New graph edge: " << iSourceID << "(" << iOutputPort << ") --> "
  << iTargetID << "(" << iInputPort << ")");
  boost::shared_ptr<CPipelineItem> sourcePtr ( modulePtrMap[iSourceID] );
  boost::shared_ptr<CPipelineItem> targetPtr ( modulePtrMap[iTargetID] );
#ifdef DEBUG
  if ( sourcePtr == NULL || targetPtr == NULL )
  {
    alog << LFATAL << SERROR("Error on creating connection") << endl;
    terminate();
  }
#endif
  boost::shared_ptr<CGraphEdge> anEdge ( new CGraphEdge( sourcePtr, targetPtr, iInputPort, iOutputPort ) );
/*  sourcePtr->addOutgoingEdge(anEdge);
  targetPtr->addIncomingEdge(anEdge);*/
	targetPtr->addConnection( sourcePtr, iInputPort, iOutputPort );  
  edgePtrVec.push_back(anEdge);
	bPipelineChanged = true;
  if ( !doNotUpdate ) process( iTargetID );
}

/**
 * Deletes an edge from the processing graph    
 * \param iSourceID id of source module
 * \param iTargetID id of target module
 * \param iInputPort input port to use
 * \param iOutputPort output port to use
 */
void CMainWin::deleteGraphEdge( int iSourceID, int iTargetID, int iInputPort, int iOutputPort )
  throw()
{
DBG("Killing graph edge: " << iSourceID << "(" << iOutputPort << ") --> "
<< iTargetID << "(" << iInputPort << ")");
  bool bFound = false;
  for( vector<boost::shared_ptr<CGraphEdge> >::iterator it = edgePtrVec.begin();
    it != edgePtrVec.end() && !bFound; ++it )
  {
#ifdef DL3
  alog << ".";
#endif
    // IDs and ports must match
    if ( ( (*it)->getSource()->getID() == static_cast<ulong>( iSourceID ) )
      && ( (*it)->getTarget()->getID() == static_cast<ulong>( iTargetID ) )
      && ( (*it)->getTargetInputNumber() == static_cast<ushort>( iInputPort ) )
      && ( (*it)->getSourceOutputNumber() == static_cast<ushort>( iOutputPort ) ) )
    {
      bFound = true;
      boost::shared_ptr<CGraphEdge> anEdge = *it;
			boost::shared_ptr<CDataSet> dummy; dummy.reset();
      // Delete the edge and set the corresponding input to zero
//       anEdge->getSource()->deleteOutgoingEdge( anEdge );
//       anEdge->getTarget()->deleteIncomingEdge( anEdge );
//       anEdge->getTarget()->setInput( dummy, iInputPort );
			anEdge->getTarget()->deleteConnection( iInputPort );
      edgePtrVec.erase( it );
    }
  }
#ifdef DL3
  alog << endl;
#endif
#ifdef DEBUG
  if (!bFound) // We haven't found that edge. This is a FATAL error that should never occur
  {
    alog << LFATAL << SERROR("Edge not found!") << endl;
    terminate();
  }
	bPipelineChanged = true;
#endif
FEND;
}

/**
 * Adds a new module to the processing graph
 */
void CMainWin::addGraphNode( CPipeItemWidget* itemWidgetPtr ) throw()
{
FBEGIN;
DBG("Adding item " << itemWidgetPtr->getID() << " " << itemWidgetPtr->getType()
	<< "/" << itemWidgetPtr->getLib() << "/" << itemWidgetPtr->getModule());
  boost::shared_ptr<CPipelineItem> itemPtr ( modules[itemWidgetPtr->getType()]
		[itemWidgetPtr->getLib()][itemWidgetPtr->getModule()]->newInstance( itemWidgetPtr->getID() ) );	
	if ( itemPtr->getType() == CPipelineItem::ITypeSource )
	{
				itemPtr->getParameters()->setString( "Path", sDataDir );
				itemPtr->apply();
	}
	// Set the parameters
	vector<string> keyList = lastParameters.getKeyList();
	CTypedMap* parameters = itemPtr->getParameters();
	for( vector<string>::iterator it = keyList.begin(); it != keyList.end(); ++it )
	{
		DS( "Setting " << *it << " to " << lastParameters.getString( *it ) );
		if ( lastParameters.getValueType( *it ) == typeid(bool) )
			parameters->setBool( *it, lastParameters.getBool( *it ) );
		else if ( lastParameters.getValueType( *it ) == typeid(long) )
			parameters->setLong( *it, lastParameters.getLong( *it ) );
		else if ( lastParameters.getValueType( *it ) == typeid(ulong) )
			parameters->setUnsignedLong( *it, lastParameters.getUnsignedLong( *it ) );
		else if ( lastParameters.getValueType( *it ) == typeid(double) )
			parameters->setDouble( *it, lastParameters.getDouble( *it ) );
		else parameters->setString( *it, lastParameters.getString( *it ) );
	}
	lastParameters.clear();
	
  // If item has a dialog insert a handle to dialog into the window menu
  if ( itemPtr->getModuleDialog() && itemPtr->getModuleDialog()->hasDialog() )
  {
    windowItemPtrMap[itemPtr->getID()] = itemPtr;
    itemPtr->getModuleDialog()->setCaption( itemWidgetPtr->getTitle().c_str() );
    windowMenuPtr->insertItem( itemWidgetPtr->getTitle().c_str(), this, SLOT( toggleWindow( int ) ),
      0, itemPtr->getID());
    windowMenuPtr->setItemChecked( itemPtr->getID(), false );
  }
  itemPtr->setModuleName( itemWidgetPtr->getTitle() );
  itemWidgetPtr->setItem( itemPtr );
  modulePtrMap[itemPtr->getID()] = itemPtr;
  if ( itemPtr->getType() == CPipelineItem::ITypeTarget )
  	targetPtrVec.push_back( itemPtr );
#ifdef DL3
//  dumpGraph();
#endif
	bPipelineChanged = true;
	//if (!doNotUpdate) process( itemPtr->getID() );
	itemPtr->attachObserver( this, CEvent::EDataChangedEvent );
/*	itemPtr->attachObserver( this, CEvent::EProgressEvent );
	itemPtr->attachObserver( this, CEvent::EProgressStartEvent );
	itemPtr->attachObserver( this, CEvent::EProgressResetEvent );*/
FEND;
}

/**
 * Deletes a module from the processing graph
 * \param ulID id of module to delete
 */
void CMainWin::deleteGraphNode( ulong ulID ) throw()
{
FBEGIN;
DBG( "Deleting node " << ulID );
  bool bErased = false;
  for ( vector<boost::shared_ptr<CPipelineItem> >::iterator it = targetPtrVec.begin();
    it != targetPtrVec.end() && !bErased; ++it )
  {
    if ( (*it)->getID() == static_cast<ulong>( ulID ) )
    {
      targetPtrVec.erase( it );
      bErased = true;
    }
  }
  
  boost::shared_ptr<CPipelineItem> aNode = modulePtrMap[ulID];
  modulePtrMap.erase( ulID );
  if ( aNode->getModuleDialog() && aNode->getModuleDialog()->hasDialog() )
  {
    aNode->getModuleDialog()->hideDialog();
    windowMenuPtr->removeItem( aNode->getID() );
    windowItemPtrMap.erase( aNode->getID() );
  }
  //delete aNode;
	bPipelineChanged = true;
FEND;
}

/** Debugging function to print out the whole processing pipeline */
void CMainWin::dumpGraph() throw()
{
  cerr << "--- PROCESSING GRAPH DUMP --- BEGIN ---" << endl;
/*  cerr << "+++ Source List +++" << endl;
  for( vector<boost::shared_ptr<CPipelineItem> >::iterator it = sourcePtrVec.begin();
    it != sourcePtrVec.end(); ++it )
  {
    cerr << " (" << (*it)->getID() << ") " << (*it)->getModuleName() << endl;
  }*/
  
  cerr << endl << "+++ Node List +++" << endl;
  for( map<ulong,boost::shared_ptr<CPipelineItem> >::iterator it = modulePtrMap.begin();
    it != modulePtrMap.end(); ++it )
  {
    cerr << " (" << (*it).second->getID() << ") " << (*it).second->getModuleName() << endl;
  }
  cerr << endl << "+++ Connection list +++" << endl;
  
  for( vector<boost::shared_ptr<CGraphEdge> >::iterator it = edgePtrVec.begin();
    it != edgePtrVec.end(); ++it )
  {
    cerr << " (" << (*it)->getSource()->getID() << ")[" << (*it)->getSourceOutputNumber()
      << "] -> (" << (*it)->getTarget()->getID() << ")[" << (*it)->getTargetInputNumber()
      << "]" << " ( " << (*it)->getActive() << " ) " << endl;
  }
  
  cerr << "--- PROCESSING GRAPH DUMP ---  END  ---" << endl;
}

/**
 * Start the processing with the module that is identified by the function parameter
 * \param ulID id of the module to be processed
 */
void CMainWin::process( ulong ulID ) throw()
{
/*cerr << "Updating " << modulePtrMap[ulID].get()->getModuleName() << endl;
	modulePtrMap[ulID].get()->update();*/
	if ( currentlyRunning )
		return;
	currentlyRunning = true;
	for( std::vector<TPipelineItemPtr>::iterator it = targetPtrVec.begin(); it != targetPtrVec.end(); ++it )
	{
cerr << "Updating " << (*it)->getModuleName() << endl;
		(*it)->update();
	}
	currentlyRunning = false;
}


/**
 * Rename the given module with the given name
 * \param ulID module id
 * \param sName new module name
 */
void CMainWin::moduleRenamed( ulong ulID, string sName ) throw()
{
  modulePtrMap[ulID]->setModuleName( sName );
  if ( modulePtrMap[ulID]->getModuleDialog() && modulePtrMap[ulID]->getModuleDialog()->hasDialog() )
    modulePtrMap[ulID]->getModuleDialog()->setCaption( sName );
  if ( modulePtrMap[ulID]->getModuleDialog() && modulePtrMap[ulID]->getModuleDialog()->hasDialog() )
  	windowMenuPtr->changeItem( ulID, sName.c_str() );
	bPipelineChanged = true;
}

void CMainWin::newPipeline() throw()
{
	if ( bPipelineChanged )
	{
		int ret = QMessageBox::question( this, "Pipeline not saved", 
			"The current pipeline was modified since the last save. Save it now?", 
			"&O.K.", "&Cancel loading", "&Discard changes", 0, 1 );
		switch( ret )
		{
			case 0:
				savePipeline(); break;
			case 1:
				return; break;				
		}
	}
	pipelineViewPtr->deletePipeline();
}

/** Load a new processing pipeline */
void CMainWin::openPipeline() throw()
{
	// Maybe you should save your unsaved files first
	if ( bPipelineChanged )
	{
		int ret = QMessageBox::question( this, "Pipeline not saved", 
			"The current pipeline was modified since the last save. Save it now?", 
			"&O.K.", "&Cancel loading", "&Discard changes", 0, 1 );
		switch( ret )
		{
			case 0:
				savePipeline(); break;
			case 1:
				return; break;				
		}
	}
  QString sTmp = QFileDialog::getOpenFileName(
                    sPipelineDir.c_str(),
                    "AIPS Pipelines (*.aips)",
                    this,
                    "Open File Dialog"
                    "Choose a pipeline to open" );
  if ( sTmp == "" )
    return;
	string sFilename = sTmp.ascii();
	// Check if filename has the correct extension. Append it if not
	DS( "Found at " << sFilename.find_last_of( ".aips" ) << " l-1 is " << sFilename.length()-1 );
	if ( sFilename.find_last_of( ".aips" ) != sFilename.length()-1 )
		sFilename += ".aips";
  
DBG("Loading " << sFilename );
  // Check if dialog was cancelled
	try
	{
		CXMLInterpreter xml( sFilename );
	pipelineViewPtr->deletePipeline();
	if ( xml.getTag() == "?xml" )
	{
		xml.nextTag();
		if ( xml.getTag() == "pipeline" )
		{
			xml.nextTag();
			bool stop = false;
			while ( !stop )
			{
				
				xml.nextTag();
				if ( xml.getTag() != "module" )
				{
					stop = true;
				}
				else
				{					   
					xml.nextTag();	ulong ulID = lexical_cast<ulong>( xml.getContent() );
					xml.nextTag();	uint uiType = lexical_cast<uint>( xml.getContent() );
					xml.nextTag(); string sIDStr = xml.getContent();
					xml.nextTag(); string sName = xml.getContent();
					xml.nextTag(); 
					while( xml.getTag() == "parameter" )
					{
						string paramName = xml.getParam( 0 );
						string paramType = xml.getParam( 1 );
						string paramValue = xml.getContent();
						DS( "Param " << paramName << " type " << paramType << " value " << paramValue );
						if ( paramType == "bool" )
							lastParameters.setBool( paramName, lexical_cast<int>( paramValue ) );
						else if ( paramType == "long" )
							lastParameters.setLong( paramName, lexical_cast<long>( paramValue ) );
						else if ( paramType == "ulong" )
							lastParameters.setUnsignedLong( paramName, lexical_cast<ulong>( paramValue ) );
						else if ( paramType == "double" )
							lastParameters.setDouble( paramName, lexical_cast<double>( paramValue ) );
						else
							lastParameters.setString( paramName, paramValue );							
						xml.nextTag(); 
					}
					xml.nextTag(); uint uiX = lexical_cast<uint>( xml.getContent() );
					xml.nextTag(); uint uiY = lexical_cast<uint>( xml.getContent() );
DS( sName << " " << sIDStr << " " << ulID << " " << uiType << " " << uiX << ";" << uiY );
					doNotUpdate = true;
					importModule( sIDStr, sName, ulID, uiType, uiX, uiY );
					doNotUpdate = false;					
					xml.nextTag(); // </geometry>
					xml.nextTag(); // </module>
				}
			}
DS( "This should be </section>	" << xml.getTag() );
			xml.nextTag();			
DS( "This should be <section>	" << xml.getTag() );				
			stop = false;
			while ( !stop )
			{
				
				xml.nextTag();
				if ( xml.getTag() != "connection" )
				{
					stop = true;
				}
				else
				{		
					xml.nextTag();	xml.nextTag();	
					ulong ulSourceID = lexical_cast<ulong>( xml.getContent() ); xml.nextTag();	
					ushort usSourcePort = lexical_cast<ushort>( xml.getContent() );
					xml.nextTag();	xml.nextTag();	xml.nextTag();	
					ulong ulTargetID = lexical_cast<ulong>( xml.getContent() ); xml.nextTag();	
					ushort usTargetPort = lexical_cast<ushort>( xml.getContent() );
					doNotUpdate = true;
					pipelineViewPtr->addConnection( ulSourceID, usSourcePort, ulTargetID, usTargetPort );
					doNotUpdate = false;
					xml.nextTag(); 
					xml.nextTag(); 
DS( ulSourceID << " " << usSourcePort << " " << ulTargetID << " " << usTargetPort  );
				}
			}			
DS( "This should be </section>	" << xml.getTag() );
			xml.nextTag();			
DS( "This should be </pipeline>	" << xml.getTag() );				
			std::string sCaption = " AIPS - (" + sFilename.substr( sFilename.find_last_of( "/" ) + 1 ) 
				+ ")";
			setCaption( sCaption );
			bPipelineChanged = false;
			return;
		}
	}
	}
	catch ( FileException &e )
	{
		alog << LERR << e.what() << endl; 
		return;
	}
	
	
	QMessageBox::warning(this, "File error","Pipeline file is not valid",
		QMessageBox::Ok, QMessageBox::NoButton);
	return;
	
}

/** Save the processing pipeline */
void CMainWin::savePipeline() throw()
{
  // Produce a graph dump that will be put into an ascii file 
  QString tmpStr = QFileDialog::getSaveFileName(
                    sPipelineDir.c_str(),
                    "AIPS Pipelines (*.aips)",
                    this,
                    "save file dialog"
                    "Choose a filename to save under" ).ascii();
  // Check if dialog was cancelled
  if ( tmpStr == "" )
    return;

  string sFilename = tmpStr.ascii();
	if ( sFilename.find_last_of( ".aips" ) != sFilename.length()-1 )
		sFilename += ".aips";
			
	// Time to drop some XML
  ofstream os( sFilename.c_str(), ios::out);	
  if ( os.is_open() )
  {
		// Write header
		os << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl
			<< "<pipeline source=\"aipsgui\" version=\"0.5\">" << endl
			<< " <section name=\"modules\">" << endl;
		// Write modules
    for( std::map<ulong, boost::shared_ptr<CPipelineItem> >::iterator it = modulePtrMap.begin();
      it != modulePtrMap.end(); ++it )
    {
			os << "  <module>" << endl;
			os << "   <id>" << (*it).second->getID() << "</id>" << endl;
			os << "   <type>" << (*it).second->getType() << "</type>" << endl;
			os << "   <moduleid>" << (*it).second->getModuleID() << "</moduleid>" << endl;
			os << "   <name>" << (*it).second->getModuleName() << "</name>" << endl;
			CTypedMap* parameters = (*it).second->getParameters();
			vector<string> keyList = parameters->getKeyList();
			for( vector<string>::iterator keyIt = keyList.begin(); keyIt != keyList.end(); ++keyIt )
			{
				if ( keyIt->find("Default") != string::npos 
					|| keyIt->find("Minimum") != string::npos 
					|| keyIt->find("Maximum") != string::npos )
					continue;
				os << "   <parameter name=\"" << *keyIt << "\" type=\"";
				if ( parameters->getValueType( *keyIt ) == typeid( bool ) )
					os << "bool\">";
				else if ( parameters->getValueType( *keyIt ) == typeid( long ) )
					os << "long\">";
				else if ( parameters->getValueType( *keyIt ) == typeid( ulong ) )
					os << "ulong\">";
				else if ( parameters->getValueType( *keyIt ) == typeid( double ) )
					os << "double\">";
				else
					os << "string\">";
				os << parameters->getString( *keyIt ) << "</parameter>" << endl;
			}
			os << "   <geometry>" << endl << "    <xpos>";
      QPoint pos = pipelineViewPtr->getItemPosition( (*it).second->getID() );
      os << pos.x() << "</xpos>" << endl << "    <ypos>" << pos.y() << "</ypos>" << endl;
			os << "   </geometry>" << endl << "  </module>" << endl;
    }
		// Write end of module section and begin
		os << " </section>" << endl << " <section name=\"connections\">" << endl;
		// Write connections
    for( std::vector<boost::shared_ptr<CGraphEdge> >::iterator it = edgePtrVec.begin();
      it != edgePtrVec.end(); ++it )
    {
			os << "  <connection>" << endl;
			os << "   <source>" << endl << "    <id>" << (*it)->getSource()->getID() << "</id>" << endl;
			os << "    <port>" << (*it)->getSourceOutputNumber() << "</port>" << endl; 
			os << "   </source>" << endl << "   <target>" << endl << "    <id>" 
				<< (*it)->getTarget()->getID() << "</id>" << endl;
			os << "    <port>" << (*it)->getTargetInputNumber() << "</port>" << endl; 
      os << "   </target>" << endl << "  </connection>" << endl;
    }
		// Write file suffix
		os << " </section>" << endl << "</pipeline>" << endl;
    os.close();
		std::string sCaption = " AIPS - ( " + sFilename.substr( sFilename.find_last_of( "/" ) ) + " )";
		setCaption( sCaption );		
		bPipelineChanged = false;
  }
}

void CMainWin::showplugindialog()
{
	dialog->setPath( sPluginDir );
	dialog->exec();
	writeConfigFile();
}

void CMainWin::loadConfigFile()
{
	string filename = sConfigDir;
	filename += "/.aips.cfg";
	try
	{
		CXMLInterpreter xml( filename );
		if ( xml.getTag() == "?xml" )
		{
			xml.nextTag();
			if ( xml.getTag() == "config" )
			{			
				bool stop = false;
				while ( !stop )
				{					
					xml.nextTag();
					if ( xml.getTag() == "plugin" )
					{
						loadPlugin( xml.getContent() );
						loadedPlugIns.back().path = xml.getContent();
					}
					else
						stop = true;
				}
			}
			return;
		}
		alog << LWARN << "Illegal format for config file" << endl;
	}
	catch ( CException &e )
	{
		alog << LERR << e.what() << endl;
	}
}

void CMainWin::writeConfigFile()
{
	string filename = sConfigDir;
	filename += "/.aips.cfg";
	if ( ::access( filename.c_str(), F_OK ) == 0 )
		if ( ::remove( filename.c_str() ) != 0 )
		{
			alog << LWARN << "Could not erase old config" << endl;
			return;		
		}	
	ofstream theFile( filename.c_str() );
	if ( !theFile.is_open() )
	{
		alog << LWARN << "Could not create config file" << endl;
		return;
	}
	theFile << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl
			<< "<config application=\"aipsgui\" version=\"0.5\">" << endl;
	for( vector<SLibItem>::iterator it = loadedPlugIns.begin(); it != loadedPlugIns.end(); ++it )
	{
		theFile << " <plugin>" << it->path << "</plugin>" << endl;
	}
	theFile << "</config>";
	theFile.close();
}

void CMainWin::loadRuntimePlugin( const std::string& l )
{
	for( vector<SLibItem>::iterator it = loadedPlugIns.begin(); it != loadedPlugIns.end(); ++it )
	{
		if ( it->path == l )
		{
			QMessageBox::warning(this, "Plugin Warning","Cannot load the same plugin twice",
				QMessageBox::Ok, QMessageBox::NoButton);
				return;
		}
	}
	try
	{
		loadPlugin( l );
	}
	catch ( CException &e )
	{
		if ( e.getExceptionType() == CException::FATAL )
		{
			cerr << LFATAL << e.what() << endl; terminate();
		}
		else
			return;
	}
	loadedPlugIns.back().path = l;
	dialog->addItem( loadedPlugIns.back() );
}

void CMainWin::removeRuntimePlugin( const SLibItem& l )
{
	/* FIXME This could be solved much more efficiently */
	for( vector<SLibItem>::iterator it = loadedPlugIns.begin(); it != loadedPlugIns.end(); ++it )
	{
		DBG("Checking lib " << it->library);
		if ( (*it) == l )
		{
			if ( (*it).isHandlerLib == false )
			{
				// Remove all modules of the plugin
				for ( ushort i = 0; i < 4; ++i )					
				{
					vector<vector<pipelineItemPtr> >::iterator it = modules[i].begin();
					while( it != modules[i].end() )
					{
						// Check if module id string is equal to library version and name
 						vector<string> compound;
						split( compound, (*it)[0]->getModuleID(), is_any_of("/") );
						string libname = compound[2];
						string libver = compound[3];
						if ( libname == l.library && libver == l.version )
						{							
							it = modules[i].erase( it );
						}					
						else
							++it;
					}							
				}
				// Remove toolbar entry
				for ( int i = 0; i < filtersToolBoxPtr->count(); ++i )
					if( filtersToolBoxPtr->itemLabel(i) == l.library )
					{	filtersToolBoxPtr->removeItem( filtersToolBoxPtr->item(i) ); break; }
				for ( int i = 0; i < convertersToolBoxPtr->count(); ++i )
					if( convertersToolBoxPtr->itemLabel(i) == l.library )
					{	convertersToolBoxPtr->removeItem( convertersToolBoxPtr->item(i) ); break; }
				for ( int i = 0; i < sourcesToolBoxPtr->count(); ++i )
					if( sourcesToolBoxPtr->itemLabel(i) == l.library )
					{	sourcesToolBoxPtr->removeItem( sourcesToolBoxPtr->item(i) ); break; }
				for ( int i = 0; i < targetsToolBoxPtr->count(); ++i )
					if( targetsToolBoxPtr->itemLabel(i) == l.library )
					{	targetsToolBoxPtr->removeItem( targetsToolBoxPtr->item(i) ); break; }
				for ( int i = 0; i < othersToolBoxPtr->count(); ++i )
					if( othersToolBoxPtr->itemLabel(i) == l.library )
					{	othersToolBoxPtr->removeItem( othersToolBoxPtr->item(i) ); break; }				
			}
			else
			{
				vector<SFileHandlerData>::iterator it = loadedHandlers.begin();
				while( it != loadedHandlers.end() )					 
				{
					if ( (*it).sLibName == ( l.library + "/" + l.version ) )
					{
						try
						{
							getFileServer().removeHandler( (*it).handle );
						}
						catch ( CException &e )
						{
							cerr << LFATAL << e.what() << endl; terminate();
						}
						it = loadedHandlers.erase( it );
					}
					else
					{
						++it;
					}
				}
			}
//			DBG("Prepare to unload" << (int)((*it).lib) << " " << (*it).lib->library());
			// Unload library
			dlclose( (*it).lib );
			//(*it).lib->unload();
			//delete (*it).lib;
			loadedPlugIns.erase( it );
			DBG("Unregistered library");
			writeConfigFile();
			return;
		}
	}
	DBG(" Not found ");
}

/**
 * Load plugins and initialize all modules
 * This method uses the file <aips.cfg> located in the working directory to determine
 * which libraries to load. The file is build up in the following structure
 * line 1: The number n of libraries to load
 * line 2: The first library to load (full absolute path name)
 * ...
 * line n+1: The n-th library to load (full absolute path name)
 * \exception Throws FileException if lib files cannot be loaded
 * \exception Throws PlugInException if library seems to be corrupted or invalid
 */
void CMainWin::loadPlugin( const std::string& sFilename )
{
	typedef uint ( getNoOfModules )();
  typedef pipelineItemPtr ( instantiateModuleByNumber )( uint );
	typedef pipelineItemPtr ( instantiateModuleByString )( string );
	typedef fileHandlerPtr ( instantiateHandlerByNumber )( uint );
	typedef fileHandlerPtr ( instantiateHandlerByString )( string );
	typedef string ( getLibraryID )();
	typedef string ( getLibraryDoc )();
	typedef void ( initFactory )();
	typedef void ( unloadFactory )();
	
	uint usModulesInLib;
	SLibItem tmpLibID;
DBG("Loading library <" << sFilename << ">");	
	void* plugIn;
	plugIn = dlopen( sFilename.c_str(), RTLD_LAZY | RTLD_GLOBAL );
//   QLibrary* plugIn = new QLibrary( sFilename );
//   plugIn->load();
	
	char* errString = dlerror();
  if ( /* !plugIn->isLoaded()*/ errString != NULL )
  {
		throw ( FileException( SERROR( errString ), CException::FATAL ) );
  }
  else
  {
    alog << LINFO << "Plugin library " << sFilename << " loaded." << endl;
		//initFactory* ptrToInitFactoryFunc = (initFactory*) plugIn->resolve("initFactory");
		initFactory* ptrToInitFactoryFunc = (initFactory*) dlsym( plugIn, "initFactory");
		ptrToInitFactoryFunc();
//     getNoOfModules* ptrToModuleNumberFunc =
//       (getNoOfModules*) plugIn->resolve("numberOfModules");
		getNoOfModules* ptrToModuleNumberFunc = (getNoOfModules*) dlsym( plugIn, "numberOfModules");
    if ( ptrToModuleNumberFunc == NULL )
    {
			// Hey, thats not one of ours!
      QMessageBox::warning( this, "Warning", "The selected library is not a valid AIPS plugin",
				QMessageBox::Ok, QMessageBox::NoButton );
			//delete plugIn;
			dlclose( plugIn );
			throw ( PlugInException( SERROR( "The selected library is not a valid AIPS plugin" ),
				CException::RECOVER ) );
    }
// 		getLibraryID* ptrToLibIDFunc = 
// 			(getLibraryID*) plugIn->resolve("libraryID");
		getLibraryID* ptrToLibIDFunc = (getLibraryID*) dlsym(plugIn, "libraryID");
		if ( ptrToLibIDFunc == NULL )
		{
			alog << LWARN << "Library doesn't provide ID string!" << endl;
			tmpLibID.library = "Unknown";
			tmpLibID.version = "Unknown";
		}
		else
		{
			tmpLibID.library = ptrToLibIDFunc();
			tmpLibID.version = tmpLibID.library.substr( tmpLibID.library.find("/",0)+1, 
			tmpLibID.library.length() - tmpLibID.library.find("/",0) );
			tmpLibID.library = tmpLibID.library.substr( 0, tmpLibID.library.find("/",0) );
		}
/*		getLibraryDoc* ptrToLibDocFunc = 
			(getLibraryDoc*) plugIn->resolve("libraryDoc");*/
		getLibraryDoc* ptrToLibDocFunc = (getLibraryDoc*) dlsym(plugIn,"libraryDoc");

		if ( ptrToLibDocFunc == NULL )
		{
			alog << LWARN << "Library doesn't provide doc string!" << endl;
			tmpLibID.documentation = "Unknown";
		}
		else
			tmpLibID.documentation = ptrToLibDocFunc();			
    usModulesInLib = ptrToModuleNumberFunc();
    alog << LINFO << "[" << __PRETTY_FUNCTION__ << "]\n Found " << usModulesInLib
      << " modules in " << sFilename << endl;			
  }
  // Create one instance of every filter we've found
/*  instantiateModuleByNumber* ptrToInstallModuleFunc =
    (instantiateModuleByNumber*) plugIn->resolve("createModuleByNumber");*/
  instantiateModuleByNumber* ptrToInstallModuleFunc =
    (instantiateModuleByNumber*) dlsym(plugIn,"createModuleByNumber");
/*	errString = dlerror();
  if ( errString != NULL )
  {
		throw ( FileException( SERROR( errString ), CException::FATAL ) );
  }*/
  
	// Try if this is a module plugin
  if ( ptrToInstallModuleFunc != NULL )
  {  
cerr << "1" << endl;
		int libCount[] = {filtersToolBoxPtr->count(),convertersToolBoxPtr->count(),
			sourcesToolBoxPtr->count(),targetsToolBoxPtr->count(), othersToolBoxPtr->count()};
		CDragListBox* availFiltersPtr = new CDragListBox( 0, libCount[0] );
  	CDragListBox* availConvertersPtr = new CDragListBox( 1, libCount[1] );
  	CDragListBox* availSourcesPtr = new CDragListBox( 2, libCount[2] );
  	CDragListBox* availTargetsPtr = new CDragListBox( 3, libCount[3] );
		CDragListBox* otherModulesPtr = new CDragListBox( 4, libCount[4] );
		tmpLibID.lib = plugIn;	
		tmpLibID.isHandlerLib = false;
		loadedPlugIns.push_back( tmpLibID );
cerr << "2" << endl;		
  	for ( uint i = 0; i < usModulesInLib; i++ )
	  {
cerr << "Register module " << i << " / " << usModulesInLib << endl;	  	
  	  pipelineItemPtr anItem = ptrToInstallModuleFunc( i );
cerr << "Gna!" << anItem->getType() << endl;  	  
			if ( anItem->getType() > 999 ) anItem->setType( anItem->getType() / 1000 );
			modules[anItem->getType()  - 1].resize( libCount[anItem->getType()  - 1] + 1,
vector<pipelineItemPtr>() );
			//DS( "Modules " << modules[anItem->getType()  - 1].size() );
cerr << "Gne!" << endl;			
    	modules[anItem->getType()  - 1][libCount[anItem->getType()  - 1]].push_back( anItem );
cerr << "Gno!" << endl;    	
			DS(" Added Item " << anItem->getType()  - 1 << "/" << libCount[anItem->getType()  - 1]
				<< "/" << modules[anItem->getType()  - 1][libCount[anItem->getType()  - 1]].size()-1);
			//DS( "Pushed back a new item ");
cerr << "Gnu!" << endl;			
	    if ( anItem->getModuleDialog() && anItem->getModuleDialog()->hasDialog() )
  	    anItem->getModuleDialog()->hideDialog();
    	alog << LINFO << "[" << __PRETTY_FUNCTION__ << "]\n Registered module "
      	<< anItem->getModuleName() << endl;
 	    switch( anItem->getType() )
   	  {
				case 1:
       	  availFiltersPtr->insertItem( QString( anItem->getModuleName().c_str() ) );
        	anItem->setType( 1000 + availFiltersPtr->count() );
	        break;
  	    case 2:
    	    availConvertersPtr->insertItem( QString( anItem->getModuleName().c_str() ) );
      	  anItem->setType( 2000 + availConvertersPtr->count() );
        	break;
	      case 3:
  	      availSourcesPtr->insertItem( QString( anItem->getModuleName().c_str() ) );
    	    anItem->setType( 3000 + availSourcesPtr->count() );					
      	  break;
	      case 4:
  	      availTargetsPtr->insertItem( QString( anItem->getModuleName().c_str() ) );
    	    anItem->setType( 4000 + availTargetsPtr->count() );
      	  break;
	      default:
  	      alog << LWARN << "Unknown module type" << endl;
     	}								
  	}
cerr << "3" << endl;  	
		// Now delete all Boxes which are empty and add the others to the toolboxes
		
		if ( availFiltersPtr->count() > 0 )
		{
			filtersToolBoxPtr->addItem( availFiltersPtr, tmpLibID.library.c_str() );
		  connect( availFiltersPtr, SIGNAL( dragItem( int, int, int ) ),
    		this, SLOT( newDragItem( int , int, int ) ) );			
		}
		else
			delete availFiltersPtr;
		if ( availConvertersPtr->count() > 0 )
		{
			convertersToolBoxPtr->addItem( availConvertersPtr, tmpLibID.library.c_str() );
		  connect( availConvertersPtr, SIGNAL( dragItem( int, int, int ) ),
    		this, SLOT( newDragItem( int , int, int ) ) );
		}
		else
			delete availConvertersPtr;
		if ( availSourcesPtr->count() > 0 )
		{
			sourcesToolBoxPtr->addItem( availSourcesPtr, tmpLibID.library.c_str() );
			connect( availSourcesPtr, SIGNAL( dragItem( int, int, int ) ),
    		this, SLOT( newDragItem( int , int, int ) ) );
		}
		else 
			delete availSourcesPtr;
		if ( availTargetsPtr->count() > 0 )
		{
			targetsToolBoxPtr->addItem( availTargetsPtr, tmpLibID.library.c_str() );
			connect( availTargetsPtr, SIGNAL( dragItem( int, int, int ) ),
    		this, SLOT( newDragItem( int , int, int ) ) );
		}
		else
			delete availTargetsPtr;
		if ( otherModulesPtr->count() > 0 )
		{
			othersToolBoxPtr->addItem( otherModulesPtr, tmpLibID.library.c_str() );
		  connect( otherModulesPtr, SIGNAL( dragItem( int, int, int ) ),
	  	  this, SLOT( newDragItem( int , int, int ) ) );			
		}
		else
			delete otherModulesPtr;
	} /* END if ( ptrToInstallModuleFunc != NULL ) */
	else 
	{
		alog << LINFO << "Plugin seems to provide file handlers" << endl;
		// Create one instance of every filter we've found
/*  	instantiateHandlerByNumber* ptrToInstallHandlerFunc =
    (instantiateHandlerByNumber*) plugIn->resolve("createHandlerByNumber");*/
  	instantiateHandlerByNumber* ptrToInstallHandlerFunc =
    (instantiateHandlerByNumber*) dlsym( plugIn,"createHandlerByNumber");
		if ( ptrToInstallHandlerFunc == NULL )
			throw ( PlugInException( SERROR( "Error registering plugins" ), CException::FATAL ) );
		tmpLibID.lib = plugIn;
		tmpLibID.isHandlerLib = true;
		SFileHandlerData tmpHandler;
		tmpHandler.sLibName = tmpLibID.library+"/"+tmpLibID.version;		
  	for ( uint i = 0; i < usModulesInLib; i++ )
	  {			
  	  fileHandlerPtr anItem = ptrToInstallHandlerFunc( i );
    	alog << LINFO << "[" << __PRETTY_FUNCTION__ << "]\n Registered handler for "
      	<< anItem->legalFileTypes() << endl;
			tmpHandler.handle = anItem;
			loadedHandlers.push_back( tmpHandler );
			getFileServer().addHandler( anItem );
  	}
		loadedPlugIns.push_back( tmpLibID );
	}
}

void CMainWin::about()
{
	QMessageBox::about( this, "About AIPS 0.2", "The Automatic Image Processing "
		"System\n\n(c)2003-2004 by the people listed in the AUTHORS file\n"
		"Released under the terms of the GPL" );
}

void CMainWin::aboutQt()
{
	QMessageBox::aboutQt( this );
}


/*!
    \fn CMainWin::importModule( string, string, ulong, uint, uint, uint )
 */
void CMainWin::importModule( string& sIDStr, string& sName, ulong ulID, 
	uint uiType, uint uiX, uint uiY )
{
  vector<string> sIDVec;
	split( sIDVec, sIDStr, is_any_of(" /") );	
	// Possible values for usFound
  // 0 - nothing found at all
  // 1 - found matchinbg module name with wrong version and library
  // 2 - found matching module name with wrong version
  // 4 - found matching module name with wrong library
  // 5 - found perfect match
  ushort usFound = 0;
  ushort usOldFound = 0;
	ulong ulType = 0;
	ulong ulLib = 0;
	ulong ulModule = 0;
  pipelineItemPtr tmpModulePtr; tmpModulePtr.reset();
  pipelineItemPtr oldModulePtr; oldModulePtr.reset();
  for( uint i = 0; i < 4 && usFound < 5; ++i )
    for( uint j = 0; j < modules[i].size() && usFound < 5; ++j )
		for( uint k = 0; k < modules[i][j].size() && usFound < 5; ++k )
    {
 			vector<string> sActModuleID;
			split( sActModuleID, modules[i][j][k]->getModuleID(), is_any_of(" /") );
      oldModulePtr = tmpModulePtr;
      usOldFound = usFound;
      if ( sIDVec[0] == sActModuleID[0] )
      {
      	tmpModulePtr = modules[i][j][k];
				ulType = i; ulLib = j; ulModule = k;
        usFound = 1;
        if ( sIDVec[2] == sActModuleID[2] )
        	usFound += 1;
        if ( sIDVec[1] == sActModuleID[1] )
        	usFound += 3;
      }
      if ( usOldFound > usFound )
      {
      	usFound = usOldFound;
      	tmpModulePtr = oldModulePtr;
      }
    }
alog << LFIXME << "This version handling is rather basic" << endl;        
    switch ( usFound )
    {
    	case 1:
      	alog << LWARN << "Found module with correct name, altough module version "
        	<< "and library do not match! This may fail!";
      break;
      case 2:
      	alog << LWARN << "Found module with correct name and library, altough module version "
        	<< "does not match! This may fail!" << endl;
        break;
      case 4:
      	alog << LWARN << "Found module with correct name and module version, "
        	<< "altough library does not match! This may fail!" << endl;
        break;
#ifdef DEBUG
          case 5:
            DBG("Perfect match for module");
            break;
#endif            
      default:
      	alog << LERR << "I cannot find the correct module in any "
        	<< "library, aborting load..." << endl;
				pipelineViewPtr->deletePipeline();
        return;
    }		
    pipelineViewPtr->addItem( ulID, tmpModulePtr.get(), uiX, uiY, sName, ulType, ulLib, ulModule );
DBG("Node id " << ulID << " Type " << uiType << " idStr " << sIDStr << " x " << uiX
  << " y " << uiY );	 
}

void CMainWin::closeAips()
{
	// Maybe you should save your unsaved files first
	if ( bPipelineChanged )
	{
		int ret = QMessageBox::question( this, "Pipeline not saved", 
			"The current pipeline was modified since the last save. Save it now?", 
			"&O.K.", "&Cancel quitting", "&Discard changes", 0, 1 );
		switch( ret )
		{
			case 0:
				savePipeline(); break;
			case 1:
				return; break;				
		}
	}	
	close();
}
