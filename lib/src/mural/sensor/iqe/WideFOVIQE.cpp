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
 * WideFOVIQE.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "WideFOVIQE.h"


#include "general/math/SimpleMath.h"


WideFOVIQE::WideFOVIQE(shared_ptr<InputDataElement> ptrInputDataElement) : IQE(ptrInputDataElement)
{
    return;
}


/* ********************************************************************************* */

double WideFOVIQE::CalculateQuality(double elevationAngle,
                                    const string &missionType,
                                    const SpaceVector &targetPos,
                                    const PositionState &ecfVehicleState,
                                    bool enhancedAreaMode)
{
    int numberOfQualities = 9;
    return(numberOfQualities);
}

/* **************************************************************************************** */

double
WideFOVIQE::GetDebugQualityParameter(const SpaceVector &targetPos,
                                     const PositionState &ecfVehicleState)
{
    return 0.0;
}

/* ***************************************************************************************** */

WideFOVIQE::~WideFOVIQE()
{
    // Do nothing
}
