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
 * AccessPattern.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include <algorithm>

#include "AccessPattern.h"
#include "mural/Earth.h"
#include "mural/Region.h"
#include "AxebladePattern.h"
#include "ButterflyPattern.h"
#include "ConeAngleData.h"

// From rageneral
#include "general/utility/Constant.h"
#include "general/array/ListIterator.h"
#include "general/math/SimpleMath.h"
#include "general/math/SpaceVector.h"
#include "general/utility/StringEditor.h"
#include "general/data/UnitData.h"
#include "general/data/InputData.h"
#include "general/data/InputDataElement.h"

AccessPattern::AccessPattern()
    : patternType_a(UNKNOWN_TYPE), alwaysIncludeSubSatelliteRegion_a(false),
      ptrOuterConeAngleData_a(NULL), ptrInnerConeAngleData_a(NULL)
{
    return;
}

AccessPattern::AccessPattern(const AccessPattern &copyMe)
    : patternType_a(copyMe.patternType_a),
      alwaysIncludeSubSatelliteRegion_a(copyMe.alwaysIncludeSubSatelliteRegion_a),
      ptrOuterConeAngleData_a(NULL), ptrInnerConeAngleData_a(NULL)
{
    if (copyMe.ptrOuterConeAngleData_a != NULL) {
        ptrOuterConeAngleData_a = shared_ptr<ConeAngleData>(copyMe.ptrOuterConeAngleData_a);
    }

    if (copyMe.ptrInnerConeAngleData_a != NULL) {
        ptrInnerConeAngleData_a = shared_ptr<ConeAngleData>(copyMe.ptrInnerConeAngleData_a);
    }

    return;
}

AccessPattern&
AccessPattern::operator = (const AccessPattern &copyMe)
{
    patternType_a                  = copyMe.patternType_a;
    alwaysIncludeSubSatelliteRegion_a = copyMe.alwaysIncludeSubSatelliteRegion_a;

    if (copyMe.ptrOuterConeAngleData_a != NULL) {
        ptrOuterConeAngleData_a = shared_ptr<ConeAngleData>(copyMe.ptrOuterConeAngleData_a);
    }

    if (copyMe.ptrInnerConeAngleData_a != NULL) {
        ptrInnerConeAngleData_a = shared_ptr<ConeAngleData>(copyMe.ptrInnerConeAngleData_a);
    }

    return(*this);
}

bool
AccessPattern::operator == (const AccessPattern &compareMe) const
{ 
    return( (patternType_a == compareMe.patternType_a) &&
            (alwaysIncludeSubSatelliteRegion_a == compareMe.alwaysIncludeSubSatelliteRegion_a) &&
            (*ptrOuterConeAngleData_a == *(compareMe.ptrOuterConeAngleData_a)) &&
            (*ptrInnerConeAngleData_a == *(compareMe.ptrInnerConeAngleData_a)));
}

bool
AccessPattern::operator != (const AccessPattern &compareMe) const
{
    return(!(operator == (compareMe)));
}

/* ***************************************************************************** */

