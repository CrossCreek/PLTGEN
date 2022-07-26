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
 * StatSingleSample.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : StatSingleSample.cpp

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  2000-03-19  JAM  Initial Release
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :

//          note: internal angular units are radians

//****************************************************************************//

//=============================================================================
#include "StatSingleSample.h"

// using float.h since Microsoft's cfloat is not in the std namespace
#include <float.h>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
StatSingleSample::StatSingleSample()
    : m_numX( 0 ),
      m_sumX( 0.0 ),
      m_sumX2( 0.0 ),
      m_minX( DBL_MAX ),
      m_maxX( -DBL_MAX ) { ; }

void StatSingleSample::AddSample( double x )
{
    m_numX++;
    m_sumX  += x;
    m_sumX2 += x * x ;

    if ( m_minX > x ) {
        m_minX  = x;
    }

    if ( m_maxX < x ) {
        m_maxX  = x;
    }
}

void StatSingleSample::RemoveSample( double x )
{
    m_numX--;
    m_sumX  -= x;
    m_sumX2 -= x * x ;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
