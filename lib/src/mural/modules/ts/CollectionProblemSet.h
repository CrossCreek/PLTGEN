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
 * CollectionProblemSet.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef COLLECTION_PROBLEM_SET_H
#define COLLECTION_PROBLEM_SET_H "CollectionProblemSet"

#include <string>


#include "general/array/Array.h"

using namespace std;

/**
 * Data class to hold information about a target collection problem set
 *
 * Name                  Description (units)
 * -------------         -------------------------------------------------------
 * ptrPriorityLevel_a*   array of priority levels by sensor type
 * ptrQualityLevel_a*    array of quality levels (NIIRS/NRIS) by sensor type
 * ptrScore_a*           array of calculated scores by sensor type
 * ptrCollectionMode_a*  array of collection modes (mono/stereo)  by sensor type
 * commonCountdown_a     indicates if target is common countdown
 * identification_a      CPS name
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class CollectionProblemSet
{
public:

    CollectionProblemSet();




    CollectionProblemSet(const CollectionProblemSet &collectionProblemSet);

    CollectionProblemSet(const string &fcpsID,
                         const Array<double> *ptrQuality,
                         const Array<int> *ptrPriority,
                         const Array<char> *ptrPreference,
                         const Array<char> *ptrMode,
                         const Array<double> *ptrScore,
                         const bool &isPointTarget);

    virtual ~CollectionProblemSet();

    int  GetQualityLevel(int sensorIndex) const;

    int   GetQualityIndex(int sensorIndex) const;
    int   GetModeIndex(int sensorIndex) const;
    int   GetPriorityIndex(int sensorIndex) const;

    double  GetTotalScore() const;

    bool   IsStereoMode(int sensorIndex) const;

    inline int  GetPriority(int sensorIndex) const;

    inline double  GetScore(int sensorIndex) const;

    inline char   GetCollectionMode(int sensorIndex) const;

    inline bool   ContainsCollectionMode(const char &mode) const;
    inline bool   IsCommonCountdown() const;

    inline string GetIdentification() const;

protected:

private:




    static const char   COMMON_COUNTDOWN_IDENTIFIER_s;

    Array<int>       *ptrPriorityLevel_a;
    Array<double>       *ptrQualityLevel_a;
    Array<double>       *ptrScore_a;
    Array<char>        *ptrCollectionMode_a;
    bool                commonCountdown_a;
    string              identification_a;
};

inline
int
CollectionProblemSet::GetPriority(int sensorIndex) const
{
    return(ptrPriorityLevel_a->ElementAt(sensorIndex));
}

inline
double
CollectionProblemSet::GetScore(int sensorIndex) const
{
    return(ptrScore_a->ElementAt(sensorIndex));
}

inline
char
CollectionProblemSet::GetCollectionMode(int sensorIndex) const
{
    return(ptrCollectionMode_a->ElementAt(sensorIndex));
}

inline
bool
CollectionProblemSet::ContainsCollectionMode(const char &mode) const
{
    return(ptrCollectionMode_a->ValueExists(mode));
}

inline
bool
CollectionProblemSet::IsCommonCountdown() const
{
    return(commonCountdown_a);
}

inline
string
CollectionProblemSet::GetIdentification() const
{
    return(identification_a);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
CollectionProblemSet::GetClassName()
{
    return(COLLECTION_PROBLEM_SET_H);
}
*/

#endif