void
AccessPattern::PrintAttributes(const shared_ptr<AccessPattern> &accessPattern, ostream &outfile)
{
    double                           coneAngle = 0.0;
    double defaultConeAngle;
    string                           missionString = "";
    shared_ptr<std::vector<string>>  ptrSpecificTags = NULL;
    std::vector<string>::iterator    tagIter;
    // data from the access pattern
    AccessPattern::PATTERN_TYPE      patternType = accessPattern->GetPatternType();
    shared_ptr<ConeAngleData>        ptrOuterConeAngleData(accessPattern->GetOuterConeAngleData());
    shared_ptr<ConeAngleData>        ptrInnerConeAngleData(accessPattern->GetInnerConeAngleData());
    bool alwaysIncludeSubSatelliteRegion = accessPattern->GetAlwaysIncludeSubSatelliteRegion();
    outfile << "      <ACCESS_PATTERN_OBJECT_BEGIN>" << NEW_LINE;
    outfile << "         Access Pattern Type (CONE/DONUT/BFLY/AXE) : ";
    outfile << GetPatternTypeString(patternType) << NEW_LINE;
    // Print the Outer Angles first
    defaultConeAngle = ptrOuterConeAngleData->GetDefaultValue();
    outfile << "         Default Outer Cone Elevation Angle        : ";
    outfile << UnitData::GetInputAngle(defaultConeAngle) << NEW_LINE;
    ptrSpecificTags = ptrOuterConeAngleData->GetSpecificTags();

    for(tagIter = ptrSpecificTags->begin(); tagIter != ptrSpecificTags->end(); ++tagIter) {
        missionString = (*tagIter);
        coneAngle = ptrOuterConeAngleData->GetAngleValue(missionString);
        missionString = InputData::GetSpecificTypeString(missionString);
        outfile << "           Outer Cone Elevation Angle " << missionString;
        outfile << setw(15 - (int)(missionString.size())) << " : ";
        outfile << UnitData::GetInputAngle(coneAngle) << NEW_LINE;
    }

    // Print the Inner Angles first
    defaultConeAngle = ptrInnerConeAngleData->GetDefaultValue();

    if ( defaultConeAngle != UNDEFINED_ANGLE ) {
        outfile << "         Default Inner Cone Elevation Angle        : ";
        outfile << UnitData::GetInputAngle(defaultConeAngle) << NEW_LINE;
        ptrSpecificTags = ptrInnerConeAngleData->GetSpecificTags();

        for(tagIter = ptrSpecificTags->begin(); tagIter != ptrSpecificTags->end(); ++tagIter) {
            missionString = (*tagIter);
            coneAngle = ptrInnerConeAngleData->GetAngleValue(missionString);
            missionString = InputData::GetSpecificTypeString(missionString);
            outfile << "           Inner Cone Elevation Angle " << missionString;
            outfile << setw(15 - (int)(missionString.size())) << " : ";
            outfile << UnitData::GetInputAngle(coneAngle) << NEW_LINE;
        }
    }

    // Specific pattern attributes
    if (patternType == AccessPattern::AXEBLADE ) {
        shared_ptr<AxebladePattern> axebladePattern(dynamic_pointer_cast<AxebladePattern>(accessPattern));
        outfile << "         Broadside Angle (Squint or Doppler Cone)  : ";
        outfile << UnitData::GetInputAngle(axebladePattern->GetBroadSideAngle()) << NEW_LINE;
    } else if (patternType == AccessPattern::BUTTERFLY ) {
        shared_ptr<ButterflyPattern> butterflyPattern(dynamic_pointer_cast<ButterflyPattern>(accessPattern));
        outfile << "         Broadside Angle (Squint or Doppler Cone)  : ";
        outfile << UnitData::GetInputAngle(butterflyPattern->GetBroadSideAngle()) << NEW_LINE;
    }

    outfile << "         Always Include Sub Satellite Region       : ";
    outfile << StringEditor::ConvertToString(alwaysIncludeSubSatelliteRegion) << NEW_LINE;
    outfile << "      <ACCESS_PATTERN_OBJECT_STOP>" << NEW_LINE;
    return;
}

/* ***************************************************************************** */

LinkList<Region>*
AccessPattern::GetRegionsInTheCone(const SpaceVector &vehiclePosition,
                                   double radiusInDegrees,
                                   double maximumEarthAngle) const
{
    const double       currentLatitude = vehiclePosition.GetLatitude(DEGREES);
    double             upperLatitude = currentLatitude + radiusInDegrees;
    double             lowerLatitude = currentLatitude - radiusInDegrees;
    LinkList<Region> *ptrRegionList = new LinkList<Region>();

    try {
        if (upperLatitude > NORTH_POLE_DEGREES) {
            upperLatitude = (2.0 * NORTH_POLE_DEGREES) - upperLatitude;
            Earth::AddAllRegions(ptrRegionList, NORTH_POLE_DEGREES, upperLatitude);
        } else if (lowerLatitude < SOUTH_POLE_DEGREES) {
            lowerLatitude = (2.0 * SOUTH_POLE_DEGREES) - lowerLatitude;
            Earth::AddAllRegions(ptrRegionList, lowerLatitude, SOUTH_POLE_DEGREES);
        }

        Earth::AddRegions(ptrRegionList, upperLatitude, lowerLatitude,
                          vehiclePosition, maximumEarthAngle);
    } catch (Exception *ptrError) {
        ptrRegionList->RemoveAll();
        delete ptrRegionList;
        ptrRegionList = NULL;
        ptrError->AddMethod(GetClassName(),
                            "GetRegionsInTheCone(const SpaceVector&, const double&, const double&, int)");
        throw;
    }

    return(ptrRegionList);
}

/* ***************************************************************************** */

