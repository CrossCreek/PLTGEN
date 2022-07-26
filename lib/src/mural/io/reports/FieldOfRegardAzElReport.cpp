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
 * FieldOfRegardAzElReport.cpp
 *
 * MURAL - Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "FieldOfRegardAzElReport.h"

#include "mural/io/MURALInputProcessor.h"
#include "mural/element/UserVehicle.h"
#include "mural/modules/lg/Link.h"
#include "mural/CollectionElement.h"
#include "mural/modules/rg/CommunicationServiceRequest.h"
#include "mural/modules/ra/ResourceServicePeriod.h"


#include "general/math/SpaceVector.h"
#include "general/array/LinkList.h"
#include "general/time/TimePiece.h"
#include "general/data/UnitData.h"
#include "general/parse/ParseUtility.h"

FieldOfRegardAzElReport::FieldOfRegardAzElReport(const string &outFile,
        const shared_ptr<MURALTypes::UserVector> &ptrUserList) :
    ptrUserList_a(ptrUserList), outputFileName_a(outFile)
{
}

void
FieldOfRegardAzElReport::PrintHeader(const shared_ptr<UserVehicle> &ptrUser, ofstream &forDebugFile)
{
    const Array<double> *ptrAzimuthOffsetAngles   = ptrUser->GetAzimuthOffsetAngles();
    int azimuthIndex = 0;
    // First Line
    forDebugFile << "    USER    RELAY     UPA    TIME      RGN    CONFIG";
    azimuthIndex = 0;

    while (azimuthIndex < ptrAzimuthOffsetAngles->Size()) {
        double azimuthOffsetAngle;
        azimuthOffsetAngle = ptrAzimuthOffsetAngles->ElementAt(azimuthIndex);
        forDebugFile << setw(20) << setprecision(2)
                     << UnitData::GetOutputAngle(azimuthOffsetAngle);
        azimuthIndex++;
    }

    forDebugFile << NEW_LINE;
    // Second line
    forDebugFile << "                                                     ";
    azimuthIndex = 0;

    while (azimuthIndex < ptrAzimuthOffsetAngles->Size()) {
        forDebugFile << "       AZ.       EL.";
        azimuthIndex++;
    }

    forDebugFile << NEW_LINE;
}

/**
 * Creates the array of imaging regions for the debug file. This is an array
 * of regions which were determined to be the one the requirement score is derived
 * from.

 * The timeline is used if no request/allocation
 * The request is used if no allocation
 * if the timestep is allocated then the allocated imaged region is used
 *
 * @param ptrUser the user vehicle for the requests/allocations
 *
 * @return the array of region numbers
 */
