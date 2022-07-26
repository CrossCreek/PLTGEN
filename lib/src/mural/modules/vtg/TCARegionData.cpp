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
 * TCARegionData.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "TCARegionData.h"


#include "general/time/TimePiece.h"

//------------------------------------------------------------------------------
//
//  Method Name: TCARegionData::TCARegionData
//
//  Purpose:
//    Constructor for the TCARegionData class.
//
//  Calling Method:
//     TCARegionData();
//
//  Calling Parameters
//  Parameters      i/o  description
//  ----------      ---  -----------
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

TCARegionData::TCARegionData()
    : regionNumber_a(0.0), crisisLevel_a(0), ptrDistanceArray_a(NULL),
      ptrScoreArray_a(NULL), ptrTimeToCollectArray_a(NULL)
{
    return;
}

TCARegionData::TCARegionData(double regionNumber,int crisisLevel,
                             Array<double> *ptrDistance,
                             Array<double> *ptrScore,
                             Array<double> *ptrTimeToCollect)

    : regionNumber_a(regionNumber),
      ptrDistanceArray_a(NULL), ptrScoreArray_a(NULL),
      ptrTimeToCollectArray_a(NULL), crisisLevel_a(crisisLevel)
{
    if (ptrDistance != NULL) {
        ptrDistanceArray_a = new Array<double>(*ptrDistance);
    }

    if (ptrScore != NULL) {
        ptrScoreArray_a = new Array<double>(*ptrScore);
    }

    if (ptrTimeToCollect != NULL) {
        ptrTimeToCollectArray_a = new Array<double>(*ptrTimeToCollect);
    }

    return;
}

TCARegionData::TCARegionData(const TCARegionData &copyMe)
    : regionNumber_a(copyMe.regionNumber_a),
      ptrDistanceArray_a(NULL), ptrScoreArray_a(NULL),
      ptrTimeToCollectArray_a(NULL), crisisLevel_a(copyMe.crisisLevel_a)
{
    if (copyMe.ptrDistanceArray_a != NULL) {
        ptrDistanceArray_a = new Array<double>(*(copyMe.ptrDistanceArray_a));
    }

    if (copyMe.ptrScoreArray_a != NULL) {
        ptrScoreArray_a = new Array<double>(*(copyMe.ptrScoreArray_a));
    }

    if (copyMe.ptrTimeToCollectArray_a != NULL) {
        ptrTimeToCollectArray_a = new Array<double>(*(copyMe.ptrTimeToCollectArray_a));
    }

    return;
}

TCARegionData&
TCARegionData::operator = (const TCARegionData &copyMe)
{
    regionNumber_a           = copyMe.regionNumber_a;
    ptrDistanceArray_a       = copyMe.ptrDistanceArray_a;
    ptrScoreArray_a          = copyMe.ptrScoreArray_a;
    ptrTimeToCollectArray_a  = copyMe.ptrTimeToCollectArray_a;
    crisisLevel_a            = copyMe.crisisLevel_a;
    return(*this);
}

bool
TCARegionData::operator==(const TCARegionData &compareMe) const
{
    return( (regionNumber_a == compareMe.regionNumber_a) &&
            (*ptrDistanceArray_a == *(compareMe.ptrDistanceArray_a)) &&
            (*ptrScoreArray_a == *(compareMe.ptrScoreArray_a)) &&
            (*ptrTimeToCollectArray_a == *(compareMe.ptrTimeToCollectArray_a)) &&
            (crisisLevel_a == compareMe.crisisLevel_a));
}

