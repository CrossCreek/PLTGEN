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
 * AzimuthAngleData.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef AZIMUTH_ANGLE_DATA_H
#define AZIMUTH_ANGLE_DATA_H "AzimuthAngleData"

#include <string>
#include <iostream>


#include "general/utility/Constant.h"

using namespace std;

/**
 *  Class to represent "bins" of Az/el data.  If an azimuth value falls between
 *  the min_az and max_az then the min_el and max_el are used to constrain elevation.
 *
 * Class Attribute Descriptions
 *
 * Name                     Description (units)
 * -------------            ----------------------------------------------------
 * useAzimuthConstraint_s   indicates if any vehicle has azimuth constraint
 * minimumElevationAngle_a  the minimum elevation angle the azimuth constraint
 *                            applies to (-90 to 90 degrees)
 * maximumElevationAngle_a  the maximum elevation angle the azimuth constraint
 *                            applies to (-90 to 90 degrees)
 * minimumAzimuthAngle_a    the minimum azimuth constraint angle (default)
 * maximumAzimuthAngle_a    the maximum azimuth constraint angle
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class AzimuthAngleData
{
public:

    AzimuthAngleData();
    AzimuthAngleData(double minimumElevation,
                     double maximumElevation,
                     double minimumAzimuthAngle,
                     double maximumAzimuthAngle = UNDEFINED_ANGLE);
    AzimuthAngleData(const AzimuthAngleData &AzimuthAngleData);
    virtual ~AzimuthAngleData();

    AzimuthAngleData& operator =  (const AzimuthAngleData &copyMe);
    bool              operator == (const AzimuthAngleData &compareMe) const;

    /**
     * Compares on the Azimuth Angle value.  Returns true if this azimuth value is
     * less than compare
     */
    bool              operator <  (const AzimuthAngleData &compareMe) const;

    static inline bool   UseAzimuthConstraint();

    inline const double GetMinimumElevationAngle() const;
    inline const double GetMaximumElevationAngle() const;
    inline const double GetAzimuthAngle() const;  // minimum is the default
    inline const double GetMinimumAzimuthAngle() const;
    inline const double GetMaximumAzimuthAngle() const;

protected:

private:

    static inline string GetClassName();

    static bool         useAzimuthConstraint_s;

    double              minimumElevationAngle_a;
    double              maximumElevationAngle_a;
    double              minimumAzimuthAngle_a;
    double              maximumAzimuthAngle_a;
};

inline
bool
AzimuthAngleData::UseAzimuthConstraint()
{
    return(useAzimuthConstraint_s);
}

inline
const double
AzimuthAngleData::GetMinimumElevationAngle() const
{
    return(minimumElevationAngle_a);
}

inline
const double
AzimuthAngleData::GetMaximumElevationAngle() const
{
    return(maximumElevationAngle_a);
}

inline
const double
AzimuthAngleData::GetAzimuthAngle() const
{
    return(minimumAzimuthAngle_a);
}

inline
const double
AzimuthAngleData::GetMinimumAzimuthAngle() const
{
    return(minimumAzimuthAngle_a);
}

inline
const double
AzimuthAngleData::GetMaximumAzimuthAngle() const
{
    return(maximumAzimuthAngle_a);
}

inline
string
AzimuthAngleData::GetClassName()
{
    return(AZIMUTH_ANGLE_DATA_H);
}

#endif
