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
 * CoorTransEciAefP.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// ECI(TOD)-Azimuth Elevation Fixed Geocentric coordinate transformation class
// Refer to CoorTransEciAefZ.h for more info.
// =============================================================================
#include "CoorTransEciAefP.h"

namespace A3
{
CoorTransEciAefP::CoorTransEciAefP(const HandleConst<TimeJ2000> hdlTime,
                                   Angle geodeticLatitude, Angle longitude,
                                   Angle azimuth, Angle elevation)
    : CoorTransEciAefZ(hdlTime, geodeticLatitude, longitude, azimuth, elevation)
{
    SetSourceSystem( CoorSystem::AEFP );
}
} // A3 namespace end
