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
 * CrisisArea.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef CRISIS_AREA_H
#define CRISIS_AREA_H "CrisisArea"

#include <fstream>
#include <string>
#include <memory>


#include "general/array/Array.h"

using namespace std;

class InputDataElement;

/**
 * Data class to hold the information about Crisis Areas.  These areas are always
 * selected during the MVR process even if their score is zero.  They get a boost in
 * score during RA by the multiplier provided
 *
 *
 * Name                      Description (units)
 * -------------             ---------------------------------------------------
 * identificationNumber_a    number designator of this crisis area
 * ptrRegionNumberArray_a*   array of region numbers comprising this crisis area
 * ptrMultiplierArray_a*     array of score multipliers for each valid user
 *                             vehicle used to boost score for time steps when
 *                             collecting this crisis area
 * ptrIdentificationLabel_a* label designator of this crisis area
 * ptrUserVehicleArray_a*    array of user vehicle designators that are active
 *                             for this crisis area
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */
class CrisisArea
{
public:

    CrisisArea();



    explicit CrisisArea(shared_ptr<InputDataElement> ptrInputDataElement);

    CrisisArea(const CrisisArea &copyMe);
    virtual ~CrisisArea();

    CrisisArea& operator =  (const CrisisArea &copyMe);
    bool        operator == (const CrisisArea &compareMe) const;
    bool        operator != (const CrisisArea &compareMe) const;

    void   PrintAttributes(ostream &outfile) const;

    inline int  GetIdentificationNumber() const;

    inline double  GetTimelineScoreMutliplier(const string &userDesignator) const;

    inline bool   ContainsRegionNumber(double regionNumber) const;
    inline bool   ContainsUserVehicle(const string &userDesignator) const;

    inline string GetIdentificationLabel() const;

protected:

private:

    void   ExtractCrisisAreaInfo(shared_ptr<InputDataElement> ptrInputDataElement);

    static inline string GetClassName();

    int               identificationNumber_a;
    Array<double>       *ptrRegionNumberArray_a;
    Array<double>       *ptrMultiplierArray_a;
    string             *ptrIdentificationLabel_a;
    Array<string>      *ptrUserVehicleArray_a;
};

inline
int
CrisisArea::GetIdentificationNumber() const
{
    return(identificationNumber_a);
}

inline
double
CrisisArea::GetTimelineScoreMutliplier(const string &userDesignator) const
{
    const int index = ptrUserVehicleArray_a->FindIndex(userDesignator);
    double      timelineMultiplier = 0.0;

    if (index >= 0) {
        timelineMultiplier = ptrMultiplierArray_a->ElementAt(index);
    }

    return(timelineMultiplier);
}

inline
string
CrisisArea::GetIdentificationLabel() const
{
    return(*ptrIdentificationLabel_a);
}

inline
bool
CrisisArea::ContainsRegionNumber(double regionNumber) const
{
    return(ptrRegionNumberArray_a->ValueExists(regionNumber));
}

inline
bool
CrisisArea::ContainsUserVehicle(const string &userDesignator) const
{
    return(ptrUserVehicleArray_a->ValueExists(userDesignator));
}

inline
string
CrisisArea::GetClassName()
{
    return(CRISIS_AREA_H);
}

#endif
