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
 * LinkConstraintData.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "LinkConstraintData.h"

#include "general/utility/Constant.h"

//------------------------------------------------------------------------------
//
//  Method Name: LinkConstraintData::LinkConstraintData
//
//  Purpose:
//    Default constructor for the LinkConstraintData class.
//
//  Calling Method:
//     LinkConstraintData();
//
//  Calling Parameters
//  Parameters      i/o  description
//  ----------      ---  -----------
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

LinkConstraintData::LinkConstraintData()
    : maximumElevationAngle_a(UNDEFINED_ANGLE),
      minimumElevationAngle_a(UNDEFINED_ANGLE),
      minimumGrazingAltitude_a(UNDEFINED_DISTANCE),
      maximumRange_a(UNDEFINED_DISTANCE), minimumRange_a(UNDEFINED_DISTANCE),
      maximumSunSeparationAngle_a(UNDEFINED_ANGLE),
      minimumSunSeparationAngle_a(UNDEFINED_ANGLE),
      ptrDesignator_a(NULL)
{
    ptrDesignator_a = new string("UNKNOWN");
    return;
}

LinkConstraintData::LinkConstraintData(const string &designator,
                                       double maxElevAngle,
                                       double minElevAngle,
                                       double minGrazing,
                                       double maxRange,
                                       double minRange,
                                       double maxSunAngle,
                                       double minSunAngle)
    : maximumElevationAngle_a(maxElevAngle), minimumElevationAngle_a(minElevAngle),
      minimumGrazingAltitude_a(minGrazing),
      maximumRange_a(maxRange), minimumRange_a(minRange),
      maximumSunSeparationAngle_a(maxSunAngle),
      minimumSunSeparationAngle_a(minSunAngle),
      ptrDesignator_a(NULL)
{
    ptrDesignator_a = new string(designator);
    return;
}

LinkConstraintData::LinkConstraintData(const LinkConstraintData &copyMe)
    : maximumElevationAngle_a(copyMe.maximumElevationAngle_a),
      minimumElevationAngle_a(copyMe.minimumElevationAngle_a),
      minimumGrazingAltitude_a(copyMe.minimumGrazingAltitude_a),
      maximumRange_a(copyMe.maximumRange_a), minimumRange_a(copyMe.minimumRange_a),
      maximumSunSeparationAngle_a(copyMe.maximumSunSeparationAngle_a),
      minimumSunSeparationAngle_a(copyMe.minimumSunSeparationAngle_a),
      ptrDesignator_a(NULL)
{
    ptrDesignator_a = new string(*(copyMe.ptrDesignator_a));
    return;
}

LinkConstraintData&
LinkConstraintData::operator = (const LinkConstraintData &copyMe)
{
    maximumElevationAngle_a     = copyMe.maximumElevationAngle_a;
    minimumElevationAngle_a     = copyMe.minimumElevationAngle_a;
    minimumGrazingAltitude_a    = copyMe.minimumGrazingAltitude_a;
    maximumRange_a              = copyMe.maximumRange_a;
    minimumRange_a              = copyMe.minimumRange_a;
    maximumSunSeparationAngle_a = copyMe.maximumSunSeparationAngle_a;
    minimumSunSeparationAngle_a = copyMe.minimumSunSeparationAngle_a;
    ptrDesignator_a = new string(*(copyMe.ptrDesignator_a));
    return(*this);
}

bool
LinkConstraintData::operator == (const LinkConstraintData &compareMe) const
{
    return( (maximumElevationAngle_a  == compareMe.maximumElevationAngle_a) &&
            (minimumElevationAngle_a  == compareMe.minimumElevationAngle_a) &&
            (minimumGrazingAltitude_a  == compareMe.minimumGrazingAltitude_a) &&
            (maximumRange_a  == compareMe.maximumRange_a) &&
            (minimumRange_a  == compareMe.minimumRange_a) &&
            (maximumSunSeparationAngle_a  == compareMe.maximumSunSeparationAngle_a) &&
            (minimumSunSeparationAngle_a  == compareMe.minimumSunSeparationAngle_a) &&
            (*ptrDesignator_a  == *(compareMe.ptrDesignator_a)) );
}

bool
LinkConstraintData::operator != (const LinkConstraintData &compareMe) const
{
    return(!(operator == (compareMe)));
}

LinkConstraintData::~LinkConstraintData()
{
    if (ptrDesignator_a != NULL) {
        delete ptrDesignator_a;
        ptrDesignator_a = NULL;
    }

    return;
}
