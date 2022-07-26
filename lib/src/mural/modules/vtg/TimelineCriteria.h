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
 * TimelineCriteria.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef TIMELINE_CRITERIA_H
#define TIMELINE_CRITERIA_H "TimelineCriteria"

#include <iostream>
#include <iomanip>
#include <string>
#include <memory>


#include "general/array/Array2D.h"
#include "general/array/LinkList.h"

using namespace std;

class InputData;
class InputDataElement;
class OffRegionData;

/**
 * Data class holding the timeline settings from the run-control for a user
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------------
 * timeToCollectReset_a          number of time steps before resetting
 *                                 collection time for requirement(s)
 * ptrElevationFactorArray_a*    score factor based on elevation angle from the
 *                                 collector to the resource
 * flatScorePerTime_a            value per time step used for a NOMINAL_REGION
 *                                 user vehicle with no requirements deck
 * generateResourceSpecificTimeline_a indicates if a resource specific timeline
 *                                      should be generated
 * ptrElevationFactorFileName_a* string containing file name of elevation
 *                                 factors
 * timelineGenerationMethod_a    method to use for generating value timeline
 * typeOfDeck_a                  the type of deck being used for creating
 *                                 requirements
 * ptrMinimumAscendingList_a*    list of minimum ascending off regions
 * ptrMinimumDescendingList_a*   list of minimum descending off regions
 * ptrMaximumAscendingList_a*    list of maximum ascending off regions
 * ptrMaximumDescendingList_a*   list of maximum descending off regions
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */
class TimelineCriteria
{
public:

    enum TIMELINE_METHOD {
        UNKNOWN_TIMELINE_METHOD = -1,
        READ_TIMELINE = 0,
        MOST_VALUABLE_REGION = 1,
        TIME_CLOSEST_APPROACH = 2,
        NOMINAL_REGION = 3
    };

    enum TYPE_OF_DECK {
        NO_DECK = 0,
        TARGET_DECK = 1,
        REQUIREMENT_DECK = 2,
        BOTH_DECKS = 3
    };

    TimelineCriteria();



    explicit TimelineCriteria(shared_ptr<InputDataElement> ptrInputDataElement);

    TimelineCriteria(const TimelineCriteria &copyMe);
    virtual ~TimelineCriteria();

    TimelineCriteria& operator =  (const TimelineCriteria &copyMe);
    bool              operator == (const TimelineCriteria &compareMe) const;
    bool              operator != (const TimelineCriteria &compareMe) const;

    void   PrintAttributes(ostream &outfile) const;

    double  GetElevationFactor(double elevationAngle,
                               int position) const;

    double GetMinAscendingOffRegion(double longitude) const;
    double GetMinDescendingOffRegion(double longitude) const;
    double GetMaxAscendingOffRegion(double longitude) const;
    double GetMaxDescendingOffRegion(double longitude) const;

    TYPE_OF_DECK    GetTypeOfDeck() const;

    inline int   GetTimeToCollectReset() const;
    inline int   GetNumberOfElevationFactors() const;

    inline double GetFlatScorePerTime() const;

    inline bool   GenerateResourceSpecificTimeline() const;
    inline bool   UseDeck() const;
    inline bool   UseElevationFactors() const;
    inline bool   UseTargetDeck() const;
    inline bool   UseRequirementDeck() const;
    inline bool   UseBothDecks() const;

    inline string GetTimelineGenerationMethodString() const;
    inline string GetTypeOfDeckString() const;

    inline const TIMELINE_METHOD GetTimelineGenerationMethod() const;

    inline const LinkList<OffRegionData>* GetMinimumAscendingList() const;
    inline const LinkList<OffRegionData>* GetMinimumDescendingList() const;
    inline const LinkList<OffRegionData>* GetMaximumAscendingList() const;
    inline const LinkList<OffRegionData>* GetMaximumDescendingList() const;

protected:

private:

    void DeleteOffRegionData();
    void ExtractTimelineCriteriaInfo(shared_ptr<InputDataElement> ptrInputDataElement);
    void RetrieveElevationAngleFactors(int numberOfFactors);
    void SetDefaultOffRegion(LinkList<OffRegionData> *&ptrOffRegionList,
                             const shared_ptr<InputData> ptrInputData);
    void SetOffRegion(LinkList<OffRegionData> *&ptrOffRegionList,
                      const shared_ptr<InputData> ptrInputData);

