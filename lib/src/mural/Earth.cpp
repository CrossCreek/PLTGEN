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
 * Earth.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "mural/Region.h"
#include "mural/Earth.h"

#include "general/data/InputData.h"
#include "general/data/InputDataElement.h"
#include "general/utility/Constant.h"
#include "general/exception/InputException.h"
#include "general/utility/StringEditor.h"
#include "general/math/SimpleMath.h"
#include "general/math/SpaceVector.h"
#include "general/time/TimePiece.h"
#include "io\FileNames.h"

Array<double>*             Earth::ptrLowestRegionNumberArray_s  = NULL;
Array<double>*             Earth::ptrHighestRegionNumberArray_s = NULL;
int                      Earth::latitudeDivisionsPerRegion_s  = 1;
int                      Earth::longitudeDivisionsPerRegion_s = 1;
//RAGUSA 11/27/2018 - Adding User Specific Tier Definitions, default input will be 1 (cannot be 0)
int                       Earth::numberOfServiceTierDefinitions_s = 1;
ArrayOfLinkLists<Region>* Earth::ptrRegionLatitudeBinList_s = NULL;

Earth::Earth()
{
    return;
}

void
Earth::CreateRegions(shared_ptr<InputDataElement> ptrInputDataElement)
{
    int                    subRegionNumber = 0;
    int                    regionNumber = 0;
    int                    numberOfLatitudeBins = 0;
    int                    latitudeBin = 0;
    int                    longitudeBin = 0;
    int                    arrayIndex = 0;
    int                    numberOfValues = 0;
    const int              northIndex    = 0;
    const int              southIndex    = 1;
    const int              westIndex     = 2;
    const int              eastIndex     = 3;
    const int              containsIndex = 4;
    const int              labelIndex    = 5;
    const int              revisitIndex  = 6;
	//RAGUSA 11/27/2018 - Needed for parsing definition columns 
	Array<int>             *ptrAccessTierArray = new Array<int>();
	Array<int>             *ptrAccessAOIArray = new Array<int>();
	const int              accessIndexStart = 8;
	int                    accessIndexEnd = accessIndexStart;
	//=========================================================
    double                   northLatitude = 0.0;
    double                   southLatitude = 0.0;
    double                   westLongitude = 0.0;
    double                   eastLongitude = 0.0;
    double                   middleLatitude = 0.0;
    double                   deltaLatitude = 0.0;
    double                   deltaLongitude = 0.0;
    double                   currentWest = 0.0;
    double                   currentEast = 0.0;
    double                   fullRegionNumber = 0.0;
    Array<double>           *ptrLatitudeBinArray = new Array<double>();
    bool                    containsLand = false;
	bool                    accessMetricsIO = false;
    bool                    revisitRegion = true; //assume region is used in revisit
    const bool              usingSubRegions = Region::UsingSubRegions();
    string                  geoRegionLabel = "";
    Region            *ptrRegion = NULL;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>              ptrInputData = NULL;

	if (usingSubRegions == true) {
		// need to find the number of divisions before creating Region classes
		for (inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
			ptrInputData = (*inputDataIter);

			if (ptrInputData->LabelContainsString("Number Of Latitude Divisions") == true) {
				latitudeDivisionsPerRegion_s = ptrInputData->GetIntValue();
			}
			else if (ptrInputData->LabelContainsString("Number Of Longitude Divisions") == true) {
				longitudeDivisionsPerRegion_s = ptrInputData->GetIntValue();
			}

			if (latitudeDivisionsPerRegion_s <= 0) {
				//CPPCHECK_FIX Error - memory leak
				delete ptrLatitudeBinArray;
				ptrLatitudeBinArray = NULL;
				throw new InputException(GetClassName(), "CreateRegions(InputDataElement*)",
					"Missing/invalid 'Number Of Latitude Divisions' input");
			}

			if (longitudeDivisionsPerRegion_s <= 0) {
				throw new InputException(GetClassName(), "CreateRegions(InputDataElement*)",
					"Missing/invalid 'Number Of Longitude Divisions' input");
			}
			if (longitudeDivisionsPerRegion_s <= 0) {
				throw new InputException(GetClassName(), "CreateRegions(InputDataElement*)",
					"Missing/invalid 'Number Of Longitude Divisions' input");
			}
		}
	}

	//RAGUSA 11/27/2018 - Adding User Specific Service Tier Definitions to MURAL. This is the input to
	//specify how many definitions will be read in for this run. Each service definition has 2 columns each:
    //one for accessLevel and one for AOI true/false (1/0)
		for (inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
			ptrInputData = (*inputDataIter);
			if (ptrInputData->LabelContainsString("Number Of Service Tier Definitions") == true) {
				numberOfServiceTierDefinitions_s = ptrInputData->GetIntValue();
			}

			if (numberOfServiceTierDefinitions_s <= 0) {
				throw new InputException(GetClassName(), "CreateRegions(InputDataElement*)",
					"Missing/invalid 'Number Of Service Tier Definitions' input");
			}
		}
		accessIndexEnd = accessIndexStart + 2 * numberOfServiceTierDefinitions_s - 1;
	//=================================================================================================

    ptrLatitudeBinArray->SetOrderToDecreasing();

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if ( (ptrInputData->LabelContainsString("Region") == true) &&
                (ptrInputData->GetSpecificIntNumber(regionNumber) == true) ) {
			//RAGUSA 11/27/2018
            if (ptrInputData->GetNumberOfValues() <= accessIndexEnd) {
                throw new InputException(GetClassName(), "CreateRegions(InputDataElement*)",
                                         ("Missing data for Region " + StringEditor::ConvertToString(regionNumber)));
            }

            northLatitude = ptrInputData->GetDoubleValue(northIndex);
            deltaLatitude = (northLatitude - ptrInputData->GetDoubleValue(southIndex))
                            / (double)latitudeDivisionsPerRegion_s;

            if (deltaLatitude <= 0.0) {
                throw new InputException(GetClassName(), "CreateRegions(InputDataElement*)",
                                         ("Invalid Latitudes for Region " + StringEditor::ConvertToString(regionNumber)));
            }

            middleLatitude = northLatitude - (deltaLatitude / 2.0);
            latitudeBin = 0;

            while (latitudeBin < latitudeDivisionsPerRegion_s) {
                ptrLatitudeBinArray->InsertUniqueElement(middleLatitude);
                middleLatitude -= deltaLatitude;
                ++latitudeBin;
            }
        } 
		/*RAGUSA 11/27/2018 - Moving Area of Interest Input to column input method
		//DEPRECATED CODE 
		else if ( (ptrInputData->LabelContainsString("Area Of Interest") == true) &&
                    ((numberOfValues = ptrInputData->GetNumberOfValues()) > 0) ) {
            arrayIndex = 0;

            while (arrayIndex < numberOfValues) {
                Region::SetAreaOfInterest(ptrInputData->GetIntValue(arrayIndex));
                ++arrayIndex;
            }
        }*/
    }

    numberOfLatitudeBins = ptrLatitudeBinArray->Size();

    if (numberOfLatitudeBins == 0) {
        delete ptrLatitudeBinArray;
        ptrLatitudeBinArray = NULL;
        throw new InputException(GetClassName(), "CreateRegions(InputDataElement*)",
                                 "No regions were found");
    }

    ptrRegionLatitudeBinList_s = new ArrayOfLinkLists<Region>(numberOfLatitudeBins);
    ptrLowestRegionNumberArray_s = new Array<double>(numberOfLatitudeBins, FLT_MAX);
    ptrHighestRegionNumberArray_s = new Array<double>(numberOfLatitudeBins, 0);

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

		if ((ptrInputData->LabelContainsString("Region") == true) &&
			(ptrInputData->GetSpecificIntNumber(regionNumber) == true)) {
			northLatitude = ptrInputData->GetDoubleValue(northIndex);
			westLongitude = ptrInputData->GetDoubleValue(westIndex);
			eastLongitude = ptrInputData->GetDoubleValue(eastIndex);
			containsLand = ptrInputData->GetBoolValue(containsIndex);
			geoRegionLabel = ptrInputData->GetStringValue(labelIndex);
			revisitRegion = ptrInputData->GetBoolValue(revisitIndex);
			//RAGUSA 11/27/2018 - Need to read in columns for tier defintions (AOI is second column of each pair)
			for (int accessIndex = accessIndexStart; accessIndex <= accessIndexEnd - 1; accessIndex += 2) {
				ptrAccessTierArray->AddElement(ptrInputData->GetIntValue(accessIndex));
				ptrAccessAOIArray->AddElement(ptrInputData->GetIntValue(accessIndex + 1));
				//Checking AOI input, can only be 0 or 1
				if ((ptrAccessAOIArray->LastElement() != 1) && (ptrAccessAOIArray->LastElement() != 0)) {
					delete ptrAccessTierArray;
					ptrAccessTierArray = NULL;
					delete ptrAccessAOIArray;
					ptrAccessAOIArray = NULL;
					throw new InputException(GetClassName(), "CreateRegions(InputDataElement*)",
						("Invalid AOI input, must be equal to 0 or 1. Occurs for Region: " + StringEditor::ConvertToString(regionNumber)));
				}
			}
			//===============================================================
            deltaLatitude = (northLatitude - ptrInputData->GetDoubleValue(southIndex))
                            / (double)latitudeDivisionsPerRegion_s;

            if (westLongitude < eastLongitude) {
                deltaLongitude = (eastLongitude - westLongitude)
                                 / (double)longitudeDivisionsPerRegion_s;
            } else {
                deltaLongitude = (360.0 + eastLongitude - westLongitude)
                                 / (double)longitudeDivisionsPerRegion_s;
            }

            if (deltaLongitude <= 0.0) {
                throw new InputException(GetClassName(), "CreateRegions(InputDataElement*)",
                                         ("Invalid Longitudes for Region " + StringEditor::ConvertToString(regionNumber)));
            }

            subRegionNumber = 0;
            middleLatitude = northLatitude - (deltaLatitude / 2.0);
            latitudeBin = 0;

            while (latitudeBin < latitudeDivisionsPerRegion_s) {
                southLatitude = northLatitude - deltaLatitude;
                arrayIndex = ptrLatitudeBinArray->FindIndex(middleLatitude);
                longitudeBin = 0;

                while (longitudeBin < longitudeDivisionsPerRegion_s) {
                    if (usingSubRegions == true) {
                        ++subRegionNumber;
                    }

                    currentWest = westLongitude + ((double)longitudeBin * deltaLongitude);

                    if (currentWest > DATE_LINE_DEGREES) {
                        currentWest -= 360.0;
                    }

                    currentEast = currentWest + deltaLongitude;

                    if (currentEast > DATE_LINE_DEGREES) {
                        currentEast -= 360.0;
                    }

                    ptrRegion = new Region(regionNumber, subRegionNumber,
                                           northLatitude, southLatitude, currentWest,
                                           currentEast, containsLand, geoRegionLabel,
                                           revisitRegion, ptrAccessTierArray, ptrAccessAOIArray);
                    fullRegionNumber = ptrRegion->GetFullRegionNumber();
                    // Add 180 to the sort value, because the Append function on the LinkList can
                    // not sort negative values
                    ptrRegionLatitudeBinList_s->AddObject(ptrRegion, arrayIndex, (int)(westLongitude + 180.0));
                    ptrRegion = NULL;

                    if (fullRegionNumber < ptrLowestRegionNumberArray_s->ElementAt(arrayIndex)) {
                        (*ptrLowestRegionNumberArray_s)[arrayIndex] = fullRegionNumber;
                    }

                    if (fullRegionNumber > ptrHighestRegionNumberArray_s->ElementAt(arrayIndex)) {
                        (*ptrHighestRegionNumberArray_s)[arrayIndex] = fullRegionNumber;
                    }

                    ++longitudeBin;
                }

                northLatitude -= deltaLatitude;
                middleLatitude -= deltaLatitude;
                ++latitudeBin;

				//RAGUSA 12/3/2018 - Need to reset for next region
				ptrAccessTierArray->Delete();
				ptrAccessAOIArray->Delete();
            }
        }
    }

    delete ptrLatitudeBinArray;
    ptrLatitudeBinArray = NULL;

	delete ptrAccessTierArray;
	ptrAccessTierArray = NULL;

	delete ptrAccessAOIArray;
	ptrAccessAOIArray = NULL;
    return;
}

