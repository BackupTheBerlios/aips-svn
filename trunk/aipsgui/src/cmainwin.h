/***********************************************************************
 * File: cmainwin.h                                                    *
 * Project: AIPS                                                       *
 * Description: The main window of the AIPS application                *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Version: 0.5                                                        *
 * Status : Alpha                                                      *
 * Created: 25.08.03                                                   *
 * Changed: 04.09.03 Standardized variable names                       *
 *          12.09.03 Exceptions added, attributes are now private      *
 *                   Constructor initializes constant parameters       *
 *                   Creates initial file dialog for dataset loading   *
 *                   Open datasets now uses the current slice as       *
 *                    standard slice instead of hardcoded slice 90     *
 *          16.09.03 Only shows a filter dialog if it has one          *
 *                   and filter is in pipeline                         *
 *                   Window caption now shows filename of dataset      *
 *          29.09.03 Window menu added, Image source added             *
 *          08.10.03 Added excution check box                          *
 *          16.01.04 Corrected an error in loadPipeline and            *
 *                   savePipeline that produced a crash                *
 *          22.01.04 Started to simplify oder overwork source code     *
 *          26.01.04 File handlers are now also plugins                *
 *                   Config files are now stored via nconfig           *
 ***********************************************************************/     

#ifndef CMAINWIN_H
#define CMAINWIN_H

// Standard includes
#include <queue>
#include <map>
#include <algorithm>
#include <cstdio>

// boost includes
#include <boost/shared_ptr.hpp>

// AIPS includes
#include <clog.h>
#include <cfilter.h>
#include <cgraphedge.h>
#include <cdatafileserver.h>
#include "cdraglistbox.h"
#include "cpipescroller.h"
#include <cglobalprogress.h>
#include "cplugindialog.h"
#include <aipsmacros.h>
#include "cxmlinterpreter.h"
#include <aipsstring.h>

// Qt includes
#include <qmainwindow.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qscrollbar.h>
#include <qlcdnumber.h>
#include <qfiledialog.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlibrary.h>
#include <qfile.h>
#include <qcheckbox.h>
#include <qtabwidget.h>
#include <qprogressbar.h>
#include <qtoolbox.h>
#include <qtextedit.h>
#include <qapplication.h>

using namespace aips;
using namespace boost;

struct SFileHandlerData
{
	boost::shared_ptr<CFileHandler> handle;
	string sLibName;
};

/** The main window of the AIPS application */
class CMainWin : public QMainWindow, public CObserver, public CSubject
{
  Q_OBJECT
private:
  /// Standard constructor
  CMainWin();
  /// Assignment operator
  CMainWin& operator=( CMainWin& );  
public:
  /// Start the processing with the given module 
  void process( ulong ulID )
    throw();
/* Structors */
  /// Constructor
  /// Copy constructor
  CMainWin( const CMainWin& );
  CMainWin( QWidget *parentPtr = NULL, const char *sName = "AIPS" )
    throw();
  /// Class destructor 
  ~CMainWin()
    throw();
  void importModule( string&, string&, ulong, uint, uint, uint );
	void logString( const char* text, size_t len );
	virtual void execute( shared_ptr<CEvent> anEvent )
	{
		if ( anEvent->getType() == CEvent::EDataChangedEvent )
		{
			CPipelineItem* p = static_cast<CPipelineItem*>( anEvent->getGenerator() );
			if ( p )
				process( p->getID() );
			else
				cerr << "Could not retrieve pipeline item" << endl;
		}
#ifndef NOPROGRESS
		else if ( anEvent->getType() == CEvent::EProgressResetEvent )
		{			
			currentProgress->reset();
		}
		else if ( anEvent->getType() == CEvent::EProgressStartEvent )
		{
			CProgressStartEvent* tmpEvent = static_cast<CProgressStartEvent*>( anEvent.get() );
			if ( tmpEvent )
				currentProgress->setTotalSteps( tmpEvent->getMaxProgress() );
		}
		else if ( anEvent->getType() == CEvent::EProgressEvent )
		{
			CProgressEvent* tmpEvent = static_cast<CProgressEvent*>( anEvent.get() );
			if ( tmpEvent )
				currentProgress->setProgress( tmpEvent->getValue() );
		}
#endif
	}
private slots:
	void closeAips();
	void about();
	void aboutQt();
	void removeRuntimePlugin( const SLibItem& l );
	void loadRuntimePlugin( const std::string& l );
	void showplugindialog();
  /// Create a new drag item from the selected list box entry
  void newDragItem( int iWhichType, int iWhichLib, int iWhichModule )
    throw();
  /// Shows or hides a module dialog
  void toggleWindow( int iID )
    throw();
  /// Closes all client windows 
  void closeAllClientWindows() throw();
	/// Shows all client windows 
  void openAllClientWindows() throw();
	/// Show or hide the logging window
	void toggleLogWindow() throw();
  /// Adds a new edge to the processing graph
  void addGraphEdge( int iSourceID, int iTargetID, int iInputPort, 
		int iOutputPort ) throw();
  /// Deletes an edge from the processing graph    
  void deleteGraphEdge( int iSourceID, int iTargetID, int iInputPort, 
		int iOutputPort ) throw();
  /// Adds a new module to the processing graph
  void addGraphNode( CPipeItemWidget* itemWidgetPtr )
    throw();
  /// Deletes a module from the processing graph
  void deleteGraphNode( ulong ulID )
    throw();
  /// Rename the given module with the given name
  void moduleRenamed( ulong ulID, std::string sName )
    throw();
  /// Process the item of the given id
  void processModule( ulong ulID )
    throw();
  /// Load a new processing pipeline
  void openPipeline() 
		throw();
  /// Save the processing pipeline
  void savePipeline() 
		throw();
	/// Create a new pipeline
	void newPipeline() 
		throw();

private:
	std::string encodeString( const std::string &s );
	
