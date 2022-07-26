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
 * RegionInViewReport.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "RegionInViewReport.h"
#include "mural/Sun.h"
#include "mural/io/MURALInputProcessor.h"
#include "mural/element/UserVehicle.h"

// From racore
#include "mural/Region.h"
#include "mural/access/AccessPattern.h"
#include "mural/sensor/Sensor.h"


#include "general/array/Array.h"
#include "general/data/UnitData.h"
#include "general/data/PositionState.h"
#include "general/math/SpaceVector.h"
#include "general/time/TimePiece.h"
#include "general/parse/ParseUtility.h"


RegionInViewReport::RegionInViewReport(const string &outFile,
                                       const shared_ptr<MURALTypes::UserVector> &ptrUserList) :
    outputFileName_a(outFile), ptrUserList_a(ptrUserList)
{
    return;
}

/* ************************************************************************** */

void
RegionInViewReport::CreateReport()
{
    shared_ptr<UserVehicle> ptrUser = NULL;
    ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
    // Add Header
    outfileStream_a << "    USER    TIME  SEN   MODE   TYPE REGIONS" << NEW_LINE;
    // Print the "standard regions in view"
    MURALTypes::UserVector::iterator userIter;

    for( userIter = ptrUserList_a->begin(); userIter != ptrUserList_a->end(); ++userIter) {
        ptrUser = (*userIter);
        PrintAllRegionsInView(ptrUser);
    }

    //// Print the "requested" regions in view"
    //if ( MURALInputProcessor::RunRequestGenerator() == true ) {
    //    for( userIter = ptrUserList_a->begin(); userIter != ptrUserList_a->end(); ++userIter) {
    //        ptrUser = (*userIter);
    //        PrintRequestedRegionsInView(ptrUser);
    //    }
    //}

    // Print the "allocated" regions in view"
    //if ( MURALInputProcessor::RunResourceAllocator() == true ) {
    //    for( userIter = ptrUserList_a->begin(); userIter != ptrUserList_a->end(); ++userIter) {
    //        ptrUser = (*userIter);
    //        PrintAllocatedRegionsInView(ptrUser);
    //    }
    //}

    outfileStream_a.close();
}

/* ************************************************************************** */