/* **************************************************************************************** */

void
Earth::AddRegions(LinkList<Region> *ptrRegionList,
                  double upperLatitude,
                  double lowerLatitude,
                  const SpaceVector &vehiclePosition,
                  double maximumEarthAngle)
{
    int        latitudeBin = 0;
    const int  numberOfLatitudeBins = ptrRegionLatitudeBinList_s->Size();
    bool        withinLatitudeRange = false;
    bool        foundViableRegion = false;
    bool        firstRegionIsViable = false;
    Region     *ptrRegion = NULL;
    Region     *ptrFirstRegion = NULL;

    while (latitudeBin < numberOfLatitudeBins) {
        if ( ((ptrRegionLatitudeBinList_s->GetFirstObject(latitudeBin))->GetSouthernLatitude() <= upperLatitude) &&
                ((ptrRegionLatitudeBinList_s->GetFirstObject(latitudeBin))->GetNorthernLatitude() >= lowerLatitude) ) {
            ListIterator<Region> regionListIter(ptrRegionLatitudeBinList_s->LinkListAt(latitudeBin));
            ptrFirstRegion = regionListIter.First();
            withinLatitudeRange = true;
            foundViableRegion = false;
            firstRegionIsViable = false;
            ptrRegion = ptrFirstRegion;

            while (ptrRegion != NULL) {
                if (ptrRegion->GetCenterEarthAngle(vehiclePosition) <= maximumEarthAngle) {
                    if (foundViableRegion == false) {
                        foundViableRegion = true;

                        if (ptrRegion == ptrFirstRegion) {
                            firstRegionIsViable = true;
                        }
                    }

                    ptrRegionList->Append(ptrRegion, ptrRegion->GetRegionNumber());
                    ptrRegion = regionListIter.Next();
                } else if (foundViableRegion == true) {
                    ptrRegion = NULL;
                } else {
                    ptrRegion = regionListIter.Next();
                }
            }

            // if first Region in list going west to east was viable, check from the back of list
            // forward (i.e. east to west) for cones that span the international date line
            ptrRegion = regionListIter.Last();

            if ((firstRegionIsViable == true) && (ptrRegion != NULL)) {
                // check that the east-most Region is not already on the list
                if (ptrRegionList->ContainsObject(ptrRegion) == false) {
                    while (ptrRegion != NULL) {
                        if (ptrRegion->GetCenterEarthAngle(vehiclePosition) <= maximumEarthAngle) {
                            ptrRegionList->Append(ptrRegion, ptrRegion->GetRegionNumber());
                            ptrRegion = regionListIter.Previous();
                        } else {
                            ptrRegion = NULL;
                        }
                    }
                }
            }
        } else if (withinLatitudeRange == true) {
            latitudeBin = numberOfLatitudeBins;
        }

        ++latitudeBin;
    }

    return;
}

