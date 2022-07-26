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
 * GRCItem.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
// Refer to GRCItem.h
//****************************************************************************//

#include "GRCItem.h"

#include <iostream>        // For std::cerr
#include <stdexcept>       // For standard exception handling classes
#include <sstream>

// =============================================================================
// A3 namespace start
namespace A3
{
std::istream* GRCItem::s_pIsRunControl = NULL;

const std::streampos GRCItem::s_posNotFound
    = static_cast<std::streampos>(-1);

const std::string GRCItem::s_enabledToken  = "<ENABLED>";

const std::string GRCItem::s_strEnabledOn  = "TRUE";

const std::string GRCItem::s_strEnabledOff = "FALSE";

//----------------------------------------------------------------------------//
GRCItem::GRCItem( std::istream& isRunControl, std::streampos startPosition,
                  std::streampos endPosition )
{
    try {
        // Initialize data members
        Initialize();
        // This is a run control item
        Set(isRunControl, startPosition, endPosition);
    } catch (...) {
        std::cerr << "Exception caught by\n"
                  << "GRCItem::GRCItem(istream& isRunControl, streampos startPosition\n,"
                  << "                 streampos endPosition)\n"
                  << std::endl;
        throw;
    }
}

//----------------------------------------------------------------------------//
GRCItem::GRCItem( const char* startToken, const char* endToken,
                  const GRCItem& parentBlock )
{
    try {
        // Initialize data members
        Initialize();
        Set(startToken, endToken, parentBlock);
    } catch (...) {
        std::cerr << "Exception caught by\n"
                  << "GRCItem::GRCItem(const char* startToken, const char* endToken,\n"
                  << "                 const GRCItem& parentBlock)\n"
                  << std::endl;
        throw;
    }
}

//----------------------------------------------------------------------------//
GRCItem::GRCItem( const char* token, const GRCItem& parentBlock )
{
    try {
        // Initialize data members
        Initialize();
        Set(token, parentBlock);
    } catch (...) {
        std::cerr << "Exception caught by\n"
                  << "GRCItem::GRCItem(const char* token, const GRCItem& parentBlock)\n"
                  << std::endl;
        throw;
    }
}

//----------------------------------------------------------------------------//
unsigned int GRCItem::Count( void )
{
    unsigned  int nItems     = 0;
    std::streampos startPos   = m_currentStart;
    std::streampos endPos     = m_currentEnd;

    try {
        // select stream states that will throw an exception
        std::ios_base::iostate ioOriginalState = s_pIsRunControl->exceptions();
        s_pIsRunControl->exceptions( std::ios_base::badbit );

        if ( !s_pIsRunControl->good() ) {
            s_pIsRunControl->clear();
        }

        std::streampos currentPos = s_pIsRunControl->tellg();
        Reset();

        while ( FindNext() ) {
            nItems++;
        }

        if ( !s_pIsRunControl->good() ) {
            s_pIsRunControl->clear();
        }

        s_pIsRunControl->seekg( currentPos );
        m_currentStart = startPos;
        m_currentEnd   = endPos;
        // restore original exception stream states
        s_pIsRunControl->exceptions( ioOriginalState );
    } catch ( std::ios_base::failure  ) {
        std::ostringstream exceptMessage;
        exceptMessage << "Exception thrown by "
                      << "GRCItem::Count()\n"
                      << "Run control stream threw a failure exception.\n"
                      << std::ends;
        std::runtime_error ee(exceptMessage.str());
        throw ee;
    } catch (...) {
        std::cerr << "Exception caught by\n"
                  << "GRCItem::Count()\n" << std::endl;
        throw;
    }

    return nItems;
}

//----------------------------------------------------------------------------//
bool GRCItem::FindFirst( bool bFailIsFatal )
{
    try {
        Reset();
        return FindNext( bFailIsFatal );
    } catch (...) {
        std::cerr << "Exception caught by\n"
                  << "GRCItem::FindFirst(bool bFailIsFatal)\n" << std::endl;
        throw;
    }
}

//----------------------------------------------------------------------------//
bool GRCItem::FindNext( bool bFailIsFatal )
{
    bool bFoundNext = false;

    try {
        // select stream states that will throw an exception
        std::ios_base::iostate ioOriginalState = s_pIsRunControl->exceptions();
        s_pIsRunControl->exceptions( std::ios_base::badbit );
        std::streampos posSearchStart =
            m_currentEnd > m_parentBlock->GetCurrentStart()
            ? m_currentEnd : m_parentBlock->GetCurrentStart();
        std::streampos posSearchEnd   = m_parentBlock->GetCurrentEnd();

        // if m_bBlockType == TRUE then find the data block
        // else find the data item
        if ( m_bBlockType ) {
            bool bBlockEnabled = true;
            std::string strBlockEnabled;
            std::streampos posStartToken = -1;
            std::streampos posEndToken = -1;
            std::streampos posEnableToken = -1;

            do {
                posStartToken = FindToken( m_strStartToken, posSearchStart,
                                           posSearchEnd,    bFailIsFatal );
                bFoundNext = posStartToken != s_posNotFound;

                if ( bFoundNext ) {
                    posEndToken = FindToken( m_strEndToken, posStartToken,
                                             posSearchEnd,  bFailIsFatal );
                    bFoundNext = posEndToken != s_posNotFound;

                    // process the <ENABLED> TRUE | FALSE item
                    if ( bFoundNext ) {
                        posEnableToken = FindToken( s_enabledToken,
                                                    posStartToken,
                                                    posEndToken );

                        if ( posEnableToken != s_posNotFound ) {
                            *s_pIsRunControl >> strBlockEnabled;

                            if ( s_strEnabledOn == strBlockEnabled ) {
                                bBlockEnabled = true;
                            } else if ( s_strEnabledOff == strBlockEnabled ) {
                                bBlockEnabled = false;
                                posSearchStart = posEndToken;
                            } else
                                throw std::runtime_error("Illegal <ENABLED> value: "
                                                         + strBlockEnabled);
                        } else {
                            bBlockEnabled = true;
                        }
                    }
                }
            } while ( !bBlockEnabled && bFoundNext );

            if ( bFoundNext ) {
                m_currentStart = posStartToken;
                m_currentEnd   = posEndToken
                                 - static_cast<std::streamoff>(m_strEndToken.size()-1);
                s_pIsRunControl->seekg( m_currentStart );
            }
        }
        // find data item
        else {
            posSearchStart = FindToken( m_strStartToken, posSearchStart,
                                        posSearchEnd,    bFailIsFatal );

            if ( posSearchStart != s_posNotFound ) {
                m_currentStart = posSearchStart;
                m_currentEnd   = posSearchStart;
                s_pIsRunControl->seekg( m_currentStart );
                bFoundNext = true;
            }
        }

        // restore original exception stream states
        s_pIsRunControl->exceptions( ioOriginalState );
    } catch ( std::ios_base::failure  ) {
        std::ostringstream exceptMessage;
        exceptMessage << "Exception thrown by\n"
                      << "GRCItem::FindNext( bool bFailIsFatal )\n"
                      << "Run control stream threw a failure exception.\n"
                      << std::ends;
        std::runtime_error ee(exceptMessage.str());
        throw ee;
    } catch (...) {
        std::cerr << "Exception caught by\n"
                  << "GRCItem::FindNext(bool bFailIsFatal)\n" << std::endl;
        throw;
    }

    return bFoundNext;
}

//----------------------------------------------------------------------------//
std::streampos GRCItem::FindToken( const std::string& token,
                                   std::streampos posStartSearch,
                                   std::streampos posEndSearch,
                                   bool bFailIsFatal )
{
    static std::string strBuf;
    std::streampos tokenLocation = s_posNotFound;

    try {
        std::streampos originalLocation = s_pIsRunControl->tellg();
        // select stream states that will throw an exception
        std::ios_base::iostate ioOriginalState = s_pIsRunControl->exceptions();
        s_pIsRunControl->exceptions( std::ios_base::badbit );

        if ( !s_pIsRunControl->good() ) {
            s_pIsRunControl->clear();
        }

        // move stream pointer to the beginning of the search area
        s_pIsRunControl->seekg( posStartSearch );
        // search for the token, stop when the token is found, search passes
        // outside of search area, EOF is reached or stream goes bad
        strBuf.erase();

        do {
            if ( !strBuf.empty() ) while ( s_pIsRunControl->get() != '\n' ) ;

            if ( s_pIsRunControl->good() ) {
                *s_pIsRunControl >> strBuf;
            }
        } while (s_pIsRunControl->good() &&

                 s_pIsRunControl->tellg() <= posEndSearch &&
                 token != strBuf );

        // clean up stream
        if ( !s_pIsRunControl->good() ) {
            s_pIsRunControl->clear();
        }

        // test the string agains the token, if it doesn't match, set the
        // stream pointer to the beginning of the search area and return false
        if ( token == strBuf ) {
            tokenLocation = s_pIsRunControl->tellg();
        } else {
            if ( !bFailIsFatal ) {
                s_pIsRunControl->seekg( originalLocation );
            } else {
                std::ostringstream exceptMessage;
                exceptMessage
                        << "Function -> GRCItem::FindToken( const string& token,\n"
                        << "                                streampos posStart,\n"
                        << "                                streampos posEnd,\n"
                        << "                                bool bFailIsFatal )\n"
                        << "Message  -> The token " << token
                        << " was not found.\n" << std::ends;
                std::runtime_error ee(exceptMessage.str());
                throw (ee);
            }
        }

        // restore original exception stream states
        s_pIsRunControl->exceptions( ioOriginalState );
    } catch ( std::ios_base::failure  ) {
        std::ostringstream exceptMessage;
        exceptMessage << "Exception thrown by\n"
                      << "GRCItem::FindToken(const char* token,\n"
                      << "                   streampos posStartSearch,\n"
                      << "                   streampos posEndSearch)\n"
                      << "Run control stream threw a failure exception.\n"
                      << std::ends;
        std::runtime_error ee(exceptMessage.str());
        throw ee;
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "GRCItem::FindToken(const char* token,\n"
                  << "                   streampos posStartSearch,\n"
                  << "                   streampos posEndSearch)\n"
                  << std::endl;
        throw;
    }

    return tokenLocation;
}

//----------------------------------------------------------------------------//
void GRCItem::Set( std::istream& isRunControl, std::streampos startPosition,
                   std::streampos endPosition )
{
    //try {
    // This is a run control item
    s_pIsRunControl = &isRunControl;
    m_currentStart = startPosition;
    m_currentEnd   = endPosition;
    m_bLastFindSuccessful = false;
    m_bBlockType  = true;
    //}
    //catch (...) {
    //  std::cerr << "Exception caught by\n"
    //       << "GRCItem::Set(istream& isRunControl, streampos startPosition\n"
    //       << "             streampos endPosition)\n"
    //       << std::endl;
    //  throw;
    //}
}

//----------------------------------------------------------------------------//
void GRCItem::Set( const char* startToken, const char* endToken,
                   const GRCItem& parentBlock )
{
    try {
        Set(startToken, parentBlock);
        m_strEndToken = endToken;
        m_bBlockType  = true;
    } catch (...) {
        std::cerr << "Exception caught by\n"
                  << "GRCItem::Set(const char* startToken, const char* endToken,\n"
                  << "             const GRCItem& parentBlock)\n"
                  << std::endl;
        throw;
    }
}

//----------------------------------------------------------------------------//
void GRCItem::Set( const char* token, const GRCItem& parentBlock )
{
    try {
        if ( s_pIsRunControl == NULL ) {
            std::ostringstream exceptMessage;
            exceptMessage
                    << "Function -> GRCItem::Set(const char *token, const GRCItem &parentBlock)\n"
                    << "Message  -> "
                    << "s_pIsRunControl is NULL, A Run Control item is required.\n"
                    << std::ends;
            std::runtime_error ee(exceptMessage.str());
            throw (ee);
        }

        m_strStartToken = token;
        m_strEndToken = "";
        m_parentBlock  = &parentBlock;
        m_bBlockType  = false;
        Reset();
    } catch (...) {
        std::cerr << "Exception caught by\n"
                  << "GRCItem::Set(const char* token, const GRCItem& parentBlock)\n"
                  << std::endl;
        throw;
    }
}

//----------------------------------------------------------------------------//
void GRCItem::SetPosition( const GRCItem& rcItem )
{
    try {
        m_currentStart = rcItem.m_currentStart;
        m_currentEnd   = rcItem.m_currentEnd;
        Rewind();
    } catch (...) {
        std::cerr << "Exception caught by "
                  << "GRCItem::SetPosition(const GRCItem &rcItem)" << std::endl << std::endl;
        throw;
    }
}

//----------------------------------------------------------------------------//
void GRCItem::Reset( void )
{
    try {
        // select stream states that will throw an exception
        std::ios_base::iostate ioOriginalState = s_pIsRunControl->exceptions();
        s_pIsRunControl->exceptions( std::ios_base::badbit );
        m_currentStart        = m_parentBlock->GetCurrentStart();
        m_currentEnd          = m_parentBlock->GetCurrentStart();
        s_pIsRunControl->seekg( m_currentStart );
        m_bLastFindSuccessful = false;
        // restore original exception stream states
        s_pIsRunControl->exceptions( ioOriginalState );
    } catch ( std::ios_base::failure  ) {
        std::ostringstream exceptMessage;
        exceptMessage << "Exception thrown by "
                      << "GRCItem::Reset( void )" << std::endl
                      << "Run control stream threw a failure exception."
                      << std::endl << std::ends;
        std::runtime_error ee(exceptMessage.str());
        throw ee;
    } catch (...) {
        std::cerr << "Exception caught by "
                  << "GRCItem::Reset()" << std::endl << std::endl;
        throw;
    }
}

//----------------------------------------------------------------------------//
void GRCItem::Rewind( void )
{
    try {
        // select stream states that will throw an exception
        std::ios_base::iostate ioOriginalState = s_pIsRunControl->exceptions();
        s_pIsRunControl->exceptions( std::ios_base::badbit );

        if (s_pIsRunControl->fail()) {
            s_pIsRunControl->clear();
        }

        s_pIsRunControl->seekg( m_currentStart );
        // restore original exception stream states
        s_pIsRunControl->exceptions( ioOriginalState );
    } catch ( std::ios_base::failure  ) {
        std::ostringstream exceptMessage;
        exceptMessage << "Exception thrown by "
                      << "GRCItem::Rewind( void )" << std::endl
                      << "Run control stream threw a failure exception."
                      << std::endl << std::ends;
        std::runtime_error ee(exceptMessage.str());
        throw ee;
    } catch (...) {
        std::cerr << "Exception caught by "
                  << "GRCItem::Rewind()" << std::endl << std::endl;
        throw;
    }
}

// -----------------------------------------------------------------------------
} // A3 namespace end
