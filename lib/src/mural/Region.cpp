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
 * Region.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "mural/Region.h"
#include "mural/Earth.h"


#include "general/array/ListIterator.h"
#include "general/array/Array2D.h"
#include "general/math/SpaceVector.h"
#include "general/utility/StringEditor.h"

int             Region::numberOfRegions_s        = 0;
const int       Region::SUB_REGION_DIVIDER_s     = 1000;
bool            Region::usingSubRegions_s        = false;
Array<int>*     Region::ptrAccessLevelArray_s    = NULL;
//Array<int>*   Region::ptrAreaOfInterestArray_s = NULL;
Array<string>*  Region::ptrGeoRegionLabelArray_s = NULL;

Region::Region()
	: regionNumber_a(LONG_ZERO), subRegionNumber_a(SHORT_ZERO),
	northernLatitude_a(DOUBLE_ZERO), southernLatitude_a(DOUBLE_ZERO),
	westernLongitude_a(DOUBLE_ZERO), easternLongitude_a(DOUBLE_ZERO),
	containsLandMass_a(false), geoRegionLabel_a(""),
	revisitRegion_a(true), ptrAccessTierArray_a(NULL), ptrAccessAOIArray_a(NULL)
{
    return;
}

Region::Region(int regionNumber,
               int subRegionNumber,
               double northernLatitude,
               double southernLatitude,
               double westernLongitude,
               double easternLongitude,
               const bool &containsLand,
               const string &geoRegionLabel,
               const bool &revisitRegion,
               Array<int> *ptrAccessTierArray,
	           Array<int> *ptrAccessAOIArray)
    : regionNumber_a(regionNumber), subRegionNumber_a(subRegionNumber),
      northernLatitude_a(northernLatitude), southernLatitude_a(southernLatitude),
      westernLongitude_a(westernLongitude), easternLongitude_a(easternLongitude),
      containsLandMass_a(containsLand), geoRegionLabel_a(geoRegionLabel),
      revisitRegion_a(revisitRegion)
{
	if (ptrAccessTierArray != NULL) {
		ptrAccessTierArray_a = new Array<int>(*ptrAccessTierArray);
	}

	if (ptrAccessAOIArray != NULL) {
		ptrAccessAOIArray_a = new Array<int>(*ptrAccessAOIArray);
	}

    if (regionNumber_a <= 0) {
        throw new InputException(GetClassName(),
                                 "Region(int, const double&, const double&, ...)",
                                 ("Illegal region number " + StringEditor::ConvertToString(regionNumber_a)));
    }

    if ((usingSubRegions_s == true) && (subRegionNumber_a <= 0)) {
        throw new InputException(GetClassName(),
                                 "Region(int, const double&, const double&, ...)",
                                 ("Illegal sub-region number " + StringEditor::ConvertToString(subRegionNumber_a)));
    }

    if (northernLatitude_a <= SOUTH_POLE_DEGREES) {
        throw new InputException(GetClassName(),
                                 "Region(int, const double&, const double&, ...)",
                                 ("Illegal north latitude " + StringEditor::ConvertToString(northernLatitude_a)));
    }

    if (southernLatitude_a >= NORTH_POLE_DEGREES) {
        throw new InputException(GetClassName(),
                                 "Region(int, const double&, const double&, ...)",
                                 ("Illegal south latitude " + StringEditor::ConvertToString(southernLatitude_a)));
    }

    if (northernLatitude_a <= southernLatitude_a) {
        throw new InputException(GetClassName(),
                                 "Region(int, const double&, const double&, ...)",
                                 "North latitude MUST be greater than south latitude");
    }

    if ((westernLongitude_a < -DATE_LINE_DEGREES) || (westernLongitude_a > DATE_LINE_DEGREES)) {
        throw new InputException(GetClassName(),
                                 "Region(int, const double&, const double&, ...)",
                                 ("Illegal west longitude " + StringEditor::ConvertToString(westernLongitude_a)));
    }

    if ((easternLongitude_a < -DATE_LINE_DEGREES) || (easternLongitude_a > DATE_LINE_DEGREES)) {
        throw new InputException(GetClassName(),
                                 "Region(int, const double&, const double&, ...)",
                                 ("Illegal west longitude " + StringEditor::ConvertToString(easternLongitude_a)));
    }
	
	//RAGUSA 11/28/2018 - New way to error check each definition for AOI input (cannot be AOI without Tier Level)
	int numberOfServiceDefinitions = Earth::GetNumberOfServiceTierDefinitions();
	for (int definition = 1; definition <= numberOfServiceDefinitions; ++definition) {
		if ((IsAreaOfInterest(definition) == true) && (GetRegionAccessLevel(definition) <= 0)) {
			throw new InputException(GetClassName(),
				"Region(int, const double&, const double&, ...)",
				("Area Of Interest Region " + StringEditor::ConvertToString(regionNumber_a)
					+ " not assigned access level"));
		}
	}
	//=========================================================================
    if (regionNumber_a > numberOfRegions_s) {
        numberOfRegions_s = regionNumber_a;
    }

	//RAGUSA 11/28/2018 - Create new way to store access level lists (for all definitions)
	for (int definition = 1; definition <= numberOfServiceDefinitions; ++definition) {
		if (GetRegionAccessLevel(definition) > 0) {
			if (ptrAccessLevelArray_s == NULL) {
				ptrAccessLevelArray_s = new Array<int>();
			}
			ptrAccessLevelArray_s->InsertUniqueElement(GetRegionAccessLevel(definition));
		}
	}

    if (geoRegionLabel.size() > 0) {
        if (ptrGeoRegionLabelArray_s == NULL) {
            ptrGeoRegionLabelArray_s = new Array<string>();
        }

        ptrGeoRegionLabelArray_s->AddUniqueElement(geoRegionLabel);
    }

    return;
}

