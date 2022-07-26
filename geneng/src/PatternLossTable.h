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
 * PatternLossTable.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : PatternLossTable.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : PatternLossTable (abstract class)

//  AUTHOR(s) : J. McCormick, L. DiGrasso

//  UPDATES   : V1.3-007  10/15/1999
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.5-001  2009-07-01  JAM
//              replaced use of "int" in as size type with size_type
//              to eliminate 64-bit warnings

//  NOTES :  Given am off-bore sight angle, return the pattern loss



//****************************************************************************//

//==============================================================================
#ifndef PATTERNLOSSTABLE_H
#define PATTERNLOSSTABLE_H  "PatternLossTable 2.0"

//------------------------------------------------------------------------------
// required headers
#include "PatternLoss.h"
// #include "Table2DLinear.h"
#include "Table2DLagrangePoly.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class PatternLossTable : public PatternLoss
{
    //--------------------------------------------------------------------------
public:
    typedef Table2DLagrangePoly<Angle, double>::size_type size_type;
    //----------------------------------------------------------------------
    // constructors & destructors
    // virtual destructor is defined since this class will be inherited
    PatternLossTable();
    virtual ~PatternLossTable() { ; }

    //----------------------------------------------------------------------
    // compute antenna pattern loss
    virtual double
    GetPatternLoss_dB( Angle offBoreSightAngle ) const;

    size_type LoadTable( std::istream & stm );

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
    //----------------------------------------------------------------------
    Table2DLagrangePoly< Angle, double > m_table;
};

//------------------------------------------------------------------------------
// misc functions
inline const char* PatternLossTable::GetVersion() const
{
    return PATTERNLOSSTABLE_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
