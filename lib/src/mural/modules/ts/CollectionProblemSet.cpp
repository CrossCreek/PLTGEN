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
 * CollectionProblemSet.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "CollectionProblemSet.h"
#include "Target.h"
#include "mural/sensor/SpectralSensor.h"

const char CollectionProblemSet::COMMON_COUNTDOWN_IDENTIFIER_s = 'C';

CollectionProblemSet::CollectionProblemSet()
    : ptrQualityLevel_a(NULL), ptrPriorityLevel_a(NULL),
      ptrScore_a(NULL), ptrCollectionMode_a(NULL), commonCountdown_a(false),
      identification_a("UNKNOWN")
{
    return;
}

CollectionProblemSet::CollectionProblemSet(const string &fcpsID,
        const Array<double> *ptrQuality,
        const Array<int> *ptrPriority,
        const Array<char> *ptrPreference,
        const Array<char> *ptrMode,
        const Array<double> *ptrScore,
        const bool &isPointTarget)
    : ptrQualityLevel_a(new Array<double>(*ptrQuality)),
      ptrPriorityLevel_a(new Array<int>(*ptrPriority)),
      ptrScore_a(new Array<double>(*ptrScore)),
      ptrCollectionMode_a(new Array<char>(*ptrMode)), commonCountdown_a(false),
      identification_a(fcpsID)
{
    if (isPointTarget == false) {
        commonCountdown_a = ptrPreference->ValueExists(COMMON_COUNTDOWN_IDENTIFIER_s);
    }

    return;
}

int
CollectionProblemSet::GetQualityIndex(int sensorIndex) const
{
    int  qualityIndex = -1;
    double qualityValue = ptrQualityLevel_a->ElementAt(sensorIndex);

    if (qualityValue > 0.0) {
        if (Sensor::IsSpectralSensor(sensorIndex) == false) {
            qualityIndex = Target::ConvertQualityToIndex((int)qualityValue);
        } else {
            qualityIndex = SpectralSensor::GetQualityIndex(qualityValue);
        }
    }

    return(qualityIndex);
}

int
CollectionProblemSet::GetModeIndex(int sensorIndex) const
{
    return(Target::ConvertModeToIndex(ptrCollectionMode_a->ElementAt(sensorIndex)));
}

int
CollectionProblemSet::GetPriorityIndex(int sensorIndex) const
{
    return(Target::ConvertPriorityToIndex(ptrPriorityLevel_a->ElementAt(sensorIndex)));
}

int
CollectionProblemSet::GetQualityLevel(int sensorIndex) const
{
    int qualityLevel = 0;

    if ( (sensorIndex >= 0) &&
            (sensorIndex < ptrQualityLevel_a->Size()) ) {
        //RAGUSA 5/2/2018 - Casting double to int for 64-bit processing
        qualityLevel = Utility::CastDoubleToInt(ptrQualityLevel_a->ElementAt(sensorIndex));
    }

    return(qualityLevel);
}

double
CollectionProblemSet::GetTotalScore() const
{
    int       sensorIndex = 0;
    const int numberOfSensors = ptrScore_a->Size();
    double      totalScore = 0.0;

    while (sensorIndex < numberOfSensors) {
        if (commonCountdown_a == false) {
            totalScore += ptrScore_a->ElementAt(sensorIndex);
        } else if (ptrScore_a->ElementAt(sensorIndex) > totalScore) {
            totalScore = ptrScore_a->ElementAt(sensorIndex);
        }

        ++sensorIndex;
    }

    return(totalScore);
}

bool
CollectionProblemSet::IsStereoMode(int sensorIndex) const
{
    return(Target::IsStereoMode(ptrCollectionMode_a->ElementAt(sensorIndex)));
}

CollectionProblemSet::~CollectionProblemSet()
{
    if (ptrQualityLevel_a != NULL) {
        delete ptrQualityLevel_a;
        ptrQualityLevel_a = NULL;
    }

    if (ptrPriorityLevel_a != NULL) {
        delete ptrPriorityLevel_a;
        ptrPriorityLevel_a = NULL;
    }

    if (ptrScore_a != NULL) {
        delete ptrScore_a;
        ptrScore_a = NULL;
    }

    if (ptrCollectionMode_a != NULL) {
        delete ptrCollectionMode_a;
        ptrCollectionMode_a = NULL;
    }

    return;
}
