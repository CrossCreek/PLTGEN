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
 * Antenna.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include <algorithm>

#include "Antenna.h"
#include "mural/io/InputProcessor.h"
#include "mural/io/MURALInputProcessor.h"
#include "mural/element/Element.h"
#include "mural/element/DataProvider.h"
#include "mural/modules/lg/AzimuthAngleData.h"
#include "mural/modules/lg/LinkConstraintData.h"
#include "mural/coorframe/AntennaCoordinateFrame.h"

#include "general/utility/StringEditor.h"
#include "general/utility/Utility.h"
#include "general/time/TimePiece.h"
#include "general/exception/InputException.h"
#include "general/math/CoordinateFrame.h"
#include "general/math/SimpleMath.h"
#include "general/array/ListIterator.h"
#include "general\utility\Utility.h"

const char Antenna::ASSIGNMENT_INTERVAL_ID_s = 'a';

//------------------------------------------------------------------------------
//
//  Operation Name: Antenna::Antenna
//
//  Purpose:
//     This is the alternate constructor
//
//  Calling Operation:
//     UserAntenna::UserAntenna, RelayAntenna::RelayAntenna, or
//     ReceiveFacilityAntenna::ReceiveFacilityAntenna
//
//  Calling Parameters
//  Parameters        i/o/r  description
//  ----------        -----  -----------
//  numberOfAntenna     i    the capacity to be set
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------
Antenna::Antenna()
	: AntennaBase(), frequencyBand_a(UNKNOWN_BAND), capacity_a(0), ptrCapacityArray_a(NULL),
	ptrAllocatedAssetArray_a(NULL), preferencedCapacity_a(0),
	previousCapacity_a(0), preparationTime_a(0),
	dataRate_a(0.00),
	minimumGimbalStopAngle_a(UNDEFINED_ANGLE),
	maximumGimbalStopAngle_a(UNDEFINED_ANGLE),
	ptrDedicatedTransmitterArray_a(NULL), ptrPrecludedReceiverArray_a(NULL),
	ptrAzimuthAngleDataList_a(NULL), ptrLinksList_a(NULL)
{
    // Init the link constraint data unique pointer
    ptrLinkConstraintData_a = unique_ptr<DefaultedData<MURALTypes::LinkConstraintDataPtr>>(
                                  new DefaultedData<MURALTypes::LinkConstraintDataPtr>()
                              );
    return;
}

Antenna::Antenna(shared_ptr<Element> ptrElement,
	int previousCapacity)
	: AntennaBase(ptrElement), frequencyBand_a(UNKNOWN_BAND), capacity_a(0), ptrCapacityArray_a(NULL),
	ptrAllocatedAssetArray_a(NULL), preferencedCapacity_a(0),
	previousCapacity_a(previousCapacity), preparationTime_a(0),
	dataRate_a(0.00),
	minimumGimbalStopAngle_a(UNDEFINED_ANGLE),
	maximumGimbalStopAngle_a(UNDEFINED_ANGLE),
	ptrDedicatedTransmitterArray_a(NULL), ptrPrecludedReceiverArray_a(NULL),
	ptrAzimuthAngleDataList_a(NULL), ptrLinksList_a(NULL)
{
    // Init the link constraint data unique pointer
    ptrLinkConstraintData_a = unique_ptr<DefaultedData<MURALTypes::LinkConstraintDataPtr>>(
                                  new DefaultedData<MURALTypes::LinkConstraintDataPtr>()
                              );
    return;
}

Antenna&
Antenna::operator = (const Antenna &copyMe)
{
    frequencyBand_a           = copyMe.frequencyBand_a;
    capacity_a                = copyMe.capacity_a;
    preferencedCapacity_a     = copyMe.preferencedCapacity_a;
    previousCapacity_a        = copyMe.previousCapacity_a;
    preparationTime_a         = copyMe.preparationTime_a;
    dataRate_a                = copyMe.dataRate_a;
    minimumGimbalStopAngle_a  = copyMe.minimumGimbalStopAngle_a;
    maximumGimbalStopAngle_a  = copyMe.maximumGimbalStopAngle_a;
    ptrAntennaDesignator_a    = copyMe.ptrAntennaDesignator_a;
    ptrCapacityArray_a        = NULL;
    ptrAntennaCoordinateFrame_a = copyMe.ptrAntennaCoordinateFrame_a;

    if (copyMe.ptrCapacityArray_a != NULL) {
        ptrCapacityArray_a = shared_ptr<vector<int>>(
                                 new vector<int>(*copyMe.ptrCapacityArray_a)
                             );
    }

    if (copyMe.ptrDedicatedTransmitterArray_a != NULL) {
        ptrDedicatedTransmitterArray_a = shared_ptr<vector<string>>(
                                             new vector<string>(*copyMe.ptrDedicatedTransmitterArray_a)
                                         );
    }

    if (copyMe.ptrPrecludedReceiverArray_a != NULL) {
        ptrPrecludedReceiverArray_a = shared_ptr<vector<string>>(
                                          new vector<string>(*copyMe.ptrPrecludedReceiverArray_a)
                                      );
    }

    if (copyMe.ptrAzimuthAngleDataList_a != NULL) {
        ptrAzimuthAngleDataList_a = shared_ptr<MURALTypes::AzimuthAngleDataVector>(
                                        new MURALTypes::AzimuthAngleDataVector(*copyMe.ptrAzimuthAngleDataList_a)
                                    );
    }

    if (copyMe.ptrLinkConstraintData_a != NULL) {
        ptrLinkConstraintData_a = unique_ptr<DefaultedData<MURALTypes::LinkConstraintDataPtr>>(
                                      new DefaultedData<MURALTypes::LinkConstraintDataPtr>(*copyMe.ptrLinkConstraintData_a)
                                  );
    }

    if (copyMe.ptrLinksList_a != NULL) {
        ptrLinksList_a = new LinkList<Link>(*(copyMe.ptrLinksList_a));
    }

    return(*this);
}

bool
Antenna::operator == (const Antenna &compareMe)
{
    return( (frequencyBand_a == compareMe.frequencyBand_a) &&
            (capacity_a == compareMe.capacity_a) &&
            (preferencedCapacity_a == compareMe.preferencedCapacity_a) &&
            (previousCapacity_a == compareMe.previousCapacity_a) &&
            (preparationTime_a == compareMe.preparationTime_a) &&
            (dataRate_a == compareMe.dataRate_a) &&
            (minimumGimbalStopAngle_a == compareMe.minimumGimbalStopAngle_a) &&
            (maximumGimbalStopAngle_a == compareMe.maximumGimbalStopAngle_a) );
}

bool
Antenna::operator != (const Antenna &compareMe)
{
    return(!(operator == (compareMe)));
}

/* ****************************************************************************************** */

shared_ptr<DataProvider>
Antenna::GetDataProvider() const
{
    shared_ptr<DataProvider> ptrDataProvider(dynamic_pointer_cast<DataProvider>(ptrElement_a));
    return(ptrDataProvider);
}

/* ****************************************************************************************** */

