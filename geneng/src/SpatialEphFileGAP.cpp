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
 * SpatialEphFileGAP.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//==============================================================================
//                         U N C L A S S I F I E D
//==============================================================================
// UNITS 030109
// Function Arg & Return: Standard GenEngr (see constants.h)
// Stream I/O:            N/A
// Initialization Files:  nmi, s

// ============================================================================
//  AutoUpdate Enabled
//  On copy and assignment, resources are SHARED (i.e., NOT duplicated)

//  FILE      : SpatialEphFileGAP.h

//  NAMESPACE : A3

//  MODULE(s) : SpatialEphFileGAP

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V01.00    2007-07-25  JAM  Initial Release for EMAF
//              V4.4-001  2008-10-03  JAM  1st SATSIM build
//                                         Changed from TimeClockVT to TimeClock
//                                         Changed units to nmi and s
//                                         Moved RefTime from file
//                                         to constructor
//                                         Changed pos & vel before start to
//                                         location of 1st way pt w/ECF vel = 0
//                                         pos & vel after end to locations of
//                                         last way pt w/ ECF vel = 0
//              V4.5-001  2009-07-01  JAM  Deleted unused local test variables
//                                         Applied new/more eff mtx multiply fcn

//  NOTES :

//==============================================================================
#include "SpatialEphFileGAP.h"
#include "VectorGeodetic.h"

#include <fstream>

