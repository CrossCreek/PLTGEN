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
 * TCARegionData.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef TCA_REGION_DATA_H
#define TCA_REGION_DATA_H "TCARegionData"

#include <fstream>
#include <iomanip>
#include <iostream>


#include "general/array/Array.h"

using namespace std;

/**
 * Data class used to calculate the Time of Closest Approach timeline method.
 *
 * Name                      Description (units)
 * -------------             ---------------------------------------------------
 * regionNumber_a            number of the Region
 * ptrDistanceArray_a*       array of distances to the Region at each time step
 * ptrScoreArray_a*          array of scores of the Region at each time step
 * ptrTimeToCollectArray_a*  array of collection times of the Region at each
 *                             time step
 * RAGUSA 1/12/2018 - Added new parameter for crisis region mode
 * crisisLevel_a             level of the crisis area that is associated with this region
 *							   at each timestep
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */

class TCARegionData
{
public:

    TCARegionData();
    TCARegionData(double regionNumber,int crisisLevel,
                  Array<double> *ptrDistance = NULL,
                  Array<double> *ptrScore = NULL,
                  Array<double> *ptrTimeToCollect = NULL);
    TCARegionData(const TCARegionData &copyMe);
    virtual ~TCARegionData();

    TCARegionData&  operator =  (const TCARegionData &copyMe);
    bool            operator == (const TCARegionData &compareMe) const;

    void   AddTimeStepData(int timeIndex,
                           double distance,
                           double score,
                           double timeToCollect);
    void   PrintTCAPeriods(ofstream &outfile) const;

    Array<int>* DetermineTCATimeSteps() const;

    inline double  GetRegionNumber() const;
    inline int  GetCrisisRegionLevel() const;

    inline double GetDistance(int timeIndex) const;
    inline double GetScore(int timeIndex) const;
    inline double GetTimeToCollect(int timeIndex) const;

protected:

private:




    double               regionNumber_a;
    Array<double>      *ptrDistanceArray_a;
    Array<double>      *ptrScoreArray_a;
    Array<double>      *ptrTimeToCollectArray_a;
    //RAGUSA 1/12/2018
    int					 crisisLevel_a;

};

inline
double
TCARegionData::GetRegionNumber() const
{
    return(regionNumber_a);
}

//RAGUSA 1/12/2018
inline
int
TCARegionData::GetCrisisRegionLevel() const
{
    return(crisisLevel_a);
}

inline
double
TCARegionData::GetDistance(int timeIndex) const
{
    double distance = 0.0;

    if (ptrDistanceArray_a != NULL) {
        distance = ptrDistanceArray_a->ElementAt(timeIndex);
    }

    return(distance);
}

inline
double
TCARegionData::GetScore(int timeIndex) const
{
    double score = 0.0;

    if (ptrScoreArray_a != NULL) {
        score = ptrScoreArray_a->ElementAt(timeIndex);
    }

    return(score);
}

inline
double
TCARegionData::GetTimeToCollect(int timeIndex) const
{
    double timeToCollect = 0.0;

    if (ptrTimeToCollectArray_a != NULL) {
        timeToCollect = ptrTimeToCollectArray_a->ElementAt(timeIndex);
    }

    return(timeToCollect);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
TCARegionData::GetClassName()
{
    return(TCA_REGION_DATA_H);
}
*/

#endif