    TYPE_OF_DECK    ConvertTypeOfDeck(const string &inputString) const;

    TIMELINE_METHOD GetTimelineGenerationMethod(const string &inputString) const;

    static inline string GetClassName();

    int                     timeToCollectReset_a;
    Array2D<double>          *ptrElevationFactorArray_a;
    double                   flatScorePerTime_a;
    bool                     generateResourceSpecificTimeline_a;
    string                  *ptrElevationFactorFileName_a;
    TIMELINE_METHOD          timelineGenerationMethod_a;
    TYPE_OF_DECK             typeOfDeck_a;
    LinkList<OffRegionData> *ptrMinimumAscendingList_a;
    LinkList<OffRegionData> *ptrMinimumDescendingList_a;
    LinkList<OffRegionData> *ptrMaximumAscendingList_a;
    LinkList<OffRegionData> *ptrMaximumDescendingList_a;
};

inline
int
TimelineCriteria::GetTimeToCollectReset() const
{
    return(timeToCollectReset_a);
}

inline
int
TimelineCriteria::GetNumberOfElevationFactors() const
{
    int numberOfElevationFactors = 0;

    if (ptrElevationFactorArray_a != NULL) {
        numberOfElevationFactors = ptrElevationFactorArray_a->Size();
    }

    return(numberOfElevationFactors);
}

inline
double
TimelineCriteria::GetFlatScorePerTime() const
{
    return(flatScorePerTime_a);
}

inline
const TimelineCriteria::TIMELINE_METHOD
TimelineCriteria::GetTimelineGenerationMethod() const
{
    return(timelineGenerationMethod_a);
}

inline
string
TimelineCriteria::GetTimelineGenerationMethodString() const
{
    string returnString = "UNKNOWN";

    if (timelineGenerationMethod_a == READ_TIMELINE) {
        returnString = "READ";
    } else if (timelineGenerationMethod_a == MOST_VALUABLE_REGION) {
        returnString = "MVR";
    } else if (timelineGenerationMethod_a == TIME_CLOSEST_APPROACH) {
        returnString = "TCA";
    } else if (timelineGenerationMethod_a == NOMINAL_REGION) {
        returnString = "REGION";
    }

    return(returnString);
}

inline
string
TimelineCriteria::GetTypeOfDeckString() const
{
    string returnString = "NONE";

    if (typeOfDeck_a == TARGET_DECK) {
        returnString = "TARGET";
    } else if (typeOfDeck_a == REQUIREMENT_DECK) {
        returnString = "REQUIREMENT";
    } else if (typeOfDeck_a == BOTH_DECKS) {
        returnString = "BOTH";
    }

    return(returnString);
}

inline
bool
TimelineCriteria::GenerateResourceSpecificTimeline() const
{
    return(generateResourceSpecificTimeline_a);
}

inline
bool
TimelineCriteria::UseDeck() const
{
    return(typeOfDeck_a != NO_DECK);
}

inline
bool
TimelineCriteria::UseTargetDeck() const
{
    return( (typeOfDeck_a == TARGET_DECK) ||
            (typeOfDeck_a == BOTH_DECKS) );
}

inline
bool
TimelineCriteria::UseRequirementDeck() const
{
    return( (typeOfDeck_a == REQUIREMENT_DECK) ||
            (typeOfDeck_a == BOTH_DECKS) );
}

inline
bool
TimelineCriteria::UseBothDecks() const
{
    return(typeOfDeck_a == BOTH_DECKS);
}

inline
bool
TimelineCriteria::UseElevationFactors() const
{
    return(ptrElevationFactorArray_a != NULL);
}

inline
const LinkList<OffRegionData>*
TimelineCriteria::GetMinimumAscendingList() const
{
    return(ptrMinimumAscendingList_a);
}

inline
const LinkList<OffRegionData>*
TimelineCriteria::GetMinimumDescendingList() const
{
    return(ptrMinimumDescendingList_a);
}

inline
const LinkList<OffRegionData>*
TimelineCriteria::GetMaximumAscendingList() const
{
    return(ptrMaximumAscendingList_a);
}

inline
const LinkList<OffRegionData>*
TimelineCriteria::GetMaximumDescendingList() const
{
    return(ptrMaximumDescendingList_a);
}

inline
string
TimelineCriteria::GetClassName()
{
    return(TIMELINE_CRITERIA_H);
}

#endif