void
Antenna::PrintAttributes(ostream &outfile) const
{
    int                element = 0;
    double              defaultValue = 0.0;
    double              value = 0.0;
    string              designator = "";
    const TIME_UNIT     inputTimeUnits = UnitData::GetInputTimeUnits();
    shared_ptr<AzimuthAngleData>     ptrAzimuthAngle = NULL;
    shared_ptr<vector<string>> ptrSpecificDesignators = ptrLinkConstraintData_a->GetSpecificTags();
    MURALTypes::AzimuthAngleDataVector::iterator azimuthDataIter;
    MURALTypes::LinkConstraintDataPtr ptrDefaultData = ptrLinkConstraintData_a->GetDefaultValue();
    vector<string>::iterator desIter;
    outfile << "      Antenna Designator                                      : ";
    outfile << (*ptrAntennaDesignator_a) << NEW_LINE;
    outfile << "      Communication Frequency Band                            : ";
    outfile << GetFrequencyBandString() << NEW_LINE;
    outfile << "      Capacity                                                : ";
    outfile << capacity_a << NEW_LINE;

    if (preferencedCapacity_a > 0) {
        outfile << "      Preferenced Antenna                                     : ";
        outfile << preferencedCapacity_a << NEW_LINE;
    }

    if (preparationTime_a > 0) {
        outfile << "      Preparation Time                                        : ";
        TimePiece::PrintDuration(outfile, preparationTime_a, UnitData::GetInputTimeUnits());
        outfile << NEW_LINE;
    }

    outfile << "      Data Rate (Gbits/sec)                                   : ";
    outfile << dataRate_a << NEW_LINE;
    // Print Maximum Elevation Angle
    defaultValue = ptrDefaultData->GetMaximumElevationAngle();

    if (defaultValue != UNDEFINED_ANGLE) {
        outfile << "      Default Maximum Elevation Angle                         : ";
        outfile << UnitData::GetInputAngle(defaultValue) << NEW_LINE;

        for(desIter = ptrSpecificDesignators->begin(); desIter != ptrSpecificDesignators->end(); ++desIter) {
            MURALTypes::LinkConstraintDataPtr ptrSpecificData = ptrLinkConstraintData_a->GetValue(*desIter);
            value = ptrSpecificData->GetMaximumElevationAngle();

            if (value != defaultValue) {
                designator = InputData::GetSpecificTypeString(*desIter);
                outfile << "        Maximum Elevation Angle ";
                outfile << setw(10) << designator;
                outfile << "                     : ";
                outfile << UnitData::GetInputAngle(value) << NEW_LINE;
            }
        }
    }

    // Print Minimum Elevation Angle
    defaultValue = ptrDefaultData->GetMinimumElevationAngle();

    if (defaultValue != UNDEFINED_ANGLE) {
        outfile << "      Default Minimum Elevation Angle                         : ";
        outfile << UnitData::GetInputAngle(defaultValue) << NEW_LINE;

        for(desIter = ptrSpecificDesignators->begin(); desIter != ptrSpecificDesignators->end(); ++desIter) {
            MURALTypes::LinkConstraintDataPtr ptrSpecificData = ptrLinkConstraintData_a->GetValue(*desIter);
            value = ptrSpecificData->GetMinimumElevationAngle();

            if (value != defaultValue) {
                designator = InputData::GetSpecificTypeString(*desIter);
                outfile << "        Minimum Elevation Angle ";
                outfile << setw(10) << (designator);
                outfile << "                     : ";
                outfile << UnitData::GetInputAngle(value) << NEW_LINE;
            }
        }
    }

    // Print Minimum Grazing Altitude Angle
    defaultValue = ptrDefaultData->GetMinimumGrazingAltitude();

    if (defaultValue != UNDEFINED_DISTANCE) {
        outfile << "      Default Minimum Grazing Altitude                        : ";
        outfile << UnitData::GetInputDistance(defaultValue) << NEW_LINE;

        for(desIter = ptrSpecificDesignators->begin(); desIter != ptrSpecificDesignators->end(); ++desIter) {
            MURALTypes::LinkConstraintDataPtr ptrSpecificData = ptrLinkConstraintData_a->GetValue(*desIter);
            value = ptrSpecificData->GetMinimumGrazingAltitude();

            if (value != defaultValue) {
                designator = InputData::GetSpecificTypeString(*desIter);
                outfile << "        Minimum Grazing Altitude ";
                outfile << setw(10) << (designator);
                outfile << "                    : ";
                outfile << UnitData::GetInputDistance(value) << NEW_LINE;
            }
        }
    }

    // Print Maximum Range
    defaultValue = ptrDefaultData->GetMaximumRange();

    if (defaultValue != UNDEFINED_DISTANCE) {
        outfile << "      Default Maximum Range                                   : ";
        outfile << UnitData::GetInputDistance(defaultValue) << NEW_LINE;

        for(desIter = ptrSpecificDesignators->begin(); desIter != ptrSpecificDesignators->end(); ++desIter) {
            MURALTypes::LinkConstraintDataPtr ptrSpecificData = ptrLinkConstraintData_a->GetValue(*desIter);
            value = ptrSpecificData->GetMaximumRange();

            if (value != defaultValue) {
                designator = InputData::GetSpecificTypeString(*desIter);
                outfile << "        Maximum Range ";
                outfile << setw(10) << (designator);
                outfile << "                               : ";
                outfile << UnitData::GetInputDistance(value) << NEW_LINE;
            }
        }
    }

    // Print Minimum Range
    defaultValue = ptrDefaultData->GetMinimumRange();

    if (defaultValue != UNDEFINED_DISTANCE) {
        outfile << "      Default Minimum Range                                   : ";
        outfile << UnitData::GetInputDistance(defaultValue) << NEW_LINE;

        for(desIter = ptrSpecificDesignators->begin(); desIter != ptrSpecificDesignators->end(); ++desIter) {
            MURALTypes::LinkConstraintDataPtr ptrSpecificData = ptrLinkConstraintData_a->GetValue(*desIter);
            value = ptrSpecificData->GetMinimumRange();

            if (value != defaultValue) {
                designator = InputData::GetSpecificTypeString(*desIter);
                outfile << "        Minimum Range ";
                outfile << setw(10) << (designator);
                outfile << "                               : ";
                outfile << UnitData::GetInputDistance(value) << NEW_LINE;
            }
        }
    }

    // Print Maximum Sun Separation Angle
    defaultValue = ptrDefaultData->GetMaximumSunSeparationAngle();

    if (defaultValue != UNDEFINED_ANGLE) {
        outfile << "      Default Maximum Sun Separation Angle                    : ";
        outfile << UnitData::GetInputAngle(defaultValue) << NEW_LINE;

        for(desIter = ptrSpecificDesignators->begin(); desIter != ptrSpecificDesignators->end(); ++desIter) {
            MURALTypes::LinkConstraintDataPtr ptrSpecificData = ptrLinkConstraintData_a->GetValue(*desIter);
            value = ptrSpecificData->GetMaximumSunSeparationAngle();

            if (value != defaultValue) {
                designator = InputData::GetSpecificTypeString(*desIter);
                outfile << "        Maximum Sun Separation Angle ";
                outfile << setw(10) << (designator);
                outfile << "                : ";
                outfile << UnitData::GetInputAngle(value) << NEW_LINE;
            }
        }
    }

    // Print Minimum Sun Separation Angle
    defaultValue = ptrDefaultData->GetMinimumSunSeparationAngle();

    if (defaultValue != UNDEFINED_ANGLE) {
        outfile << "      Default Minimum Sun Separation Angle                    : ";
        outfile << UnitData::GetInputAngle(defaultValue) << NEW_LINE;

        for(desIter = ptrSpecificDesignators->begin(); desIter != ptrSpecificDesignators->end(); ++desIter) {
            MURALTypes::LinkConstraintDataPtr ptrSpecificData = ptrLinkConstraintData_a->GetValue(*desIter);
            value = ptrSpecificData->GetMinimumSunSeparationAngle();

            if (value != defaultValue) {
                designator = InputData::GetSpecificTypeString(*desIter);
                outfile << "        Minimum Sun Separation Angle ";
                outfile << setw(10) << (designator);
                outfile << "                : ";
                outfile << UnitData::GetInputAngle(value) << NEW_LINE;
            }
        }
    }

    double platformOffsetAngle = ptrAntennaCoordinateFrame_a->GetPlatformOffsetAngle();
    double gimbalFrameOffsetAngle = ptrAntennaCoordinateFrame_a->GetGimbalFrameOffsetAngle();
    double rollOffsetAngle = ptrAntennaCoordinateFrame_a->GetRollOffsetAngle();

    if (platformOffsetAngle != 0.0) {
        outfile << "      Platform Offset Angle                                   : ";
        outfile << UnitData::GetInputAngle(platformOffsetAngle) << NEW_LINE;
    }

    if (gimbalFrameOffsetAngle != 0.0) {
        outfile << "      Gimbal Frame Offset Angle                               : ";
        outfile << UnitData::GetInputAngle(gimbalFrameOffsetAngle) << NEW_LINE;
    }

    if (rollOffsetAngle != 0.0) {
        outfile << "      Roll Offset Angle                                       : ";
        outfile << UnitData::GetInputAngle(rollOffsetAngle) << NEW_LINE;
    }

    if (minimumGimbalStopAngle_a != UNDEFINED_ANGLE) {
        outfile << "      Minimum Gimbal Stop Angle                               : ";
        outfile << UnitData::GetInputAngle(minimumGimbalStopAngle_a) << NEW_LINE;
    }

    if (maximumGimbalStopAngle_a != UNDEFINED_ANGLE) {
        outfile << "      Maximum Gimbal Stop Angle                               : ";
        outfile << UnitData::GetInputAngle(maximumGimbalStopAngle_a) << NEW_LINE;
    }

    // Print the Azimuth Angle Data
    if (ptrAzimuthAngleDataList_a != NULL) {
        for(azimuthDataIter = ptrAzimuthAngleDataList_a->begin(); azimuthDataIter != ptrAzimuthAngleDataList_a->end(); ++azimuthDataIter) {
            ptrAzimuthAngle = (*azimuthDataIter);
            outfile << "      Azimuth Based Elevation Angles (az, min el, max el)     : ";
            outfile << UnitData::GetInputAngle(ptrAzimuthAngle->GetAzimuthAngle()) << ", ";
            outfile << UnitData::GetInputAngle(ptrAzimuthAngle->GetMinimumElevationAngle()) << ", ";
            outfile << UnitData::GetInputAngle(ptrAzimuthAngle->GetMaximumElevationAngle()) << NEW_LINE;
        }
    }

    // Print the Outage Data
    //if (ptrOutageTimePeriodList_a != NULL) {
    //    for(outageIter = ptrOutageTimePeriodList_a->begin(); outageIter != ptrOutageTimePeriodList_a->end(); ++outageIter) {
    //        int preferencedCapacity = 0;
    //        ptrOutage = (*outageIter);
    //        preferencedCapacity = ptrOutage->GetPreferencedOutageCapacity();

    //        if (preferencedCapacity > 0) {
    //            designator = InputData::GetSpecificTypeString(
    //                             ptrOutage->GetDesignatorString(),
    //                             StringEditor::ConvertToString(preferencedCapacity)
    //                         );
    //        } else {
    //            designator = InputData::GetSpecificTypeString(ptrOutage->GetDesignatorString());
    //        }

    //        outfile << "      Outage Time Period ";
    //        outfile << setw(12) << designator;
    //        outfile << " (start,end)             : ";
    //        TimePiece::PrintIndex(outfile, ptrOutage->GetStartTime(), inputTimeUnits);
    //        outfile << ", ";
    //        TimePiece::PrintIndex(outfile, ptrOutage->GetEndTime(), inputTimeUnits);
    //        outfile << NEW_LINE;
    //    }
    //}

    if (ptrDedicatedTransmitterArray_a != NULL) {
        element = 0;
        outfile << "      Dedicated Transmitter                                   : ";
        outfile << ptrDedicatedTransmitterArray_a->at(element);
        ++element;

        while (element < ptrDedicatedTransmitterArray_a->size()) {
            outfile << ", " << ptrDedicatedTransmitterArray_a->at(element);
            ++element;
        }

        outfile << NEW_LINE;
    }

    if (ptrPrecludedReceiverArray_a != NULL) {
        element = 0;
        outfile << "      Precluded Receiver                                      : ";
        outfile << ptrPrecludedReceiverArray_a->at(element);
        ++element;

        while (element < ptrPrecludedReceiverArray_a->size()) {
            outfile << ", " << ptrPrecludedReceiverArray_a->at(element);
            ++element;
        }

        outfile << NEW_LINE;
    }

    return;
}

