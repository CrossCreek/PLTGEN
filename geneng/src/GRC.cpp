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
 * GRC.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : GRC.h

//  NAMESPACE : A3

//  SYSTEM    : SATSIM 2000

//  SUBSYSTEM : SST General Library

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  2000-03-29  JAM  Initial release of this software
//              V2.0      04/29/2002  JAM  1st A3 library build
//              v5.0       04/13/2010 KER add security banner


//  NOTES : Processes run control for the SatSim system
//          The full tags are used in this class (i.e. <TAG_STRING>)
//          in lieu of just the unique string (i.e. TAG_STRING) to
//          simplify searching (i.e. reduce the number of false hits),
//          scanning (i.e. its easier to spot a tag in the code if
//          it bounded by brackets) and automated comparison of tags that
//          are contained in a run control file and this class

//          This class was designed to be inherited by other classes designed
//          to read a particular section of the Run Control file.
//          Public and privated functions have been defined as virtual.

//****************************************************************************//

#include "GRC.h"

#include <iostream>        // For cerr
#include <stdexcept>       // For standard exception handling classes

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
GRC::GRC( const std::string &nmRcFile )
{
    std::ifstream runControlFile;
    std::streampos rcStartPos, rcEndPos;

    try {
        // load run control into memory for processing
        m_rcStream.exceptions( std::ios_base::badbit );
        m_strRunControlFileNm = nmRcFile;
        runControlFile.exceptions( std::ios_base::badbit );
        runControlFile.open( m_strRunControlFileNm.c_str(), std::ios::in );
        m_rcStream << runControlFile.rdbuf();
        runControlFile.close();
        // set the start and end position of rc control file
        // these values must be initialize before FindUnboundedToken
        // can be used
        m_rcStream.seekg( 0, std::ios::end );
        rcEndPos    = m_rcStream.tellg();
        m_rcStream.seekg( 0, std::ios::beg );
        rcStartPos  = m_rcStream.tellg();
        m_itmRunControl.Set( m_rcStream, rcStartPos, rcEndPos );
        // find the start and end position of rc
        // and initialize the other positions
        InitItems();
    } catch (...) {
        if ( runControlFile.is_open() ) {
            runControlFile.close();
        }

        std::cerr << "Exception caught by\n"
                  << "GRC::GRC(const string &nmRcFile)\n"
                  << std::endl;
        throw;
    }
}

//----------------------------------------------------------------------------//

GRC::~GRC()
{
}

//----------------------------------------------------------------------------//

void GRC::InitItems()
{
    try {
        m_itmClassification.Set("<CLASSIFICATION>", m_itmRunControl);
        m_itmSecurityBanner.Set("<SECURITY_BANNER_START>", "<SECURITY_BANNER_END>", m_itmRunControl);
        m_itmDescription.Set("<DESCRIPTION_START>", "<DESCRIPTION_END>", m_itmRunControl);
    } catch (...) {
        std::cerr << "Exception caught by GRC::InitItems()\n"
                  << std::endl;
        throw;
    }
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
