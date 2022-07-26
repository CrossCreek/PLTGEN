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
 * ConjunctionAngleData.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef CONJUNCTION_ANGLE_DATA_H
#define CONJUNCTION_ANGLE_DATA_H "ConjunctionAngleData"

#include <string>
#include <iostream>

using namespace std;

class ConjunctionAngleData;

/**
 * Data class to hold the conjunction constraint values for two data providers
 *
 * Name                           Description (units)
 * -------------                  ----------------------------------------------
 * numberOfSimultaneousSenders_a  the number of competing data providers
 *                                  involved for this conjunction angle to be in
 *                                  effect (standard is 2)
 * conjunctionAngle_a             the conjunction constraint angle
 * ptrNearDesignator_a*           string containing the designator of the sender
 *                                  closest to the receive antenna
 * ptrFarDesignator_a*            string containing the designator of the sender
 *                                  farthest from the receive antenna
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class ConjunctionAngleData
{
public:

    ConjunctionAngleData();
    ConjunctionAngleData(double newAngle,
                         const string &newActionTagNear,
                         const string &newActionTagFar,
                         int simultaneousSenders = 2);
    ConjunctionAngleData(const ConjunctionAngleData &conjunctionAngleData);
    virtual ~ConjunctionAngleData();

    ConjunctionAngleData& operator =  (const ConjunctionAngleData &copyMe);
    bool            operator == (const ConjunctionAngleData &compareMe) const;
    bool            operator != (const ConjunctionAngleData &compareMe) const;

    inline int GetNumberOfSimultaneousSenders() const;

    inline double GetConjunctionAngle() const;

    inline const string GetNearDataProviderDesignator() const;
    inline const string GetFarDataProviderDesignator() const;

protected:

private:













    int               numberOfSimultaneousSenders_a;
    double              conjunctionAngle_a;
    string             *ptrNearDesignator_a;
    string             *ptrFarDesignator_a;
};

inline
int
ConjunctionAngleData::GetNumberOfSimultaneousSenders() const
{
    return(numberOfSimultaneousSenders_a);
}

inline
double
ConjunctionAngleData::GetConjunctionAngle() const
{
    return(conjunctionAngle_a);
}

inline
const string
ConjunctionAngleData::GetNearDataProviderDesignator() const
{
    return(*ptrNearDesignator_a);
}

inline
const string
ConjunctionAngleData::GetFarDataProviderDesignator() const
{
    return(*ptrFarDesignator_a);
}

//CPPCHECK_RELATED
//Removed SetConjunctionAngle() since it wasn't called in code base
/*
inline
void
ConjunctionAngleData::SetConjunctionAngle(double newAngle)
{
    conjunctionAngle_a = newAngle;
    return;
}
*/

//CPPCHECK_RELATED
//Removed SetNearDataProviderDesignator() since it wasn't called in code base
/*
inline
void
ConjunctionAngleData::SetNearDataProviderDesignator(const string &newActionTag)
{
    if (ptrNearDesignator_a != NULL) {
        delete ptrNearDesignator_a;
    }

    ptrNearDesignator_a = new string(newActionTag);
    return;
}
*/

//CPPCHECK_RELATED
//Removed SetFarDataProviderDesignator() since it wasn't called in code base
/*
inline
void
ConjunctionAngleData::SetFarDataProviderDesignator(const string &newActionTag)
{
    if (ptrFarDesignator_a != NULL) {
        delete ptrFarDesignator_a;
    }

    ptrFarDesignator_a = new string(newActionTag);
    return;
}
*/

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
ConjunctionAngleData::GetClassName()
{
    return(CONJUNCTION_ANGLE_DATA_H);
}
*/

#endif