void
Antenna::GetAzimuthBasedElevationAngles(double azimuthAngle,
                                        const string &designator,
                                        double &minimumElevationAngle,
                                        double &maximumElevationAngle) const
{
    string											errorString = "";
    shared_ptr<AzimuthAngleData>					ptrLowerAzimuthAngleData = NULL;
    shared_ptr<AzimuthAngleData>					ptrAzimuthAngleData = NULL;
    MURALTypes::AzimuthAngleDataVector::iterator	azimuthDataIter;
    minimumElevationAngle = UNDEFINED_ANGLE;
    maximumElevationAngle = UNDEFINED_ANGLE;

    for(azimuthDataIter = ptrAzimuthAngleDataList_a->begin(); (azimuthDataIter != ptrAzimuthAngleDataList_a->end()) && (minimumElevationAngle == UNDEFINED_ANGLE); ++azimuthDataIter) {
        ptrAzimuthAngleData = (*azimuthDataIter);

        if (ptrAzimuthAngleData->GetAzimuthAngle() > azimuthAngle) {
            ptrLowerAzimuthAngleData = *(azimuthDataIter - 1); // Previous
            minimumElevationAngle = SimpleMath::Interpolate(
                                        ptrLowerAzimuthAngleData->GetAzimuthAngle(),
                                        ptrAzimuthAngleData->GetAzimuthAngle(),
                                        azimuthAngle,
                                        ptrLowerAzimuthAngleData->GetMinimumElevationAngle(),
                                        ptrAzimuthAngleData->GetMinimumElevationAngle()
                                    );
            maximumElevationAngle = SimpleMath::Interpolate(
                                        ptrLowerAzimuthAngleData->GetAzimuthAngle(),
                                        ptrAzimuthAngleData->GetAzimuthAngle(),
                                        azimuthAngle,
                                        ptrLowerAzimuthAngleData->GetMaximumElevationAngle(),
                                        ptrAzimuthAngleData->GetMaximumElevationAngle()
                                    );
        } else if (ptrAzimuthAngleData->GetAzimuthAngle() == azimuthAngle) {
            minimumElevationAngle = ptrAzimuthAngleData->GetMinimumElevationAngle();
            maximumElevationAngle = ptrAzimuthAngleData->GetMaximumElevationAngle();
        }
    }

    if (minimumElevationAngle == UNDEFINED_ANGLE) {
        errorString = "Could not find range for azimuth angle " +
                      StringEditor::ConvertToString(UnitData::GetInputAngle(azimuthAngle));
        throw new InputException(
            GetClassName(),
            "GetAzimuthBasedElevationAngles(const double&, const string&, double&, double&)",
            errorString
        );
    }

    return;
}

