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
 * CrisisData.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef CRISIS_DATA_H
#define CRISIS_DATA_H "CrisisData"

#include <memory>

#include "mural/modules/vtg/CrisisArea.h"


#include "general/array/LinkList.h"

using namespace std;

class InputDataElement;

/**
 * Data class containing all the specified Crisis Area's of the run
 *
 * Name                             Description (units)
 * -------------                    --------------------------------------------
 * ptrCrisisAreaList_s*             list of the individual crisis area(s)
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */
class CrisisData
{
public:

    CrisisData();
    CrisisData(const CrisisData &copyMe);
    virtual ~CrisisData();

    CrisisData& operator =  (const CrisisData &copyMe);
    bool        operator == (const CrisisData &compareMe) const;
    bool        operator != (const CrisisData &compareMe) const;

    static void   DestroyCrisisData();
    static void   ExtractCrisisDataInfo(shared_ptr<InputDataElement> ptrInputDataElement);
    static void   PrintAttributes(ostream &outfile);

    static int   GetCrisisIndex(int crisisLevel);
    static double  GetTimelineScoreMultiplier(int crisisLevel,
            const string &userDesignator);

    static bool   HasCrisisArea(const string &userDesignator);

    static CrisisArea* GetCrisisArea(const string &userDesignator,
                                     double regionNumber);


    static inline int   GetNumberOfCrisisAreas();

    static inline LinkList<CrisisArea>* GetCrisisAreaList();

protected:

private:

    static inline string GetClassName();

    static LinkList<CrisisArea> *ptrCrisisAreaList_s;
};

inline

int
CrisisData::GetNumberOfCrisisAreas()
{
    int numberOfCrisisAreas = 0;

    if (ptrCrisisAreaList_s != NULL) {
        numberOfCrisisAreas = ptrCrisisAreaList_s->Size();
    }

    return(numberOfCrisisAreas);
}

inline
LinkList<CrisisArea>*
CrisisData::GetCrisisAreaList()
{
    return(ptrCrisisAreaList_s);
}

inline
string
CrisisData::GetClassName()
{
    return(CRISIS_DATA_H);
}

#endif
