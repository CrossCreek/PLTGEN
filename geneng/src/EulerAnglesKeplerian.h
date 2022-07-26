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
 * EulerAnglesKeplerian.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : EulerAnglesKeplerian.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : EulerAnglesKeplerian

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM   new general engineering library
//                                          derived from fcntlib and SE
//              V1.2-026  09/01/1999  JAM   additional i/o support for
//                                          SST reporting
//              V1.3-006  12/20/1999  JAM  reorganized manipulator decl & defs
//                                         to reduce compile time overhead;
//                                         made IoManipulator more
//                                         flexible; changed ioSeparator from
//                                         global to stream dependent (hence
//                                         more flexible) like other manips
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.5-001   2009-07-01 JAM  changed insertion operator ref to
//                                         const ref

//  NOTES :

//****************************************************************************//

#ifndef EULERANGLESKEPLERIAN_H
#define EULERANGLESKEPLERIAN_H  "EulerAnglesKeplerian V4.5"

#include "EulerAngles.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
//  forward declarations
class EulerAnglesKeplerian;

// inline i/o stream operators
std::istream&
operator>>( std::istream& is, EulerAnglesKeplerian& angles );

std::ostream&
operator<<( std::ostream& os, const EulerAnglesKeplerian& angles );

//  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class EulerAnglesKeplerian : public EulerAngles
{
public:
    // constructors
    EulerAnglesKeplerian();
    EulerAnglesKeplerian( Angle raAscendingNode,
                          Angle inclination,
                          Angle argumentOfPeriapsis );

    // -------------------------------------------------------------------
    // access
    void SetRAAscendingNode( Angle raAscendingNode );
    void SetInclination( Angle inclination );
    void SetArgumentOfPeriapsis( Angle argumentOfPeriapsis );

    Angle GetRAAscendingNode() const;
    Angle GetInclination() const;
    Angle GetArgumentOfPeriapsis() const;

    // -------------------------------------------------------------------
    // general class functions
    virtual const char *GetVersion() const;

    // ---------------------------------------------------------------------
protected:
    // ---------------------------------------------------------------------
private:
};

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// constructors & destructors
inline
EulerAnglesKeplerian::EulerAnglesKeplerian()
    : EulerAngles( 313 ) { ; }

inline
EulerAnglesKeplerian::EulerAnglesKeplerian( Angle raAscendingNode,
        Angle inclination,
        Angle argumentOfPeriapsis  )
    : EulerAngles( 313, raAscendingNode, inclination, argumentOfPeriapsis )
{ ; }

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
inline void
EulerAnglesKeplerian::SetRAAscendingNode( Angle raAscendingNode )
{
    m_v[0] = raAscendingNode;
}

inline void
EulerAnglesKeplerian::SetInclination( Angle inclination )
{
    m_v[1] = inclination;
}

inline void
EulerAnglesKeplerian::SetArgumentOfPeriapsis( Angle argumentOfPeriapsis )
{
    m_v[2] = argumentOfPeriapsis;
}

inline Angle
EulerAnglesKeplerian::GetRAAscendingNode() const
{
    return m_v[0];
}

inline Angle
EulerAnglesKeplerian::GetInclination() const
{
    return m_v[1];
}

inline Angle
EulerAnglesKeplerian::GetArgumentOfPeriapsis() const
{
    return m_v[2];
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// general class functions
inline const char*
EulerAnglesKeplerian::GetVersion() const
{
    return EULERANGLESKEPLERIAN_H ;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif // EULERANGLESKEPLERIAN_H
