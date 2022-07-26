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
 * ValueTimelineElement.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef VALUE_TIMELINE_ELEMENT_H
#define VALUE_TIMELINE_ELEMENT_H "ValueTimelineElement"

#include <iostream>
#include <fstream>

#include "mural/CollectionElement.h"

using namespace std;

/**
 * Class which holds the relevent data for a timeline entry.  It is a child
 * of Collection Element which actualy holds the score.  THis class holds supporting
 * information like collecting sensor, region imaging, time to collect, etc.
 *
 * Name                            Description (units)
 * -------------                   ---------------------------------------------
 * crisisLevel_a                   number of the crisis level (if any) of a
 *                                   Region being imaged
 * ptrSensorNumber_a*              number of the sensor being used (by resource
 *                                   if resource specific timeline)
 * ptrMissionNumber_a*             number of the mission being imaged if using
 *                                   MVR method (by resource if resource
 *                                   specific timeline)
 * ptrSubPriorityLevel_a*          sub-priority level (if any) of a Region being
 *                                   imaged (by resource if resource specific
 *                                   timeline)
 * overflownRegion_a               sub-satellite region number of the user
 * ptrImagedRegion_a*              region number imaged if using MVR method (by
 *                                   resource if resource specific timeline)
 * ptrCollectionTime_a*            time to collect (by resource if resource
 *                                   specific timeline)
 * ptrMaxPossibleBitsCollected_a*  maximum number of bits being collected (by
 *                                   resource if resource specific timeline)
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */
class ValueTimelineElement : public CollectionElement
{
public:

    ValueTimelineElement();
    ValueTimelineElement(double overRegion,
                         int numberOfResources,
                         LinkList<RegionData> *ptrValidRegionDataList,
                         double score = 0.0,
                         RegionData *ptrMVRegionData = NULL,
                         int crisisLevel = 0,
                         Array<double> *ptrImagedRegion = NULL,
                         Array<int> *ptrSensorNumber = NULL,
                         Array<int> *ptrMissionNumber = NULL,
                         Array<double> *ptrCollectionTime = NULL,
                         Array<double> *ptrMaxPossibleBitsCollected = NULL,
                         Array<int> *ptrSubPriorityLevel = NULL,
                         Array<double> *ptrResourceFactor = NULL);
    ValueTimelineElement(const ValueTimelineElement &copyMe);
    virtual ~ValueTimelineElement();

    ValueTimelineElement& operator =  (const ValueTimelineElement &copyMe);
    bool              operator == (const ValueTimelineElement &compareMe) const;
    bool              operator != (const ValueTimelineElement &compareMe) const;

    int  GetCrisisLevel() const;
    int  GetSensorNumber(int resourceIndex = -1) const;
    int  GetHighestScoreSensorNumber() const;
    int  GetMissionNumber(int resourceIndex = -1) const;
    int  GetSubPriorityLevel(int resourceIndex = -1) const;

    double  GetCollectionTime(int resourceIndex) const;
    double  GetMaximumCollectionTime() const;
    double  GetMaxPossibleBitsCollected(int resourceIndex) const;
    double  GetMaximumMaxPossibleBitsCollected() const;
    double  GetImagedRegion(int resourceIndex = -1) const;

    inline void SetCrisisLevel(int newValue);
    inline void SetSensorNumber(int newValue,
                                int resourceIndex);
    inline void SetMissionNumber(int newValue,
                                 int resourceIndex);
    inline void SetSubPriorityLevel(int newValue,
                                    int resourceIndex);
    inline void SetImagedRegion(double newValue,
                                int resourceIndex);
    inline void SetCollectionTime(double newValue,
                                  int resourceIndex);
    inline void SetMaxPossibleBitsCollected(double newValue,
                                            int resourceIndex);

    inline int  GetNumberOfSensors() const;
    inline int  GetNumberOfMissions() const;
    inline int  GetNumberOfSubPriorities() const;
    inline int  GetNumberOfImagedRegions() const;
    inline int  GetNumberOfMaxPossibleBitsCollected() const;
    inline int  GetNumberOfCollectionTimes() const;

