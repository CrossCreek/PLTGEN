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
 * OrbitElKeplerian.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : OrbitElKeplerian.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : OrbitElKeplerian

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
//              V1.4      05/22/2001  JAM  added constructor to accept
//                                         a state vector
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//              Change: added empty destructor
//              Reason: stops pointless no destructor warnings

//              V4.4-001  2008-08-22  JAM  added get argument of latitude fctns
//                                         removed inlines from class def

//              V4.5-001   2009-07-01 JAM  Added Handle.h include, required
//                                         since HandleConst.h not longer
//                                         includes Handle.h
//                                         Added get Keplerian position &
//                                         velocity function
//                                         Added get radius function

//  NOTES :

//****************************************************************************//
#ifndef ORBITELKEPLERIAN_H
#define ORBITELKEPLERIAN_H  "OrbitElKeplerian V4.5"

#include "anomaly.h"
#include "TimeJ2000.h"
#include "HandleConst.h"
#include "Handle.h"
#include "EulerAnglesKeplerian.h"
#include "Angle.h"
#include "GenMath.h"
#include "VectorCartesian.h"
#include "Normalize.h"
#include "MatrixFP.h"

// =============================================================================
// A3 namespace start
namespace A3
{
// =============================================================================
// forward declaration & prototypes
class OrbitElKeplerian;

std::ostream& operator<<( std::ostream& os, const OrbitElKeplerian &elem );
std::istream& operator>>( std::istream& is, OrbitElKeplerian &elem );

// =============================================================================
class OrbitElKeplerian
{
public:
    enum SvType { KEPLERIAN, MJ2, MJ2J4, OSCULATING };

    OrbitElKeplerian();

    OrbitElKeplerian( TimeJ2000 epoch,
                      double semimajorAxis,
                      double eccentricity,
                      Angle  meanAnomaly,
                      Angle  raAscendingNode,
                      Angle  inclination,
                      Angle  argumentOfPerigee );

    OrbitElKeplerian( TimeJ2000 epoch,
                      double semimajorAxis,
                      double eccentricity,
                      Angle  meanAnomaly,
                      const EulerAnglesKeplerian& eak );

    OrbitElKeplerian( TimeJ2000 epoch,
                      const VectorCartesian& position,
                      const VectorCartesian& velocity,
                      SvType svType = KEPLERIAN );

    OrbitElKeplerian( const OrbitElKeplerian &elem );

    virtual ~OrbitElKeplerian() {;}

    // ---------------------------------------------------------------------
    TimeJ2000 GetEpoch() const;
    void SetEpoch( const TimeJ2000 epoch );

    double GetSemiMajorAxis() const;
    void SetSemiMajorAxis( double sma );

    double GetEccentricity() const;
    void SetEccentricity( double ecc );

    Angle GetInclination() const;
    void SetInclination( Angle inc );

    Angle GetRAAscendingNode() const;
    void SetRAAscendingNode( Angle ra );

    Angle GetArgumentOfPeriapsis() const;
    void SetArgumentOfPeriapsis( Angle ap );

    Angle GetMeanAnomaly() const;
    void SetMeanAnomaly( Angle anomaly );

    // ---------------------------------------------------------------------
    // Euler Angle Access ( RAAN, Incl, ArgPer )
    HandleConst< EulerAnglesKeplerian > GetHdlConstEulerAngles() const;

    void SetEulerAngles( const EulerAnglesKeplerian& eulerAngles );
    // ---------------------------------------------------------------------
    // operators
    OrbitElKeplerian& operator=( const OrbitElKeplerian &elem );

    bool operator==( const OrbitElKeplerian& elem ) const;
    bool operator!=( const OrbitElKeplerian& elem ) const;

    // ---------------------------------------------------------------------
    // alternative elements
    double GetSemiLatusRectum() const;

    // eccentricity must be set for these functions to work
    Angle GetTrueAnomaly() const;
    void SetTrueAnomaly( Angle anomaly );

    // In lieu of semi-major axis and eccentricity for ecc < 1.0
    void SetPeriapsisAndApoapsisRadius( double periapsisRadius,
                                        double apoapsisRadius );

    Angle GetTrueArgOfLat() const;
    Angle GetMeanArgOfLat() const;

    double GetPeriapsisRadius() const;
    double GetApoapsisRadius() const;

    double GetRadius() const;

    void GetKeplerianPositionVelocity(VectorCartesian& p,
                                      VectorCartesian& v);

