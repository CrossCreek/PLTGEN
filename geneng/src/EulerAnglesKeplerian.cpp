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
 * EulerAnglesKeplerian.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Keplerian Euler Angles class
// Refer to EulerAnglesKeplerian.h for more info.
// =============================================================================
#include "EulerAnglesKeplerian.h"
#include "IoSeparator.h"

#include <iomanip>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// inline i/o stream operators
std::istream&
operator>>( std::istream& is, EulerAnglesKeplerian& angles )
{
    is >> static_cast<Vector3DFP<Angle>&>(angles);
    return is;
}

std::ostream&
operator<<( std::ostream& os, const EulerAnglesKeplerian& angles )
{
    os << static_cast<const Vector3DFP<Angle>&>(angles);
    return os;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
