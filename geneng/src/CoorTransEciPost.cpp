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
 * CoorTransEciPost.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Refer to CoorTransEciPost.h
// =============================================================================
#include "CoorTransEciPost.h"
#include "GenUtility.h"
#include <string>
#include <fstream>

namespace A3
{
PostHeader::PostHeader()
    : m_time0(0.0) {;}

PostHeader::PostHeader(std::istream& is)
    : m_time0(0.0)
{
    // Line 1: descriptive text
    // Line 2: text: TIMEO GDLAT LONG LATL LONL AZL
    // Line 3: white space
    // Line 4: text: TIME ROLI YAWI PITI
    // Units: second, degrees
    // remove header line 1
    std::string strBuff;
    std::getline(is, strBuff);
    // read in Post header
    is >> ioAngleDegrees
       >> m_time0
       >> m_gdlat
       >> m_long
       >> m_latl
       >> m_lonl
       >> m_azl
       >> ioAngleRestore;
    // remove header line 3 & 4
    is >> std::ws;
    std::getline(is, strBuff);
}

// =========================================================================
PostRollYawPitch::PostRollYawPitch()
    : m_roll(EER::A1, 0.0), m_yaw(EER::A3, 0.0), m_pitch(EER::A2, 0.0) {;}

PostRollYawPitch::PostRollYawPitch(Angle roll, Angle yaw, Angle pitch)
    : m_roll(EER::A1, roll), m_yaw(EER::A3, yaw), m_pitch(EER::A2, pitch) {;}

void PostRollYawPitch::DcmAttWrtLnchAxes(CoorTransDcm& dcm) const
{
    const CoorTransDcm dcmRoll(m_roll);
    const CoorTransDcm dcmYaw(m_yaw);
    const CoorTransDcm dcmPitch(m_pitch);
    const CoorTransDcm dcmYP(dcmYaw*dcmPitch);
    dcm = dcmRoll * dcmYP;
}

// =========================================================================
PostLonLatAz::PostLonLatAz()
    : m_lon(EER::A3, 0.0), m_lat(EER::A2, 0.0), m_az(EER::A1, 0.0) {;}

PostLonLatAz::PostLonLatAz(Angle lon, Angle lat, Angle az)
    : m_lon(EER::A3, lon), m_lat(EER::A2, -lat), m_az(EER::A1, -az) {;}

PostLonLatAz::PostLonLatAz(const PostHeader& ph)
    : m_lon(EER::A3, ph.Lonl()),
      m_lat(EER::A2, -ph.Latl()),
      m_az(EER::A1, -ph.Azl()) {;}

void PostLonLatAz::DcmLnchAxesWrtPci(CoorTransDcm& dcm) const
{
    const CoorTransDcm dcmLon(m_lon);
    const CoorTransDcm dcmLat(m_lat);
    const CoorTransDcm dcmAz(m_az);
    const CoorTransDcm dcmLL(dcmLat*dcmAz);
    dcm = dcmLon*dcmLL;
}

// =========================================================================
CoorTransEciPci::CoorTransEciPci(TimeJ2000 tmPostInertialRefEpoch,
                                 GrAngTyp grAngTyp)
    : CoorTransEciEcf(HandleConst<TimeJ2000>(
                          new TimeJ2000(tmPostInertialRefEpoch)), grAngTyp)
{
    SetSourceSystem(CoorSystem::PCI);
    SetAngularVelocityZero();
    UpdateState();
}

// =========================================================================
// Note: DcmAttWrtPci = DcmLnchAxesWrtPci * DcmAttWrtLnchAxes
//       DcmAttWrtEci = DcmPciWrtEci * DcmAttWrtPci
CoorTransEciPat::CoorTransEciPat(HandleConst<TimeJ2000> hTime,
                                 TimeJ2000 tmPostInertialRefEpoch,
                                 std::istream& is,
                                 CoorTransEciEcf::GrAngTyp grAngTyp)
    : CoorTransEciQuat(hTime),
      m_tmPostInertialRefEpoch(tmPostInertialRefEpoch),
      m_dtInertialEpochWrtLaunch(0.0)
{
    SetSourceSystem(CoorSystem::PAT);
    const CoorTransEciPci xEciPci(m_tmPostInertialRefEpoch, grAngTyp);
    const CoorTransDcm dcmPciWrtEci(xEciPci.Dcm());
    // std::cout << std::endl << dcmPciWrtEci << std::endl;
    const PostHeader ph(is);
    m_dtInertialEpochWrtLaunch = ph.Time0();
    const PostLonLatAz plla(ph);
    CoorTransDcm dcmLnchAxesWrtPci;
    plla.DcmLnchAxesWrtPci(dcmLnchAxesWrtPci);
    // std::cout << std::endl << dcmLnchAxesWrtPci << std::endl;
    double dt;
    Angle roll, pitch, yaw;
    CoorTransDcm dcmAttWrtLnchAxes, dcmAttWrtPci, dcmAttWrtEci;
    is >> ioAngleDegrees;

    while (is >> dt >> roll >> pitch >> yaw) {
        PostRollYawPitch(roll, pitch, yaw).DcmAttWrtLnchAxes(dcmAttWrtLnchAxes);
        // std::cout << std::endl << dcmAttWrtLnchAxes << std::endl;
        dcmAttWrtPci = dcmLnchAxesWrtPci * dcmAttWrtLnchAxes;
        // std::cout << std::endl << dcmAttWrtPci << std::endl;
        dcmAttWrtEci = dcmPciWrtEci * dcmAttWrtPci;
        // std::cout << std::endl << dcmAttWrtEci << std::endl;
        InsertTimeQuat(m_tmPostInertialRefEpoch+dt, Quaternion(dcmAttWrtEci));
    }

    is.clear();
    is >> ioAngleRestore;
}
} // A3 namespace end