void
Earth::AddAllRegions(LinkList<Region> *ptrRegionList,
                     double upperLatitude,
                     double lowerLatitude)
{
    int        latitudeBin = 0;
    const int  numberOfLatitudeBins = ptrRegionLatitudeBinList_s->Size();
    bool        withinLatitudeRange = false;
    Region     *ptrRegion = NULL;

    while (latitudeBin < numberOfLatitudeBins) {
        ListIterator<Region> regionListIter(ptrRegionLatitudeBinList_s->LinkListAt(latitudeBin));
        ptrRegion = regionListIter.First();

        if ( (ptrRegion->GetSouthernLatitude() <= upperLatitude) &&
                (ptrRegion->GetNorthernLatitude() >= lowerLatitude) ) {
            withinLatitudeRange = true;

            while (ptrRegion != NULL) {
                ptrRegionList->Append(ptrRegion, ptrRegion->GetRegionNumber());
                ptrRegion = regionListIter.Next();
            }
        } else if (withinLatitudeRange == true) {
            latitudeBin = numberOfLatitudeBins;
        }

        ++latitudeBin;
    }

    return;
}

Region*
Earth::GetRegion(double latitude,
                 double longitude)
{
    bool                 foundRegion = false;
    ListIterator<Region> latitudeListIter(GetRegionLatitudeList(latitude));
    Region              *ptrRegion = latitudeListIter.First();

    while ((ptrRegion != NULL) && (foundRegion == false)) {
        foundRegion = ptrRegion->ContainsLongitude(longitude);

        if (foundRegion == false) {
            ptrRegion = latitudeListIter.Next();
        }
    }

    return(ptrRegion);
}

