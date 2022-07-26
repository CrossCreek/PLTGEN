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
 * TimelineReport.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "TimelineReport.h"
#include "mural/Region.h"
#include "mural/element/UserVehicle.h"
#include "mural/modules/vtg/ValueTimelineElement.h"


#include "general/exception/NumberFormatException.h"
#include "general/array/Array.h"
#include "general/utility/StringEditor.h"
#include "general/data/UnitData.h"
#include "general/time/TimePiece.h"
#include "general/parse/ParseUtility.h"

TimelineReport::TimelineReport(const std::string &outFile,
                               const shared_ptr<MURALTypes::UserVector> &ptrUserList ) :
    outputFileName_a(outFile), ptrUserList_a(ptrUserList)
{
    return;
}

/* ************************************************************************** */

void
TimelineReport::CreateReport()
{
    shared_ptr<UserVehicle> ptrUser = NULL;
    ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
    MURALTypes::UserVector::iterator userIter;

    for( userIter = ptrUserList_a->begin(); userIter != ptrUserList_a->end(); ++userIter) {
        ptrUser = (*userIter);
        PrintValueTimeline(*ptrUser);
    }

    outfileStream_a.close();
}


/* ************************************************************************** */

void
TimelineReport::PrintValueTimeline(const UserVehicle &user)
{
    int              resourceIndex = 0;
    int              numberOfResources = 0;
    int              timeIndex = 0;
    const int        numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    const bool        usingSubRegions = Region::UsingSubRegions();
    ValueTimelineElement *ptrElement = user.GetValueTimelineElement(timeIndex);
    outfileStream_a << "    USER    TIME";

    if (usingSubRegions == true) {
        outfileStream_a << "    ";
    }

    outfileStream_a << "  OVER         SCORE CRI";
    numberOfResources = ptrElement->GetNumberOfImagedRegions();
    resourceIndex = 0;

    while (resourceIndex < numberOfResources) {
        if (usingSubRegions == true) {
            outfileStream_a << "    ";
        }

        outfileStream_a << "  IMAG";
        ++resourceIndex;
    }

    numberOfResources = ptrElement->GetNumberOfSensors();
    resourceIndex = 0;

    while (resourceIndex < numberOfResources) {
        outfileStream_a << " SEN";
        ++resourceIndex;
    }

    numberOfResources = ptrElement->GetNumberOfMissions();
    resourceIndex = 0;

    while (resourceIndex < numberOfResources) {
        outfileStream_a << " MSN";
        ++resourceIndex;
    }

    numberOfResources = ptrElement->GetNumberOfCollectionTimes();
    resourceIndex = 0;

    while (resourceIndex < numberOfResources) {
        outfileStream_a << "      TTC";
        ++resourceIndex;
    }

    numberOfResources = ptrElement->GetNumberOfMaxPossibleBitsCollected();
    resourceIndex = 0;

    while (resourceIndex < numberOfResources) {
        outfileStream_a << "      BITS";
        ++resourceIndex;
    }

    numberOfResources = ptrElement->GetNumberOfSubPriorities();
    resourceIndex = 0;

    while (resourceIndex < numberOfResources) {
        outfileStream_a << " SUB";
        ++resourceIndex;
    }

    numberOfResources = ptrElement->GetNumberOfResourceFactors();
    resourceIndex = 0;

    while (resourceIndex < numberOfResources) {
        outfileStream_a << " FACTOR";
        ++resourceIndex;
    }

    outfileStream_a << NEW_LINE;

    while (timeIndex < numberOfTimeSteps) {
        outfileStream_a << setw(8) << user.GetDesignator() << " ";
        TimePiece::PrintIndex(outfileStream_a, timeIndex);
        // print the element
        PrintElement(*(user.GetValueTimelineElement(timeIndex)));
        outfileStream_a << NEW_LINE;
        ++timeIndex;
    }

    return;
}

/* ************************************************************************** */

void
TimelineReport::PrintElement(const ValueTimelineElement &element)
{
    int       resourceIndex = 0;
    int numberOfResources = element.GetNumberOfResourceFactors();
    outfileStream_a << "  ";
    Region::PrintRegionNumber(outfileStream_a, element.GetOverflownRegion());
    outfileStream_a << " " << setw(13) << setprecision(3) << element.GetCollectionScore();
    outfileStream_a << " " << setw(3) << element.GetCrisisLevel();
    resourceIndex = 0;

    while (resourceIndex <= numberOfResources) {
        outfileStream_a << "  ";
        Region::PrintRegionNumber(outfileStream_a, element.GetImagedRegion(resourceIndex));
        ++resourceIndex;
    }

    resourceIndex = 0;

    while (resourceIndex <= numberOfResources) {
        outfileStream_a << " " << setw(3) << element.GetSensorNumber(resourceIndex);
        ++resourceIndex;
    }

    resourceIndex = 0;

    while (resourceIndex <= numberOfResources) {
        outfileStream_a << " " << setw(3) << element.GetMissionNumber(resourceIndex);
        ++resourceIndex;
    }

    resourceIndex = 0;

    while (resourceIndex <= numberOfResources) {
        outfileStream_a << " " << setw(8) << setprecision(2)
                        << element.GetCollectionTime(resourceIndex);
        ++resourceIndex;
    }

    resourceIndex = 0;

    while (resourceIndex <= numberOfResources) {
        outfileStream_a << " " << setw(9) << setprecision(2)
                        << element.GetMaxPossibleBitsCollected(resourceIndex);
        ++resourceIndex;
    }

    resourceIndex = 0;

    while (resourceIndex <= numberOfResources) {
        outfileStream_a << " " << setw(3) << element.GetSubPriorityLevel(resourceIndex);
        ++resourceIndex;
    }

    if (element.GetResourceFactors() != NULL) {
        resourceIndex = 0;

        while (resourceIndex <= numberOfResources) {
            outfileStream_a << " " << setw(6) << setprecision(4)
                            << element.GetResourceFactor(resourceIndex);
            ++resourceIndex;
        }
    }

    return;
}

/* ********************************************************************************************** */

TimelineReport::~TimelineReport()
{
    // Do nothing
}