void
RegionInViewReport::PrintAllRegionsInView(const shared_ptr<UserVehicle> &ptrUser)
{
    const int        numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    const double      quarterCircle = UnitData::GetStoreQuarterCircle();
    Array<double>     *ptrRegionNumbers = new Array<double>();
    Array2D<double>   *ptrSensorRegionNumbers = NULL;
    const string      userDesignator = ptrUser->GetDesignator();
    const string      modeString = "INVIEW";
    AccessPattern    *ptrAccessPattern = NULL;
    Region           *ptrRegion = NULL;
    Sensor           *ptrSensor = NULL;
    SpaceVector       sunPosition;
    SpaceVector       targetPosition;
    SpaceVector       targetPositionNegated;
    SpaceVector       targetToSun;
    SpaceVector       userPosition;
    SpaceVector       userVelocity;
    LinkList<Region> *ptrRegionsInViewList = new LinkList<Region>();
    ListIterator<Sensor> sensorListIter(ptrUser->GetSensorList());

    try {
        int timeIndex = 0;
        int numberOfSunAngleSensors = 0;
        double regionNumber = 0.0;
        double sunAngle = 0.0;
        bool hasSunAngleRequirement = false;
        ptrSensor = sensorListIter.First();

        while (ptrSensor != NULL) {
            if (ptrSensor->HasSunAngleRequirement() == true) {
                ++numberOfSunAngleSensors;
            }

            ptrSensor = sensorListIter.Next();
        }

        if (numberOfSunAngleSensors > 0) {
            ptrSensorRegionNumbers = new Array2D<double>(numberOfSunAngleSensors);
        }

        while (timeIndex < numberOfTimeSteps) {
            int sensorIndex = 0;
            userPosition = ptrUser->GetPosition(timeIndex);
            userVelocity = ptrUser->GetVelocity(timeIndex);

            if (ptrSensorRegionNumbers != NULL) {
                sunPosition = Sun::GetPosition(timeIndex);
            }

            sensorIndex = 0;
            ptrSensor = sensorListIter.First();

            while (ptrSensor != NULL) {
                ptrAccessPattern = ptrSensor->GetAccessPattern();
                ptrAccessPattern->GetAllContainedRegions(ptrRegionsInViewList,
                        userPosition, userVelocity,
                        AccessPattern::LARGEST_PATTERN);
                hasSunAngleRequirement = ptrSensor->HasSunAngleRequirement();
                ptrRegion = ptrRegionsInViewList->Retrieve();

                while (ptrRegion != NULL) {
                    regionNumber = ptrRegion->GetFullRegionNumber();
                    ptrRegionNumbers->InsertUniqueElement(regionNumber);

                    if (hasSunAngleRequirement == true) {
                        targetPosition = ptrRegion->GetRegionCenter(timeIndex);
                        targetPositionNegated = targetPosition.Negate();
                        targetToSun = sunPosition - targetPosition;
                        sunAngle = targetToSun.GetSeparationAngle(targetPositionNegated) - quarterCircle;

                        if (ptrSensor->MeetsSunAngleRequirement(sunAngle) == true) {
                            ptrSensorRegionNumbers->InsertUniqueElement(sensorIndex, regionNumber);
                        }
                    }

                    ptrRegion = ptrRegionsInViewList->Retrieve();
                }

                if (hasSunAngleRequirement == true) {
                    ++sensorIndex;
                }

                ptrSensor = sensorListIter.Next();
            }

            if (ptrRegionNumbers->Size() > 0) {
                PrintRegionsInView(userDesignator, timeIndex, DEFAULT_STRING,
                                   modeString, DEFAULT_STRING, ptrRegionNumbers);
            }

            if (ptrSensorRegionNumbers != NULL) {
                sensorIndex = 0;
                ptrSensor = sensorListIter.First();

                while (ptrSensor != NULL) {
                    if (ptrSensor->HasSunAngleRequirement() == true) {
                        if (ptrSensorRegionNumbers->GetSize(sensorIndex) > 0) {
                            PrintRegionsInView(userDesignator, timeIndex, ptrSensor->GetSensorName(),
                                               modeString, DEFAULT_STRING, ptrSensorRegionNumbers->ElementAt(sensorIndex));
                        }

                        ++sensorIndex;
                    }

                    ptrSensor = sensorListIter.Next();
                }

                ptrSensorRegionNumbers->RemoveAll();
            }

            ptrRegionNumbers->Delete();
            ++timeIndex;
        }

        delete ptrRegionNumbers;
        ptrRegionNumbers = NULL;
        delete ptrRegionsInViewList;
        ptrRegionsInViewList = NULL;

        if (ptrSensorRegionNumbers != NULL) {
            delete ptrSensorRegionNumbers;
            ptrSensorRegionNumbers = NULL;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "PrintAllRegionsInView(ofstream&)");
        throw;
    }

    return;
}

/* ************************************************************************** */

