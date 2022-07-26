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
 * CollectionElement.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef COLLECTION_ELEMENT_H
#define COLLECTION_ELEMENT_H "CollectionElement"


#include "general/array/Array.h"
#include "general/array/LinkList.h"

using namespace std;

class RegionData;

/**
 * Class which represnets collected score/area for a given timeline entry.
 *
 * CollectionElement is the class that contains collection data stored by
 *  the ValueTimeline, the CommunicationServiceRequest (if requested MVR), and
 *  the ResourceServicePeriod (if allocated MVR).
 *
 * Name                            Description (units)
 * -------------                   ---------------------------------------------
 * ptrCollectedPointTargets_a*     number of collected point targets by priority
 * ptrCollectedArea_a*             amount of collected area by priority
 * collectionScore_a               the score at this time step associated with
 *                                   the collected requirement(s)
 * ptrResourceFactor_a*            factor applied to collection score for a
 *                                   specific resource (resource specific
 *                                   timeline or elevation angle factor)
 * ptrMostValuableRegionData_a*    the RegionData in the list considered the
 *                                   most valuable at this time step
 * ptrRegionDataList_a*            list of valid RegionData instances (region in
 *                                   view with one or more valid requirements)
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class CollectionElement
{
public:

    CollectionElement();
    CollectionElement(double score,
                      LinkList<RegionData> *ptrValidRegionDataList = NULL,
                      RegionData *ptrMVRegionData = NULL,
                      //RAGUSA
                      int crisisLevel = 0,
                      Array<double> *ptrResourceFactor = NULL,
                      Array<int> *ptrCollectedPoints = NULL,
                      Array<double> *ptrCollectedArea = NULL);
    CollectionElement(const CollectionElement &copyMe);
    virtual ~CollectionElement();

    CollectionElement& operator =  (const CollectionElement &copyMe);
    bool               operator == (const CollectionElement &compareMe) const;
    bool               operator != (const CollectionElement &compareMe) const;

    //RAGUSA
    inline int  GetCrisisLevel() const;
    virtual int  GetMissionNumber(int resourceIndex = -1) const;
    virtual int  GetSensorNumber(int resourceIndex = -1) const;
    virtual int  GetSubPriorityLevel(int resourceIndex = -1) const;

    virtual double  GetImagedRegion(int resourceIndex = -1) const;

    void   AddRegionData(RegionData *ptrRegionData);
    void   RemoveNonCrisisRegionDataRequirements();

    Array<double>* GetRegionsInView() const;

    void   RemoveAllRegionData();

    bool   IsRegionIncluded(int regionNumber) const;

    string  GetSensorName() const;

    inline void   SetMostValuableRegionData(RegionData *ptrMVRegionData);
    inline void   SetResourceFactor(double newValue,
                                    int resourceIndex);
    inline void   SetResourceFactors(Array<double> *ptrFactorArray);
    inline void   SetScore(double newValue);

    inline int  GetCollectedPointTargets(int priorityIndex) const;

    inline int   GetNumberOfScores() const;
    inline int   GetNumberOfResourceFactors() const;

    inline double  GetCollectedArea(int priorityIndex) const;

    inline double GetScore(int resourceIndex = -1) const;
    inline double GetCollectionScore() const;
    inline double GetResourceFactor(int resourceIndex) const;

    inline Array<double>* GetResourceFactors() const;

    inline RegionData*           GetMostValuableRegionData() const;

    inline LinkList<RegionData>* GetRegionDataList() const;

protected:

    Array<int>         *ptrCollectedPointTargets_a;
    Array<double>         *ptrCollectedArea_a;
    double                collectionScore_a;
    Array<double>        *ptrResourceFactor_a;
    RegionData           *ptrMostValuableRegionData_a;
    LinkList<RegionData> *ptrRegionDataList_a;
    //RAGUSA 1/30/2018 - Added Crisis Level
    int                  crisisLevel_a;

private:

    static inline string GetClassName();
};

inline
void
CollectionElement::SetScore(double newValue)
{
    collectionScore_a = newValue;
}

inline
void
CollectionElement::SetResourceFactor(double newValue,
                                     int resourceIndex)
{
    (*ptrResourceFactor_a)[resourceIndex] *= newValue;
}

inline
void
CollectionElement::SetResourceFactors(Array<double> *ptrFactorArray)
{
    int       factorIndex = 0;
    const int numberOfFactors = ptrFactorArray->Size();

    if (ptrResourceFactor_a != NULL) {
        while (factorIndex < numberOfFactors) {
            (*ptrFactorArray)[factorIndex] *= ptrResourceFactor_a->ElementAt(factorIndex);
            ++factorIndex;
        }

        delete ptrResourceFactor_a;
        ptrResourceFactor_a = NULL;
    }

    ptrResourceFactor_a = ptrFactorArray;
}

inline
void
CollectionElement::SetMostValuableRegionData(RegionData *ptrMVRegionData)
{
    ptrMostValuableRegionData_a = ptrMVRegionData;
}

inline
int
CollectionElement::GetCollectedPointTargets(int priorityIndex) const
{
    int collectedPointTargets = 0;

    if (ptrCollectedPointTargets_a != NULL) {
        collectedPointTargets = ptrCollectedPointTargets_a->ElementAt(priorityIndex);
    }

    return(collectedPointTargets);
}

inline
int
CollectionElement::GetNumberOfScores() const
{
    int numberOfScores = 1;

    if (ptrResourceFactor_a != NULL) {
        numberOfScores = ptrResourceFactor_a->Size();
    }

    return(numberOfScores);
}

inline
int
CollectionElement::GetNumberOfResourceFactors() const
{
    int numberOfResourceFactors = 0;

    if (ptrResourceFactor_a != NULL) {
        numberOfResourceFactors = ptrResourceFactor_a->Size();
    }

    return(numberOfResourceFactors);
}

inline
double
CollectionElement::GetCollectedArea(int priorityIndex) const
{
    double collectedArea = 0;

    if (ptrCollectedArea_a != NULL) {
        collectedArea = ptrCollectedArea_a->ElementAt(priorityIndex);
    }

    return(collectedArea);
}

inline
double
CollectionElement::GetScore(int resourceIndex) const
{
    return(collectionScore_a * GetResourceFactor(resourceIndex));
}

inline
double
CollectionElement::GetCollectionScore() const
{
    return(collectionScore_a);
}

inline
Array<double>*
CollectionElement::GetResourceFactors() const
{
    return(ptrResourceFactor_a);
}

inline
double
CollectionElement::GetResourceFactor(int resourceIndex) const
{
    double resourceFactor = 1.0;

    if ((ptrResourceFactor_a != NULL) && (resourceIndex >= 0)) {
        resourceFactor = ptrResourceFactor_a->ElementAt(resourceIndex);
    }

    return(resourceFactor);
}

inline
RegionData*
CollectionElement::GetMostValuableRegionData() const
{
    return(ptrMostValuableRegionData_a);
}

inline
LinkList<RegionData>*
CollectionElement::GetRegionDataList() const
{
    return(ptrRegionDataList_a);
}

inline
string
CollectionElement::GetClassName()
{
    return(COLLECTION_ELEMENT_H);
}

//RAGUSA 1/30/2018 - Replaces deprecated method from CollectionElement.cpp
inline
int
CollectionElement::GetCrisisLevel() const
{
    return(crisisLevel_a);
}

#endif
