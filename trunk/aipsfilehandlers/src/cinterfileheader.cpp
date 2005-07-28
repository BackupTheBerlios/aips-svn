                           /***************************************************************************
 *   Copyright (C) 2004 by Patrick Musmann                                 *
 *   p.musmann@fz-juelich.de                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "cinterfileheader.h"

using namespace aips;
using namespace std;

/***************************************************************************
 * Structors                                                               *
 ***************************************************************************/
/** Constructor */
CInterfileHeader::CInterfileHeader() throw()
  : CImageHeader()
{}

/**
 * Constructor with default pairs
 * \param aPairVector with defaults
 */
CInterfileHeader::CInterfileHeader( const std::vector<std::pair<std::string, std::string> > &aPairVector ) throw()
  : CImageHeader()
{
    // init headerword vector
    for( vector<pair<string, string> >::const_iterator it = aPairVector.begin();
         it != aPairVector.end(); ++it )
    {
        setString( it->first, it->second );
//         _theHwVector.push_back( it->first );
    }
}

/** Destructor */
CInterfileHeader::~CInterfileHeader() throw()
{}


/* Other methods */
/// Loads the header from an open stream
void CInterfileHeader::loadHeader( std::istream& theFile )
  throw( FileException )
{
    // Load all Headerwords detected by scanLine stop after '!end of interfile' key
    char cLine[256];
    while( theFile.good() )
    {
        theFile >> std::ws;            //skip leading white spaces
        theFile.getline( cLine, sizeof cLine );
        if( cLine[0] == ';' )    //skip comment line
            continue;

        std::string sKey, sValue;
        if( scanLine( cLine, ":=", sKey, sValue ) )
        {
            setString( sKey, sValue );
            if( sKey == "!END OF INTERFILE" )
                break;
        }
    }

    // Set cimageheader keywords
    // dimension
    if( getString( "number of dimensions" ) != "3" )
        throw( FileException( SERROR( "number of dimensions != 3" ),
        	CException::RECOVER, ERR_FILEFORMATUNSUPPORTED ) );
    vector<size_t> dimensionSize(3);
    dimensionSize[0] = string2numtype<uint>( getString( "!matrix size [1]" ) );
    dimensionSize[1] = string2numtype<uint>( getString( "!matrix size [2]" ) );
    dimensionSize[2] = string2numtype<uint>( getString( "!matrix size [3]" ) );
    setExtents( dimensionSize );

    // endian
    if( getString( "imagedata byte order" ) == "LITTLEENDIAN" )
        setEndianess( false );
    else
        setEndianess( true );

    // voxel size in bytes
    if( ( getString( "!number format" ) != "float" )
     || ( getString( "!number of bytes per pixel" ) != "4" ) )
        throw( FileException( SERROR( "No 4byte float dataset" ),
        	CException::RECOVER, ERR_FILEFORMATUNSUPPORTED ) );
    setVoxelType( "Float16" );  //sizeof(float)
}


/// Saves the header to an open stream
void CInterfileHeader::saveHeader( std::ostream& theFile )
    throw( FileException )
{
    std::vector<string> _theHwVector = getKeyList();
    //TODO: warn or mark if key[i] is not in header
    for( std::vector<std::string>::const_iterator it = _theHwVector.begin();
         it != _theHwVector.end(); ++it )
    {
        theFile << *it << " := ";

        // we don't now the value type for the keys, so we try everyone
        try
        {
            theFile << getString( *it );
        }
        catch( std::exception& e )
        {
            try
            {
                theFile << getDouble( *it );
            }
            catch( std::exception& e )
            {
                try
                {
                    theFile << getLong( *it );
                }
                catch( std::exception& e )
                {
                    try
                    {
                        theFile << getUnsignedLong( *it );
                    }
                    catch( std::exception& e )
                    {
                        try
                        {
                            theFile << getBool( *it );
                        }
                        catch( std::exception& e )
                        {
                            throw( e );
                        }
                    }
                }
            }
        }

    theFile << std::endl;

    }   //end loop over header words
}


/************************************************************************
 * static methods/ string manipulators                                  *
 ************************************************************************/
/**
 * Scans a string line for a key pair value separted by separator
 * \param sLine string line
 * \param sSeparator separator string between key and value
 * \param sKey found key string
 * \param sValue found value string
 * \return wether a key value pair was detected or not
 */
const bool CInterfileHeader::scanLine(const std::string &sLine, const std::string &sSeparator, std::string &sKey, std::string &sValue )
{
    // looking for separator
    size_t loc = sLine.find( sSeparator, 1 );
    if( loc != string::npos )
    {
        // Getting Key and Value strings
         sKey = rmSurroundingChars( sLine.substr( 0, loc ) );
         sValue = rmSurroundingChars( sLine.substr( loc + sSeparator.length()) );
//         cout << " Found Separator at loc = " << loc << endl;
//         cout << "\tkey = '" << sKey << "'" << endl;
//         cout << "\tvalue = '" << sValue << "'" << endl;
        return true;
    }
    return false;
}

/**
 * Removes leading and trailing chars
 * \param s a string
 * \param sChars string of characters to remove
 * \return new modified string
 */
const std::string CInterfileHeader::rmSurroundingChars( const std::string &s, const std::string &sChars )
{
    size_t loc_first = s.find_first_not_of( sChars );
    if( loc_first != string::npos )
        return s.substr( loc_first, s.find_last_not_of( sChars ) - loc_first + 1 );
    else    // all characters are white spaces
        return string();
}

