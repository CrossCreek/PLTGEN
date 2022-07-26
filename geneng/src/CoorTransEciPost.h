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
 * CoorTransEciPost.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================

//  FILE      : CoorTransEciPost.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciPost

//  AUTHOR(s) : J. McCormick

//  UPDATES   :
//      V5.0    2010-07-23  JAM  initial release

//
/// @file
/// @brief Quaternion w.r.t. Earth Centered Inertial (TOD)
/// Rotation
///
/// @details Provides Post Inertial, Post Launch and Post Attitude
/// transforamtions and quaternion interpolation for post attitude.
/// Displacement transformation only.
///
/// @author J. McCormick
/// @version V5.0  initial release
/// @date 2010-07-23
///
/// @par Units
/// - Function Arg & Return: standard GenEngr (see constants.h)
/// - Stream I/O:            L = n/a, T, A = manipulator controlled
/// - Initialization Files:  N/A
// =============================================================================
#ifndef COORTRANSECIPOST_H
#define COORTRANSECIPOST_H	"CoorTransEciPost 2010-07-23"

#include "CoorTransEciQuat.h"
#include "CoorTransEciEcf.h"
#include "EulerElemRotation.h"
#include "HandleConst.h"
#include "TimeJ2000.h"
#include "Angle.h"
#include <string>
#include <iostream>

namespace A3
{
// Post Header
// Line 1: descriptive text
// Line 2: TIMEO GDLAT LONG LATL LONL AZL
// Line 3: white space
// Line 4: text: TIME ROLI YAWI PITI
// Units: second, degrees
// Note: tmLaunch = m_tmPostInertialRefEpoch - m_dtInertialEpochWrtLaunch
class PostHeader
{
public:
    PostHeader();

    explicit PostHeader(std::istream& is);

    const double Time0() const
    {
        return m_time0;
    }
    const Angle  Gdlat() const
    {
        return m_gdlat;
    }
    const Angle  Long() const
    {
        return m_long;
    }
    const Angle  Latl() const
    {
        return m_latl;
    }
    const Angle  Lonl() const
    {
        return m_lonl;
    }
    const Angle  Azl() const
    {
        return m_azl;
    }

private:
    double m_time0; // m_dtInertialEpochWrtLaunch
    Angle  m_gdlat;
    Angle  m_long;
    Angle  m_latl;
    Angle  m_lonl;
    Angle  m_azl;
};

// =========================================================================
// Post vechicle attitude w.r.t. Post launch inertial system
// Type 132 rotation
class PostRollYawPitch
{
public:
    typedef EulerElemRotation EER;

    PostRollYawPitch();

    //CPPCHECK_FIX Warning - Function 'PostRollYawPitch' argument order different: declaration 'roll, pitch, yaw' definition 'roll, yaw, pitch'

    PostRollYawPitch(Angle roll, Angle yaw, Angle pitch);

    void DcmAttWrtLnchAxes(CoorTransDcm& dcm) const;

private:
    EER m_roll;
    EER m_yaw;
    EER m_pitch;
};

// =========================================================================
// Post launch inertial system w.r.t. Post centered inertial
// Type 321
class PostLonLatAz
{
public:
    typedef EulerElemRotation EER;

    PostLonLatAz();
    PostLonLatAz(Angle lon, Angle lat, Angle az);

    explicit PostLonLatAz(const PostHeader& ph);

    void DcmLnchAxesWrtPci(CoorTransDcm& dcm) const;

private:
    EER m_lon;
    EER m_lat;
    EER m_az;
};

// Note: DcmAttWrtPci = DcmLnchAxesWrtPci * DcmAttWrtLnchAxes
//       DcmAttWrtEci = DcmPciWrtEci * DcmAttWrtPci

// =========================================================================
// ECI/PCI tranformation
// base system: ECI
// source system: POST CENTERED INERTIAL (PCI)
// Type 3 rotation
class CoorTransEciPci : public CoorTransEciEcf
{
public:
    CoorTransEciPci(TimeJ2000 tmPostInertialRefEpoch,
                    GrAngTyp grAngTyp = GMSA96);
};

// =========================================================================
// ECI/POST tranformation
// base system: ECI
// source system: Post Attitude (PAT)
class CoorTransEciPat : public CoorTransEciQuat
{
public:
    CoorTransEciPat(HandleConst<TimeJ2000> hTime,
                    TimeJ2000 tmPostInertialRefEpoch,
                    std::istream& is,
                    CoorTransEciEcf::GrAngTyp grAngTyp
                    = CoorTransEciEcf::GMSA96);

private:
    TimeJ2000 m_tmPostInertialRefEpoch;
    double    m_dtInertialEpochWrtLaunch;
};
} // A3 namespace end
#endif  // COORTRANSECIPOST_H