  /// Load plugins and initialize all modules
	void loadConfigFile();
	void writeConfigFile();
	void loadPlugin( const std::string& sFilename );
  /// Debugging function to print out the whole processing pipeline 
  void dumpGraph()
    throw();

  std::vector<std::vector<boost::shared_ptr<CPipelineItem> > > modules[4];   ///< Array of available filters
  
  /// All pipeline items that have windows are in here
  std::map<int,boost::shared_ptr<CPipelineItem> > windowItemPtrMap;
  ulong ulHighestIndex;                       ///< Highest used index
  // Widgets
  QPopupMenu* fileMenuPtr;                    ///< File menu
  QPopupMenu* helpMenuPtr;                    ///< Help menu
  QPopupMenu* windowMenuPtr;                  ///< Window menu
	QPopupMenu* moduleMenuPtr;
  CPipeScroller* pipelineViewPtr;             ///< Shows the processing pipeline
  CDragListBox *availFiltersPtr;              ///< Shows all available processing filters
  CDragListBox *availConvertersPtr;           ///< Shows all available processing converters
  CDragListBox *availSourcesPtr;              ///< Shows all available processing sources
  CDragListBox *availTargetsPtr;              ///< Shows all available processing targets
  CDragListBox *otherModulesPtr;              ///< Shows all available processing modules
  QTabWidget *tabBoxPtr;                      ///< Tabbox to display all module lists
	QToolBox *filtersToolBoxPtr;
	QToolBox *convertersToolBoxPtr;
	QToolBox *sourcesToolBoxPtr;
	QToolBox *targetsToolBoxPtr;
	QToolBox *othersToolBoxPtr;
  QCheckBox* executeCheckPtr;                 ///< Execute pipeline checkbox
#ifndef NOPROGRESS	
	QProgressBar* currentProgress;              ///< Progess of current module
#endif	
  //std::vector<boost::shared_ptr<CPipelineItem> > sourcePtrVec;   ///< List of all image sources
  std::vector<boost::shared_ptr<CPipelineItem> > targetPtrVec;   ///< List of all image targets
  std::map<ulong,boost::shared_ptr<CPipelineItem> > modulePtrMap;///< List of all graph nodes
  std::vector<boost::shared_ptr<CGraphEdge> > edgePtrVec;        ///< List of all graph edges
	std::vector<SLibItem> loadedPlugIns;       ///< Vector of all active plugins
	CPluginDialog* dialog;	
	std::vector<SFileHandlerData> loadedHandlers;
	string sPluginDir;
	string sPipelineDir;
	string sDataDir;
	string sConfigDir;
	bool bPipelineChanged;
	CTypedMap lastParameters;
	QTextEdit* logWidget;
	bool curFont;
	bool showLog;
	bool doNotUpdate;
	bool currentlyRunning;
};

static CMainWin* actualInstance;

static void logStringInWindow( const char*  s, size_t l )
{
	actualInstance->logString( s, l );
}

#endif
