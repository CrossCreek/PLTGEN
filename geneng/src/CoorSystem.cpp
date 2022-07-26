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
 * CoorSystem.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// CoorSystem namespace and enumeration
// Refer to CoorSystem.h for more info.
// =============================================================================
#include "CoorSystem.h"
#include <algorithm>
#include <stdexcept>
#include <sstream>

namespace A3
{
CoorSystem::CoorSystem()
{
    /*insert(value_type(ECI, std::string("ECI")))
    ->second.insert(std::string("ECI: Earth Centered Inertial True of Date"));
    insert(value_type(ECF, "ECF"))
    ->second.insert("ECF: Earth Centered Fixed");
    insert(value_type(PQW, "PQW"))
    ->second.insert("PQW: Perifocal");
    insert(value_type(NWZ, "NWZ"))
    ->second.insert("NWZ: North, West, Zenith, Reference Fixed");
    insert(value_type(NWZF, "NWZF"))
    ->second.insert("NWZF: North, West, Zenith, Earth Fixed");
    insert(value_type(NWP, "NWP"))
    ->second.insert(
        "NWP: North, West, Zenith, Geodetic Reference Fixed");s
    insert(value_type(NWPF, "NWPF"))
    ->second.insert("NWPF: North, West, Zenith, Geodetic Earth Fixed");
    insert(value_type(ICR, "ICR"))
    ->second.insert(
        "ICR: Intrack, Crosstrack, Radial, Reference Fixed");
    insert(value_type(PQSA, "PQSA"))
    ->second.insert("PQSA: Gyrostat model A");
    insert(value_type(PQSB, "PQSB"))
    ->second.insert("PQSB: Gyrostat model B");
    insert(value_type(PQSC, "PQSC"))
    ->second.insert("PQSC: Gyrostat model C");
    insert(value_type(NOP, "NOP"))
    ->second.insert("NOP: Nadir Pole");
    insert(value_type(NES, "NES"))
    ->second.insert("NES: Nadir, East, South");
    insert(value_type(NIC, "NIC"))
    ->second.insert("NIC: Nadir, Intrack, Crosstrack");
    insert(value_type(POST, "POST"))
    ->second.insert("POST: Post Launch Vehicle Reference System");
    insert(value_type(J2000, "J2000"))
    ->second.insert("J2000: Mean of Date ECI Frame on Jan 1st 2000");
    insert(value_type(MOD, "MOD"))
    ->second.insert("MOD: Mean of Date ECI Frame");
    insert(value_type(TOD, "TOD"))
    ->second.insert("TOD: True of Date ECI Frame, same as \"ECI\"");
    insert(value_type(PBF, "PBF"))
    ->second.insert("PBF: Psuedo Body Fixed Frame, same as \"ECF\"");
    insert(value_type(AEFZ, "AEFZ"))
    ->second.insert("AEFZ: Azimuth Elevation Fixed Frame w.r.t. NWZ");
    insert(value_type(AEFP, "AEFZ"))
    ->second.insert("AEFP: Azimuth Elevation Fixed Frame w.r.t. NWP");
    insert(value_type(QUAT, "QUAT"))
    ->second.insert("QUAT: Quaternion");
    insert(value_type(PCI, "PCI"))
    ->second.insert("PCI: Post Earth-Centered Inertial");
    insert(value_type(PAT, "PAT"))
    ->second.insert("PAT: Post Vehicle Attitude");*/
}
} // A3
