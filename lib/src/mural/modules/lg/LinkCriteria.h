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
 * LinkCriteria.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef LINK_CRITERIA_H
#define LINK_CRITERIA_H "LinkCriteria"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>


#include "general/array/LinkList.h"

using namespace std;

class ConjunctionAngleData;
class InputData;
class InputDataElement;
class OverheadData;

/**
 *  Data class representing various link generation parameters read in from the run-control.
 *
 * Name                             Description (units)
 * -------------                    --------------------------------------------
 * maximumManeuverTime_a            overhead time in addition to acquisition
 *                                    time when determining requests for IAOOC
 *                                    user vehicles
 * ptrRelayCoverageContourArray_a*  pointer to an array of contour values to be
 *                                    printed out
 * linkPathMethod_a                 CREATION_METHOD used to create link paths
 * conjunctionMethod_a              CREATION_METHOD used to create conjunction
 *                                    data
 * ptrNBConjunctionAngleDataList_a* list of ConjunctionAngleData instances
 *                                    containing NB angles between users
 * ptrWBConjunctionAngleDataList_a* list of ConjunctionAngleData instances
 *                                     containing WB angles between users
 * ptrOverheadDataList_a*           list of OverheadData instances containing
 *                                    the different acquisition, start up, and
 *                                    droplink times
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class LinkCriteria
{
public:

    enum CREATION_METHOD {
        UNKNOWN_METHOD = -1,
        READ_DATA = 0,
        GENERATE_DATA = 1
    };

    LinkCriteria();
    LinkCriteria(shared_ptr<InputDataElement> ptrInputDataElement,
                 const bool &dataTransmitter);
    LinkCriteria(const LinkCriteria &copyMe);
    virtual ~LinkCriteria();

    LinkCriteria&   operator =  (const LinkCriteria &copyMe);
    bool            operator == (const LinkCriteria &compareMe) const;
    bool            operator != (const LinkCriteria &compareMe) const;

    void  PrintAttributes(ostream &outfile) const;
    void  RemoveConjunctionData(int maximumSenders);
    void  ResetMakeBeforeBreakTimes() const;
    void  VerifyMakeBeforeBreakTimes() const;

    int  GetMaximumSimultaneousConjunctionSenders() const;

    double GetDefaultNarrowbandConjunctionAngle(int simultaneousSenders) const;
    double GetDefaultWidebandConjunctionAngle(int simultaneousSenders) const;
    double GetNarrowbandConjunctionAngle(const string &nearDesignator,
                                         const string &farDesignator,
                                         int simultaneousSenders) const;
    double GetWidebandConjunctionAngle(const string &nearDesignator,
                                       const string &farDesignator,
                                       int simultaneousSenders) const;

    bool   HasSimultaneousConjunctionSenders(int simultaneousSenders) const;

    const OverheadData* GetLinkOverheadData(const string &designator) const;

    inline int  GetMaximumManeuverTime() const;

    inline shared_ptr<vector<int>> GetRelayCoverageContourArray() const;

    inline double GetMinimumICCLSolarIntrusionAngle() const;

    inline bool   HasNarrowbandConjunctionData() const;
    inline bool   HasWidebandConjunctionData() const;
    inline bool   ReadLinkPaths() const;
    inline bool   ReadConjunctionData() const;

    inline string GetLinkPathMethodString() const;
    inline string GetConjunctionMethodString() const;

    inline CREATION_METHOD GetLinkPathMethod() const;
    inline CREATION_METHOD GetConjunctionMethod() const;

    inline LinkList<ConjunctionAngleData>* GetNarrowbandConjunctionAngleDataList() const;
    inline LinkList<ConjunctionAngleData>* GetWidebandConjunctionAngleDataList() const;

    inline const LinkList<OverheadData>* GetOverheadDataList() const;

protected:

private:

    static inline string GetCreationString(const CREATION_METHOD &method);

    void ExtractLinkCriteriaInfo(shared_ptr<InputDataElement> ptrInputDataElement,
                                 const bool &dataTransmitter);
    void SetNarrowbandConjunctionAngleData(const shared_ptr<InputData> ptrInputData);
    void SetWidebandConjunctionAngleData(const shared_ptr<InputData> ptrInputData);

    CREATION_METHOD GetCreationMethod(const string &inputString) const;

    OverheadData* GetOverheadData(const string &resourceDesignator) const;

    static inline string GetClassName();

    static const int              MINIMUM_CONJUNCTION_SENDERS_s;

    int                             maximumManeuverTime_a;
    double                          minimumICCLSolarIntrusionAngle_a;
    shared_ptr<vector<int>>         ptrRelayCoverageContourArray_a;
    CREATION_METHOD                 linkPathMethod_a;
    CREATION_METHOD                 conjunctionMethod_a;
    LinkList<ConjunctionAngleData> *ptrNBConjunctionAngleDataList_a;
    LinkList<ConjunctionAngleData> *ptrWBConjunctionAngleDataList_a;
    LinkList<OverheadData>         *ptrOverheadDataList_a;
};

inline
int
LinkCriteria::GetMaximumManeuverTime() const
{
    return(maximumManeuverTime_a);
}

inline
shared_ptr<vector<int>>
                     LinkCriteria::GetRelayCoverageContourArray() const
{
    return(ptrRelayCoverageContourArray_a);
}

inline
double
LinkCriteria::GetMinimumICCLSolarIntrusionAngle() const
{
    return(minimumICCLSolarIntrusionAngle_a);
}

inline
bool
LinkCriteria::HasNarrowbandConjunctionData() const
{
    return(ptrNBConjunctionAngleDataList_a != NULL);
}

inline
bool
LinkCriteria::HasWidebandConjunctionData() const
{
    return(ptrWBConjunctionAngleDataList_a != NULL);
}

inline
bool
LinkCriteria::ReadLinkPaths() const
{
    return(linkPathMethod_a == READ_DATA);
}

inline
bool
LinkCriteria::ReadConjunctionData() const
{
    return(conjunctionMethod_a == READ_DATA);
}

inline
string
LinkCriteria::GetCreationString(const CREATION_METHOD &method)
{
    string returnString = "UNKNOWN";

    if (method == READ_DATA) {
        returnString = "READ";
    } else if (method == GENERATE_DATA) {
        returnString = "GENERATE";
    }

    return(returnString);
}

inline
LinkCriteria::CREATION_METHOD
LinkCriteria::GetLinkPathMethod() const
{
    return(linkPathMethod_a);
}

inline
LinkCriteria::CREATION_METHOD
LinkCriteria::GetConjunctionMethod() const
{
    return(conjunctionMethod_a);
}

inline
string
LinkCriteria::GetLinkPathMethodString() const
{
    return(GetCreationString(linkPathMethod_a));
}

inline
string
LinkCriteria::GetConjunctionMethodString() const
{
    return(GetCreationString(conjunctionMethod_a));
}

inline
LinkList<ConjunctionAngleData>*
LinkCriteria::GetNarrowbandConjunctionAngleDataList() const
{
    return(ptrNBConjunctionAngleDataList_a);
}

inline
LinkList<ConjunctionAngleData>*
LinkCriteria::GetWidebandConjunctionAngleDataList() const
{
    return(ptrWBConjunctionAngleDataList_a);
}

inline
const LinkList<OverheadData>*
LinkCriteria::GetOverheadDataList() const
{
    return(ptrOverheadDataList_a);
}

inline
string
LinkCriteria::GetClassName()
{
    return(LINK_CRITERIA_H);
}

#endif