Region::Region(const Region &copyMe)
    : regionNumber_a(copyMe.regionNumber_a),
      subRegionNumber_a(copyMe.subRegionNumber_a),
      northernLatitude_a(copyMe.northernLatitude_a),
      southernLatitude_a(copyMe.southernLatitude_a),
      westernLongitude_a(copyMe.westernLongitude_a),
      easternLongitude_a(copyMe.easternLongitude_a),
      containsLandMass_a(copyMe.containsLandMass_a),
      geoRegionLabel_a(copyMe.geoRegionLabel_a),
      revisitRegion_a(copyMe.revisitRegion_a)
{
    return;
}

Region&
Region::operator = (const Region &copyMe)
{
    regionNumber_a        = copyMe.regionNumber_a;
    subRegionNumber_a     = copyMe.subRegionNumber_a;
    northernLatitude_a    = copyMe.northernLatitude_a;
    southernLatitude_a    = copyMe.southernLatitude_a;
    westernLongitude_a    = copyMe.westernLongitude_a;
    easternLongitude_a    = copyMe.easternLongitude_a;
    containsLandMass_a    = copyMe.containsLandMass_a;
    geoRegionLabel_a      = copyMe.geoRegionLabel_a;
    revisitRegion_a       = copyMe.revisitRegion_a;
    return(*this);
}

bool
Region::operator == (const Region &compareMe) const
{
    return( (regionNumber_a == compareMe.regionNumber_a) &&
            (subRegionNumber_a == compareMe.subRegionNumber_a) &&
            (northernLatitude_a == compareMe.northernLatitude_a) &&
            (southernLatitude_a == compareMe.southernLatitude_a) &&
            (westernLongitude_a == compareMe.westernLongitude_a) &&
            (easternLongitude_a == compareMe.easternLongitude_a) &&
            (containsLandMass_a == compareMe.containsLandMass_a) &&
            (geoRegionLabel_a   == compareMe.geoRegionLabel_a) &&
            (revisitRegion_a    == compareMe.revisitRegion_a));
}

bool
Region::operator != (const Region &compareMe) const
{
    return(!(operator == (compareMe)));
}

SpaceVector
Region::GetRegionCenter(double j2000Time,
                        REFERENCE_FRAME returnFrame) const
{
    const double midLatitude = UnitData::GetInternalAngle((double)GetMiddleLatitude());
    const double midLongitude = UnitData::GetInternalAngle((double)GetMiddleLongitude());
    const double altitude = Earth::GetStoreRadius();
    SpaceVector  regionCenter(midLatitude, midLongitude, altitude, LAT_LON_ALTITUDE);
    return(regionCenter.ConvertTo(returnFrame, j2000Time));
}

double
Region::GetCenterEarthAngle(const SpaceVector &vehiclePosition,
                            double j2000Time) const
{
    return(vehiclePosition.GetSeparationAngle(GetRegionCenter(j2000Time)));
}

double
Region::GetMinimumEarthAngle(const SpaceVector &vehiclePosition,
                             double j2000Time) const
{
    double                minimumEarthAngle = 0.0;
    double                minimumLatitude = 0.0;
    double                minimumLongitude = 0.0;
    const double          altitude = Earth::GetStoreRadius();
    const double          middleLongitude = (double)GetMiddleLongitude();
    const double          vehicleLatitude = vehiclePosition.GetLatitude(DEGREES);
    const double          vehicleLongitude = vehiclePosition.GetLongitude(DEGREES);
    const REFERENCE_FRAME convertFrame = vehiclePosition.GetReferenceFrame();
    SpaceVector           minimumVector;

    if (ContainsLatitude(vehicleLatitude) == true) {
        minimumLatitude = vehicleLatitude;
    } else if (GetMiddleLatitude() >= vehicleLatitude) {
        minimumLatitude = (double)southernLatitude_a;
    } else {
        minimumLatitude = (double)northernLatitude_a;
    }

    if (ContainsLongitude(vehicleLongitude) == true) {
        minimumLongitude = vehicleLongitude;
    } else if ( ((middleLongitude >= 0.0) && (vehicleLongitude >= 0.0)) ||
                ((middleLongitude < 0.0) && (vehicleLongitude < 0.0)) ) {
        if (middleLongitude > vehicleLongitude) {
            minimumLongitude = (double)westernLongitude_a;
        } else {
            minimumLongitude = (double)easternLongitude_a;
        }
    } else if (vehicleLongitude >= 0.0) {
        if (vehicleLongitude > (DATE_LINE_DEGREES / 2.0)) {
            minimumLongitude = (double)westernLongitude_a;
        } else {
            minimumLongitude = (double)easternLongitude_a;
        }
    } else if (middleLongitude < (DATE_LINE_DEGREES / 2.0)) {
        minimumLongitude = (double)westernLongitude_a;
    } else {
        minimumLongitude = (double)easternLongitude_a;
    }

    minimumLatitude = UnitData::GetInternalAngle(minimumLatitude);
    minimumLongitude = UnitData::GetInternalAngle(minimumLongitude);
    minimumVector = SpaceVector(minimumLatitude, minimumLongitude, altitude, LAT_LON_ALTITUDE);
    minimumEarthAngle = vehiclePosition.GetSeparationAngle(minimumVector.ConvertTo(convertFrame, j2000Time));
    return(minimumEarthAngle);
}