double
Antenna::GetMaximumElevationAngle(const string &designator) const
{
    double                             returnAngle = UNDEFINED_ANGLE;
    MURALTypes::LinkConstraintDataPtr  ptrConstraintData = NULL;

    if (ptrLinkConstraintData_a != NULL) {
        // GetValue will return the default if the desingator does not exist
        ptrConstraintData = ptrLinkConstraintData_a->GetValue(designator);
        returnAngle = ptrConstraintData->GetMaximumElevationAngle();
    }

    return (returnAngle);
}

double
Antenna::GetMinimumElevationAngle(const string &designator) const
{
    double                             returnAngle = UNDEFINED_ANGLE;
    MURALTypes::LinkConstraintDataPtr  ptrConstraintData = NULL;

    if (ptrLinkConstraintData_a != NULL) {
        // GetValue will return the default if the desingator does not exist
        ptrConstraintData = ptrLinkConstraintData_a->GetValue(designator);
        returnAngle = ptrConstraintData->GetMinimumElevationAngle();
    }

    return (returnAngle);
}

double
Antenna::GetMinimumGrazingAltitude(const string &designator) const
{
    double                             returnAngle = UNDEFINED_ANGLE;
    MURALTypes::LinkConstraintDataPtr  ptrConstraintData = NULL;

    if (ptrLinkConstraintData_a != NULL) {
        // GetValue will return the default if the desingator does not exist
        ptrConstraintData = ptrLinkConstraintData_a->GetValue(designator);
        returnAngle = ptrConstraintData->GetMinimumGrazingAltitude();
    }

    return (returnAngle);
}

double
Antenna::GetMaximumRange(const string &designator) const
{
    double                             returnAngle = UNDEFINED_ANGLE;
    MURALTypes::LinkConstraintDataPtr  ptrConstraintData = NULL;

    if (ptrLinkConstraintData_a != NULL) {
        // GetValue will return the default if the desingator does not exist
        ptrConstraintData = ptrLinkConstraintData_a->GetValue(designator);
        returnAngle = ptrConstraintData->GetMaximumRange();
    }

    return (returnAngle);
}

double
Antenna::GetMinimumRange(const string &designator) const
{
    double                             returnAngle = UNDEFINED_ANGLE;
    MURALTypes::LinkConstraintDataPtr  ptrConstraintData = NULL;

    if (ptrLinkConstraintData_a != NULL) {
        // GetValue will return the default if the desingator does not exist
        ptrConstraintData = ptrLinkConstraintData_a->GetValue(designator);
        returnAngle = ptrConstraintData->GetMinimumRange();
    }

    return (returnAngle);
}

double
Antenna::GetMaximumSunSeparationAngle(const string &designator) const
{
    double                             returnAngle = UNDEFINED_ANGLE;
    MURALTypes::LinkConstraintDataPtr  ptrConstraintData = NULL;

    if (ptrLinkConstraintData_a != NULL) {
        // GetValue will return the default if the desingator does not exist
        ptrConstraintData = ptrLinkConstraintData_a->GetValue(designator);
        returnAngle = ptrConstraintData->GetMaximumSunSeparationAngle();
    }

    return (returnAngle);
}

double
Antenna::GetMinimumSunSeparationAngle(const string &designator) const
{
    double                             returnAngle = UNDEFINED_ANGLE;
    MURALTypes::LinkConstraintDataPtr  ptrConstraintData = NULL;

    if (ptrLinkConstraintData_a != NULL) {
        // GetValue will return the default if the desingator does not exist
        ptrConstraintData = ptrLinkConstraintData_a->GetValue(designator);
        returnAngle = ptrConstraintData->GetMinimumSunSeparationAngle();
    }

    return (returnAngle);
}

bool
Antenna::IsAnOutageTimeStep(int timeIndex,
	const string &designatorString) const
{
	bool                           outageTimeStep = false;
	if (GetCapacity(timeIndex) == 0) {
		outageTimeStep = true;
	}

	return(outageTimeStep);
}

bool
Antenna::IsRequiredDownlinkOverheadTime(int timeIndex) const
{
    bool               requiredDownlinkOverheadTime = false;
    ListIterator<Link> linksListIter(ptrLinksList_a);
    Link              *ptrLink = linksListIter.First();

    while ((ptrLink != NULL) && (requiredDownlinkOverheadTime == false)) {
        requiredDownlinkOverheadTime = ptrLink->RequiredDownlinkOverhead(timeIndex);
        ptrLink = linksListIter.Next();
    }

    return(requiredDownlinkOverheadTime);
}

bool
Antenna::FindContactStart(int &timeIndex,
                          int &startContact) const
{
    const int numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    startContact = -1;

    if (timeIndex == 0) {
        if (CheckCapacityEmpty(timeIndex) == false) {
            startContact = timeIndex;
        } else {
            ++timeIndex;
        }
    }

    while ((timeIndex < numberOfTimeSteps) && (startContact < 0)) {
        if ((CheckCapacityEmpty(timeIndex - 1) == true) &&
                (CheckCapacityEmpty(timeIndex) == false)) {
            startContact = timeIndex;
        } else {
            ++timeIndex;
        }
    }

    return(startContact >= 0);
}

bool
Antenna::FindLossInContact(int &timeIndex,
                           int &endPreviousContact,
                           int &startCurrentContact) const
{
    const int ONE = 1;
    const int numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    timeIndex = std::max(timeIndex, ONE);
    endPreviousContact = -1;
    startCurrentContact = -1;

    while ((timeIndex < numberOfTimeSteps) && (endPreviousContact < 0)) {
        if ((CheckCapacityEmpty(timeIndex - ONE) == false) &&
                (CheckCapacityEmpty(timeIndex) == true) ) {
            endPreviousContact = timeIndex - ONE;
        } else {
            ++timeIndex;
        }
    }

    if (endPreviousContact >= 0) {
        ++timeIndex;

        while ((timeIndex < numberOfTimeSteps) && (startCurrentContact < 0)) {
            if ( (CheckCapacityEmpty(timeIndex - ONE) == true) &&
                    (CheckCapacityEmpty(timeIndex) == false) ) {
                startCurrentContact = timeIndex;
            } else {
                ++timeIndex;
            }
        }
    }

    return(startCurrentContact >= 0);
}

bool
Antenna::IsThereAnyLink(int timeIndex) const
{
    bool               isALink = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (isALink == false)) {
        isALink = ptrLink->IsInview(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(isALink);
}

bool
Antenna::IsThereALinkTo(const string &receiveDesignator,
                        int timeIndex) const
{
    bool  isALink = false;
    Link *ptrLink = GetLinkTo(receiveDesignator);

    if (ptrLink != NULL) {
        isALink = ptrLink->IsInview(timeIndex);
    }

    return(isALink);
}

Link*
Antenna::GetAllocatedLinkTo(const string &receiveDesignator,
                            int timeIndex) const
{
    bool               found = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (found == false)) {
        if ((ptrLink->GetReceiveDesignator() == receiveDesignator) &&
                (ptrLink->IsAllocated(timeIndex) == true)) {
            found = true;
        } else {
            ptrLink = linkIter.Next();
        }
    }

    return(ptrLink);
}

