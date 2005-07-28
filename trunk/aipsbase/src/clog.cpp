/***********************************************************************
 * File: clog.h                                                        *
 * Project: AIPS                                                       *
 * Description: Logfile and error printing functions                   *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 15.09.03                                                   *
 ***********************************************************************/
#include "clog.h"

using namespace aips;
using namespace std;
   
ostream aips::alog( clog.rdbuf() );
CSubject aips::newLog( "CSubject" );
ofstream* aips::CLogServer::logFilePtr = NULL;
aips::ELogLevel aips::logState = LDEBUG;


std::streamsize cloneStreamBuf::xsputn ( const char * s, std::streamsize n ) 
{	
	if ( logState != LDEBUG )
		newLog.notify( boost::shared_ptr<CNewLogEvent>( new CNewLogEvent( &newLog, s ) ) );
	return assoc->sputn( s, n );
}

int cloneStreamBuf::overflow( int c )
{
	newLog.notify( boost::shared_ptr<CNewLogEvent>( new CNewLogEvent( &newLog, "" ) ) );
	return assoc->sputc( c );
}

std::streampos cloneStreamBuf::seekoff ( streamoff off, ios_base::seekdir way, ios_base::openmode
which )
{
	return assoc->pubseekoff( off, way, which );
}

std::streampos cloneStreamBuf::seekpos ( streampos sp, ios_base::openmode which )
{
	return assoc->pubseekpos( sp, which );
}

std::streambuf* cloneStreamBuf::setbuf ( char * s, streamsize n )
{
	return assoc->pubsetbuf( s,n );
}

int cloneStreamBuf::sync()
{
	return assoc->pubsync();
}

/**
 * Constructor
 * \param sLogFilename Filename of logfile. "screen" prints to stderr
 * \param bAppend Append messages to existing logfile (true) or overwrite file (false)
 */
CLogServer::CLogServer ( const string& sLogFilename, const bool bAppend )
  throw( BadInitException )
  : CBase ( "CLogServer", "0.5", "CBase" ) 
{
  cerr << "<AIPS> Initializing logfile system..." << endl;
	cloneStreamBuf* buf;
  if ( sLogFilename == "screen" )
  {
    alog << "<** STARTUP **> Logfile system initialized" << endl;
    logFilePtr = NULL;
		buf = new cloneStreamBuf( cerr.rdbuf() );
  }
  else
  {
    // Create logfile
    try
    {   if ( bAppend )
          logFilePtr = new ofstream( sLogFilename.c_str(), ios::out | ios::app );
        else
          logFilePtr = new ofstream( sLogFilename.c_str() );
    }
    catch ( std::bad_alloc &exc )
    {
      cerr << "<LOG ERROR>" << exc.what();
      throw BadInitException(
        "Logging system initialization failed! Bad alloc on creating logfile instance!" );
    }
		buf = new cloneStreamBuf( logFilePtr->rdbuf() );    
    alog << "<** STARTUP **> Logfile system initialized" << endl;
  }
	cerr << "BUF set ";
	alog.rdbuf( buf );
}

/** Destructor */
CLogServer::~CLogServer () throw()
{
	if ( logFilePtr )
		alog.rdbuf( logFilePtr->rdbuf() );
	else
		alog.rdbuf( cerr.rdbuf() );
  alog << "<** SHUTDOWN **> Closing logfile" << endl;
  if ( logFilePtr )
  {
    logFilePtr->close();
    delete logFilePtr;
  }
  cerr << "<AIPS> Logfile system shut down" << endl;
}

/** Reimplemented from CBase */
const string CLogServer::dump() const throw()
{
  std::ostringstream os;
  os << "\nLogfile pointer " << static_cast<void*>( logFilePtr ) << " ";
  return CBase::dump() + os.str();
}

/** Overloaded stream operator for log level type */
ostream& aips::operator<<( ostream& ostr, const ELogLevel theLogLevel )
{
  time_t currentTime = time ( 0 );
  char* currentDate = ctime( &currentTime );
  // REMARK: ctime returns a pointer to a statically allocated string, so we need not to delete
  // currentDate at the end of this function
  currentDate[strlen( currentDate ) - 1]='\0';
	logState = theLogLevel;
  switch ( theLogLevel )
  {
    case LINFO:
      ostr << "<INFO> "; 
      break;
    case LWARN:
      ostr << "<WARNING> "; 
      break;
    case LERR:
      ostr << "<ERROR> "; 
      break;
    case LFATAL:
      ostr << "<FATAL> ";
      break;
    case LDEBUG:
      ostr << "<DEBUG> ";
      break;
    case LFIXME:
      ostr << "<FIXME> ";
      break;
    default:
      ostr << "<UNKNOWN> ";
  }
  ostr << "[" << currentDate << "] ";
  return ostr;
}

void aips::initLogServer( const string& sLogFilename, const bool bAppend ) throw()
{
  static aips::CLogServer theLogServer( sLogFilename, bAppend );
}