//void
//RegionInViewReport::PrintRequestedRegionsInView(const shared_ptr<UserVehicle> &ptrUser)
//{
//    Array<double>         *ptrRegionNumbers = NULL;
//    string                sensorString = "";
//    const string          userDesignator = ptrUser->GetDesignator();
//    string                typeString = "";
//    ListIterator<CommunicationServiceRequest> requestListIter(ptrUser->GetRequestList());
//    CommunicationServiceRequest *ptrCSR = requestListIter.First();
//
//    try {
//        int timeIndex = 0;
//        int endTimeIndex = 0;
//        double regionNumber = 0.0;
//
//        while (ptrCSR != NULL) {
//            if ((ptrCSR->IsRealTimeRequest() == true) || (ptrCSR->IsStoreRequest() == true)) {
//                timeIndex = ptrCSR->GetStartTime();
//                endTimeIndex = ptrCSR->GetEndTime();
//                typeString = ptrCSR->GetActivityTypeString();
//
//                while (timeIndex <= endTimeIndex) {
//                    sensorString = ptrCSR->GetRequestedSensorName(timeIndex);
//                    ptrRegionNumbers = ptrCSR->GetRequestedRegionsInView(timeIndex);
//
//                    if (ptrRegionNumbers != NULL) {
//                        PrintRegionsInView(userDesignator, timeIndex, sensorString,
//                                           "REQ_IN", typeString, ptrRegionNumbers);
//                        delete ptrRegionNumbers;
//                        ptrRegionNumbers = NULL;
//                    }
//
//                    regionNumber = ptrCSR->GetRequestedRegionNumber(timeIndex);
//
//                    if (regionNumber > 0.0) {
//                        PrintMostValuableRegion(userDesignator, timeIndex, sensorString,
//                                                "REQ_MV", typeString, regionNumber);
//                    }
//
//                    ++timeIndex;
//                }
//            }
//
//            ptrCSR = requestListIter.Next();
//        }
//    } catch (Exception *ptrError) {
//        ptrError->AddMethod(GetClassName(), "PrintRequestedRegionsInView(ofstream&)");
//        throw;
//    }
//
//    return;
//}

/* ************************************************************************** */

//void
//RegionInViewReport::PrintAllocatedRegionsInView(const shared_ptr<UserVehicle> &ptrUser)
//{
//    Array<double>         *ptrRegionNumbers = NULL;
//    string                sensorString = "";
//    const string          userDesignator = ptrUser->GetDesignator();
//    string                typeString = "";
//    ResourceServicePeriod       *ptrRSP = NULL;
//    ListIterator<CommunicationServiceRequest> requestListIter(ptrUser->GetRequestList());
//    CommunicationServiceRequest *ptrCSR = requestListIter.First();
//
//    try {
//        int timeIndex = 0;
//        int endTimeIndex = 0;
//        double regionNumber = 0.0;
//
//        while (ptrCSR != NULL) {
//            typeString  = ptrCSR->GetActivityTypeString();
//
//            if (ptrCSR->IsRealTimeRequest() == true) {
//                ListIterator<ResourceServicePeriod> requestListIter(ptrCSR->GetAllocationList());
//                ptrRSP = requestListIter.First();
//
//                while (ptrRSP != NULL) {
//                    timeIndex = ptrRSP->GetStartMissionTime();
//                    endTimeIndex = ptrRSP->GetEndMissionTime();
//
//                    while (timeIndex <= endTimeIndex) {
//                        sensorString = ptrRSP->GetAllocatedSensorName(timeIndex);
//                        ptrRegionNumbers = ptrRSP->GetAllocatedRegionsInView(timeIndex);
//
//                        if (ptrRegionNumbers != NULL) {
//                            PrintRegionsInView(userDesignator, timeIndex, sensorString,
//                                               "ALL_IN", typeString, ptrRegionNumbers);
//                            delete ptrRegionNumbers;
//                            ptrRegionNumbers = NULL;
//                        }
//
//                        regionNumber = ptrRSP->GetAllocatedRegionNumber(timeIndex);
//
//                        if (regionNumber > 0.0) {
//                            PrintMostValuableRegion(userDesignator, timeIndex, sensorString,
//                                                    "ALL_MV", typeString, regionNumber);
//                        }
//
//                        ++timeIndex;
//                    }
//
//                    ptrRSP = requestListIter.Next();
//                }
//            } else if (ptrCSR->IsDumpAllocated() == true) {
//                ptrRSP = ptrCSR->GetStoreAllocation();
//
//                if (ptrRSP != NULL) {
//                    timeIndex = ptrRSP->GetStartMissionTime();
//                    endTimeIndex = ptrRSP->GetEndMissionTime();
//
//                    while (timeIndex <= endTimeIndex) {
//                        sensorString = ptrRSP->GetAllocatedSensorName(timeIndex);
//                        ptrRegionNumbers = ptrRSP->GetAllocatedRegionsInView(timeIndex);
//
//                        if (ptrRegionNumbers != NULL) {
//                            PrintRegionsInView(userDesignator, timeIndex, sensorString,
//                                               "ALL_IN", typeString, ptrRegionNumbers);
//                            delete ptrRegionNumbers;
//                            ptrRegionNumbers = NULL;
//                        }
//
//                        regionNumber = ptrRSP->GetAllocatedRegionNumber(timeIndex);
//
//                        if (regionNumber > 0.0) {
//                            PrintMostValuableRegion(userDesignator, timeIndex, sensorString,
//                                                    "ALL_MV", typeString, regionNumber);
//                        }
//
//                        ++timeIndex;
//                    }
//                } else {
//                    timeIndex = ptrCSR->GetStartTime();
//                    endTimeIndex = ptrCSR->GetEndTime();
//
//                    while (timeIndex <= endTimeIndex) {
//                        sensorString = ptrCSR->GetRequestedSensorName(timeIndex);
//                        ptrRegionNumbers = ptrCSR->GetRequestedRegionsInView(timeIndex);
//
//                        if (ptrRegionNumbers != NULL) {
//                            PrintRegionsInView(userDesignator, timeIndex, sensorString,
//                                               "ALL_IN", typeString, ptrRegionNumbers);
//                            delete ptrRegionNumbers;
//                            ptrRegionNumbers = NULL;
//                        }
//
//                        regionNumber = ptrCSR->GetRequestedRegionNumber(timeIndex);
//
//                        if (regionNumber > 0.0) {
//                            PrintMostValuableRegion(userDesignator, timeIndex, sensorString,
//                                                    "ALL_MV", typeString, regionNumber);
//                        }
//
//                        ++timeIndex;
//                    }
//                }
//            }
//
//            ptrCSR = requestListIter.Next();
//        }
//    } catch (Exception *ptrError) {
//        ptrError->AddMethod(GetClassName(), "PrintAllocatedRegionsInView(ofstream&)");
//        throw;
//    }
//
//    return;
//}

