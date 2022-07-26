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
 * EulerAngles.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : EulerAngles.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : EulerAngles

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  new general engineering library
//                                         derived from fcntlib and SE
//              V1.2-026  09/01/1999  JAM  additional i/o support for
//                                         SST reporting
//              V1.3-006  12/20/1999  JAM  reorganized manipulator decl & defs
//                                         to reduce compile time overhead;
//                                         made IoManipulator more
//                                         flexible; changed ioSeparator from
//                                         global to stream dependent (hence
//                                         more flexible) like other manips
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: changed WIN32 marco to _MSC_VER
//              Reason: newer GNU compilers recognize MS predefined macros

//              V5.0      2009-12-18 JAM  add functions to compute DCM of
//                                        euler angle rotations
//                                        added Vector3DFP inheritance to
//                                        reduce code redundancyy

//  NOTES :

//****************************************************************************//
#ifndef EULERANGLES_H
#define EULERANGLES_H  __FILE__ " " __DATE__

#include "VectorReal3D.h"
#include "Angle.h"
#include <iosfwd>

// A3 namespace start
namespace A3
{
//  forward declarations
class EulerAngles;

// inline i/o stream operators
std::istream&
operator>>( std::istream& is, EulerAngles& eulerAngles );

std::ostream&
operator<<( std::ostream& os, const EulerAngles& eulerAngles );

class EulerAngles : public Vector3DFP<Angle>
{
public:
    // constructors
    EulerAngles();

    explicit EulerAngles( unsigned rotationSequence );

    EulerAngles( unsigned rotationSequence,
                 Angle alpha, Angle beta, Angle gamma );
    virtual ~EulerAngles() {;}

    // access
    void SetAlpha( Angle alpha );
    void SetBeta( Angle beta );
    void SetGamma( Angle gamma );

    Angle GetAlpha() const;
    Angle GetBeta() const;
    Angle GetGamma() const;

    void SetRotationSequence( unsigned rotationSequence );
    unsigned GetRotationSequence() const;

    bool operator==( const EulerAngles& eulerAngles ) const;
    bool operator!=( const EulerAngles& eulerAngles ) const;

protected:
    unsigned m_seq;

private:
};

// -------------------------------------------------------------------------
inline void
EulerAngles::SetAlpha( Angle alpha )
{
    m_v[0] = alpha;
}

inline void
EulerAngles::SetBeta( Angle beta )
{
    m_v[1] = beta;
}

inline void
EulerAngles::SetGamma( Angle gamma )
{
    m_v[2] = gamma;
}

inline Angle
EulerAngles::GetAlpha() const
{
    return m_v[0];
}

inline Angle
EulerAngles::GetBeta() const
{
    return m_v[1];
}

inline Angle
EulerAngles::GetGamma() const
{
    return m_v[2];
}

inline void
EulerAngles::SetRotationSequence(unsigned rotationSequence)
{
    m_seq = rotationSequence;
}

inline unsigned
EulerAngles::GetRotationSequence() const
{
    return m_seq;
}

} // A3 namespace end

#endif // EULERANGLES_H