// ============================================================================
// A3 namespace start
namespace A3
{
// constructors & destructors
// Position & Velocity in ECI
SpatialEphFileGAP
::SpatialEphFileGAP( HandleConst< TimeJ2000 > hTime )
    : SpatialTimeDep( hTime ), m_coorTransEciEcf( hTime ) { ; }

// Coordinate Transformation to be applied to position & velocity
SpatialEphFileGAP
::SpatialEphFileGAP( HandleConst< TimeJ2000 > hTime,
                     Handle< CoorTransBaseEci > hCoorTrans )
    : SpatialTimeDep( hTime, hCoorTrans ), m_coorTransEciEcf( hTime ) { ; }

SpatialEphFileGAP
::SpatialEphFileGAP( HandleConst< TimeJ2000 > hTime,
                     const TimeJ2000& refTime,
                     const std::string& sFileNm )
    : SpatialTimeDep( hTime ), m_epoch( refTime ),
      m_coorTransEciEcf( hTime ), m_sFileNm( sFileNm )
{
    try {
        ReadExtWayPtFile();
    } catch ( ... ) {
        std::cerr << "SpatialEphFileGAP(HandleConst<TimeJ2000>, "
                  << "const TimeJ2000&, const std::string&)"  << std::endl;
        throw;
    }
}

// Coordinate Transformation to be applied to position & velocity
SpatialEphFileGAP
::SpatialEphFileGAP( HandleConst< TimeJ2000 > hTime,
                     Handle< CoorTransBaseEci > hCoorTrans,
                     const TimeJ2000& refTime,
                     const std::string& sFileNm )
    : SpatialTimeDep( hTime, hCoorTrans ), m_epoch( refTime ),
      m_coorTransEciEcf( hTime ), m_sFileNm( sFileNm )
{
    try {
        ReadExtWayPtFile();
    } catch ( ... ) {
        std::cerr << "SpatialEphFileGAP(HandleConst< TimeJ2000>, "
                  << "Handle<CoorTransBaseEci>, "
                  << "const TimeJ2000&, const std::string&)"  << std::endl;
        throw;
    }
}

SpatialEphFileGAP
::SpatialEphFileGAP( HandleConst< TimeJ2000 > hTime,
                     const TimeJ2000& refTime,
                     std::istream& isWayPts )
    : SpatialTimeDep( hTime ), m_epoch( refTime ), m_coorTransEciEcf( hTime )
{
    try {
        ReadExtWayPt(isWayPts);
    } catch ( ... ) {
        std::cerr << "SpatialEphFileGAP(HandleConst< TimeJ2000>, "
                  << "const TimeJ2000&, std::istream&)"  << std::endl;
        throw;
    }
}

// Coordinate Transformation to be applied to position & velocity
SpatialEphFileGAP
::SpatialEphFileGAP( HandleConst< TimeJ2000 > hTime,
                     Handle< CoorTransBaseEci > hCoorTrans,
                     const TimeJ2000& refTime,
                     std::istream& isWayPts )
    : SpatialTimeDep( hTime, hCoorTrans ), m_epoch( refTime ),
      m_coorTransEciEcf( hTime )
{
    try {
        ReadExtWayPt(isWayPts);
    } catch ( ... ) {
        std::cerr << "SpatialEphFileGAP(HandleConst< TimeJ2000>, "
                  << "Handle<CoorTransBaseEci>, "
                  << "const TimeJ2000&, std::istream&)"  << std::endl;
        throw;
    }
}

SpatialEphFileGAP::~SpatialEphFileGAP()
{
    ;
}

void SpatialEphFileGAP::ReadExtWayPtFile()
{
    std::ifstream isWayPts;

    try {
        isWayPts.exceptions(std::ios_base::failbit | std::ios_base::badbit);
        isWayPts.open(m_sFileNm.c_str(), std::ios::in);
        isWayPts.exceptions(std::ios_base::badbit);
        isWayPts >> ioAngleDegrees;
        ReadExtWayPt( isWayPts );
        isWayPts.close();
    } catch ( ... ) {
        if ( isWayPts ) {
            isWayPts.close();
        }

        std::cerr << "SpatialEphFileGAP::ReadExtWayPtFile()"  << std::endl
                  << "ephemeris file name = " + m_sFileNm << std::endl;
        throw;
    }
}

std::istream& SpatialEphFileGAP::Initialize( std::istream& is )
{
    is >> m_epoch;
    ReadExtWayPt(is);
    return is;
}

void SpatialEphFileGAP::ReadExtWayPt( std::istream& isWayPts )
{
    try {
        double tmRel;
        VectorGeodetic lla;

        while ( isWayPts >> tmRel >> lla ) {
            m_waypts[tmRel] = static_cast<VectorCartesian>(lla);
        }
    } catch ( ... ) {
        std::cerr << "SpatialEphFileGAP::ReadExtWayPt(std::istream&)"
                  << std::endl;
        throw;
    }
}

void SpatialEphFileGAP::UpdateState()
{
    try {
        if ( m_bIsAccelerationEnabled ) {
            ThrowNotImplemented( "SpatialEphFileGAP::UpdateState(): ACCELERATION" );
        }

        SpatialTimeDep::UpdateState();
        double tmRel(GetTime() - m_epoch);

        if (m_waypts.begin()->first > tmRel) {
            m_vecEcfPosition = m_waypts.begin()->second;
            m_vecEcfVelocity = VectorCartesian(0.0, 0.0, 0.0);
        } else if(m_waypts.rbegin()->first <= tmRel) {
            m_vecEcfPosition = m_waypts.rbegin()->second;
            m_vecEcfVelocity = VectorCartesian(0.0, 0.0, 0.0);
        } else {
            std::map<double, VectorCartesian>::iterator
            // find end point of current segment
            itrH(m_waypts.upper_bound( tmRel )),
                 itrL(m_waypts.end());

            if (m_waypts.begin()==itrH || m_waypts.end()==itrH) {
                throw std::logic_error("find way point failed");
            }

            // set start point of segment
            itrL = itrH;
            --itrL;
            // set start time and vector of segment
            double tmRel0(itrL->first);
            VectorCartesian posEcf0(itrL->second);
            // set end time and vector of segement
            double tmRel1(itrH->first);
            VectorCartesian posEcf1(itrH->second);
            GreatArcPath(tmRel0, posEcf0, tmRel1, posEcf1, tmRel,
                         m_vecEcfPosition, m_vecEcfVelocity);
        }

        if (CoorSystem::ECF == m_hdlCoorTrans->GetSourceSystem()) {
            m_vecPosition = m_vecEcfPosition;
            m_vecVelocity = m_vecEcfVelocity;
        } else {
            m_coorTransEciEcf.ToBase( m_vecEcfPosition, m_vecEcfVelocity,
                                      m_vecPosition,    m_vecVelocity );

            if ( CoorSystem::ECI != m_hdlCoorTrans->GetSourceSystem() ) {
                VectorCartesian m_vecEciPosition( m_vecPosition );
                VectorCartesian m_vecEciVelocity( m_vecVelocity );
                m_hdlCoorTrans->ToSource( m_vecEciPosition, m_vecEciVelocity,
                                          m_vecPosition,    m_vecVelocity );
            }
        }
    } catch ( ... ) {
        std::cerr << "SpatialEphFileGAP::UpdateState()" << std::endl;
        throw;
    }
}

// given 2 ECF way pts, GreatArcPath returns the ECF at some time between
// the way pts
void GreatArcPath(double tmRel0, const VectorCartesian& posEcf0,
                  double tmRel1, const VectorCartesian& posEcf1,
                  double tmRel,
                  VectorCartesian& posEcf, VectorCartesian& velEcf)
{
    VectorCartesian posABC, velABC, sABC, uABC;
    VectorCartesian rui, ruf, au, bu, cu;
    MatrixFP<double> dcm(3, 3);

    try {
        double theta;
        double thetad;
        double thetaLR;
        double radius;
        double radiusd;
        double radiusLR;
        double dt;

        if (tmRel0 > tmRel) {
            posEcf = posEcf0;
            velEcf = VectorCartesian(0.0, 0.0, 0.0);
        } else if (tmRel1 <= tmRel) {
            posEcf = posEcf1;
            velEcf = VectorCartesian(0.0, 0.0, 0.0);
        } else {
            // compute delta time between segment start and end
            dt = tmRel1 - tmRel0;
            // compute unit vectors
            rui = posEcf0.Unit();
            ruf = posEcf1.Unit();
            // compute angle between segment start and end vectors
            // and linear angular rate
            thetad = rui.SeparationAngle( ruf );
            thetaLR = thetad / dt;
            // compute angle between segement start and current position
            theta = thetad * (tmRel - tmRel0) / (tmRel1 - tmRel0);
            // compute altitude difference between segment start and end vectors
            // and linear altitude rate
            radiusd = posEcf1.Magnitude() - posEcf0.Magnitude();
            radiusLR = radiusd / dt;
            // compute radius of current position
            radius = posEcf0.Magnitude() + radiusd
                     * (tmRel-tmRel0)/(tmRel1-tmRel0);
            // compute position in the segment coordinate frame
            uABC = VectorCartesian( cos(theta), sin(theta), 0.0 );
            posABC = radius * uABC;
            // compute the unit tangential vector
            sABC = VectorCartesian( -sin(theta), cos(theta), 0.0 );
            // compute the velocity vector
            velABC = radiusLR * uABC + radius * thetaLR * sABC;
            // construct the DCM to rotate position from the segment
            // frame to the earth fixed frame
            au = rui;
            cu = rui.CrossProduct( ruf );
            cu = cu.Unit();
            bu = cu.CrossProduct( au );
            std::copy(au.begin(), au.end(), dcm.begin_column(0));
            std::copy(bu.begin(), bu.end(), dcm.begin_column(1));
            std::copy(cu.begin(), cu.end(), dcm.begin_column(2));
            // perform rotation
            dcm.Multiply(posABC, posEcf);
            dcm.Multiply(velABC, velEcf);
        }
    } catch ( ... ) {
        std::cerr << "GreatArcPath(double, const VectorCartesian&, double, "
                  << "const VectorCartesian&, double, VectorCartesian&, "
                  << "VectorCartesian&)"
                  << std::endl;
        throw;
    }
}
}
// A3 namespace end
// ============================================================================
