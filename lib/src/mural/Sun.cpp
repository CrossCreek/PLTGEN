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
 * Sun.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include "mural/Sun.h"
#include "mural/Earth.h"

#include "general/math/SimpleMath.h"
#include "general/math/SpaceVector.h"
#include "general/time/TimePiece.h"

const double Sun::RADIUS_NMI_s = 751664.00;

Sun::Sun()
{
    return;
}

Sun::Sun(const Sun &copyMe)
{
    return;
}

Sun&
Sun::operator = (const Sun &copyMe)
{
    return(*this);
}

void
Sun::PrintPositionFile(ofstream &outfile)
{
    int        timeIndex = 0;
    const int  numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    SpaceVector sunPosition;

    while (timeIndex < numberOfTimeSteps) {
        sunPosition = GetPosition(timeIndex).GetOutputSpaceVector(timeIndex);
        outfile << "     SUN ";
        TimePiece::PrintIndex(outfile, timeIndex);
        outfile << " " << setw(16) << setprecision(3) << sunPosition.GetX();
        outfile << " " << setw(15) << setprecision(3) << sunPosition.GetY();
        outfile << " " << setw(15) << setprecision(3) << sunPosition.GetZ();
        outfile << NEW_LINE;
        ++timeIndex;
    }
}

SpaceVector
Sun::GetPosition(int timeIndex)
{
    return(GetPosition(timeIndex, UnitData::GetInternalReferenceFrame()));
}

SpaceVector
Sun::GetPosition(int timeIndex,
                 const REFERENCE_FRAME &returnFrame)
{
    double meanLongitude;
    double       meanAnomaly = 0.0;
    double       eclipticLongitude = 0.0;
    double       obliquityOfEcliptic = 0.0;
    double       magnitude = 0.0;
    double       sineOfDeclination = 0.0;
    double       cosineOfDeclination = 0.0;
    double       rightAscension = 0.0;
    const double day = TimePiece::GetJ2000Time(timeIndex) * DAYS_PER_SECOND;
    const double earthRadius = Earth::GetStoreRadius();
    const double fullCircle = UnitData::GetStoreFullCircle();
    SpaceVector  positionVector;
    meanLongitude = SimpleMath::Remainder(4.89495042 + (0.017202792 * day), fullCircle);

    if (meanLongitude < 0.0) {
        meanLongitude += fullCircle;
    }

    meanAnomaly = SimpleMath::Remainder(6.240040768 + (0.01720197034 * day), fullCircle);

    if (meanAnomaly < 0.0) {
        meanAnomaly += fullCircle;
    }

    eclipticLongitude = SimpleMath::Remainder(meanLongitude + (0.033423055 * sin(meanAnomaly))
                        + (0.000349065 * sin(2.0 * meanAnomaly)), fullCircle);
    obliquityOfEcliptic = 0.409087723 + (6.9813e-9 * day);
    magnitude = earthRadius * (23458.07467 - (391.92956 * cos(meanAnomaly))
                               - (3.28367 * cos(2.0 * meanAnomaly)));
    sineOfDeclination = sin(obliquityOfEcliptic) * sin(eclipticLongitude);
    cosineOfDeclination = SimpleMath::SquareRoot(1.0 - (sineOfDeclination * sineOfDeclination));
    rightAscension = SimpleMath::ArcTangent(cos(obliquityOfEcliptic) * sin(eclipticLongitude),
                                            cos(eclipticLongitude));
    positionVector = SpaceVector((cosineOfDeclination * cos(rightAscension) * magnitude),
                                 (cosineOfDeclination * sin(rightAscension) * magnitude),
                                 (sineOfDeclination * magnitude), EARTH_CENTER_INERTIAL);
    return(positionVector.ConvertTo(returnFrame, timeIndex));
}

bool
Sun::IsInLight(int timeIndex,
               const SpaceVector &positionVector)
{
    bool              isInLight = true;
    const SpaceVector sunPosition = GetPosition(timeIndex);
    const double      actualAngle = positionVector.GetSeparationAngle(sunPosition);
    const double      quarterCircle = UnitData::GetStoreQuarterCircle();

    if (actualAngle > quarterCircle) {
        double maximumAngle;
        maximumAngle = SimpleMath::ArcCosine(Earth::GetStoreRadius() / positionVector.GetMagnitude())
                       + quarterCircle;

        if (actualAngle > maximumAngle) {
            isInLight = false;
        }
    }

    return(isInLight);
}

Sun::~Sun()
{
    return;
}