Link*
Antenna::GetLinkTo(const string &receiveDesignator) const
{
    bool               found = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (found == false)) {
        if (ptrLink->GetReceiveDesignator() == receiveDesignator) {
            found = true;
        } else {
            ptrLink = linkIter.Next();
        }
    }

    return(ptrLink);
}

Link*
Antenna::GetLinkTo(const string &receiveDesignator,
                   const string &antennaDesignator) const
{
    bool               foundLink = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (foundLink == false)) {
        if ((ptrLink->GetReceiveDesignator() == receiveDesignator) &&
                (ptrLink->GetReceiveAntennaDesignator() == antennaDesignator) ) {
            foundLink = true;
        } else {
            ptrLink = linkIter.Next();
        }
    }

    return(ptrLink);
}

bool
Antenna::IsThereALinkFrom(const string &transmitDesignator,
                          int timeIndex) const
{
    bool  isALink = false;
    Link *ptrLink = GetLinkFrom(transmitDesignator);

    if (ptrLink != NULL) {
        isALink = ptrLink->IsInview(timeIndex);
    }

    return(isALink);
}

bool
Antenna::ShouldCheckConjunction(const string &transmitDesignator,
                                int timeIndex) const
{
    bool     shouldCheck = false;
    Link    *ptrLink = GetLinkFrom(transmitDesignator);

    if (ptrLink != NULL) {
        if ((ptrLink->IsInview(timeIndex) == true ) ||
                ((ptrAzimuthAngleDataList_a != NULL) && (ptrLink->TransmitElevationLinkProblem(timeIndex) == true))) {
            shouldCheck = true;
        }
    }

    return(shouldCheck);
}

Link*
Antenna::GetLinkFrom(const string &transmitDesignator) const
{
    bool               foundLink = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (foundLink == false)) {
        if (ptrLink->GetTransmitDesignator() == transmitDesignator) {
            foundLink = true;
        } else {
            ptrLink = linkIter.Next();
        }
    }

    return(ptrLink);
}

Link*
Antenna::GetLinkFrom(const string &transmitDesignator,
                     const string &antennaDesignator) const
{
    bool               foundLink = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (foundLink == false)) {
        if ((ptrLink->GetTransmitDesignator() == transmitDesignator) &&
                (ptrLink->GetTransmitAntennaDesignator() == antennaDesignator) ) {
            foundLink = true;
        } else {
            ptrLink = linkIter.Next();
        }
    }

    return(ptrLink);
}

Link*
Antenna::GetAllocatedLink(int timeIndex) const
{
    bool               foundAllocatedLink = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (foundAllocatedLink == false)) {
        if (ptrLink->IsAllocated(timeIndex) == true) {
            foundAllocatedLink = true;
        } else {
            ptrLink = linkIter.Next();
        }
    }

    return(ptrLink);
}

bool
Antenna::CheckCapacityEmpty(int timeIndex) const
{
    bool               capacityIsEmpty = true;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (capacityIsEmpty == true)) {
        if (ptrLink->IsAllocated(timeIndex) == true) {
            capacityIsEmpty = false;
        }

        ptrLink = linkIter.Next();
    }

    return(capacityIsEmpty);
}

bool
Antenna::CheckCapacityFull(int timeIndex) const
{
    int              numberAllocated = 0;
    const int        capacity = GetCapacity(timeIndex);
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while (ptrLink != NULL) {
        if (ptrLink->IsAllocated(timeIndex) == true) {
            ++numberAllocated;
        }

        ptrLink = linkIter.Next();
    }

    return(numberAllocated >= capacity);
}

bool
Antenna::CheckCapacityFull(int startTime, int endTime) const
{
    bool               capacityFull = false;
    int               timeIndex = startTime;
    int              numberAllocated = 0;
    int              capacity = GetCapacity(timeIndex);
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = NULL;

    while (timeIndex <= endTime && capacityFull == false) {
        capacity = GetCapacity(timeIndex);
        ptrLink  = linkIter.First();

        while (ptrLink != NULL && capacityFull == false) {
            if (ptrLink->IsAllocated(timeIndex) == true) {
                ++numberAllocated;
            }

            if(numberAllocated >= capacity) {
                capacityFull = true;
            }

            ptrLink = linkIter.Next();
        }

        ++timeIndex;
    }

    return(capacityFull);
}

bool
Antenna::CheckReceiveCapacityFull(int timeIndex) const
{
    int              numberAllocated = 0;
    const int        capacity = GetCapacity(timeIndex);
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while (ptrLink != NULL) {
        if ((ptrLink->IsAllocated(timeIndex) == true) &&
                (ptrLink->GetAllocationStatus(timeIndex) != Link::TRANSMIT_ANTENNA_PREP) ) {
            ++numberAllocated;
        }

        ptrLink = linkIter.Next();
    }

    return(numberAllocated >= capacity);
}

bool
Antenna::CheckTransmitCapacityFull(int timeIndex) const
{
    int              numberAllocated = 0;
    const int        capacity = GetCapacity(timeIndex);
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while (ptrLink != NULL) {
        if ((ptrLink->IsAllocated(timeIndex) == true) &&
                (ptrLink->GetAllocationStatus(timeIndex) != Link::RECEIVE_ANTENNA_PREP) ) {
            ++numberAllocated;
        }

        ptrLink = linkIter.Next();
    }

    return(numberAllocated >= capacity);
}

bool
Antenna::CheckNonDedicatedCapacityFull(int timeIndex,
                                       const string &transmitDesignator) const
{
    int nonDedicatedAvailable = GetCapacity(timeIndex);
    int nonDedicatedAllocated = 0;
    Link *ptrLink = NULL;

    if (ptrDedicatedTransmitterArray_a != NULL) {
        if (Utility::VectorContains(*ptrDedicatedTransmitterArray_a, transmitDesignator) == false) {
            ListIterator<Link> linkIter(ptrLinksList_a);
            //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
            nonDedicatedAvailable -= Utility::CastSizetToInt(ptrDedicatedTransmitterArray_a->size());
            ptrLink = linkIter.First();

            while (ptrLink != NULL) {
                if ((ptrLink->IsAllocated(timeIndex) == true) &&
                        (IsDedicatedTransmitter(ptrLink->GetTransmitDesignator()) == false) ) {
                    ++nonDedicatedAllocated;
                }

                ptrLink = linkIter.Next();
            }
        }
    }

    return(nonDedicatedAllocated >= nonDedicatedAvailable);
}

