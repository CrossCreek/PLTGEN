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
 * OpPeriodTpvPerigee.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : OpPeriodTpvPerigee.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : OpPeriodTpvPerigee

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  980126  JAM  Alpha test
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: reordered data members
//              Reason: stops pointless initialized here warnings

//  NOTES : Assumes the object is in orbit
//          Assumes the mean motion of the object is constant



//****************************************************************************//

//==============================================================================
#ifndef OPPERIODTPVPERIGEE_H
#define OPPERIODTPVPERIGEE_H	"OpPeriodTpvPerigee V4.3-009"

//------------------------------------------------------------------------------
// required headers
#include "timePerigeeApogee.h"
#include "OpPeriodTpv.h"
#include "SpatialMJ2J4.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class OpPeriodTpvPerigee : public OpPeriodTpv
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    inline OpPeriodTpvPerigee( Handle< Spatial > hdlSpatial,
                               double timeSincePerigeeStart,
                               double timeSincePerigeeEnd );
    inline virtual ~OpPeriodTpvPerigee();

    //----------------------------------------------------------------------
    // test function
    inline virtual bool Test();

    //----------------------------------------------------------------------
    // identification functions
    inline virtual OpPeriodType GetType() const;

    //----------------------------------------------------------------------
    // operators
    inline bool operator==( const OpPeriodTpvPerigee& opPeriod ) const;
    inline bool operator!=( const OpPeriodTpvPerigee& opPeriod ) const;

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
    SpatialMJ2J4* m_hdlSpatialMJ2J4;
    TestBounds< double > m_bndPerigeeTm;
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// constructors & destructors
inline OpPeriodTpvPerigee::OpPeriodTpvPerigee( Handle< Spatial > hdlSpatial,
        double timeSincePerigeeStart,
        double timeSincePerigeeEnd )
    : OpPeriodTpv( hdlSpatial ),
      m_hdlSpatialMJ2J4( NULL ),
      m_bndPerigeeTm( timeSincePerigeeStart, timeSincePerigeeEnd )
{
    char m_fFixTime = 0;
    double tmSincePer;
    m_hdlSpatialMJ2J4 = dynamic_cast< SpatialMJ2J4* >( m_hdlSpatial.Ptr() );

    if ( timeSincePerigeeStart < 0.0 ) {
        m_fFixTime = 0x01;
    }

    if ( timeSincePerigeeEnd < 0.0 ) {
        m_fFixTime = char( m_fFixTime | 0x02 );
    }

    if ( m_fFixTime != 0 ) {
        double lowLimit  = timeSincePerigeeStart;
        double highLimit = timeSincePerigeeEnd;
        double period;

        if ( m_hdlSpatialMJ2J4 != NULL ) {
            period = TWO_PI / m_hdlSpatialMJ2J4->GetAnomalisticMeanMotion();
            tmSincePer = m_hdlSpatialMJ2J4->GetTimeSincePerigee();
        } else {
            double meanMotion = 0.0;
            tmSincePer = timeSincePerigee( m_hdlSpatial->GetPosition(),
                                           m_hdlSpatial->GetVelocity(),
                                           meanMotion );
            period = TWO_PI / meanMotion;
        }

        if ( (m_fFixTime & 0x01) != 0 ) {
            lowLimit = fmod( timeSincePerigeeStart, period );
            lowLimit += lowLimit < 0.0 ? period : 0.0;
        }

        if ( (m_fFixTime & 0x02) != 0 ) {
            highLimit = fmod( timeSincePerigeeEnd, period );
            highLimit += highLimit < 0.0 ? period : 0.0;
        }

        m_fFixTime = 0;
        m_bndPerigeeTm = TestBounds< double >( lowLimit, highLimit );
    } else {
        if ( m_hdlSpatialMJ2J4 != NULL ) {
            tmSincePer = m_hdlSpatialMJ2J4->GetTimeSincePerigee();
        } else {
            tmSincePer = timeSincePerigee( m_hdlSpatial->GetPosition(),
                                           m_hdlSpatial->GetVelocity() );
        }
    }

    m_bLastStatus = m_bndPerigeeTm.Test( tmSincePer );
}

inline OpPeriodTpvPerigee::~OpPeriodTpvPerigee()
{
    ;
}

//------------------------------------------------------------------------------
// test function
inline bool OpPeriodTpvPerigee::Test()
{
    double tmSincePer = 0;

    if ( m_hdlSpatialMJ2J4 != NULL ) {
        tmSincePer = m_hdlSpatialMJ2J4->GetTimeSincePerigee();
    } else
        tmSincePer = timeSincePerigee( m_hdlSpatial->GetPosition(),
                                       m_hdlSpatial->GetVelocity() );

    m_bLastStatus = m_bndPerigeeTm.Test( tmSincePer );
    return m_bLastStatus;
}

//------------------------------------------------------------------------------
// identification functions
inline OpPeriodTpvPerigee::OpPeriodType OpPeriodTpvPerigee::GetType() const
{
    return TPV_PERIGEE;
}

//------------------------------------------------------------------------------
// operators
inline bool OpPeriodTpvPerigee::operator==( const OpPeriodTpvPerigee& opPeriod ) const
{
    // don't need to compare m_hdlSpatial and m_hdlSpatialMJ2J4
    return OpPeriodTpv::operator==( opPeriod )
           && ( m_bndPerigeeTm == opPeriod.m_bndPerigeeTm );
}

inline bool OpPeriodTpvPerigee::operator!=( const OpPeriodTpvPerigee& opPeriod ) const
{
    return !operator==( opPeriod );
}

//------------------------------------------------------------------------------
// misc functions
inline const char* OpPeriodTpvPerigee::GetVersion() const
{
    return OPPERIODTPVPERIGEE_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