/* ************************************************************************** */

void
RegionInViewReport::PrintRegionsInView(const string &userDesignator,
                                       const int &timeIndex,
                                       const string &sensorString,
                                       const string &modeString,
                                       const string &typeString,
                                       const Array<double> *ptrRegionNumbers)
{
    const int   numberOfRegions = ptrRegionNumbers->Size();

    try {
        int regionIndex = 0;
        outfileStream_a << setw(8) << userDesignator;
        outfileStream_a << " ";
        TimePiece::PrintIndex(outfileStream_a, timeIndex);
        outfileStream_a << " ";
        outfileStream_a << setw(4) << sensorString;
        outfileStream_a << " ";
        outfileStream_a << setw(6) << modeString;
        outfileStream_a << " ";
        outfileStream_a << setw(6) << typeString;
        outfileStream_a << " ";
        Region::PrintRegionNumber(outfileStream_a, ptrRegionNumbers->ElementAt(regionIndex));
        ++regionIndex;

        while (regionIndex < numberOfRegions) {
            outfileStream_a << ", ";
            Region::PrintRegionNumber(outfileStream_a, ptrRegionNumbers->ElementAt(regionIndex));
            ++regionIndex;
        }

        outfileStream_a << NEW_LINE;
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "PrintRegionsInView(ofstream&, const string&, ...)");
        throw;
    }

    return;
}

void
RegionInViewReport::PrintMostValuableRegion(const string &userDesignator,
        const int &timeIndex,
        const string &sensorString,
        const string &modeString,
        const string &typeString,
        const double &regionNumber)
{
    outfileStream_a << setw(8) << userDesignator;
    outfileStream_a << " ";
    TimePiece::PrintIndex(outfileStream_a, timeIndex);
    outfileStream_a << " ";
    outfileStream_a << setw(4) << sensorString;
    outfileStream_a << " ";
    outfileStream_a << setw(6) << modeString;
    outfileStream_a << " ";
    outfileStream_a << setw(6) << typeString;
    outfileStream_a << " ";
    Region::PrintRegionNumber(outfileStream_a, regionNumber);
    outfileStream_a << NEW_LINE;
    return;
}

/**
 * Destructor
 */
RegionInViewReport::~RegionInViewReport()
{
    // Do nothing
}
