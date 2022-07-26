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
 * OrbitElKeplerian.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// OrbitElKeplerian class
// Refer to OrbitElKeplerian.h for more info.
// =============================================================================
#include "OrbitElKeplerian.h"
#include "CoorTransEciPqw.h"
#include "IoSeparator.h"
#include <iomanip>
#include <math.h>
#include <sstream>
#include <stdexcept>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
OrbitElKeplerian::OrbitElKeplerian()
    : m_hdlEulerAngles( new EulerAnglesKeplerian )
{
    SetEpoch( 0.0 );
    SetSemiMajorAxis( 0.0 );
    SetEccentricity( 0.0 );
    SetMeanAnomaly( 0.0 );
    ResetAttribFlag( ALL );
}

OrbitElKeplerian::OrbitElKeplerian( TimeJ2000 epoch,
                                    double semimajorAxis,
                                    double eccentricity,
                                    Angle meanAnomaly,
                                    Angle raAscendingNode,
                                    Angle inclination,
                                    Angle argumentOfPerigee )
    : m_hdlEulerAngles( new EulerAnglesKeplerian(
                            raAscendingNode, inclination, argumentOfPerigee ) )
{
    ResetAttribFlag( ALL );
    SetAttribFlag( INC | RA | AP );
    SetEpoch( epoch );
    SetSemiMajorAxis( semimajorAxis );
    SetEccentricity( eccentricity );
    SetMeanAnomaly( meanAnomaly );
}

OrbitElKeplerian::OrbitElKeplerian( TimeJ2000 epoch,
                                    double semimajorAxis,
                                    double eccentricity,
                                    Angle meanAnomaly,
                                    const EulerAnglesKeplerian& eak )
    : m_hdlEulerAngles( new EulerAnglesKeplerian(eak) )
{
    ResetAttribFlag( ALL );
    SetAttribFlag( INC | RA | AP );
    SetEpoch( epoch );
    SetSemiMajorAxis( semimajorAxis );
    SetEccentricity( eccentricity );
    SetMeanAnomaly( meanAnomaly );
}

OrbitElKeplerian::OrbitElKeplerian(TimeJ2000 epoch, const MatrixFP<double>& a)
{
    if (6 != a.size())
        throw std::runtime_error(
            "OrbitElKeplerian(TimeJ2000, const MatrixFP<double>&)");

    ResetAttribFlag( ALL );
    SetEpoch( epoch );
    MatrixFP<double>::const_iterator i(a.begin());
    SetSemiMajorAxis(i[0]);
    SetEccentricity(i[1]);
    SetMeanAnomaly(i[2]);
    m_hdlEulerAngles = new EulerAnglesKeplerian(i[3], i[4], i[5]);
    SetAttribFlag( RA | INC | AP );
}

OrbitElKeplerian::OrbitElKeplerian( TimeJ2000 epoch,
                                    const VectorCartesian& position,
                                    const VectorCartesian& velocity,
                                    SvType svType )
{
    static const VectorCartesian north( 0.0, 0.0, 1.0 );

    if ( KEPLERIAN != svType ) {
        std::string strMsg( "OrbitElKeplerian(...): " );
        strMsg += "svType must be KEPLERIAN";
        throw std::runtime_error( strMsg );
    }

    double radius = position.Magnitude();
    VectorCartesian vAngularMomentum = position.CrossProduct( velocity );
    VectorCartesian uAngularMomentum = vAngularMomentum.Unit();
    VectorCartesian vNode( -vAngularMomentum[1], vAngularMomentum[0], 0.0 );
    VectorCartesian uNode( 1.0, 0.0, 0.0 );

    if ( 1.0e-12 < vNode.Magnitude() ) {
        uNode = vNode.Unit();
    }

    VectorCartesian vEcc = (( velocity.MagnitudeSquared()
                              - EARTH_GRAV_CONST_NMI3_PER_S2 / radius )
                            * position
                            - position.DotProduct( velocity )
                            * velocity ) / EARTH_GRAV_CONST_NMI3_PER_S2;
    double ecc = vEcc.Magnitude();
    VectorCartesian uEcc( 1.0, 0.0, 0.0 );

    if ( 1.0e-12 < ecc ) {
        uEcc = vEcc.Unit();
    }

    double slr = vAngularMomentum.MagnitudeSquared()
                 / EARTH_GRAV_CONST_NMI3_PER_S2;
    double sma = slr / ( 1 - ecc * ecc );
    double inc = acos( uAngularMomentum[2] );
    double ra  = uNode[1] >= 0.0
                 ? acos( uNode[0] )
                 : TWO_PI - acos( uNode[0] );
    double ap  = uEcc[2] >= 0.0
                 ? acos( uNode.DotProduct( uEcc ) )
                 : TWO_PI - acos( uNode.DotProduct( uEcc ) );
    double ta  = position.DotProduct( velocity ) >= 0.0
                 ? acos( uEcc.DotProduct( position ) / radius )
                 : TWO_PI - acos( uEcc.DotProduct( position ) / radius );
    ResetAttribFlag( ALL );
    m_hdlEulerAngles = Handle< EulerAnglesKeplerian >(
                           new EulerAnglesKeplerian( ra, inc, ap ) );
    SetAttribFlag( INC | RA | AP );
    SetEpoch( epoch );
    SetSemiMajorAxis( sma );
    SetEccentricity( ecc );
    SetTrueAnomaly( ta );
}

