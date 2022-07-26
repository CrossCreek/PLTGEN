/*=============================================================================================
 * CLASSIFICATION: Unclassified
 * Government Purpose Rights
 * Contract No: 12-C-0158
 * Contractor Name: Perspecta
 * Contractor Address: 15052 Conference Center Dr. Chantilly, VA 20151
 * Expiration Date: 31 Aug. 2023
 * The Government's rights to use, modify, reproduce, release, perform, display, or disclose these
 * technical data and computer software are restricted by paragraph (b)(1) of clause N52.227-002,
 * Rights in Technical Data and Computer Software: Noncommercial Items, contained in the contract
 * identified above. No restrictions apply after the expiration date shown above. Any reproduction of
 * technical data or computer software, or portions thereof marked with this legend,must also reproduce the markings.
 *
 *
 * RCX.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
// UNITS 030109
// Function Arg & Return: N/A
// Stream I/O:            N/A
// Initialization Files:  N/A

// ============================================================================
//  FILE      : RCX.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0      04/29/2002  JAM  initial build
//              V1.1      2007-09-05  JAM  added ASSERT option for Find failure
//                                         if Find fails and
//                                         OPTION=NULL then return end()
//                                         OPTION=ASSERT then throw exception
//              V4.4-001  2008-10-03  JAM  1st CM GenEngr build

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  CLASS

//  Name:       RCX
//  Purpose:    Navigate XML tree, extract attributes and data

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  MEMBER FUNCTIONS

//  Public Member:  RCX()
//  Purpose:        empty default constructor
//  Return Type:    n/a
//  Return Value:   n/a

//  Public Member:  RCX(std::istream&)
//  Purpose:        calls Process(std::istream&) to read and parse an XML tree
//  Return Type:    n/a
//  Return Value:   n/a

//  Public Member:  Process(std::istream&)
//  Purpose:        searches for XML root node, reads and parses the XML tree
//  Return Type:    bool
//  Return Value:   true if root node found, else false

//  Public Member:  GetTag() const
//  Purpose:        return node name
//  Return Type:    const std::string&
//  Return Value:   name of node

//  Public Member:  GetData() const
//  Purpose:        return node data
//  Return Type:    const std::string&
//  Return Value:   string containing node data

//  Public Member:  GetHdlDataStream() const
//  Purpose:        provides handle to a stream that may be used to extract
//                  node data
//  Return Type:    Handle< std::istream >
//  Return Value:   stream handle for extracting data

//  Public Member:  GetNAttributes() const
//  Purpose:        return count of node attributes
//  Return Type:    unsigned
//  Return Value:   count of node attributes

//  Public Member:  GetAttribute(const std::string&) const
//  Purpose:        return value of attribute
//  Return Type:    std::string
//  Return Value:   attribute value
//  Argument Type:  const std::string&
//  Argument Value: name of attribute

//  Public Member:  Size() const
//  Purpose:        return count of nodes beneath this node
//  Return Type:    unsigned
//  Return Value:   count of nodes

//  Public Member:  Size(const std::string&) const
//  Purpose:        return count of nodes beneath this node with specified tag
//  Return Type:    unsigned
//  Return Value:   count of nodes with tag
//  Argument Type:  const std::string&
//  Argument Value: tag of interest

//  Public Member:  Size(const std::string&, const std::string&) const
//  Purpose:        return count of nodes beneath this node with specified tag
//                  and attribute name
//  Return Type:    unsigned
//  Return Value:   count of nodes with tag
//  Argument Type:  const std::string&
//  Argument Value: tag of interest
//  Argument Type:  const std::string&
//  Argument Value: attribute name of interest

//  Public Member:  FindNode(const std::string&, OPTION=NONE)
//  Purpose:        return iterator to node matching tag one level beneath
//                  this node
//  Return Type:    iterator
//  Return Value:   random-access iterator to node matching tag,
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, iterator,
//                  OPTION=NONE)
//  Purpose:        return iterator to node matching tag one level beneath
//                  this node, start search at specified iterator
//  Return Type:    iterator
//  Return Value:   random-access iterator to node matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  iterator
//  Argument Value: iterator to search start
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, const std::string&,
//                  OPTION=NONE)
//  Purpose:        return iterator to node matching tag and containing
//                  attribute name one level beneath this node
//  Return Type:    iterator
//  Return Value:   random-access iterator to node matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  const std::string&
//  Argument Value: attribute name required
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, const std::string&,
//                           iterator, OPTION=NONE)
//  Purpose:        return iterator to node matching tag and containing
//                  attribute name one level beneath this node, start search
//                  at specified iterator
//  Return Type:    iterator
//  Return Value:   random-access iterator to node matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  const std::string&
//  Argument Value: attribute name required
//  Argument Type:  iterator
//  Argument Value: iterator to search start
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, OPTION=NONE) const
//  Purpose:        return iterator to node matching tag one level beneath
//                  this node
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to node matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, iterator,
//                  OPTION=NONE) const
//  Purpose:        return iterator to node matching tag one level beneath
//                  this node, start search at specified iterator
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to node  matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  iterator
//  Argument Value: iterator to search start
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, const std::string&,
//                  OPTION=NONE) const
//  Purpose:        return iterator to node matching tag and containing
//                  attribute name one level beneath this node
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to node  matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  const std::string&
//  Argument Value: attribute name required
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, const std::string&,
//                           iterator, OPTION=NONE) const
//  Purpose:        return iterator to node matching tag and containing
//                  attribute name one level beneath this node, start search
//                  at specified iterator
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to node  matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  const std::string&
//  Argument Value: attribute name required
//  Argument Type:  iterator
//  Argument Value: iterator to search start
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNodeId(const std::string&, OPTION=NONE)
//  Purpose:        return iterator to node matching id="name"
//  Return Type:    iterator
//  Return Value:   random-access iterator to node  with id "name"
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: id "name" to be found
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNodeId(const std::string&, OPTION=NONE) const
//  Purpose:        return iterator to node matching id="name"
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to node  with id "name"
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: id "name" to be found
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  begin()
//  Purpose:        return iterator to 1st node one level beneath this node
//  Return Type:    iterator
//  Return Value:   random-access iterator to 1st node

//  Public Member:  end()
//  Purpose:        return iterator that addresses the location succeeding the
//                  last node one level beneath this node
//  Return Type:    iterator
//  Return Value:   random-access iterator to 1st node

//  Public Member:  begin() const
//  Purpose:        return iterator to 1st node one level beneath this node
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to 1st node

//  Public Member:  end() const
//  Purpose:        return iterator that addresses the location succeeding the
//                  last node one level beneath this node
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to 1st node

//  Public Member:  SizeTotal() const
//  Purpose:        count of nodes in entire XML tree beneath this node
//  Return Type:    unsigned
//  Return Value:   count of nodes
//  Argument Type:
//  Argument Value:

//  Public Member:  SizeTotal(const std::string&) const
//  Purpose:        count of nodes in entire XML tree beneath this node
//                  matching tag
//  Return Type:    unsigned
//  Return Value:   count of nodes
//  Argument Type:  const std::string&
//  Argument Value: tag

//  Public Member:  SizeTotal(const std::string&, const std::string&) const
//  Purpose:        count of nodes in entire XML tree beneath this node
//                  matching tag and type
//  Return Type:    unsigned
//  Return Value:   count of nodes
//  Argument Type:  const std::string&
//  Argument Value: tag
//  Argument Type:  const std::string&
//  Argument Value: type

//  Public Member:  ReportState(std::ostream&)
//  Purpose:        dump XML tree to stream
//  Return Type:    void
//  Return Value:   n/a
//  Argument Type:  std::ostream&
//  Argument Value: stream receiving XML tree

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  NOTES :

//****************************************************************************//
#include "RCX.h"
#include "GenUtility.h"
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string.h>

