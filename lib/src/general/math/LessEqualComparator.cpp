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
 * LessEqualComparator.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "general/math/LessEqualComparator.h"


/* ***************************************************************************** */

LessEqualComparator::LessEqualComparator(double compareValue) : compareValue_a(compareValue)
{
    return;
}

/* ***************************************************************************** */

bool   LessEqualComparator::operator()  (double value) const
{
    return (value <= compareValue_a);
}

/* ***************************************************************************** */

LessEqualComparator::~LessEqualComparator()
{
    return;
}