    // ---------------------------------------------------------------------
    std::ostream& ReportState( std::ostream& os );

    // ---------------------------------------------------------------------
    virtual const char *GetVersion() const;

    // ---------------------------------------------------------------------
    friend void meanToOsculating(const OrbitElKeplerian&,
                                 OrbitElKeplerian&);

    friend bool osculatingToMean(const OrbitElKeplerian&,
                                 OrbitElKeplerian&);

    // -------------------------------------------------------------------------
protected:
    OrbitElKeplerian(TimeJ2000 epoch, const MatrixFP<double>& a);
    void GetElements(MatrixFP<double> &a) const;

    // ---------------------------------------------------------------------
    void SetSemiLatusRectum();

    // -------------------------------------------------------------------------
private:
    TimeJ2000  m_epoch;     // epoch of state, TimeJ2000 sec

    double m_sma;       // semi-major axis, nmi
    double m_ecc;       // eccentricity
    Angle m_ma;        // mean anomaly, rad

    Handle< EulerAnglesKeplerian > m_hdlEulerAngles;

    double m_slr;       // semi-latus rectum, nmi

    // ---------------------------------------------------------------------

    enum AttribFlag { NONE   = 0x00, EPOCH = 0x01, SMA = 0x02,
                      ECC    = 0x04, INC   = 0x08, RA  = 0x10,
                      AP     = 0x20, MA    = 0x40, TA  = 0x80,
                      NORMAL = 0x7F, ALL   = 0xFF
                    };

    typedef unsigned char AttribType;

    // ---------------------------------------------------------------------
    AttribType m_attribFlag;

    void SetAttribFlag( AttribType flag );
    void ResetAttribFlag( AttribType flag );
    bool IsAttribSet( AttribType flag ) const ;
};

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
inline TimeJ2000 OrbitElKeplerian::GetEpoch() const
{
    return m_epoch;
}

inline double OrbitElKeplerian::GetSemiMajorAxis() const
{
    return m_sma;
}

inline double OrbitElKeplerian::GetEccentricity() const
{
    return m_ecc;
}

inline Angle OrbitElKeplerian::GetRAAscendingNode() const
{
    return m_hdlEulerAngles->GetRAAscendingNode();
}

inline Angle OrbitElKeplerian::GetInclination() const
{
    return m_hdlEulerAngles->GetInclination();
}

inline Angle OrbitElKeplerian::GetArgumentOfPeriapsis() const
{
    return m_hdlEulerAngles->GetArgumentOfPeriapsis();
}

inline Angle OrbitElKeplerian::GetMeanAnomaly() const
{
    return m_ma;
}

inline HandleConst< EulerAnglesKeplerian >
OrbitElKeplerian::GetHdlConstEulerAngles() const
{
    return m_hdlEulerAngles;
}

inline double OrbitElKeplerian::GetSemiLatusRectum() const
{
    return m_slr;
}

inline Angle OrbitElKeplerian::GetMeanArgOfLat() const
{
    return normalizeZeroTwoPi(GetMeanAnomaly()+GetArgumentOfPeriapsis());
}

inline Angle OrbitElKeplerian::GetTrueArgOfLat() const
{
    return normalizeZeroTwoPi(GetTrueAnomaly()+GetArgumentOfPeriapsis());
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
inline bool OrbitElKeplerian::operator!=( const OrbitElKeplerian& elem ) const
{
    return !operator==( elem );
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
inline double OrbitElKeplerian::GetPeriapsisRadius() const
{
    return m_sma * ( 1.0 - m_ecc );
}

inline double OrbitElKeplerian::GetApoapsisRadius() const
{
    return m_sma * ( 1.0 + m_ecc );
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
inline void
OrbitElKeplerian::SetAttribFlag( OrbitElKeplerian::AttribType flag )
{
    setBitsOn( flag, m_attribFlag );
    // if ( flag & TA ) ResetAttribFlag( MA );
    // if ( flag & MA ) ResetAttribFlag( TA );
}

inline void
OrbitElKeplerian::ResetAttribFlag( OrbitElKeplerian::AttribType flag )
{
    setBitsOff( flag, m_attribFlag );
}

inline bool
OrbitElKeplerian::IsAttribSet( OrbitElKeplerian::AttribType flag ) const
{
    return areBitsOn( flag, m_attribFlag );
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

inline const char *OrbitElKeplerian::GetVersion() const
{
    return ORBITELKEPLERIAN_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================

#endif
