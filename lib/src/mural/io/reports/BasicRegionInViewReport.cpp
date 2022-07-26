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
 * BasicRegionInViewReport.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "BasicRegionInViewReport.h"

#include "mural/access/AccessPattern.h"
#include "mural/Region.h"

#include "general/exception/NumberFormatException.h"
#include "general/array/Array.h"
#include "general/data/UnitData.h"
#include "general/data/PositionState.h"
#include "general/math/SpaceVector.h"
#include "general/time/TimePiece.h"
#include "general/parse/ParseUtility.h"


BasicRegionInViewReport::BasicRegionInViewReport(const std::string &outFile,
        const std::shared_ptr<AccessPattern> &ptrAccessPattern,
        const std::shared_ptr<std::vector<PositionState>> &ptrPositionStates) :
    outputFileName_a(outFile), ptrAccessPattern_a(ptrAccessPattern), ptrPositionStates_a(ptrPositionStates)
{
    return;
}

/* ************************************************************************** */

void
BasicRegionInViewReport::CreateReport()
{
    if (ParseUtility::FileExists(outputFileName_a)) {
        ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
        // Add Header
        outfileStream_a << "    NAME    TIME  SEN   MODE   TYPE REGIONS" << NEW_LINE;
        // Print the "standard regions in view"
        PrintRegionsInView();
        outfileStream_a.close();
    }
}

/* ************************************************************************** */

void
BasicRegionInViewReport::PrintRegionsInView()
{
    LinkList<Region> *ptrRegionsInViewList = NULL;
    Array<double>    *ptrRegionNumbers = new Array<double>();
    Region           *ptrRegion = NULL;

    try {
        int timeIndex = 0;

        for (timeIndex = 0; timeIndex < TimePiece::GetNumberOfTimeSteps(); timeIndex++) {
            const PositionState *positionState = &(ptrPositionStates_a->at(timeIndex));

            if ( positionState == NULL ) {
                throw new NumberFormatException(GetClassName(), "PrintRegionsInView()", "Missing Positions/Velocity at time " + timeIndex);
            }

            ptrAccessPattern_a->GetAllContainedRegions(ptrRegionsInViewList,
                    (*positionState->GetPosition()), (*positionState->GetVelocity()), AccessPattern::LARGEST_PATTERN);
            //JOE:
            ptrRegion = ptrRegionsInViewList->Retrieve();

            while (ptrRegion != NULL) {
                ptrRegionNumbers->InsertUniqueElement(ptrRegion->GetFullRegionNumber());
                ptrRegion = ptrRegionsInViewList->Retrieve();
            }
        }

        if (ptrRegionNumbers->Size() > 0) {
            std::string patternName = AccessPattern::GetPatternTypeString(ptrAccessPattern_a->GetPatternType());
            PrintRegionsInView(patternName, timeIndex, DEFAULT_STRING,
                               DEFAULT_STRING, DEFAULT_STRING, ptrRegionNumbers);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "PrintAllRegionsInView()");
        throw;
    }

    return;
}

/* ************************************************************************** */

void
BasicRegionInViewReport::PrintRegionsInView(const string &name,
        const int &timeIndex,
        const std::string &sensorString,
        const std::string &modeString,
        const std::string &typeString,
        const Array<double> *ptrRegionNumbers)
{
    const int   numberOfRegions = ptrRegionNumbers->Size();

    try {
        int regionIndex = 0;
        outfileStream_a << setw(8) << name;
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

/**
 * Destructor
 */
BasicRegionInViewReport::~BasicRegionInViewReport()
{
    // Do nothing
}

