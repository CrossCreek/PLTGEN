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
 * ConeAngleData.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include "ConeAngleData.h"
#include "general/utility/Constant.h"
//RAGUSA 7/24/2017 - Needed to resolve min/max function removal from std library.
#include <algorithm>

ConeAngleData::ConeAngleData()
    : defaultElevationAngle_a(UNDEFINED_ANGLE),
      ptrSpecficMissionAngleMap_a(new std::map<string, double>()),
      ptrSpecficMissionTags_a(new std::vector<string>())
{
    return;
}

ConeAngleData::ConeAngleData(const double defaultValue)
    : defaultElevationAngle_a(defaultValue),
      ptrSpecficMissionAngleMap_a(new std::map<string, double>()),
      ptrSpecficMissionTags_a(new std::vector<string>())
{
    return;
}

ConeAngleData::ConeAngleData(const ConeAngleData &copyMe)
    : defaultElevationAngle_a(copyMe.defaultElevationAngle_a),
      ptrSpecficMissionAngleMap_a(copyMe.ptrSpecficMissionAngleMap_a),
      ptrSpecficMissionTags_a(copyMe.ptrSpecficMissionTags_a)
{
    return;
}

ConeAngleData&
ConeAngleData::operator = (const ConeAngleData &copyMe)
{
    defaultElevationAngle_a = copyMe.defaultElevationAngle_a;
    ptrSpecficMissionAngleMap_a = copyMe.ptrSpecficMissionAngleMap_a;
    ptrSpecficMissionTags_a = copyMe.ptrSpecficMissionTags_a;
    return(*this);
}

bool
ConeAngleData::operator == (const ConeAngleData &compareMe) const
{
    return( (defaultElevationAngle_a == (compareMe.defaultElevationAngle_a)) &&
            (ptrSpecficMissionAngleMap_a == compareMe.ptrSpecficMissionAngleMap_a) &&
            (ptrSpecficMissionTags_a == compareMe.ptrSpecficMissionTags_a) );
}

bool
ConeAngleData::operator != (const ConeAngleData &compareMe) const
{
    return(!(operator == (compareMe)));
}

/* ***************************************************************************** */

void
ConeAngleData::AddSpecificMission(const string &missionKey, double angleValue)
{
    if ( ptrSpecficMissionAngleMap_a->find(missionKey) == ptrSpecficMissionAngleMap_a->end()) {
        ptrSpecficMissionAngleMap_a->insert(std::pair<string, double>(missionKey, angleValue));
        ptrSpecficMissionTags_a->push_back(missionKey);
    }
}

/* ***************************************************************************** */

double
ConeAngleData::GetAngleValue(const string &missionKey) const
{
    double angleValue = defaultElevationAngle_a;

    if ( ptrSpecficMissionAngleMap_a->find(missionKey) != ptrSpecficMissionAngleMap_a->end()) {
        angleValue = ptrSpecficMissionAngleMap_a->find(missionKey)->second;
    }

    return angleValue;
}

/* ***************************************************************************** */

bool
ConeAngleData::HasSpecificMission(const string &missionKey) const
{
    bool returnValue = false;

    if ( ptrSpecficMissionAngleMap_a->find(missionKey) != ptrSpecficMissionAngleMap_a->end()) {
        returnValue = true;
    }

    return returnValue;
}

/* ***************************************************************************** */

double
ConeAngleData::GetDefaultValue() const
{
    return defaultElevationAngle_a;
}

/* ***************************************************************************** */

double
ConeAngleData::GetMinimumAngle() const
{
    double minAngle = defaultElevationAngle_a;
    std::map<string, double>::iterator mapIter;

    for ( mapIter = ptrSpecficMissionAngleMap_a->begin(); mapIter != ptrSpecficMissionAngleMap_a->end(); ++mapIter) {
        if ( (*mapIter).first != REVISIT_STRING ) {
            double specificAngle = (*mapIter).second;
            minAngle = std::min(minAngle, specificAngle);
        }
    }

    return minAngle;
}

/* ***************************************************************************** */

double
ConeAngleData::GetMaximumAngle() const
{
    double maxAngle = defaultElevationAngle_a;
    std::map<string, double>::iterator mapIter;

    for ( mapIter = ptrSpecficMissionAngleMap_a->begin(); mapIter != ptrSpecficMissionAngleMap_a->end(); ++mapIter) {
        if ( (*mapIter).first != REVISIT_STRING ) {
            double specificAngle = (*mapIter).second;
            maxAngle = std::max(maxAngle, specificAngle);
        }
    }

    return maxAngle;
}

/* ***************************************************************************** */

shared_ptr<std::vector<string>>
                             ConeAngleData::GetSpecificTags() const
{
    return ptrSpecficMissionTags_a;
}

/* ***************************************************************************** */

ConeAngleData::~ConeAngleData()
{
    return;
}