Array<double>*
FieldOfRegardAzElReport::DetermineImagingRegions(const shared_ptr<UserVehicle> &ptrUser) const
{
    const int      numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    Array<double> *ptrImagedRegions = NULL;
    ListIterator<CommunicationServiceRequest> requestListIter(ptrUser->GetRequestList());
    CommunicationServiceRequest              *ptrCSR = requestListIter.First();
    ResourceServicePeriod                    *ptrRSP = NULL;

    try {
        int timeIndex = 0;
        int endTimeIndex = 0;
        ptrImagedRegions = new Array<double>(numberOfTimeSteps);
        // Determine the imaging region numbers
        timeIndex = 0;

        while (timeIndex < numberOfTimeSteps) {
            // Start with value timeline
            (*ptrImagedRegions)[timeIndex] = ptrUser->GetImagedRegion(timeIndex);
            timeIndex++;
        }

        // Stomp with requests
        ptrCSR = requestListIter.First();

        while (ptrCSR != NULL) {
            if ( ptrCSR->IsDumpRequest() == false ) {
                timeIndex = ptrCSR->GetStartTime();
                endTimeIndex = ptrCSR->GetEndTime();

                while (timeIndex <= endTimeIndex) {
                    (*ptrImagedRegions)[timeIndex] = ptrCSR->GetCollectionElement(timeIndex)->GetImagedRegion();
                    ++timeIndex;
                }
            }

            ptrCSR = requestListIter.Next();
        }

        // Stomp with allocations
        ptrCSR = requestListIter.First();

        if (MURALInputProcessor::RunResourceAllocator() == true) {
            while (ptrCSR != NULL) {
                if ( ptrCSR->IsStoreRequest() == true ) {
                    ptrRSP = ptrCSR->GetStoreAllocation();

                    if ( ptrRSP != NULL ) {
                        timeIndex = ptrRSP->GetStartMissionTime();
                        endTimeIndex = ptrRSP->GetEndMissionTime();

                        while (timeIndex <= endTimeIndex) {
                            CollectionElement* ptrCollectionElement = ptrRSP->GetCollectionElement(timeIndex);

                            if ( ptrCollectionElement != NULL ) {
                                (*ptrImagedRegions)[timeIndex] = ptrCollectionElement->GetImagedRegion();
                            }

                            ++timeIndex;
                        }
                    }
                } else if ( ptrCSR->IsRealTimeRequest() == true ) {
                    ListIterator<ResourceServicePeriod> allocationListIter(ptrCSR->GetAllocationList());
                    ptrRSP = allocationListIter.First();

                    while (ptrRSP != NULL) {
                        timeIndex = ptrRSP->GetStartMissionTime();
                        endTimeIndex = ptrRSP->GetEndMissionTime();

                        while (timeIndex <= endTimeIndex) {
                            CollectionElement* ptrCollectionElement = ptrRSP->GetCollectionElement(timeIndex);

                            if ( ptrCollectionElement != NULL ) {
                                (*ptrImagedRegions)[timeIndex] = ptrCollectionElement->GetImagedRegion();
                            }

                            ++timeIndex;
                        }

                        ptrRSP = allocationListIter.Next();
                    }
                }

                ptrCSR = requestListIter.Next();
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "DetermineImagingRegions(const int&)");
        throw;
    }

    return(ptrImagedRegions);
}

/**
 * Creates the array of az configurations based on the allocated FOR results
 *
 * @param ptrUser the user vehicle for the requests/allocations
 *
 * @return the array az configurations
 */
Array<double>*
FieldOfRegardAzElReport::DetermineAzConfiguration(const shared_ptr<UserVehicle> &ptrUser) const
{
    const int      numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    Array<double> *ptrAzConfigArray = NULL;
    ListIterator<CommunicationServiceRequest> requestListIter(ptrUser->GetRequestList());
    CommunicationServiceRequest              *ptrCSR = requestListIter.First();
    ResourceServicePeriod                    *ptrRSP = NULL;

    try {
        int timeIndex = 0;
        ptrAzConfigArray = new Array<double>(numberOfTimeSteps);
        // Init with -1.0 to signify N/A
        timeIndex = 0;

        while (timeIndex < numberOfTimeSteps) {
            // Undefined means that the az config does not apply because its a STOR or there is no allocation
            (*ptrAzConfigArray)[timeIndex] = UNDEFINED_COEFFICIENT;
            timeIndex++;
        }

        // Stomp with allocations
        ptrCSR = requestListIter.First();

        if (MURALInputProcessor::RunResourceAllocator() == true) {
            int endTimeIndex = 0;

            while (ptrCSR != NULL) {
                if ( ptrCSR->IsRealTimeRequest() == true ) {
                    ListIterator<ResourceServicePeriod> allocationListIter(ptrCSR->GetAllocationList());
                    ptrRSP = allocationListIter.First();

                    while (ptrRSP != NULL) {
                        timeIndex = ptrRSP->GetStartMissionTime();
                        endTimeIndex = ptrRSP->GetEndMissionTime();

                        while (timeIndex <= endTimeIndex) {
                            (*ptrAzConfigArray)[timeIndex] = UnitData::GetOutputAngle(ptrRSP->GetAzimuthAngle());
                            ++timeIndex;
                        }

                        ptrRSP = allocationListIter.Next();
                    }
                }

                ptrCSR = requestListIter.Next();
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "DetermineAzConfiguration(const int&)");
        throw;
    }

    return(ptrAzConfigArray);
}

/**
 * Creates the array of allocated relays based on the allocated FOR results.  It only considers
 * Real time CSRS/RSPs because they are the only ones affected by FOR
 *
 * @param ptrUser the user vehicle for the requests/allocations
 *
 * @return the array of allocated relay designators
 */
Array<string>*
FieldOfRegardAzElReport::DetermineRealTimeAllocatedRelays(const shared_ptr<UserVehicle> &ptrUser) const
{
    const int      numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    Array<string> *ptrAllocatedRelays = NULL;
    ListIterator<CommunicationServiceRequest> requestListIter(ptrUser->GetRequestList());
    CommunicationServiceRequest              *ptrCSR = requestListIter.First();
    ResourceServicePeriod                    *ptrRSP = NULL;

    try {
        int timeIndex = 0;
        ptrAllocatedRelays = new Array<string>(numberOfTimeSteps);
        timeIndex = 0;

        while (timeIndex < numberOfTimeSteps) {
            (*ptrAllocatedRelays)[timeIndex] = "";
            timeIndex++;
        }

        ptrCSR = requestListIter.First();

        if (MURALInputProcessor::RunResourceAllocator() == true) {
            int endTimeIndex = 0;

            while (ptrCSR != NULL) {
                if ( ptrCSR->IsRealTimeRequest() == true ) {
                    ListIterator<ResourceServicePeriod> allocationListIter(ptrCSR->GetAllocationList());
                    ptrRSP = allocationListIter.First();

                    while (ptrRSP != NULL) {
                        timeIndex = ptrRSP->GetStartMissionTime();
                        endTimeIndex = ptrRSP->GetEndMissionTime();

                        while (timeIndex <= endTimeIndex) {
                            (*ptrAllocatedRelays)[timeIndex] = ptrRSP->GetAllocatedResourceDesignator(ptrUser->GetDesignator());
                            ++timeIndex;
                        }

                        ptrRSP = allocationListIter.Next();
                    }
                }

                ptrCSR = requestListIter.Next();
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "DetermineAzConfiguration(const int&)");
        throw;
    }

    return(ptrAllocatedRelays);
}

/**
 * Creates the report for the specified user.  The report consists of the azimuth and
 * elevation angles from the transmit antenna of the user to the relay based on the
 * yaw configuration (azimuth based offset angles).
 *
 * The imaged region is used to orient the Z axis to point to its center
 *
 * @param ptrUser the user to process
 * @param forDebugFile the output stream
 */
void
FieldOfRegardAzElReport::DetermineAzElAngles(const shared_ptr<UserVehicle> &ptrUser, ofstream &forDebugFile)
{
    const int                        numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    double                           azimuthOffsetAngle = UNDEFINED_ANGLE;
    string                           userDesignator = ptrUser->GetDesignator();
    string                           relayDesignator = "";
    string                           upaDesignator = "";
    string                           allocatedRelay = "";
    Array<double>                   *regionsInView = NULL;
    const Array<double>             *ptrAzimuthOffsetAngles   = ptrUser->GetAzimuthOffsetAngles();
    Link                            *ptrTransmitLink = NULL;
    SpaceVector                      userPosition;
    SpaceVector                      userVelocity;
    CollectionElement*               ptrElement = NULL;
    shared_ptr<UserTransmitAntenna>  ptrUTA = ptrUser->GetUserTransmitAntenna();
    Array<double>                   *ptrImageRegionsArray = DetermineImagingRegions(ptrUser);
    Array<double>                   *ptrAzimuthConfigArray = DetermineAzConfiguration(ptrUser);
    Array<string>                   *ptrRealAllocRelayArray = DetermineRealTimeAllocatedRelays(ptrUser);

    try {
        // Print the out put
        if (ptrAzimuthOffsetAngles != NULL) {
            // loop on valid transmit link(s)
            int azimuthIndex = 0;
            double regionNumber = 0.0;
            double azimuthAngle = 0.0;
            double elevationAngle = 0.0;
            double azConfig = 0.0;
            ListIterator<Link> transmitLinkListIter(ptrUser->GetAllTransmitLinks());
            ptrTransmitLink = transmitLinkListIter.First();

            while (ptrTransmitLink != NULL) {
                int timeIndex = 0;
                relayDesignator = ptrTransmitLink->GetReceiveDesignator();
                upaDesignator = ptrTransmitLink->GetReceiveAntennaDesignator();
                timeIndex = 0;

                while (timeIndex < numberOfTimeSteps) {
                    allocatedRelay = (*ptrRealAllocRelayArray)[timeIndex];
                    // Get User Information
                    userPosition = ptrUser->GetPosition(timeIndex);
                    userVelocity = ptrUser->GetVelocity(timeIndex);
                    regionNumber = (*ptrImageRegionsArray)[timeIndex];
                    azConfig = (*ptrAzimuthConfigArray)[timeIndex];
                    forDebugFile << setw(8) << userDesignator;
                    forDebugFile << " ";
                    forDebugFile << setw(8) << relayDesignator;
                    forDebugFile << " ";
                    forDebugFile << setw(8) << upaDesignator;
                    forDebugFile << " ";
                    TimePiece::PrintIndex(forDebugFile, timeIndex);
                    forDebugFile << " ";
                    forDebugFile << setw(8) << regionNumber;
                    forDebugFile << " ";

                    if ( (azConfig == UNDEFINED_COEFFICIENT) || (relayDesignator != allocatedRelay) ) {
                        forDebugFile << setw(8) << "N/A";
                    } else {
                        forDebugFile << setprecision(1) << setw(8) << azConfig;
                    }

                    forDebugFile << " ";
                    // Now that we have the valid links, loop on the az/cable combinations
                    azimuthIndex = 0;

                    while (azimuthIndex < ptrAzimuthOffsetAngles->Size()) {
                        azimuthOffsetAngle = ptrAzimuthOffsetAngles->ElementAt(azimuthIndex);
                        ptrUTA->DetermineAzElAngles(timeIndex,
                                                    ptrTransmitLink->GetLineOfSightVector(timeIndex), 0.0,
                                                    azimuthAngle, elevationAngle, regionNumber,
                                                    azimuthOffsetAngle);
                        // Print out the az/el values
                        forDebugFile << setprecision(2) << setw(10) << UnitData::GetOutputAngle(azimuthAngle);
                        forDebugFile << setprecision(2) << setw(10) << UnitData::GetOutputAngle(elevationAngle);
                        ++azimuthIndex;
                    }

                    forDebugFile << NEW_LINE;
                    ++timeIndex;
                }

                ptrTransmitLink = transmitLinkListIter.Next();
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "DetermineFORRegionsInviewDebug(ofstream&)");
        throw;
    }

    delete ptrImageRegionsArray;
    ptrImageRegionsArray = NULL;
    delete ptrAzimuthConfigArray;
    ptrAzimuthConfigArray = NULL;
    delete ptrRealAllocRelayArray;
    ptrRealAllocRelayArray = NULL;
}


/**
 * Main entry method which loops on all users and creates the Report
 */
void
FieldOfRegardAzElReport::CreateReport()
{
    shared_ptr<UserVehicle> ptrUser = NULL;
    // Open the file
    ofstream forDebugFile;
    ParseUtility::OpenOutputFile(forDebugFile, &outputFileName_a);
    // Loop on the users to determine the debug data
    MURALTypes::UserVector::iterator userIter;

    for( userIter = ptrUserList_a->begin(); userIter != ptrUserList_a->end(); ++userIter) {
        ptrUser = (*userIter);

        if ( (ptrUser->DetermineAllocatedMostValuableRegion() == true) &&
                (ptrUser->GetAzimuthOffsetAngles() != NULL) ) {
            // Print the Header
            PrintHeader(ptrUser, forDebugFile);
            // Create the Debug
            DetermineAzElAngles(ptrUser, forDebugFile);
        }
    }

    forDebugFile.close();
}

/**
 * Destructor
 */
FieldOfRegardAzElReport::~FieldOfRegardAzElReport()
{
    // Do nothing
}