namespace A3
{

RCX::RCX()
{ ; }

RCX::RCX( std::istream& is )
{
    try {
        Process( is );
    } catch ( std::bad_alloc& ) {
        throw;
    } catch ( std::ios_base::failure& e ) {
        std::string sMsg
            = "RCX::RCX( std::istream& ) caught xml run control\n";
        sMsg += "stream exception.  Failure message:\n";
        sMsg += e.what();
        throw std::ios_base::failure( sMsg );
    } catch ( std::exception& e ) {
        std::string sMsg
            = "RCX::RCX( std::istream& ) caught a standard\n";
        sMsg += "exception of type ";
        sMsg += typeid( e ).name();
        sMsg += ".\nException message:\n";
        sMsg += e.what();
        throw std::runtime_error( sMsg );
    } catch ( ... ) {
        std::string sMsg
            = "RCX::RCX( std::istream& ) caught an unknown exception";
        throw std::runtime_error( sMsg );
    }
}

bool RCX::Process( std::istream& is )
{
    bool bFoundRoot = false;
    std::string    sBuf;
    std::string    sAttribData;
    std::streampos blockStart;
    std::streampos blockEnd;
    std::streampos dataStart;
    std::streampos dataEnd;
    std::streampos commentEnd;

    try {
        char c;
        // select stream states that will throw an exception
        std::ios_base::iostate ioOriginalState = is.exceptions();
        is.exceptions( std::ios_base::badbit );

        while ( !bFoundRoot && is ) {
            std::getline( is, sBuf, '<' );
            bFoundRoot = ( is >> c && c != '!' && c != '?' );

            if ( c == '!' && is >> c && c == '-' ) {
                is.seekg( -3, std::istream::cur );
                commentEnd = findBlock( is, "<!--", "-->" );
                is.seekg( commentEnd );
            }
        }

        if ( bFoundRoot ) {
            is.seekg( -2, std::istream::cur );
            FindBlock( is, blockStart, blockEnd,
                       dataStart,  dataEnd,
                       m_sTagName, sAttribData );

            if ( !sAttribData.empty() ) {
                ParseAttributes( sAttribData, m_mAttributes );
            }

            if ( dataStart != dataEnd ) {
                ParseNodes( is, dataEnd, m_sData, m_vNodes );
            }

            is.seekg( blockEnd + std::streampos(1) );
        }

        // restore original exception stream states
        is.exceptions( ioOriginalState );
    } catch ( std::bad_alloc& ) {
        throw;
    } catch ( std::ios_base::failure& e ) {
        std::string sMsg
            = "RCX::Process( std::istream& ) caught xml run control\n";
        sMsg += "stream exception";
        sMsg += "\nTag = " + m_sTagName + "\nId  = " + GetAttribute( "id" );
        sMsg += "\nFailure message:\n";
        sMsg += e.what();
        throw std::ios_base::failure( sMsg );
    } catch ( std::exception& e ) {
        std::string sMsg
            = "RCX::Process( std::istream& ) caught a standard\n";
        sMsg += "exception of type ";
        sMsg += typeid( e ).name();
        sMsg += "\nTag = " + m_sTagName + "\nId  = " + GetAttribute( "id" );
        sMsg += ".\nException message:\n";
        sMsg += e.what();
        throw std::runtime_error( sMsg );
    } catch ( ... ) {
        std::string sMsg
            = "RCX::Process( std::istream& ) caught an unknown exception";
        sMsg += "\nTag = " + m_sTagName + "\nId  = " + GetAttribute( "id" );
        throw std::runtime_error( sMsg );
    }

    return bFoundRoot;
}

void RCX::ParseAttributes( const std::string& sAttributes,
                           ContAttributes& mAttributes  )
{
    std::string sBuf = sAttributes;
    std::string sAttr, sValue;
    std::string::size_type sIdx = 0;
    unsigned cntE = 0, cntQ = 0;
    
    
    sIdx = sBuf.find( '=', sIdx );

    while ( std::string::npos != sIdx ) {
        sBuf[ sIdx ] = ' ';
        ++cntE;
        sIdx = sBuf.find( '=', sIdx );
    }

    sIdx = 0;
    
    
    sIdx = sBuf.find( '\"', sIdx );

    while ( std::string::npos != sIdx ) {
        if (++cntQ%2) {
            sBuf[ sIdx ] = ' ';
        } else {
            sBuf[ sIdx ] = '\n';
        }

        sIdx = sBuf.find( '\"', sIdx );
    }

    if ( ( (cntQ >> 1 << 1) != cntQ || (cntQ >> 1) != cntE ) ) {
        std::runtime_error( "RCX::ParseAttributes bad attributes" );
    }

    if ( cntE ) {
        std::istringstream iss( sBuf );

        while ( cntE-- ) {
            iss >> sAttr >> std::ws;
            std::getline( iss, sValue );
            mAttributes[ sAttr ] = sValue;
        }
    }
}

void RCX::ParseNodes( std::istream& is,  std::streampos dataEnd,
                      std::string& sData, ContNodes& vNodes )
{
    RCX node;
    std::string sBuf;
    std::streampos commentEnd;
    sData.erase();

    do {
        is >> std::ws;
        std::getline( is, sBuf, '<' );

        if ( !sBuf.empty() ) {
            sData += sBuf;
        }

        if ( is.tellg() < dataEnd ) {
            if ( !peekCompareString( is, "!--" ) ) {
                is.seekg( -1, std::istream::cur );
                vNodes.push_back( node );
                vNodes.back().Process( is );
            } else {
                is.seekg( -1, std::istream::cur );
                commentEnd = findBlock( is, "<!--", "-->" );
                is.seekg( commentEnd );
                // findString( is, "-->" );
                // is.seekg( 3, std::istream::cur );
            }
        }
    } while( is.tellg() < dataEnd );
}

void RCX::FindBlock( std::istream&  is,
                     std::streampos& blockStart,
                     std::streampos& blockEnd,
                     std::streampos& dataStart,
                     std::streampos& dataEnd,
                     std::string&    sTagName,
                     std::string&    sAttribData )
{
    static const std::runtime_error except( "RCX::FindBlock(...) failed!\n" );
    std::string sBuf;
    std::string::size_type nmEnd = 0;
    bool bIsAttribOnly = false;
    char c;
    // select stream states that will throw an exception
    std::ios_base::iostate ioOriginalState = is.exceptions();
    is.exceptions( std::ios_base::badbit
                   | std::ios_base::failbit
                   | std::ios_base::eofbit );
    // find start and combined tag name/attribute string
    is >> std::ws;
    blockStart = dataStart = dataEnd = blockEnd = is.tellg();
    sTagName.erase();
    sAttribData.erase();
    is >> c >> std::ws;

    if ( '<' != c ) {
        throw except;
    }

    std::getline( is, sBuf, '>' );

    if ( '/' == *(sBuf.end()-1) ) {
        dataStart = dataEnd = blockEnd = is.tellg()-std::streampos(1);
        sBuf.erase( sBuf.end()-1 );
        bIsAttribOnly = true;
    } else {
        is >> std::ws;
        dataStart = dataEnd = blockEnd = is.tellg();
    }

    // separate tag name and attribute strings
    nmEnd = sBuf.find_first_of( " \t\n\r\f" );

    if ( std::string::npos == nmEnd ) {
        sTagName = sBuf;
    } else {
        sTagName.assign( sBuf.begin(), sBuf.begin() + nmEnd );
        sAttribData.assign( sBuf.begin() + nmEnd, sBuf.end() );
    }

    // find end
    if ( !bIsAttribOnly ) {
        sBuf = "</" + sTagName;

        if ( !findString( is, sBuf ) ) {
            throw except;
        }

        dataEnd = blockEnd = is.tellg() - std::streampos( 1 );
        std::getline( is, sBuf, '>' );
        blockEnd = is.tellg()-std::streampos(1);
    }

    is.seekg( dataStart );
    // restore original exception stream states
    is.exceptions( ioOriginalState );
}


void RCX::ReportState( std::ostream& os )
{
    os << '<' << m_sTagName;
    ContAttributes::iterator itrM = m_mAttributes.begin();
    ContAttributes::iterator etrM = m_mAttributes.end();

    for ( ; itrM != etrM; ++itrM ) {
        os << ' ' << itrM->first << "=\"" << itrM->second << '\"';
    }

    os << '>' << std::endl;

    if ( !m_sData.empty() ) {
        os << m_sData << std::endl;
    }

    iterator itrV = m_vNodes.begin();
    iterator etrV = m_vNodes.end();

    for ( ; itrV != etrV; ++itrV ) {
        itrV->ReportState( os );
    }

    os << "</" << m_sTagName << '>' << std::endl;
}

const std::string& RCX::GetTag() const
{
    return m_sTagName;
}

const std::string& RCX::GetData() const
{
    return m_sData;
}

Handle< std::istream > RCX::GetHdlDataStream() const
{
    Handle< std::istringstream > hStream( new std::istringstream( m_sData ) );
    return hStream;
}

unsigned RCX::GetNAttributes() const
{
    return static_cast<unsigned>(m_mAttributes.size());
}

std::string RCX::GetAttribute( const std::string& tag ) const
{
    static const std::string empty;
    ContAttributes::const_iterator itr = m_mAttributes.find( tag );
    return m_mAttributes.end() != itr ? itr->second : empty;
}

unsigned RCX::Size() const
{
    return static_cast<unsigned>(m_vNodes.size());
}

unsigned RCX::Size( const std::string& sTagName ) const
{
    unsigned count = 0;
    const_iterator itr = begin();
    
    
    itr = FindNode( sTagName, itr, NONE );

    while ( end() != itr ) {
        ++itr;
        ++count;
        itr = FindNode( sTagName, itr, NONE );
    }

    return count;
}

unsigned RCX::Size( const std::string& sTagName,
                    const std::string& sType ) const
{
    unsigned count = 0;
    const_iterator itr = begin();
    
    
    itr = FindNode( sTagName, sType, itr, NONE );

    while ( end() != itr ) {
        ++itr;
        ++count;
        itr = FindNode( sTagName, sType, itr, NONE );
    }

    return count;
}

RCX::iterator
RCX::FindNode( const std::string& sTagName, OPTION opt )
{
    return FindNode( sTagName, begin(), opt );
}

RCX::iterator
RCX::FindNode( const std::string& sTagName,
               RCX::iterator itrStart,
               OPTION opt )
{
    A3::CompareTag< RCX > cn( sTagName );
    iterator node = std::find_if( itrStart, end(), cn );

    if ( end() == node && ASSERT==opt ) {
        throw std::runtime_error( "RCX can't find tag: "+sTagName );
    }

    return node;
}

RCX::iterator
RCX::FindNode( const std::string& sTagName,
               const std::string& sType,
               OPTION opt )
{
    return FindNode( sTagName, sType, begin(), opt );
}

RCX::iterator
RCX::FindNode( const std::string& sTagName,
               const std::string& sType,
               RCX::iterator itrStart,
               OPTION opt )
{
    iterator node(end());

    try {
        node = FindNode( sTagName, itrStart, opt );

        while ( end() != node && sType != node->GetAttribute( "type" ) ) {
            node = FindNode( sTagName, node+1, opt );
        }
    } catch ( std::runtime_error& re ) {
        if ( 0==strncmp( "RCX", re.what(), 3 ) )
            throw std::runtime_error( "RCX can't find tag/type: "+
                                      sTagName+"/"+sType );
        else {
            throw;
        }
    }

    return node;
}

RCX::const_iterator
RCX::FindNode( const std::string& sTagName, OPTION opt ) const
{
    return FindNode( sTagName, begin(), opt );
}

RCX::const_iterator
RCX::FindNode( const std::string& sTagName,
               RCX::const_iterator itrStart,
               OPTION opt ) const
{
    A3::CompareTag< RCX > cn( sTagName );
    const_iterator node = std::find_if( itrStart, end(), cn );

    if ( end() == node && ASSERT==opt ) {
        throw std::runtime_error( "RCX can't find tag: "+sTagName );
    }

    return node;
}

RCX::const_iterator
RCX::FindNode( const std::string& sTagName,
               const std::string& sType,
               OPTION opt ) const
{
    return FindNode( sTagName, sType, begin(), opt );
}

RCX::const_iterator
RCX::FindNode( const std::string& sTagName,
               const std::string& sType,
               RCX::const_iterator itrStart,
               OPTION opt ) const
{
    const_iterator node(end());

    try {
        node = FindNode( sTagName, itrStart, opt );

        while ( end() != node && sType != node->GetAttribute( "type" ) ) {
            node = FindNode( sTagName, node+1, opt );
        }
    } catch ( std::runtime_error& re ) {
        if ( 0==strncmp( "RCX", re.what(), 3 ) )
            throw std::runtime_error( "RCX can't find tag/type: "+
                                      sTagName+"/"+sType );
        else {
            throw;
        }
    }

    return node;
}

RCX::iterator RCX::FindNodeId( const std::string& sId,
                               OPTION opt )
{
    bool bIsFound = false;
    A3::CompareAttribute< RCX > cn( "id", sId );
    RCX::iterator itr0 = std::find_if( begin(), end(), cn );
    RCX::iterator itr1;

    if ( end() == itr0 ) {
        for ( itr0 = begin(); !bIsFound && end() != itr0; ++itr0 ) {
            itr1 = itr0->FindNodeId( sId, NONE );

            if ( itr0->end() != itr1 ) {
                bIsFound = true;
            }
        }

        if ( bIsFound ) {
            itr0 = itr1;
        }
    } else {
        bIsFound = true;
    }

    if ( !bIsFound && ASSERT==opt ) {
        throw std::runtime_error( "RCX can't find id: "+sId );
    }

    return itr0;
}

RCX::const_iterator RCX::FindNodeId( const std::string& sId,
                                     OPTION opt ) const
{
    bool bIsFound = false;
    A3::CompareAttribute< RCX > cn( "id", sId );
    RCX::const_iterator itr0 = std::find_if( begin(), end(), cn );
    RCX::const_iterator itr1;

    if ( end() == itr0 ) {
        for ( itr0 = begin(); !bIsFound && end() != itr0; ++itr0 ) {
            itr1 = itr0->FindNodeId( sId, NONE );

            if ( itr0->end() != itr1 ) {
                bIsFound = true;
            }
        }

        if ( bIsFound ) {
            itr0 = itr1;
        }
    } else {
        bIsFound = true;
    }

    if ( !bIsFound && ASSERT==opt ) {
        throw std::runtime_error( "RCX can't find id: "+sId );
    }

    return itr0;
}

RCX::iterator RCX::begin()
{
    return m_vNodes.begin();
}

RCX::iterator RCX::end()
{
    return m_vNodes.end();
}

RCX::const_iterator RCX::begin() const
{
    return m_vNodes.begin();
}

RCX::const_iterator RCX::end() const
{
    return m_vNodes.end();
}

unsigned RCX::SizeTotal() const
{
    unsigned count = static_cast<unsigned>(m_vNodes.size());
    const_iterator itr = m_vNodes.begin();
    const_iterator etr = m_vNodes.end();

    for ( ; etr != itr; ++itr ) {
        count += itr->SizeTotal();
    }

    return count;
}

unsigned RCX::SizeTotal( const std::string& sTagName ) const
{
    unsigned count = Size( sTagName );
    const_iterator itr = m_vNodes.begin();
    const_iterator etr = m_vNodes.end();

    for ( ; etr != itr; ++itr ) {
        count += itr->SizeTotal( sTagName );
    }

    return count;
}

unsigned RCX::SizeTotal( const std::string& sTagName,
                         const std::string& sType ) const
{
    unsigned count = Size( sTagName, sType );
    const_iterator itr = m_vNodes.begin();
    const_iterator etr = m_vNodes.end();

    for ( ; etr != itr; ++itr ) {
        count += itr->SizeTotal( sTagName, sType );
    }

    return count;
}

}