double
Region::GetMaximumEarthAngle(const SpaceVector &vehiclePosition,
                             double j2000Time) const
{
    double                maximumEarthAngle = 0.0;
    double                maximumLatitude = 0.0;
    double                maximumLongitude = 0.0;
    const double          altitude = Earth::GetStoreRadius();
    const double          middleLongitude = (double)GetMiddleLongitude();
    const double          vehicleLatitude = vehiclePosition.GetLatitude(DEGREES);
    const double          vehicleLongitude = vehiclePosition.GetLongitude(DEGREES);
    const REFERENCE_FRAME convertFrame = vehiclePosition.GetReferenceFrame();
    SpaceVector           maximumVector;

    if (GetMiddleLatitude() >= vehicleLatitude) {
        maximumLatitude = (double)northernLatitude_a;
    } else {
        maximumLatitude = (double)southernLatitude_a;
    }

    if ( ((middleLongitude >= 0.0) && (vehicleLongitude >= 0.0)) ||
            ((middleLongitude < 0.0) && (vehicleLongitude < 0.0)) ) {
        if (middleLongitude < vehicleLongitude) {
            maximumLongitude = (double)westernLongitude_a;
        } else {
            maximumLongitude = (double)easternLongitude_a;
        }
    } else if (vehicleLongitude >= 0.0) {
        if (vehicleLongitude < (DATE_LINE_DEGREES / 2.0)) {
            maximumLongitude = (double)westernLongitude_a;
        } else {
            maximumLongitude = (double)easternLongitude_a;
        }
    } else if (middleLongitude > (DATE_LINE_DEGREES / 2.0)) {
        maximumLongitude = (double)westernLongitude_a;
    } else {
        maximumLongitude = (double)easternLongitude_a;
    }

    maximumLatitude = UnitData::GetInternalAngle(maximumLatitude);
    maximumLongitude = UnitData::GetInternalAngle(maximumLongitude);
    maximumVector = SpaceVector(maximumLatitude, maximumLongitude, altitude, LAT_LON_ALTITUDE);
    maximumEarthAngle = vehiclePosition.GetSeparationAngle(maximumVector.ConvertTo(convertFrame, j2000Time));
    return(maximumEarthAngle);
}

void
Region::PrintRegionNumber(ostream &outfile,
                          double regionNumber)
{
    if (usingSubRegions_s == false) {
        outfile << setw(4) << (int)regionNumber;
    } else {
        outfile << setw(8) << setprecision(3) << regionNumber;
    }

    return;
}

void
Region::PrintFixedWidthRegionNumber(ostream &outfile,
                                    double regionNumber)
{
    outfile << setw(8);

    if (usingSubRegions_s == false) {
        outfile << (int)regionNumber;
    } else {
        outfile << setprecision(3) << regionNumber;
    }

    return;
}

void
Region::PrintRegionNumber(ostream &outfile) const
{
    if (usingSubRegions_s == false) {
        outfile << setw(4) << regionNumber_a;
    } else {
        outfile << setw(8) << setprecision(3) << GetFullRegionNumber();
    }

    return;
}

void
Region::PrintFixedWidthRegionNumber(ostream &outfile) const
{
    outfile << setw(8);

    if (usingSubRegions_s == false) {
        outfile << regionNumber_a;
    } else {
        outfile << setprecision(3) << GetFullRegionNumber();
    }

    return;
}

void
Region::DestroyRegion()
{
    if (ptrAccessLevelArray_s != NULL) {
        delete ptrAccessLevelArray_s;
        ptrAccessLevelArray_s = NULL;
    }
	/*RAGUSA 11/27/2018 - Don't need 
    if (ptrAreaOfInterestArray_s != NULL) {
        delete ptrAreaOfInterestArray_s;
        ptrAreaOfInterestArray_s = NULL;
    }
    */
    if (ptrGeoRegionLabelArray_s != NULL) {
        delete ptrGeoRegionLabelArray_s;
        ptrGeoRegionLabelArray_s = NULL;
    }

    return;
}

Region::~Region()
{
    --numberOfRegions_s;
    return;
}
