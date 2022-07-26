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
 * FieldOfRegardRIVReport.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "FieldOfRegardRIVReport.h"
#include "mural/element/UserVehicle.h"
#include "mural/modules/vtg/TimelineGenerator.h"
#include "mural/modules/lg/Link.h"

#include "mural/Region.h"
#include "mural/access/AccessPattern.h"
#include "mural/sensor/Sensor.h"

#include "general/array/LinkList.h"
#include "general/data/UnitData.h"
#include "general/math/SpaceVector.h"
#include "general/time/TimePiece.h"
#include "general/parse/ParseUtility.h"

FieldOfRegardRIVReport::FieldOfRegardRIVReport(const string &outFile,
        const shared_ptr<MURALTypes::UserVector> &ptrUserList) :
    ptrUserList_a(ptrUserList), outputFileName_a(outFile)
{
}

/**
 * Used to determine and print the data for the Field of Regard if the user has the input set.
 * This method prints the region list each az/cable/relay configuration can see for every time step.
 * This data method uses the ::LinkWhenPointingAtRegion method to deterimine if a region has access
 * when pointing at the given region in the access pattern
 *
 * @param forDebugFile the reference to the output stream
 */
void
FieldOfRegardRIVReport::DetermineFORRegionsInviewDebug(const shared_ptr<UserVehicle> &ptrUser, ofstream &forDebugFile)
{
    const int           numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    double               azimuthOffsetAngle = UNDEFINED_ANGLE;
    double               cableWrapOffsetAngle = UNDEFINED_ANGLE;
    string               userDesignator = "";
    string               relayDesignator = "";
    Array<double>        *regionsInView = NULL;
    const Array<double> *ptrAzimuthOffsetAngles   = ptrUser->GetAzimuthOffsetAngles();
    const Array<double> *ptrCableWrapOffsetAngles = ptrUser->GetCableWrapOffsetAngles();
    Link                *ptrTransmitLink = NULL;
    Sensor              *ptrSensor = NULL;
    AccessPattern    *ptrAccessPattern = NULL;
    Region           *ptrRegion = NULL;
    LinkList<Region> *ptrRegionsInViewList = new LinkList<Region>();
    SpaceVector       userPosition;
    SpaceVector       userVelocity;

    try {
        if ((ptrAzimuthOffsetAngles != NULL) && (ptrCableWrapOffsetAngles != NULL)) {
            int timeIndex = 0;
            int azimuthIndex = 0;
            int cableIndex = 0;
            int regionIndex = 0;
            double regionNumber = 0.0;
            // loop on all sensors on the user
            ListIterator<Sensor> sensorListIter(ptrUser->GetSensorList());
            ptrSensor = sensorListIter.First();

            while( ptrSensor != NULL ) {
                // loop on valid transmit link(s)
                ListIterator<Link> transmitLinkListIter(ptrUser->GetAllTransmitLinks());
                ptrTransmitLink = transmitLinkListIter.First();

                while (ptrTransmitLink != NULL) {
                    // Now that we have the valid links, loop on the az/cable combinations
                    azimuthIndex = 0;

                    while (azimuthIndex < ptrAzimuthOffsetAngles->Size()) {
                        azimuthOffsetAngle = ptrAzimuthOffsetAngles->ElementAt(azimuthIndex);
                        cableIndex = 0;

                        while (cableIndex < ptrCableWrapOffsetAngles->Size()) {
                            cableWrapOffsetAngle = ptrCableWrapOffsetAngles->ElementAt(cableIndex);
                            timeIndex = 0;

                            while (timeIndex < numberOfTimeSteps) {
                                userPosition = ptrUser->GetPosition(timeIndex);
                                userVelocity = ptrUser->GetVelocity(timeIndex);
                                userDesignator = ptrUser->GetDesignator();
                                relayDesignator = ptrTransmitLink->GetReceiveDesignator();
                                regionsInView = new Array<double>();

                                if ( (ptrTransmitLink->IsInview(timeIndex) == true) ||
                                        (ptrTransmitLink->TransmitElevationLinkProblem(timeIndex) == true) ) {
                                    // determine the valid region data(s) in the access pattern and determine which regions
                                    // have access at the given az/cable/relay combination
                                    ptrAccessPattern = ptrSensor->GetAccessPattern();
                                    ptrAccessPattern->GetAllContainedRegions(ptrRegionsInViewList,
                                            userPosition, userVelocity,
                                            AccessPattern::LARGEST_PATTERN);
                                    //JOE:
                                    ptrRegion = ptrRegionsInViewList->Retrieve();

                                    while (ptrRegion != NULL) {
                                        regionNumber = ptrRegion->GetFullRegionNumber();

                                        if (TimelineGenerator::LinkWhenPointingAtRegion(ptrTransmitLink, ptrUser, timeIndex, regionNumber,
                                                azimuthOffsetAngle, cableWrapOffsetAngle) == true) {
                                            regionsInView->AddElement(regionNumber);
                                        }

                                        ptrRegion = ptrRegionsInViewList->Retrieve();
                                    }
                                }

                                // Print out if we found Regions in View list
                                if ( regionsInView->Size() > 0 ) {
                                    forDebugFile << setw(8) << userDesignator;
                                    forDebugFile << " ";
                                    forDebugFile << setw(8) << ptrSensor->GetSensorName();
                                    forDebugFile << " ";
                                    TimePiece::PrintIndex(forDebugFile, timeIndex);
                                    forDebugFile << " ";
                                    forDebugFile << setw(8) << relayDesignator;
                                    forDebugFile << " ";
                                    forDebugFile << setw(6) << setprecision(1) << UnitData::GetOutputAngle(azimuthOffsetAngle);
                                    forDebugFile << setw(6) << setprecision(1) << UnitData::GetOutputAngle(cableWrapOffsetAngle);
                                    forDebugFile << "  ";

                                    for( regionIndex = 0; regionIndex < regionsInView->Size(); regionIndex++ ) {
                                        Region::PrintRegionNumber(forDebugFile, regionsInView->ElementAt(regionIndex));
                                        forDebugFile << ", ";
                                    }

                                    forDebugFile << NEW_LINE;
                                }

                                ++timeIndex;
                            }

                            ++cableIndex;
                        }

                        ++azimuthIndex;
                    }

                    ptrTransmitLink = transmitLinkListIter.Next();
                }

                ptrSensor = sensorListIter.Next();
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "DetermineFORRegionsInviewDebug(ofstream&)");
        throw;
    }
}

/**
 * Main entry method which loops on all users and creates the Report
 */
void
FieldOfRegardRIVReport::CreateReport()
{
    shared_ptr<UserVehicle>       ptrUser = NULL;
    // Open the file
    ofstream forDebugFile;
    ParseUtility::OpenOutputFile(forDebugFile, &outputFileName_a);
    // Print Header
    forDebugFile << "    USER      SEN    TIME       RS     AZ   CBL   REGIONS" << NEW_LINE;
    // Loop on the users to determine the debug data
    MURALTypes::UserVector::iterator userIter;

    for( userIter = ptrUserList_a->begin(); userIter != ptrUserList_a->end(); ++userIter) {
        ptrUser = (*userIter);
        // Create the Basic RSPs
        DetermineFORRegionsInviewDebug(ptrUser, forDebugFile);
    }

    forDebugFile.close();
}

/**
 * Destructor
 */
FieldOfRegardRIVReport::~FieldOfRegardRIVReport()
{
    // Do nothing
}
