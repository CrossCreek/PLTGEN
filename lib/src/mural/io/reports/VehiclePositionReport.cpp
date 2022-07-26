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
 * VehiclePositionReport.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "mural/Region.h"
#include "mural/Sun.h"
#include "mural/element/Element.h"
#include "mural/access/AccessPattern.h"
#include "mural/io/reports/VehiclePositionReport.h"


#include "general/exception/NumberFormatException.h"
#include "general/array/Array.h"
#include "general/data/UnitData.h"
#include "general/data/PositionState.h"
#include "general/math/SpaceVector.h"
#include "general/time/TimePiece.h"
#include "general/parse/ParseUtility.h"

VehiclePositionReport::VehiclePositionReport(const std::string &outFile,
        const shared_ptr<MURALTypes::ElementVector> &ptrElementList ) :
    outputFileName_a(outFile), ptrElementList_a(ptrElementList)
{
    return;
}

/* ************************************************************************** */

void
VehiclePositionReport::CreateReport()
{
    ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
    // Add Header
    outfileStream_a << "DATA PRV    TIME       POSITION X      POSITION Y      POSITION Z      ";
    outfileStream_a << "VELOCITY X      VELOCITY Y      VELOCITY Z       DATE     TIME" << NEW_LINE;
    // Iterate and print the elements
    MURALTypes::ElementVector::iterator elementIter;

    if ( ptrElementList_a != NULL ) {
        for( elementIter = ptrElementList_a->begin(); elementIter != ptrElementList_a->end(); ++elementIter) {
            PrintPositions(*(*elementIter));
        }
    }

    // Print the Sun
    Sun::PrintPositionFile(outfileStream_a);
    outfileStream_a.close();
}

/* ************************************************************************** */

void
VehiclePositionReport::PrintPositions(const Element &element)
{
    int           timeIndex = 0;
    shared_ptr<Journey> ptrJourney = element.GetJourney();
    const string  designator = element.GetDesignator();
    const int     numberOfStates = ptrJourney->GetNumberOfPositionStates();
    const int     numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    SpaceVector    positionVector;
    SpaceVector    velocityVector;
    Journey::POSITION_METHOD positionGenerationMethod = ptrJourney->GetGenerationMethod();
    PositionState *ptrPositionState = NULL;

    // if this is a stationary journey and ECI output is desired, each time
    // step must be printed since ECI is time dependent
    if ( (positionGenerationMethod == Journey::STATIONARY) &&
            (UnitData::GetOutputReferenceFrame() == EARTH_CENTER_INERTIAL) ) {
        positionVector = ptrJourney->GetPosition(timeIndex);
        velocityVector = ptrJourney->GetVelocity(timeIndex);

        while (timeIndex < numberOfTimeSteps) {
            ptrPositionState = new PositionState(positionVector, velocityVector);
            outfileStream_a << setw(8) << designator << " ";
            TimePiece::PrintIndex(outfileStream_a, timeIndex);
            ptrPositionState->PrintPositionState(outfileStream_a, timeIndex);
            outfileStream_a << " " << TimePiece::GetClockTimeAndDate(timeIndex);
            outfileStream_a << NEW_LINE;
            delete ptrPositionState;
            ptrPositionState = NULL;
            ++timeIndex;
        }
    } else {
        while (timeIndex < numberOfStates) {
            outfileStream_a << setw(8) << designator;
            outfileStream_a << " ";
            TimePiece::PrintIndex(outfileStream_a, timeIndex);
            (ptrJourney->GetPositionStateAt(timeIndex))->PrintPositionState(outfileStream_a, timeIndex);
            outfileStream_a << " " << TimePiece::GetClockTimeAndDate(timeIndex);
            outfileStream_a << NEW_LINE;
            ++timeIndex;
        }
    }

    return;
}

/* ********************************************************************************************** */

VehiclePositionReport::~VehiclePositionReport()
{
    // Do nothing
}