double
AccessPattern::CalculateSquintAngle(const SpaceVector &vehicleToTargetLOS,
                                    const SpaceVector &vehiclePosition,
                                    const SpaceVector &vehicleVelocity) const
{
    double squintAngle;
    const SpaceVector intrackNormal = vehicleToTargetLOS.CrossProduct(vehiclePosition.Negate());
    const SpaceVector velocityNormal = vehicleVelocity.CrossProduct(vehiclePosition.Negate());
    squintAngle = velocityNormal.GetSeparationAngle(intrackNormal);
    return(squintAngle);
}

/* ***************************************************************************** */

double
AccessPattern::GetOuterConeElevationAngle(const PATTERN &pattern,
        const string &missionString) const
{
    double                      outerConeAngle = UNDEFINED_ANGLE;

    if (pattern == MISSION_SPECIFIC) {
        if (StringEditor::IsEmpty(&missionString)) {
            throw new InputException(GetClassName(),
                                     "GetOuterConeElevationAngle(const PATTERN&, int)"
                                     " Must input mission index for MISSION_SPECIFIC pattern...Mission: " + missionString);
        }

        outerConeAngle = ptrOuterConeAngleData_a->GetAngleValue(missionString);
    } else if (pattern == REVISIT) {
        outerConeAngle = ptrOuterConeAngleData_a->GetAngleValue(REVISIT_STRING);
    } else if (pattern == LARGEST_PATTERN) {
        // smallest angle yields the largest outer cone
        outerConeAngle = ptrOuterConeAngleData_a->GetMinimumAngle();
    }//RAGUSA 1/9/2018 - Adding CRISIS Mission access patterns for Crisis Region Mode
    else if (pattern == CRISIS_PATTERN) {
        if (ptrOuterConeAngleData_a->HasSpecificMission(missionString) == true) {
            outerConeAngle = ptrOuterConeAngleData_a->GetAngleValue(missionString);
        } else {
            outerConeAngle = UNDEFINED_ANGLE;
        }
    }//RAGUSA 1/10/2018 - Need to add default pattern when in Crisis Region Mode,
    //Used to always use LARGEST_PATTERN
    else if (pattern == DEFAULT_PATTERN) {
        outerConeAngle = ptrOuterConeAngleData_a->GetDefaultValue();
    }

    return(outerConeAngle);
}

/* ***************************************************************************** */

double
AccessPattern::GetOuterConeElevationAngle(const string &missionString) const
{
    double                      outerConeAngle = UNDEFINED_ANGLE;

    if ( ptrOuterConeAngleData_a->HasSpecificMission(missionString) == true ) {
        outerConeAngle = ptrOuterConeAngleData_a->GetAngleValue(missionString);
    }

    return(outerConeAngle);
}

/* ***************************************************************************** */

double
AccessPattern::GetInnerConeElevationAngle(const PATTERN &pattern,
        const string &missionString) const
{
    double                      innerConeAngle = UNDEFINED_ANGLE;

    if (pattern == MISSION_SPECIFIC) {
        if (StringEditor::IsEmpty(&missionString)) {
            throw new InputException(GetClassName(),
                                     "GetInnerConeElevationAngle(const PATTERN&, int)"
                                     "Must input mission index for MISSION_SPECIFIC pattern...Mission: "+ missionString);
        }

        innerConeAngle = ptrInnerConeAngleData_a->GetAngleValue(missionString);
    } else if (pattern == REVISIT) {
        innerConeAngle = ptrInnerConeAngleData_a->GetAngleValue(REVISIT_STRING);
    } else if (pattern == LARGEST_PATTERN) {
        // largest angle yields the smallest inner cone
        innerConeAngle = ptrInnerConeAngleData_a->GetMaximumAngle();
    }//RAGUSA 1/9/2018 - Adding CRISIS Mission access patterns for Crisis Region Mode
    else if (pattern == CRISIS_PATTERN) {
        if (ptrInnerConeAngleData_a->HasSpecificMission(missionString) == true) {
            innerConeAngle = ptrInnerConeAngleData_a->GetAngleValue(missionString);
        } else {
            innerConeAngle = UNDEFINED_ANGLE;
        }
    }//RAGUSA 1/10/2018 - Need to add default pattern when in Crisis Region Mode
    //Used to always use LARGEST_PATTERN
    else if (pattern == DEFAULT_PATTERN) {
        innerConeAngle = ptrInnerConeAngleData_a->GetDefaultValue();
    }

    return(innerConeAngle);
}