OrbitElKeplerian::OrbitElKeplerian( const OrbitElKeplerian &elem )
    : m_hdlEulerAngles( new EulerAnglesKeplerian( elem.m_hdlEulerAngles.Ref() ) )
{
    m_epoch = elem.m_epoch;
    m_sma   = elem.m_sma;
    m_ecc   = elem.m_ecc;
    m_ma    = elem.m_ma;
    m_slr   = elem.m_slr;
    m_attribFlag =  elem.m_attribFlag ;
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OrbitElKeplerian::SetEpoch( const TimeJ2000 epoch )
{
    m_epoch = epoch;
    SetAttribFlag( EPOCH );
}

void OrbitElKeplerian::SetSemiMajorAxis( double sma )
{
    m_sma = sma;
    SetAttribFlag( SMA );
    SetSemiLatusRectum();
}

void OrbitElKeplerian::SetEccentricity( double ecc )
{
    m_ecc = ecc;
    SetAttribFlag( ECC );
    SetSemiLatusRectum();
}

void OrbitElKeplerian::SetRAAscendingNode( Angle ra )
{
    SetAttribFlag( RA );
    m_hdlEulerAngles->SetRAAscendingNode( ra );
}

void OrbitElKeplerian::SetInclination( Angle inc )
{
    SetAttribFlag( INC );
    m_hdlEulerAngles->SetInclination( inc );
}

void OrbitElKeplerian::SetArgumentOfPeriapsis( Angle ap )
{
    SetAttribFlag( AP );
    m_hdlEulerAngles->SetArgumentOfPeriapsis( ap );
}

void OrbitElKeplerian::SetMeanAnomaly( Angle anomaly )
{
    m_ma = anomaly;
    SetAttribFlag( MA );
}

void OrbitElKeplerian::SetTrueAnomaly( Angle anomaly )
{
    if ( !IsAttribSet( ECC ) ) {
        std::ostringstream stmMsg;
        stmMsg << "Function: Angle OrbitElKeplerian"
               << "::SetTrueAnomaly( Angle anomaly )" << std::endl
               << "Eccentricity is not set!" << std::endl
               << "Eccentricity must be set before SetTrueAnomaly is called."
               << std::ends;
        std::invalid_argument except( stmMsg.str() );
        throw except;
    }

    m_ma = trueToMeanAnomaly( m_ecc, anomaly );
    SetAttribFlag( MA );
}

Angle OrbitElKeplerian::GetTrueAnomaly() const
{
    Angle anomaly;

    if ( IsAttribSet( ECC ) ) {
        anomaly = meanToTrueAnomaly( m_ecc, m_ma );
    } else {
        std::ostringstream stmMsg;
        stmMsg << "Function: Angle OrbitElKeplerian"
               << "::GetTrueAnomaly()" << std::endl
               << "Eccentricity is not set!" << std::endl
               << "Eccentricity must be set before GetTrueAnomaly is called."
               << std::ends;
        std::runtime_error except( stmMsg.str() );
        throw except;
    }

    return anomaly;
}

void OrbitElKeplerian::SetSemiLatusRectum()
{
    m_slr = m_sma * ( 1.0 - m_ecc * m_ecc );
}

void OrbitElKeplerian::SetEulerAngles( const EulerAnglesKeplerian &eulerAngles )
{
    m_hdlEulerAngles.Ref() = eulerAngles;
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

OrbitElKeplerian& OrbitElKeplerian::operator=( const OrbitElKeplerian &elem )
{
    if ( this != &elem ) {
        m_hdlEulerAngles.Ref() = elem.m_hdlEulerAngles.Ref();
        m_epoch = elem.m_epoch;
        m_sma = elem.m_sma;
        m_ecc = elem.m_ecc;
        m_ma = elem.m_ma;
        m_slr = elem.m_slr;
        m_attribFlag = elem.m_attribFlag ;
    }

    return *this;
}

bool OrbitElKeplerian::operator==( const OrbitElKeplerian& elem ) const
{
    return m_epoch == elem.m_epoch
           && m_sma == elem.m_sma
           && m_ecc == elem.m_ecc
           && m_ma  == elem.m_ma
           && m_slr  == elem.m_slr
           && m_hdlEulerAngles.Ref() == elem.m_hdlEulerAngles.Ref()
           && m_attribFlag == elem.m_attribFlag;
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OrbitElKeplerian::SetPeriapsisAndApoapsisRadius( double perigeeRadius,
        double apogeeRadius )
{
    m_sma = ( apogeeRadius + perigeeRadius ) / 2.0;
    m_ecc = ( apogeeRadius - perigeeRadius ) / ( apogeeRadius + perigeeRadius );
    SetAttribFlag( SMA );
    SetAttribFlag( ECC );
    SetSemiLatusRectum();
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
double OrbitElKeplerian::GetRadius() const
{
    return GetSemiLatusRectum()/(1.0+GetEccentricity()
                                 *cos(GetTrueAnomaly()));
}

void OrbitElKeplerian::GetKeplerianPositionVelocity(VectorCartesian& p,
        VectorCartesian& v)
{
    double r(GetRadius());
    double slr(GetSemiLatusRectum());
    double e(GetEccentricity());
    double s(sqrt(EARTH_GRAV_CONST/slr));
    Angle  ta(GetTrueAnomaly());
    double cta(cos(ta));
    double sta(sin(ta));
    VectorCartesian pqwP(r*cta, r*sta, 0.0);
    VectorCartesian pqwV(-s*sta, s*(e+cta), 0.0);
    CoorTransEciPqw xEciPqw(m_hdlEulerAngles);
    xEciPqw.ToBase(pqwP, pqwV, p, v);
}

void OrbitElKeplerian::GetElements(MatrixFP<double> &a) const
{
    if (6 != a.size()) {
        a.Resize(1,6);
    }

    MatrixFP<double>::iterator i(a.begin());
    i[0] = GetSemiMajorAxis();
    i[1] = GetEccentricity();
    i[2] = GetMeanAnomaly();
    i[3] = GetRAAscendingNode();
    i[4] = GetInclination();
    i[5] = GetArgumentOfPeriapsis();
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
std::ostream& OrbitElKeplerian::ReportState( std::ostream& os )
{
    os << "Base Elements" << std::endl
       << "------------------" << std::endl
       << "Epoch:                    " << m_epoch << std::endl
       << "Semi-major Axis:          " << m_sma << " nmi" << std::endl
       << "Eccentricity:             " << m_ecc << std::endl
       << "Mean Anomaly:             " << m_ma  << " deg"  << std::endl
       << "Rt Asc of Ascending Node: " << m_hdlEulerAngles->GetRAAscendingNode()
       << " deg" << std::endl
       << "Inclination:              " << m_hdlEulerAngles->GetInclination()
       << " deg" << std::endl
       << "Argument of Periapsis:    "
       << m_hdlEulerAngles->GetArgumentOfPeriapsis() << " deg" << std::endl
       << "\nOther Parameters" << std::endl
       << "----------------" << std::endl
       << "True Anomaly:             " << GetTrueAnomaly() << " deg" << std::endl
       << "True Arg of Latitude:     " << GetTrueAnomaly()+GetArgumentOfPeriapsis()
       << " deg" << std::endl
       << "Semi-latus Rectum:        " << m_slr << " nmi" << std::endl
       << "Periapsis Radius:         " << GetPeriapsisRadius() << " nmi" << std::endl
       << "Apoapsis Radius:          " << GetApoapsisRadius() << " nmi" << std::endl
       << std::endl;
    return os;
}

std::ostream& operator<<( std::ostream& os, const OrbitElKeplerian &elem )
{
    //WARNING_FIX:
    //1>src\OrbitElKeplerian.cpp(346): warning C4244: 'initializing' : conversion from 'std::streamsize' to 'int', possible loss of data
    long long int w = os.width();
    os << elem.GetEpoch() << ioSeparator << std::setw( w )
       << elem.GetSemiMajorAxis() << ioSeparator << std::setw( w )
       << elem.GetEccentricity() << ioSeparator << std::setw( w )
       << elem.GetMeanAnomaly() << ioSeparator << std::setw( w )
       << elem.GetHdlConstEulerAngles().Ref();
    return os;
}

std::istream& operator>>( std::istream& is, OrbitElKeplerian &elem )
{
    TimeJ2000 epoch;
    double sma, ecc;
    Angle ma;
    EulerAnglesKeplerian eulerAngles;
    is >> epoch >> sma >> ecc >> ma >> eulerAngles;
    elem.SetEpoch( epoch );
    elem.SetSemiMajorAxis( sma );
    elem.SetEccentricity( ecc );
    elem.SetMeanAnomaly( ma );
    elem.SetEulerAngles( eulerAngles );
    return is;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
