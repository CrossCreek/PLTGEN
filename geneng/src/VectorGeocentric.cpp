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
 * VectorGeocentric.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// VectorGeocentric class
// Refer to VectorGeocentric.h for more info.
//===============================================================================
#include "VectorGeocentric.h"
#include "VectorGeodetic.h"
#include "Angle.h"
#include "IoManip.h"
#include "IoSeparator.h"
#include "PlanetEarth.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>

// ============================================================================
// A3 namespace start
namespace A3
{
// conversion constructor from geodetic coordinates to geocentric
VectorGeocentric::VectorGeocentric(const VectorGeodetic& llh)
{
    // 1: convert the geodetic coordinates to cartesian coordinates
    VectorCartesian cartesian(llh);
    // 2: convert the cartesian coordinates to geodetic coordinates
    *this = VectorGeocentric(cartesian, llh.IsNormalized());
}

// -------------------------------------------------------------------------
// geocentric manipulators
namespace
{
// default format is geocentric height w.r.t. reference ellipsoid
const unsigned GEOCHEIGHT   = 0x01;
const unsigned GEOCHEIGHTEQ = 0x02;
const unsigned GEOCRADIUS   = 0x04;

class IoManipGeoHg : public IoManip<unsigned>
{
public:



    explicit IoManipGeoHg(size_t refs=0);

    static unsigned Default()
    {
        return GEOCHEIGHT;
    }

    static std::locale::id id;
};

std::locale::id IoManipGeoHg::id;

IoManipGeoHg::IoManipGeoHg(size_t refs)
    : IoManip<unsigned>(Default(), refs) {;}
}

// -------------------------------------------------------------------------
std::ios_base& ioGeocentricHeight(std::ios_base& stm)
{
    return ioManipSet<IoManipGeoHg>(stm, GEOCHEIGHT);
}

std::ios_base& ioGeocentricHeightEq(std::ios_base& stm)
{
    return ioManipSet<IoManipGeoHg>(stm, GEOCHEIGHTEQ);
}

std::ios_base& ioGeocentricRadius(std::ios_base& stm)
{
    return ioManipSet<IoManipGeoHg>(stm, GEOCRADIUS);
}

std::ios_base& ioGeocentricRestore(std::ios_base& stm)
{
    return ioManipRestore<IoManipGeoHg>(stm);
}

// -------------------------------------------------------------------------
// extraction and insertion operators
std::istream&
operator>>(std::istream& is, VectorGeocentric& llr)
{
    unsigned fmt(ioManipGet<IoManipGeoHg>(is));
    Angle latitude, longitude;
    double radius;
    is >> latitude >> longitude >> radius;

    switch (fmt) {
        case GEOCHEIGHT:
            radius += earthRadiusToRefEllipse(latitude);
            break;

        case GEOCHEIGHTEQ:
            radius += EARTH_RADIUS_NMI;
            break;

        case GEOCRADIUS:
            break;

        default:
            throw std::runtime_error(
                "operator>>(istream&, VectorGeocentric&) reports bad format");
    };

    llr.Set(latitude, longitude, radius);

    return is;
}

std::ostream&
operator<<(std::ostream& os, const VectorGeocentric& llr)
{
    unsigned fmt(ioManipGet<IoManipGeoHg>(os));
    //WARNING_FIX:
    //1>src\VectorGeocentric.cpp(118): warning C4244: 'initializing' : conversion from 'std::streamsize' to 'int', possible loss of data
    long long int w(os.width());
    os << llr.GetLatitude() << ioSeparator <<  std::setw(w)
       << llr.GetLongitude() << ioSeparator <<  std::setw(w);

    switch (fmt) {
        case GEOCHEIGHT:
            os << llr.GetRadius() - earthRadiusToRefEllipse(llr.GetLatitude());
            break;

        case GEOCHEIGHTEQ:
            os << llr.GetRadius() - EARTH_RADIUS_NMI;
            break;

        case GEOCRADIUS:
            os << llr.GetRadius();
            break;

        default:
            throw std::runtime_error(
                "operator<<(ostream&, VectorGeocentric&) reports bad format");
    };

    return os;
}
}
// A3 namespace end
// ============================================================================