/* ***************************************************************************** */

double
AccessPattern::GetInnerConeElevationAngle(const string &missionString) const
{
    double                      innerConeAngle = UNDEFINED_ANGLE;

    if (ptrInnerConeAngleData_a->HasSpecificMission(missionString)) {
        innerConeAngle = ptrInnerConeAngleData_a->GetAngleValue(missionString);
    }

    return(innerConeAngle);
}

/* ***************************************************************************** */

bool
AccessPattern::MissionSpecificEqualsDefault(const string &missionString) const
{
    bool specificEqualsDefault = false;

    if (GetOuterConeElevationAngle(DEFAULT_PATTERN)
            == GetOuterConeElevationAngle(MISSION_SPECIFIC, missionString)) {
        if (GetInnerConeElevationAngle(DEFAULT_PATTERN)
                == GetInnerConeElevationAngle(MISSION_SPECIFIC, missionString)) {
            specificEqualsDefault = true;
        }
    }

    return(specificEqualsDefault);
}

/* ***************************************************************************** */

bool
AccessPattern::MissionSpecificEqualsLargest(const string &missionString) const
{
    bool specificEqualsLargest = false;

    if (GetOuterConeElevationAngle(LARGEST_PATTERN)
            == GetOuterConeElevationAngle(MISSION_SPECIFIC, missionString)) {
        if (GetInnerConeElevationAngle(LARGEST_PATTERN)
                == GetInnerConeElevationAngle(MISSION_SPECIFIC, missionString)) {
            specificEqualsLargest = true;
        }
    }

    return(specificEqualsLargest);
}

/* ***************************************************************************** */

string
AccessPattern::GetPatternTypeString(const PATTERN_TYPE &patternType)
{
    string patternString = "UNKNOWN";

    if (patternType == VISIBILITY_CONE) {
        patternString = "CONE";
    } else if (patternType == DOUGHNUT) {
        patternString = "DONUT";
    } else if (patternType == BUTTERFLY) {
        patternString = "BFLY";
    } else if (patternType == AXEBLADE) {
        patternString = "AXE";
    }

    return(patternString);
}

/* ***************************************************************************** */

AccessPattern::PATTERN_TYPE
AccessPattern::GetPatternTypeFromString(const string &inputString)
{
    PATTERN_TYPE returnPattern = AccessPattern::UNKNOWN_TYPE;

    if (inputString == "CONE") {
        returnPattern = AccessPattern::VISIBILITY_CONE;
    } else if (inputString == "DONUT") {
        returnPattern = AccessPattern::DOUGHNUT;
    } else if (inputString == "BFLY") {
        returnPattern = AccessPattern::BUTTERFLY;
    } else if (inputString == "AXE") {
        returnPattern = AccessPattern::AXEBLADE;
    }

    return(returnPattern);
}

/* ***************************************************************************** */

void
AccessPattern::SetPatternType(PATTERN_TYPE patternType)
{
    patternType_a = patternType;
}

/* ***************************************************************************** */

void
AccessPattern::SetAlwaysIncludeSubSatelliteRegion(bool alwaysInclude)
{
    alwaysIncludeSubSatelliteRegion_a = alwaysInclude;
}

/* ***************************************************************************** */

shared_ptr<ConeAngleData>
AccessPattern::GetOuterConeAngleData() const
{
    return ptrOuterConeAngleData_a;
}

/* ***************************************************************************** */
shared_ptr<ConeAngleData>
AccessPattern::GetInnerConeAngleData() const
{
    return ptrInnerConeAngleData_a;
}

/* ***************************************************************************** */

bool
AccessPattern::GetAlwaysIncludeSubSatelliteRegion() const
{
    return alwaysIncludeSubSatelliteRegion_a;
}

/* ***************************************************************************** */

void
AccessPattern::SetOuterConeAngleData(const shared_ptr<ConeAngleData> &coneAngleData)
{
    ptrOuterConeAngleData_a = coneAngleData;
}

/* ***************************************************************************** */
void
AccessPattern::SetInnerConeAngleData(const shared_ptr<ConeAngleData> &coneAngleData)
{
    ptrInnerConeAngleData_a = coneAngleData;
}

/* ***************************************************************************** */

AccessPattern::PATTERN_TYPE
AccessPattern::GetPatternType() const
{
    return(patternType_a);
}

/* ***************************************************************************** */

AccessPattern::~AccessPattern()
{
    return;
}
