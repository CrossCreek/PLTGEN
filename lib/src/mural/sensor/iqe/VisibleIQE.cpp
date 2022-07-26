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
 * VisibleIQE.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "VisibleIQE.h"


#include "general/data/InputDataElement.h"
#include "general/math/SimpleMath.h"


VisibleIQE::VisibleIQE(shared_ptr<InputDataElement> ptrInputDataElement) : IQE(ptrInputDataElement)
{
    return;
}

/* ********************************************************************************* */

double VisibleIQE::CalculateQuality(double elevationAngle,
                                    const string &missionType,
                                    const SpaceVector &targetPos,
                                    const PositionState &ecfVehicleState,
                                    bool enhancedAreaMode)
{
    const double range = ComputeRange(targetPos, ecfVehicleState);
    double quality;
    double gsd = GetGroundSampleDistance(elevationAngle, range, missionType);
    // the ground sample distance must be convert to INCHES for
    // the NIIRS equation
    gsd = SimpleMath::ConvertDistance(gsd, UnitData::GetInternalDistanceUnits(),
                                      INCHES);
    quality = niirsA_a - (niirsB_a * SimpleMath::Log10(gsd));
    return(quality);
}

/* **************************************************************************************** */

double
VisibleIQE::GetDebugQualityParameter(const SpaceVector &targetPos,
                                     const PositionState &ecfVehicleState)
{
    double range = ComputeRange(targetPos, ecfVehicleState);
    return ( UnitData::GetOutputDistance(range) );
}

/* **************************************************************************************** */


VisibleIQE::~VisibleIQE()
{
    // Do nothing
}
