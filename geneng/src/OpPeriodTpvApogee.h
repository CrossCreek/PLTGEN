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
 * OpPeriodTpvApogee.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : OpPeriodTpvApogee.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : OpPeriodTpvApogee

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  980126  JAM  Alpha test
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: reordered data members
//              Reason: stops pointless initialized here warnings

//  NOTES :



//****************************************************************************//

//==============================================================================
#ifndef OPPERIODTPVAPOGEE_H
#define OPPERIODTPVAPOGEE_H	"OpPeriodTpvApogee Version V4.3-009"

//------------------------------------------------------------------------------
// required headers
#include "timePerigeeApogee.h"
#include "OpPeriodTpv.h"
#include "TestValue.h"
#include "SpatialMJ2J4.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class OpPeriodTpvApogee : public OpPeriodTpv
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    inline OpPeriodTpvApogee( Handle< Spatial > hdlSpatialm,
                              double timeSinceApogeeStart,
                              double timeSinceApogeeEnd );
    inline virtual ~OpPeriodTpvApogee();

    //----------------------------------------------------------------------
    // test function
    inline virtual bool Test();

    //----------------------------------------------------------------------
    // identification functions
    inline virtual OpPeriodType GetType() const;

    //----------------------------------------------------------------------
    // operators
    inline bool operator==( const OpPeriodTpvApogee& opPeriod ) const;
    inline bool operator!=( const OpPeriodTpvApogee& opPeriod ) const;

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
    // pointers used to save time
    // holding these pointers is safe since the OpPeriod class
    // holds a handle to the platform, whichs holds a handle
    // to Spatial
    SpatialMJ2J4* m_hdlSpatialMJ2J4;

    TestBounds<double> m_bndApogeeTm;
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// constructors & destructors
inline OpPeriodTpvApogee::OpPeriodTpvApogee( Handle< Spatial > hdlSpatial,
        double timeSinceApogeeStart,
        double timeSinceApogeeEnd )
    : OpPeriodTpv( hdlSpatial ),
      m_hdlSpatialMJ2J4( NULL ),
      m_bndApogeeTm( timeSinceApogeeStart, timeSinceApogeeEnd )
{
    char m_fFixTime = 0;
    double tmSinceApo;
    m_hdlSpatialMJ2J4 = dynamic_cast< SpatialMJ2J4* >( m_hdlSpatial.Ptr() );

    if ( timeSinceApogeeStart < 0.0 ) {
        m_fFixTime = 0x01;
    }

    if ( timeSinceApogeeEnd < 0.0 ) {
        m_fFixTime = char( m_fFixTime | 0x02 );
    }

    if ( m_fFixTime != 0 ) {
        double lowLimit  = timeSinceApogeeStart;
        double highLimit = timeSinceApogeeEnd;
        double period;

        if ( m_hdlSpatialMJ2J4 != NULL ) {
            period = TWO_PI / m_hdlSpatialMJ2J4->GetAnomalisticMeanMotion();
            tmSinceApo = m_hdlSpatialMJ2J4->GetTimeSinceApogee();
        } else {
            double meanMotion = 0.0;
            tmSinceApo = timeSinceApogee( m_hdlSpatial->GetPosition(),
                                          m_hdlSpatial->GetVelocity(),
                                          meanMotion );
            period = TWO_PI / meanMotion;
        }

        if ( (m_fFixTime & 0x01) != 0 ) {
            lowLimit = fmod( timeSinceApogeeStart, period );
            lowLimit += lowLimit < 0.0 ? period : 0.0;
        }

        if ( (m_fFixTime & 0x02) != 0 ) {
            highLimit = fmod( timeSinceApogeeEnd, period );
            highLimit += highLimit < 0.0 ? period : 0.0;
        }

        m_fFixTime = 0;
        m_bndApogeeTm = TestBounds< double >( lowLimit, highLimit );
    } else {
        if ( m_hdlSpatialMJ2J4 != NULL ) {
            tmSinceApo = m_hdlSpatialMJ2J4->GetTimeSinceApogee();
        } else {
            tmSinceApo = timeSinceApogee( m_hdlSpatial->GetPosition(),
                                          m_hdlSpatial->GetVelocity() );
        }
    }

    m_bLastStatus = m_bndApogeeTm.Test( tmSinceApo );
}

inline OpPeriodTpvApogee::~OpPeriodTpvApogee()
{
    ;
}

//------------------------------------------------------------------------------
// test function
inline bool OpPeriodTpvApogee::Test()
{
    double tmSinceApo = 0;

    if ( m_hdlSpatialMJ2J4 != NULL ) {
        tmSinceApo = m_hdlSpatialMJ2J4->GetTimeSinceApogee();
    } else
        tmSinceApo = timeSinceApogee( m_hdlSpatial->GetPosition(),
                                      m_hdlSpatial->GetVelocity() );

    m_bLastStatus = m_bndApogeeTm.Test( tmSinceApo );
    return m_bLastStatus;
}

//------------------------------------------------------------------------------
// identification functions
inline OpPeriodTpvApogee::OpPeriodType OpPeriodTpvApogee::GetType() const
{
    return TPV_PERIGEE;
}

//------------------------------------------------------------------------------
// operators
inline bool OpPeriodTpvApogee::operator==( const OpPeriodTpvApogee& opPeriod ) const
{
    return OpPeriodTpv::operator==( opPeriod )
           && ( m_bndApogeeTm == opPeriod.m_bndApogeeTm );
}

inline bool OpPeriodTpvApogee::operator!=( const OpPeriodTpvApogee& opPeriod ) const
{
    return !operator==( opPeriod );
}

//------------------------------------------------------------------------------
// misc functions
inline const char* OpPeriodTpvApogee::GetVersion() const
{
    return OPPERIODTPVAPOGEE_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
