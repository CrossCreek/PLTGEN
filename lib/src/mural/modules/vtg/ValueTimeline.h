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
 * ValueTimeline.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef VALUE_TIMELINE_H
#define VALUE_TIMELINE_H "ValueTimeline"

#include <fstream>
#include <string>


#include "general/array/ArrayOfPointers.h"
#include "general/array/Array.h"
#include "general/array/LinkList.h"

using namespace std;

class ValueTimelineElement;
class RegionData;

/**
 * Data class which represents a user's timeline (score profile).  It holds ValueTimelineElements
 * which contain the actual data.
 *
 * Name                          Description (units)
 * -------------                 -----------------------------------------------
 * maximumTimelineScore_a        highest individual time step score for this
 *                                 user
 * ptrTimelineElementArray_a*    array of pointers of collection elements at
 *                                 each time step
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */
class ValueTimeline
{
public:

    ValueTimeline();
    ValueTimeline(const ValueTimeline &copyMe);
    virtual ~ValueTimeline();

    ValueTimeline&  operator =  (const ValueTimeline &copyMe);
    bool            operator == (const ValueTimeline &compareMe) const;
    bool            operator != (const ValueTimeline &compareMe) const;

    void   AddValueTimelineElement(int timeIndex,
                                   ValueTimelineElement *ptrNewElement);
    void   DetermineMaximumTimelineScore();
    void   RemoveRegionData(int timeIndex);
    void   SetScore(int timeIndex,
                    double newScore);

    int  GetSensorNumber(int timeIndex,
                         int resourceIndex) const;
    int  GetHighestScoreSensorNumber(int timeIndex) const;
    int  GetMissionNumber(int timeIndex,
                          int resourceIndex) const;
    int  GetCrisisLevel(int timeIndex) const;
    int  GetSubPriorityLevel(int timeIndex,
                             int resourceIndex) const;

    double  GetImagedRegion(int timeIndex,
                            int resourceIndex) const;
    double  GetOverflownRegion(int timeIndex) const;
    double  GetScore(int timeIndex,
                     int resourceIndex = -1) const;
    double  GetObjectiveValueScore(int timeIndex,
                                   const string &userDesignator,
                                   int resourceIndex = -1) const;
    double  GetMaxPossibleBitsCollected(int timeIndex,
                                        int resourceIndex) const;
    double  GetMaximumMaxPossibleBitsCollected(int timeIndex) const;
    double  GetCollectionTime(int timeIndex,
                              int resourceIndex) const;
    double  GetMaximumCollectionTime(int timeIndex) const;
    double  GetResourceFactor(int timeIndex,
                              int resourceIndex) const;

    RegionData*           GetMostValuableRegionData(int timeIndex) const;

    ValueTimelineElement* GetValueTimelineElement(int timeIndex) const;

    LinkList<RegionData>* GetRegionDataList(int timeIndex) const;

    inline double  GetMaximumTimelineScore() const;

    inline ArrayOfPointers<ValueTimelineElement>* GetValueTimeline() const;

protected:

private:

    static inline string GetClassName();

    double                                  maximumTimelineScore_a;
    ArrayOfPointers<ValueTimelineElement> *ptrTimelineElementArray_a;
};

inline
double
ValueTimeline::GetMaximumTimelineScore() const
{
    return(maximumTimelineScore_a);
}

inline
ArrayOfPointers<ValueTimelineElement>*
ValueTimeline::GetValueTimeline() const
{
    return(ptrTimelineElementArray_a);
}

inline
string
ValueTimeline::GetClassName()
{
    return(VALUE_TIMELINE_H);
}

#endif