void
TCARegionData::AddTimeStepData(int timeIndex,
                               double distance,
                               double score,
                               double timeToCollect)
{
    const int numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();

    if (ptrDistanceArray_a == NULL) {
        ptrDistanceArray_a = new Array<double>(numberOfTimeSteps, 0.0);
    }

    (*ptrDistanceArray_a)[timeIndex] = distance;

    if (ptrScoreArray_a == NULL) {
        ptrScoreArray_a = new Array<double>(numberOfTimeSteps, 0.0);
    }

    (*ptrScoreArray_a)[timeIndex] = score;

    if (ptrTimeToCollectArray_a == NULL) {
        ptrTimeToCollectArray_a = new Array<double>(numberOfTimeSteps, 0.0);
    }

    (*ptrTimeToCollectArray_a)[timeIndex] = timeToCollect;
    return;
}

Array<int>*
TCARegionData::DetermineTCATimeSteps() const
{
    Array<int> *ptrTCATimeIndexArray = NULL;

    if (ptrDistanceArray_a != NULL) {
        int timeIndex = 0;
        int timeIndexOfTCA = 0;
        const int   numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
        double minimumDistance = 0.0;
        bool foundEndOfInviewPeriod = false;
        ptrTCATimeIndexArray = new Array<int>();

        while (timeIndex < numberOfTimeSteps) {
            double distance = 0.0;
            distance = ptrDistanceArray_a->ElementAt(timeIndex);

            if (distance > 0.0) {
                minimumDistance = distance;
                timeIndexOfTCA = timeIndex++;
                foundEndOfInviewPeriod = false;

                while ((timeIndex < numberOfTimeSteps) && (foundEndOfInviewPeriod == false)) {
                    distance = ptrDistanceArray_a->ElementAt(timeIndex);

                    if (distance > 0.0) {
                        if (distance < minimumDistance) {
                            timeIndexOfTCA = timeIndex;
                            minimumDistance = distance;
                        }

                        ++timeIndex;
                    } else {
                        foundEndOfInviewPeriod = true;
                    }
                }

                ptrTCATimeIndexArray->AddElement(timeIndexOfTCA);
            }

            ++timeIndex;
        }

        if (ptrTCATimeIndexArray->Size() == 0) {
            delete ptrTCATimeIndexArray;
            ptrTCATimeIndexArray = NULL;
        }
    }

    return(ptrTCATimeIndexArray);
}

void
TCARegionData::PrintTCAPeriods(ofstream &outfile) const
{
    int         timeIndex = 0;
    int         timeIndexOfTCA = 0;
    const int   numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    double       minimumDistance = 0.0;
    bool         foundEndOfInviewPeriod = false;
    Array<int> *ptrTCATimeIndexArray = NULL;

    while (timeIndex < numberOfTimeSteps) {
        double distance = 0.0;
        distance = ptrDistanceArray_a->ElementAt(timeIndex);

        if (distance > 0.0) {
            outfile << "  " << TimePiece::GetTimeStep(timeIndex);
            minimumDistance = distance;
            timeIndexOfTCA = timeIndex++;
            foundEndOfInviewPeriod = false;

            while ((timeIndex < numberOfTimeSteps) && (foundEndOfInviewPeriod == false)) {
                distance = ptrDistanceArray_a->ElementAt(timeIndex);

                if (distance > 0.0) {
                    if (distance < minimumDistance) {
                        timeIndexOfTCA = timeIndex;
                        minimumDistance = distance;
                    }

                    ++timeIndex;
                } else {
                    foundEndOfInviewPeriod = true;
                }
            }

            outfile << " to " << TimePiece::GetTimeStep(timeIndex - 1);
            outfile << "  (" << TimePiece::GetTimeStep(timeIndexOfTCA) << "),";
        }

        ++timeIndex;
    }

    return;
}

TCARegionData::~TCARegionData()
{
    if (ptrDistanceArray_a != NULL) {
        delete ptrDistanceArray_a;
        ptrDistanceArray_a = NULL;
    }

    if (ptrScoreArray_a != NULL) {
        delete ptrScoreArray_a;
        ptrScoreArray_a = NULL;
    }

    if (ptrTimeToCollectArray_a != NULL) {
        delete ptrTimeToCollectArray_a;
        ptrTimeToCollectArray_a = NULL;
    }
}
