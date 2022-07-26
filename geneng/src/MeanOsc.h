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
 * MeanOsc.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
/// @file
/// @brief Mean-Osculating Orbit Element Transformations
///
/// @details Provides mean to osculating and osculating to mean transformation
/// using methods described in the Goddard Trajactory Determination System
/// Mathematical Theory, Revision 1, July 1989
///
/// @author J. McCormick
/// @version V5.0 Placed helper functions in unnamed namespaces
/// @date 2010-04-06
///
/// @author J. McCormick
/// @version V4.5 Initial release.
/// @date 2009-07-01
///
/// @par Units
/// - Function Arg & Return: standard GenEngr (see constants.h)
/// - Stream I/O:            L, T = standard, A = manipulator controlled
/// - Initialization Files:  N/A
// =============================================================================
#ifndef MEANOSC
/// Include guard macro
#define MEANOSC	 __FILE__ " " __DATE__

#include "OrbitElKeplerian.h"

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// @brief Transform orbit elements from mean to osculating
/// @param [in] mn mean orbit elements
/// @param [out] osc osculating orbit elements
/// @return ratio
void meanToOsculating(const OrbitElKeplerian& mn, OrbitElKeplerian& osc);

/// @brief Transform orbit elements from osculating to mean
/// @param [in] osc osculating orbit elements
/// @param [out] mn mean orbit elements
/// @return true = converge successfully, false = failure
/// On failure, mn contains last estimate
bool osculatingToMean(const OrbitElKeplerian& osc, OrbitElKeplerian& mn);

} // A3
#endif  // MEANOSC

