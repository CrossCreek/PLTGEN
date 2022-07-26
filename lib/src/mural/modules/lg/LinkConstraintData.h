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
 * LinkConstraintData.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef LINK_CONSTRAINT_DATA_H
#define LINK_CONSTRAINT_DATA_H "LinkConstraintData"

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

/**
 * Data class which represents various link constraint values read in from the
 * run control to be applied during link generation
 *
 * LinkConstraintData is the class that controls the data for link
 *     constraints for an antenna (default or provider specific)..
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------------
 * ptrDesignator_a*             string containing default or DataProvider
 *                                designator
 * maximumElevationAngle_a      the maximum the Earth-Antenna-DataProvider angle
 *                                may be to have a link (measured perpendicular
 *                                to the Earth-Antenna vector)
 * minimumElevationAngle_a      the minimum the Earth-Antenna-DataProvider angle
 *                                may be to have a link (measured perpendicular
 *                                to the Earth-Antenna vector)
 * minimumGrazingAltitude_a     the minimum the line of sight vector may be
 *                                above the Earth's surface to have a link
 * maximumRange_a               the maximum the Antenna-DataProvider distance
 *                                may be to have a link
 * minimumRange_a               the minimum the Antenna-DataProvider distance
 *                                may be to have a link
 * maximumSunSeparationAngle_a  the maximum the Sun-Antenna-DataProvider angle
 *                                may be to have a link
 * minimumSunSeparationAngle_a  the minimum the Sun-Antenna-DataProvider angle
 *                                may be to have a link
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class LinkConstraintData
{
public:

    LinkConstraintData();
    LinkConstraintData(const string &designator,
                       double maxElevAngle,
                       double minElevAngle,
                       double minGrazing,
                       double maxRange,
                       double minRange,
                       double maxSunAngle,
                       double minSunAngle);
    LinkConstraintData(const LinkConstraintData &copyMe);
    virtual ~LinkConstraintData();

    LinkConstraintData& operator = (const LinkConstraintData &copyMe);
    bool                operator == (const LinkConstraintData &compareMe) const;
    bool                operator != (const LinkConstraintData &compareMe) const;

    inline void   SetDataProviderDesignator(const string &designator);
    inline void   SetMaximumElevationAngle(double maxElevAngle);
    inline void   SetMinimumElevationAngle(double minElevAngle);
    inline void   SetMinimumGrazingAltitude(double minGrazing);
    inline void   SetMaximumRange(double maxRange);
    inline void   SetMinimumRange(double minRange);
    inline void   SetMaximumSunSeparationAngle(double maxSunAngle);
    inline void   SetMinimumSunSeparationAngle(double minSunAngle);

    inline double GetMaximumElevationAngle() const;
    inline double GetMinimumElevationAngle() const;
    inline double GetMinimumGrazingAltitude() const;
    inline double GetMaximumRange() const;
    inline double GetMinimumRange() const;
    inline double GetMaximumSunSeparationAngle() const;
    inline double GetMinimumSunSeparationAngle() const;

    inline string GetDataProviderDesignator() const;

protected:

private:




    double              maximumElevationAngle_a;
    double              minimumElevationAngle_a;
    double              minimumGrazingAltitude_a;
    double              maximumRange_a;
    double              minimumRange_a;
    double              maximumSunSeparationAngle_a;
    double              minimumSunSeparationAngle_a;
    string             *ptrDesignator_a;
};

inline
void
LinkConstraintData::SetDataProviderDesignator(const string &designator)
{
    if (ptrDesignator_a != NULL) {
        delete ptrDesignator_a;
    }

    ptrDesignator_a = new string(designator);
}

inline
void
LinkConstraintData::SetMaximumElevationAngle(double maxElevAngle)
{
    maximumElevationAngle_a = maxElevAngle;
}

inline
void
LinkConstraintData::SetMinimumElevationAngle(double minElevAngle)
{
    minimumElevationAngle_a = minElevAngle;
}

inline
void
LinkConstraintData::SetMinimumGrazingAltitude(double minGrazing)
{
    minimumGrazingAltitude_a = minGrazing;
}

inline
void
LinkConstraintData::SetMaximumRange(double maxRange)
{
    maximumRange_a = maxRange;
}

inline
void
LinkConstraintData::SetMinimumRange(double minRange)
{
    minimumRange_a = minRange;
}

inline
void
LinkConstraintData::SetMaximumSunSeparationAngle(double maxSunAngle)
{
    maximumSunSeparationAngle_a = maxSunAngle;
}

inline
void
LinkConstraintData::SetMinimumSunSeparationAngle(double minSunAngle)
{
    minimumSunSeparationAngle_a = minSunAngle;
}

inline
string
LinkConstraintData::GetDataProviderDesignator() const
{
    return(*ptrDesignator_a);
}

inline
double
LinkConstraintData::GetMaximumElevationAngle() const
{
    return(maximumElevationAngle_a);
}

inline
double
LinkConstraintData::GetMinimumElevationAngle() const
{
    return(minimumElevationAngle_a);
}

inline
double
LinkConstraintData::GetMinimumGrazingAltitude() const
{
    return(minimumGrazingAltitude_a);
}

inline
double
LinkConstraintData::GetMaximumRange() const
{
    return(maximumRange_a);
}

inline
double
LinkConstraintData::GetMinimumRange() const
{
    return(minimumRange_a);
}

inline
double
LinkConstraintData::GetMaximumSunSeparationAngle() const
{
    return(maximumSunSeparationAngle_a);
}

inline
double
LinkConstraintData::GetMinimumSunSeparationAngle() const
{
    return(minimumSunSeparationAngle_a);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
LinkConstraintData::GetClassName()
{
    return(LINK_CONSTRAINT_DATA_H);
}
*/

#endif