Region*
Earth::GetRegion(const SpaceVector &inputVector)
{
    const double latitude = inputVector.GetLatitude(DEGREES);
    const double longitude = inputVector.GetLongitude(DEGREES);
    return(GetRegion(latitude, longitude));
}

Region*
Earth::GetRegion(double fullRegionNumber)
{
    int        latitudeBin = 0;
    const int  numberOfLatitudeBins = ptrRegionLatitudeBinList_s->Size();
    bool        foundRegion = false;
    Region     *ptrRegion = NULL;

    while ((latitudeBin < numberOfLatitudeBins) && (foundRegion == false)) {
        if ( (fullRegionNumber >= ptrLowestRegionNumberArray_s->ElementAt(latitudeBin)) &&
                (fullRegionNumber <= ptrHighestRegionNumberArray_s->ElementAt(latitudeBin)) ) {
            ListIterator<Region> regionListIter(ptrRegionLatitudeBinList_s->LinkListAt(latitudeBin));
            ptrRegion = regionListIter.First();

            while ((ptrRegion != NULL) && (foundRegion == false)) {
                if (ptrRegion->GetFullRegionNumber() == fullRegionNumber) {
                    foundRegion = true;
                } else {
                    ptrRegion = regionListIter.Next();
                }
            }
        }

        ++latitudeBin;
    }

    return(ptrRegion);
}