bool
Antenna::IsAllocated(int timeIndex) const
{
    bool               allocated = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (allocated == false)) {
        allocated = ptrLink->IsAllocated(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(allocated);
}

bool
Antenna::IsAllocatedOverhead(int timeIndex) const
{
    bool               allocatedOverhead = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (allocatedOverhead == false)) {
        allocatedOverhead = ptrLink->IsAllocatedOverhead(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(allocatedOverhead);
}

bool
Antenna::IsAllocatedTransmitPrep(int timeIndex) const
{
    bool               allocatedTransmitPrep = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (allocatedTransmitPrep == false)) {
        allocatedTransmitPrep = ptrLink->IsAllocatedTransmitPrep(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(allocatedTransmitPrep);
}

bool
Antenna::IsAllocatedReceivePrep(int timeIndex) const
{
    bool               allocatedReceivePrep = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (allocatedReceivePrep == false)) {
        allocatedReceivePrep = ptrLink->IsAllocatedReceivePrep(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(allocatedReceivePrep);
}

bool
Antenna::IsAllocatedPreparation(int timeIndex) const
{
    bool               allocatedPreparation = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (allocatedPreparation == false)) {
        allocatedPreparation = ptrLink->IsAllocatedPreparation(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(allocatedPreparation);
}

bool
Antenna::IsAllocatedNonPrepTime(int timeIndex) const
{
    bool               allocatedNonPrepTime = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (allocatedNonPrepTime == false)) {
        if ((ptrLink->IsAllocated(timeIndex) == true) &&
                (ptrLink->IsAllocatedPreparation(timeIndex) == false) ) {
            allocatedNonPrepTime = true;
        }

        ptrLink = linkIter.Next();
    }

    return(allocatedNonPrepTime);
}

bool
Antenna::IsAllocatedMission(int timeIndex) const
{
    bool               allocatedMission = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (allocatedMission == false)) {
        allocatedMission = ptrLink->IsAllocatedMission(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(allocatedMission);
}

bool
Antenna::IsInNarrowbandContact(int timeIndex) const
{
    bool               isInNarrowbandContact = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (isInNarrowbandContact == false)) {
        isInNarrowbandContact = ptrLink->IsInNarrowbandContact(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(isInNarrowbandContact);
}

bool
Antenna::IsAllocatedBuffer(int timeIndex) const
{
    bool               allocatedBuffer = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (allocatedBuffer == false)) {
        allocatedBuffer = ptrLink->IsAllocatedBuffer(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(allocatedBuffer);
}

bool
Antenna::IsAllocatedAcquisition(int timeIndex) const
{
    bool               allocatedAcquisition = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (allocatedAcquisition == false)) {
        allocatedAcquisition = ptrLink->IsAllocatedAcquisition(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(allocatedAcquisition);
}

bool
Antenna::IsAllocatedDroplink(int timeIndex) const
{
    bool               allocatedDroplink = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (allocatedDroplink == false)) {
        allocatedDroplink = ptrLink->IsAllocatedDroplink(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(allocatedDroplink);
}

bool
Antenna::IsAllocatedStateOfHealth(int timeIndex) const
{
    bool               allocatedStateOfHealth = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (allocatedStateOfHealth == false)) {
        allocatedStateOfHealth = ptrLink->IsAllocatedStateOfHealth(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(allocatedStateOfHealth);
}

bool
Antenna::IsAllocatedWideband(int timeIndex) const
{
    bool               allocatedWideband = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (allocatedWideband == false)) {
        allocatedWideband = ptrLink->IsAllocatedWideband(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(allocatedWideband);
}


int
Antenna::GetMinimumCapacity() const
{
    int minimumCapacity = capacity_a;

    if (ptrCapacityArray_a != NULL) {
        minimumCapacity = *(std::min_element(ptrCapacityArray_a->begin(), ptrCapacityArray_a->end()));
    }

    return(minimumCapacity);
}

int
Antenna::GetMinimumCapacity(int startTimeIndex,
                            int endTimeIndex) const
{
    int minimumCapacity = capacity_a;
    int  timeIndex = startTimeIndex;

    if (ptrCapacityArray_a != NULL) {
        while (timeIndex <= endTimeIndex) {
            minimumCapacity = std::min(ptrCapacityArray_a->at(timeIndex), minimumCapacity);
            ++timeIndex;
        }
    }

    return(minimumCapacity);
}

int
Antenna::GetMaximumCapacity(int startTimeIndex,
                            int endTimeIndex) const
{
    int maximumCapacity = capacity_a;
    int  timeIndex = startTimeIndex;

    if (ptrCapacityArray_a != NULL) {
        maximumCapacity = 0;

        while (timeIndex <= endTimeIndex) {
            maximumCapacity = std::max(ptrCapacityArray_a->at(timeIndex), maximumCapacity);
            ++timeIndex;
        }
    }

    return(maximumCapacity);
}

int
Antenna::GetMinimumAcquisitionTime() const
{
    int              minimumAcquire = 0;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    if (ptrLink != NULL) {
        minimumAcquire = SHRT_MAX;

        while (ptrLink != NULL) {
            minimumAcquire = std::min(minimumAcquire,
                                      ptrLink->GetAcquisitionTime());
            ptrLink = linkIter.Next();
        }
    }

    return(minimumAcquire);
}

int
Antenna::GetMaximumAcquisitionTime() const
{
    int              maximumAcquire = 0;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while (ptrLink != NULL) {
        maximumAcquire = std::max(maximumAcquire,
                                  ptrLink->GetAcquisitionTime());
        ptrLink = linkIter.Next();
    }

    return(maximumAcquire);
}

int
Antenna::GetMinimumDroplinkTime() const
{
    int              minimumDrop = 0;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    if (ptrLink != NULL) {
        minimumDrop = SHRT_MAX;

        while (ptrLink != NULL) {
            minimumDrop = std::min(minimumDrop, ptrLink->GetDroplinkTime());
            ptrLink = linkIter.Next();
        }
    }

    return(minimumDrop);
}

int
Antenna::GetMaximumDroplinkTime() const
{
    int              maximumDrop = 0;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while (ptrLink != NULL) {
        maximumDrop = std::max(maximumDrop, ptrLink->GetMaximumDroplinkTime());
        ptrLink = linkIter.Next();
    }

    return(maximumDrop);
}

int
Antenna::GetMaximumOverheadTime() const
{
    int  maximumOverhead = 0;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while (ptrLink != NULL) {
        maximumOverhead = std::max<int>(maximumOverhead,
                                        (ptrLink->GetAcquisitionTime() + ptrLink->GetMaximumDroplinkTime()));
        ptrLink = linkIter.Next();
    }

    return(maximumOverhead);
}

int
Antenna::GetNumberOfAssetsModeled() const
{
    return(1);
}

void
Antenna::SetAllocationStatus(int startAllocation,
                             int endAllocation)
{
    return;
}

void
Antenna::BuybackAllocatedTimeStep(int timeIndex) const
{
    return;
}

int
Antenna::GetDataProviderIndex() const
{
    return(-1);
}

int
Antenna::GetDataProviderNumber() const
{
    return(0);
}

double
Antenna::GetCollectionDataRate(int timeIndex) const
{
    return(0.0);
}

double
Antenna::GetMaximumCollectionDataRate() const
{
    return(0.0);
}

bool
Antenna::UseTransmitDataRateInThroughput() const
{
    return(true);
}

bool
Antenna::ConstrainMaximumThroughput() const
{
    return(false);
}

bool
Antenna::MaximumRelayThroughputProblem(int timeIndex,
                                       double dataRate) const
{
    return(false);
}

bool
Antenna::PrecludeSimultaneousAcquisitions() const
{
    return(false);
}

char
Antenna::GetIdentification(int capacityNumber) const
{
    return(ERROR_CHARACTER);
}

LinkList<Link>*
Antenna::GetCrosslinks(const string &userDesignator) const
{
    return(NULL);
}

Link*
Antenna::GetAllocatedDownlink(int timeIndex) const
{
    return(NULL);
}

Array<string>*
Antenna::GetMultiLinksArray() const
{
    string             receiverDesignator = "";
    Array<string>     *ptrFoundLinkArray = new Array<string>();
    Array<string>     *ptrMultiLinkArray = NULL;
    ListIterator<Link> linksListIter(ptrLinksList_a);
    Link              *ptrLink = linksListIter.First();

    while (ptrLink != NULL) {
        receiverDesignator = ptrLink->GetReceiveDesignator();

        if ((ptrFoundLinkArray->ValueExists(receiverDesignator) == true) ||
                (ptrLink->GetReceiveCapacity() > 1)) {
            if (ptrMultiLinkArray == NULL) {
                ptrMultiLinkArray = new Array<string>();
            }

            ptrMultiLinkArray->AddUniqueElement(receiverDesignator);
        } else {
            ptrFoundLinkArray->AddElement(receiverDesignator);
        }

        ptrLink = linksListIter.Next();
    }

    delete ptrFoundLinkArray;
    ptrFoundLinkArray = NULL;
    return(ptrMultiLinkArray);
}

void
Antenna::MakeBestFitAllocatedAssetAssignments(bool assetIsTransmitter,
        int switchThreshold)
{
    const int        ZERO = 0;
    const int         numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    Array2D<int>    *ptrTempArray = NULL;
    ListIterator<Link> linksListIter(ptrLinksList_a);
    Link              *ptrLink = linksListIter.First();

    if (ptrLink != NULL) {
        int assetNumber = 0;
        int lastAllocatedAssetNumber = 0;
        int endThresholdTime = 0;
        int capacityCount = 0;
        int capacityIndex = 0;
        int capacitySize = 0;
        int timeIndex = 0;
        bool antennaFull = false;
        bool foundStartAllocation = false;
        bool foundEndAllocation = false;
        ptrTempArray = new Array2D<int>(numberOfTimeSteps);

        while (ptrLink != NULL) {
            if (assetIsTransmitter == false) {
                assetNumber = ptrLink->GetTransmitNumber();
            } else {
                assetNumber = ptrLink->GetReceiveNumber();
            }

            timeIndex = 0;

            while (timeIndex < numberOfTimeSteps) {
                if (ptrLink->IsAcquiringOrDroplinking(timeIndex) == true) {
                    ptrTempArray->AddElement(timeIndex, -assetNumber);
                } else if ((ptrLink->IsAllocatedMission(timeIndex) == true) ||
                           (ptrLink->IsAllocatedBuffer(timeIndex) == true) ||
                           (ptrLink->IsAllocatedStateOfHealth(timeIndex) == true) ||
                           (ptrLink->IsAllocatedOverhead(timeIndex) == true) ||
                           ((ptrLink->IsAllocatedReceivePrep(timeIndex) == true) && (assetIsTransmitter == false)) ||
                           ((ptrLink->IsAllocatedTransmitPrep(timeIndex) == true) && (assetIsTransmitter == true))) {
                    ptrTempArray->AddElement(timeIndex, assetNumber);
                }

                ++timeIndex;
            }

            ptrLink = linksListIter.Next();
        }

        ptrAllocatedAssetArray_a = new Array2D<int>(numberOfTimeSteps, capacity_a, ZERO);
        timeIndex = 0;

        while (timeIndex < numberOfTimeSteps) {
            if (GetCapacity(timeIndex) < ptrTempArray->GetSize(timeIndex)) {
                cout << "Antenna:  ERROR  Allocated asset(s) exceeds capcity at time step "
                     << TimePiece::GetTimeStep(timeIndex) << " on "
                     << GetDataProviderDesignator() << NEW_LINE;
            }

            ++timeIndex;
        }

        capacityCount = 0;

        while (capacityCount < capacity_a) {
            timeIndex = 0;

            while (timeIndex < numberOfTimeSteps) {
                foundStartAllocation = false;
                capacityIndex = 0;
                capacitySize = ptrTempArray->GetSize(timeIndex);

                while ((capacityIndex < capacitySize) && (foundStartAllocation == false)) {
                    assetNumber = ptrTempArray->ElementAt(timeIndex, capacityIndex);

                    if (assetNumber != ZERO) {
                        if (timeIndex == 0) {
                            foundStartAllocation = true;
                        } else if ((ptrTempArray->ValueExists((timeIndex - 1), assetNumber) == false) &&
                                   (ptrTempArray->ValueExists((timeIndex - 1), -assetNumber) == false)) {
                            foundStartAllocation = true;
                        }
                    }

                    if (foundStartAllocation == false) {
                        ++capacityIndex;
                    }
                }

                if (foundStartAllocation == true) {
                    (*ptrAllocatedAssetArray_a)[timeIndex][capacityCount] = ptrTempArray->ElementAt(timeIndex, capacityIndex);
                    ptrTempArray->ResetValue(timeIndex, capacityIndex, ZERO);
                    ++timeIndex;
                    foundEndAllocation = false;

                    while ((timeIndex < numberOfTimeSteps) && (foundEndAllocation == false)) {
                        capacityIndex = ptrTempArray->FindIndex(timeIndex,  assetNumber);

                        if (capacityIndex >= 0) {
                            (*ptrAllocatedAssetArray_a)[timeIndex][capacityCount]
                                = ptrTempArray->ElementAt(timeIndex, capacityIndex);
                            lastAllocatedAssetNumber = ptrTempArray->ElementAt(timeIndex, capacityIndex);
                            ptrTempArray->ResetValue(timeIndex, capacityIndex, ZERO);
                            ++timeIndex;
                        } else {
                            capacityIndex = ptrTempArray->FindIndex(timeIndex, -assetNumber);

                            if (capacityIndex >= 0) {
                                (*ptrAllocatedAssetArray_a)[timeIndex][capacityCount]
                                    = ptrTempArray->ElementAt(timeIndex, capacityIndex);
                                lastAllocatedAssetNumber = ptrTempArray->ElementAt(timeIndex, capacityIndex);
                                ptrTempArray->ResetValue(timeIndex, capacityIndex, ZERO);
                                ++timeIndex;
                            } else {
                                foundEndAllocation = true;

                                if ((switchThreshold > 0) &&
                                        ((endThresholdTime = (timeIndex + switchThreshold - 1)) < numberOfTimeSteps)) {
                                    antennaFull = false;

                                    while ((timeIndex <= endThresholdTime) && (antennaFull == false)) {
                                        if (ptrTempArray->GetSize(timeIndex) == GetCapacity(timeIndex) ) {
                                            antennaFull = true;
                                        } else if (
                                            (ptrTempArray->GetSize(timeIndex) > 0) &&
                                            (
                                                ((ptrTempArray->ValueExists(timeIndex, lastAllocatedAssetNumber) == false)) ||
                                                (ptrTempArray->ValueExists(timeIndex, -lastAllocatedAssetNumber) == false)
                                            )
                                        ) {
                                            antennaFull = true;
                                        } else {
                                            ++timeIndex;
                                        }
                                    }
                                }
                            }
                        }
                    } //END: while ((timeIndex < numberOfTimeSteps) && (foundEndAllocation == false))
                } else {
                    ++timeIndex;
                } //END: if (foundStartAllocation == true)
            } //END: while (timeIndex < numberOfTimeSteps)

            ++capacityCount;
        }

        delete ptrTempArray;
        ptrTempArray = NULL;
    }

    return;
}

int
Antenna::GetAllocatedAssetNumber(int timeIndex,
                                 int capacityIndex) const
{
    int assetNumber = 0;

    if (ptrAllocatedAssetArray_a != NULL) {
        try {
            assetNumber = ptrAllocatedAssetArray_a->ElementAt(timeIndex, capacityIndex);
        } catch (Exception *ptrError) {
            ptrError->AddMethod(
                GetClassName(),
                "GetAllocatedUser(int, int)"
            );
            throw;
        }
    }

    return(assetNumber);
}

int
Antenna::GetNumberOfAllocatedAssets(int timeIndex) const
{
    int numberOfAllocatedAssets = 0;

    if (ptrAllocatedAssetArray_a != NULL) {
        int capacityIndex = 0;

        while (capacityIndex < capacity_a) {
            if (ptrAllocatedAssetArray_a->ElementAt(timeIndex, capacityIndex) != 0) {
                ++numberOfAllocatedAssets;
            }

            ++capacityIndex;
        }
    }

    return(numberOfAllocatedAssets);
}

int
Antenna::GetNumberOfReceivePrepAllocations(int timeIndex) const
{
    int              numberOfReceivePrepAllocations = 0;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while (ptrLink != NULL) {
        if (ptrLink->IsAllocatedReceivePrep(timeIndex) == true ) {
            ++numberOfReceivePrepAllocations;
        }

        ptrLink = linkIter.Next();
    }

    return(numberOfReceivePrepAllocations);
}

int
Antenna::GetNumberOfOverheadAllocations(int timeIndex) const
{
    int              numberOfOverheadAllocations = 0;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while (ptrLink != NULL) {
        if (ptrLink->IsAllocatedOverhead(timeIndex) == true) {
            ++numberOfOverheadAllocations;
        }

        ptrLink = linkIter.Next();
    }

    return(numberOfOverheadAllocations);
}

int
Antenna::GetNumberOfAllocatedTransmitPrepIC(int timeIndex) const
{
    int              allocatedTransmitPrepIC = 0;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while (ptrLink != NULL) {
        if ((ptrLink->IsInview(timeIndex) == true) &&
                (ptrLink->IsAllocatedTransmitPrep(timeIndex) == true) ) {
            ++allocatedTransmitPrepIC;
        }

        ptrLink = linkIter.Next();
    }

    return(allocatedTransmitPrepIC);
}

int
Antenna::GetNumberOfAllocatedTransmitPrepOOC(int timeIndex) const
{
    int              allocatedTransmitPrepOOC = 0;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while (ptrLink != NULL) {
        if ((ptrLink->IsInview(timeIndex) == false) &&
                (ptrLink->IsAllocatedTransmitPrep(timeIndex) == true) ) {
            ++allocatedTransmitPrepOOC;
        }

        ptrLink = linkIter.Next();
    }

    return(allocatedTransmitPrepOOC);
}

/* *************************************************************************************** */

bool
Antenna::IsDedicatedTransmitter(const string &transmitterDesignator) const
{
    bool isDedicatedTransmitter = false;

    if (ptrDedicatedTransmitterArray_a != NULL) {
        isDedicatedTransmitter = Utility::VectorContains(*ptrDedicatedTransmitterArray_a, transmitterDesignator);
    }

    return(isDedicatedTransmitter);
}

/* *************************************************************************************** */

bool
Antenna::IsPrecludedReceiver(const string &receiverDesignator) const
{
    bool isPrecludedReceiver = false;

    if (ptrPrecludedReceiverArray_a != NULL) {
        isPrecludedReceiver = Utility::VectorContains(*ptrPrecludedReceiverArray_a, receiverDesignator);
    }

    return(isPrecludedReceiver);
}

/* *************************************************************************************** */

int
Antenna::GetPreferencedCapacity() const
{
    return (preferencedCapacity_a);
}

/* *************************************************************************************** */

shared_ptr<MURALTypes::AzimuthAngleDataVector>
Antenna::GetAzimuthAngleDataList() const
{
    return (ptrAzimuthAngleDataList_a);
}

/* *************************************************************************************** */

void
Antenna::SetFrequencyBand(const FREQUENCY_BAND &frequency)
{
    frequencyBand_a = frequency;
}

/* *************************************************************************************** */

void
Antenna::SetCapacity(int capacity)
{
    capacity_a = capacity;
}

/* *************************************************************************************** */

void
Antenna::SetPreferencedCapacity(int preferencedCapacity)
{
    preferencedCapacity_a = preferencedCapacity;
}

/* *************************************************************************************** */

void
Antenna::SetPreparationTime(int preparationTime)
{
    preparationTime_a = preparationTime;
}

/* *************************************************************************************** */

void
Antenna::SetDataRate(double dataRate)
{
    dataRate_a = dataRate;
}

/* *************************************************************************************** */

void
Antenna::SetMaximumGimbalStopAngle(double maximumGimbalStopAngle)
{
    maximumGimbalStopAngle_a = maximumGimbalStopAngle;
}

/* *************************************************************************************** */

void
Antenna::SetMinimumGimbalStopAngle(double minimumGimbalStopAngle)
{
    minimumGimbalStopAngle_a = minimumGimbalStopAngle;
}

/* *************************************************************************************** */

void
Antenna::SetAzimuthAngleDataList(const shared_ptr<MURALTypes::AzimuthAngleDataVector> &ptrAzimuthAngleDataList)
{
    ptrAzimuthAngleDataList_a = ptrAzimuthAngleDataList;
}

/* *************************************************************************************** */

void
Antenna::SetDedicatedTransmitterArray(const shared_ptr<vector<string>> &ptrDedicatedTransmitterArray)
{
    ptrDedicatedTransmitterArray_a = ptrDedicatedTransmitterArray;
}

/* *************************************************************************************** */

void
Antenna::SetPrecludedReceiverArray(const shared_ptr<vector<string>> &ptrPrecludedReceiverArray)
{
    ptrPrecludedReceiverArray_a = ptrPrecludedReceiverArray;
}

/* *************************************************************************************** */

void
Antenna::SetPrecludedReceiver(const string &receiverDesignator)
{
    if (ptrPrecludedReceiverArray_a == NULL) {
        ptrPrecludedReceiverArray_a = shared_ptr<vector<string>>(new vector<string>);
        ptrPrecludedReceiverArray_a->push_back(receiverDesignator);
    } else {
        // Add unique receivers to the precluded reciever array
        if (Utility::VectorContains(*ptrPrecludedReceiverArray_a, receiverDesignator) == false) {
            ptrPrecludedReceiverArray_a->push_back(receiverDesignator);
        }
    }

    return;
}

/* *************************************************************************************** */

void
Antenna::SetDefaultLinkConstraintData(const shared_ptr<LinkConstraintData> &ptrDefaultData)
{
    ptrLinkConstraintData_a->SetDefault(ptrDefaultData);
}

/* **************************************************************************************** */

void
Antenna::SetSpecificLinkConstraintData(const string &designator, const shared_ptr<LinkConstraintData> &ptrSpecificData)
{
    ptrLinkConstraintData_a->AddSpecific(designator, ptrSpecificData);
}

//------------------------------------------------------------------------------
//
//  Operation Name: Antenna::~Antenna
//
//  Purpose:
//     This is the destructor
//
//  Calling Operation:
//     UserAntenna::~UserAntenna, RelayAntenna::~RelayAntenna, or
//     ReceiveFacilityAntenna::~ReceiveFacilityAntenna
//
//  Calling Parameters
//  Parameters        i/o/r  description
//  ----------        -----  -----------
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------
Antenna::~Antenna()
{
    if (ptrAllocatedAssetArray_a != NULL) {
        delete ptrAllocatedAssetArray_a;
        ptrAllocatedAssetArray_a = NULL;
    }
}
