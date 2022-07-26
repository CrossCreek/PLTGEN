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
 * SarIQE.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "SarIQE.h"


#include "general/math/SimpleMath.h"


SarIQE::SarIQE(shared_ptr<InputDataElement> ptrInputDataElement) : IQE(ptrInputDataElement)
{
    return;
}


/* ********************************************************************************* */

double SarIQE::CalculateQuality(double elevationAngle,
                                const string &missionType,
                                const SpaceVector &targetPos,
                                const PositionState &ecfVehicleState,
                                bool enhancedAreaMode)
{
    double           quality;
    double           qsqnt;
    double           elevationAngleDegrees;
    double           squintAngleDegrees;
    const ANGLE_UNIT storeAngle = UnitData::GetInternalAngleUnits();
    const double currentSquintAngle = ComputeSquintAngle(targetPos, ecfVehicleState);
    elevationAngleDegrees = SimpleMath::ConvertAngle(elevationAngle, storeAngle, DEGREES);
    squintAngleDegrees = SimpleMath::ConvertAngle(currentSquintAngle, storeAngle, DEGREES);
    qsqnt = fabs(90.0 - fabs(squintAngleDegrees));
    quality = nrisD1_a + (nrisD2_a * elevationAngleDegrees
                          - (nrisD3_a * elevationAngleDegrees * elevationAngleDegrees)
                          - (nrisD4_a * qsqnt * qsqnt)
                          - (nrisD5_a * 3.321928 * SimpleMath::Log10(imageProductResolution_a) * elevationAngleDegrees)
                          + (nrisD6_a * 3.321928 * SimpleMath::Log10(imageProductResolution_a) * (elevationAngleDegrees * elevationAngleDegrees)));

    if (elevationAngleDegrees < 30.0) {
        quality = quality + (0.00080 * (30.0 - elevationAngleDegrees)
                             * (30.0 - elevationAngleDegrees));
    }

    if (enhancedAreaMode == true) {
        // Enhanced Area Mode
        quality = quality - 0.2;
    }

    return(quality);
}

/* **************************************************************************************** */

double
SarIQE::GetDebugQualityParameter(const SpaceVector &targetPos,
                                 const PositionState &ecfVehicleState)
{
    double squint = ComputeSquintAngle(targetPos, ecfVehicleState);
    return ( UnitData::GetOutputAngle(squint) );
}

/* ************************************************************************************************* */

SarIQE::~SarIQE()
{
    // Do nothing
}