Region*
Earth::GetRegion(int regionNumber,
                 int subRegionNumber)
{
    return(GetRegion(Region::GetFullRegionNumber(regionNumber, subRegionNumber)));
}

int
Earth::GetAccessLevel(int regionNumber, int serviceDefinition,
                      int subRegionNumber)
{
    int   accessLevel = 0;
    Region *ptrRegion = GetRegion(regionNumber, subRegionNumber);

    if (ptrRegion != NULL) {
		//RAGUSA 11/27/2018
        accessLevel = ptrRegion->GetRegionAccessLevel(serviceDefinition);
    }

    return(accessLevel);
}

double
Earth::GetRadius(const DISTANCE_UNIT &distanceUnits)
{
    return(SimpleMath::ConvertDistance(RADIUS_NMI, NAUTICAL_MILES, distanceUnits));
}

double
Earth::GetStoreRadius()
{
    return(GetRadius(UnitData::GetInternalDistanceUnits()));
}

double
Earth::GetMU(const DISTANCE_UNIT &distanceUnits,
             const TIME_UNIT &timeUnits)
{
    const int distanceExponent = 3;
    const int timeExponent = 2;
    double     returnMU = MU_NMI_SEC;

    if (distanceUnits != NAUTICAL_MILES) {
        double distanceCoefficient;
        distanceCoefficient = SimpleMath::ConvertDistance(1.0, NAUTICAL_MILES,
                              UnitData::GetInternalDistanceUnits());
        returnMU *= SimpleMath::Power(distanceCoefficient, distanceExponent);
    }

    if (timeUnits != SECONDS) {
        double timeCoefficient;
        timeCoefficient = TimePiece::ConvertTime(1.0, SECONDS,
                          UnitData::GetInternalTimeUnits());
        returnMU /= SimpleMath::Power(timeCoefficient, timeExponent);
    }

    return(returnMU);
}

double
Earth::GetStoreMU()
{
    return(GetMU(UnitData::GetInternalDistanceUnits(),
                 UnitData::GetInternalTimeUnits()));
}

double
Earth::GetPeriod(const TIME_UNIT &timeUnits)
{
    return(TimePiece::ConvertTime(PERIOD_SEC, SECONDS, timeUnits));
}

double
Earth::GetStorePeriod()
{
    return(GetPeriod(UnitData::GetInternalTimeUnits()));
}

const LinkList<Region>*
Earth::GetRegionLatitudeBinList(int index)
{
    const LinkList<Region> *ptrReturnList = NULL;

    if (ptrRegionLatitudeBinList_s != NULL) {
        if (index < ptrRegionLatitudeBinList_s->Size()) {
            ptrReturnList = ptrRegionLatitudeBinList_s->LinkListAt(index);
        }
    }

    return(ptrReturnList);
}

const LinkList<Region>*
Earth::GetRegionLatitudeList(double latitude)
{
    int                    latitudeBin = 0;
    const int              numberOfLatitudeBins = ptrRegionLatitudeBinList_s->Size();
    const LinkList<Region> *ptrReturnList = NULL;

    while ((latitudeBin < numberOfLatitudeBins) && (ptrReturnList == NULL)) {
        if ( (latitude <= (ptrRegionLatitudeBinList_s->GetFirstObject(latitudeBin))->GetNorthernLatitude()) &&
                (latitude >= (ptrRegionLatitudeBinList_s->GetFirstObject(latitudeBin))->GetSouthernLatitude()) ) {
            ptrReturnList = ptrRegionLatitudeBinList_s->LinkListAt(latitudeBin);
        } else {
            ++latitudeBin;
        }
    }

    return(ptrReturnList);
}

void
Earth::DestroyRegions()
{
    if (ptrRegionLatitudeBinList_s != NULL) {
        ptrRegionLatitudeBinList_s->Delete();
        delete ptrRegionLatitudeBinList_s;
        ptrRegionLatitudeBinList_s = NULL;
    }

    if (ptrLowestRegionNumberArray_s != NULL) {
        delete ptrLowestRegionNumberArray_s;
        ptrLowestRegionNumberArray_s = NULL;
    }

    if (ptrHighestRegionNumberArray_s != NULL) {
        delete ptrHighestRegionNumberArray_s;
        ptrHighestRegionNumberArray_s = NULL;
    }

    Region::DestroyRegion();
    return;
}

Earth::~Earth()
{
    return;
}