    inline double GetOverflownRegion() const;

protected:

private:

    static inline string GetClassName();

    inline void SetOverflownRegion(double newValue);

    int                 crisisLevel_a;
    Array<int>         *ptrSensorNumber_a;
    Array<int>         *ptrMissionNumber_a;
    Array<int>         *ptrSubPriorityLevel_a;
    double                 overflownRegion_a;
    Array<double>         *ptrImagedRegion_a;
    Array<double>         *ptrCollectionTime_a;
    Array<double>         *ptrMaxPossibleBitsCollected_a;
};

inline
void
ValueTimelineElement::SetCrisisLevel(int newValue)
{
    crisisLevel_a = newValue;
}

inline
int
ValueTimelineElement::GetNumberOfSensors() const
{
    return(ptrSensorNumber_a->Size());
}

inline
int
ValueTimelineElement::GetNumberOfMissions() const
{
    return(ptrMissionNumber_a->Size());
}

inline
int
ValueTimelineElement::GetNumberOfSubPriorities() const
{
    return(ptrSubPriorityLevel_a->Size());
}

inline
int
ValueTimelineElement::GetNumberOfImagedRegions() const
{
    return(ptrImagedRegion_a->Size());
}

inline
int
ValueTimelineElement::GetNumberOfMaxPossibleBitsCollected() const
{
    return(ptrMaxPossibleBitsCollected_a->Size());
}

inline
int
ValueTimelineElement::GetNumberOfCollectionTimes() const
{
    return(ptrCollectionTime_a->Size());
}

inline
double
ValueTimelineElement::GetOverflownRegion() const
{
    return(overflownRegion_a);
}

inline
void
ValueTimelineElement::SetImagedRegion(double newValue,
                                      int resourceIndex)
{
    if (resourceIndex < 0) {
        int index = 0;
        (*ptrImagedRegion_a)[index] = newValue;
    } else {
        (*ptrImagedRegion_a)[resourceIndex] = newValue;
    }
}

inline
void
ValueTimelineElement::SetOverflownRegion(double newValue)
{
    overflownRegion_a = newValue;
}

inline
void
ValueTimelineElement::SetSensorNumber(int newValue,
                                      int resourceIndex)
{
    if (resourceIndex < 0) {
        int index = 0;
        (*ptrSensorNumber_a)[index] = newValue;
    } else {
        (*ptrSensorNumber_a)[resourceIndex] = newValue;
    }
}

inline
void
ValueTimelineElement::SetMissionNumber(int newValue,
                                       int resourceIndex)
{
    if (resourceIndex < 0) {
        int index = 0;
        (*ptrMissionNumber_a)[index] = newValue;
    } else {
        (*ptrMissionNumber_a)[resourceIndex] = newValue;
    }
}

inline
void
ValueTimelineElement::SetSubPriorityLevel(int newValue,
        int resourceIndex)
{
    if (resourceIndex < 0) {
        int index = 0;
        (*ptrSubPriorityLevel_a)[index] = newValue;
    } else {
        (*ptrSubPriorityLevel_a)[resourceIndex] = newValue;
    }
}

inline
void
ValueTimelineElement::SetMaxPossibleBitsCollected(double newValue,
        int resourceIndex)
{
    if (resourceIndex < 0) {
        int index = 0;
        (*ptrMaxPossibleBitsCollected_a)[index] = newValue;
    } else {
        (*ptrMaxPossibleBitsCollected_a)[resourceIndex] = newValue;
    }
}

inline
void
ValueTimelineElement::SetCollectionTime(double newValue,
                                        int resourceIndex)
{
    if (resourceIndex < 0) {
        int index = 0;
        (*ptrCollectionTime_a)[index] = newValue;
    } else {
        (*ptrCollectionTime_a)[resourceIndex] = newValue;
    }
}

inline
string
ValueTimelineElement::GetClassName()
{
    return(VALUE_TIMELINE_ELEMENT_H);
}

#endif
