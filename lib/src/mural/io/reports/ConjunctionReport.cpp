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
 * ConjunctionReport.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "ConjunctionReport.h"
#include "mural/element/DataProvider.h"
#include "mural/element/RelaySatellite.h"
#include "mural/element/GroundStation.h"
#include "mural/modules/lg/ConjunctionPeriod.h"


#include "general/array/Array.h"
#include "general/array/ListIterator.h"
#include "general/utility/StringEditor.h"
#include "general/data/UnitData.h"
#include "general/time/TimePiece.h"
#include "general/parse/ParseUtility.h"

ConjunctionReport::ConjunctionReport(const std::string &outFile ) :
    outputFileName_a(outFile)
{
    return;
}

/* ************************************************************************** */

void
ConjunctionReport::CreateUPAConjReport(const shared_ptr<MURALTypes::RelayVector> &ptrRelayList)
{
    shared_ptr<RelaySatellite>       ptrRelay = NULL;
    ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
    outfileStream_a << "   RELAY    USER1    USER2   START     END  SENDERS   TYPE" << NEW_LINE;
    MURALTypes::RelayVector::iterator relayIter;

    for( relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
        ptrRelay = (*relayIter);
        PrintConjunctionPeriods(ptrRelay);
    }

    outfileStream_a.close();
    return;
}

/* ************************************************************************** */

void
ConjunctionReport::CreateRFConjReport(const shared_ptr<MURALTypes::GroundVector> &ptrGroundList)
{
    shared_ptr<GroundStation>       ptrGround = NULL;
    ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
    outfileStream_a << "  GROUND   RELAY1   RELAY2   START     END  SENDERS   TYPE" << NEW_LINE;
    MURALTypes::GroundVector::iterator groundIter;

    for( groundIter = ptrGroundList->begin(); groundIter != ptrGroundList->end(); ++groundIter) {
        ptrGround = (*groundIter);
        PrintConjunctionPeriods(ptrGround);
    }

    outfileStream_a.close();
    return;
}

/* ************************************************************************** */

void
ConjunctionReport::PrintConjunctionPeriods(const shared_ptr<DataProvider> &ptrDataProvider)
{
    ConjunctionPeriod   *ptrConjunctionPeriod;
    ListIterator<ConjunctionPeriod> conjunctionNBListIter(ptrDataProvider->GetNBConjunctionPeriodList());
    ListIterator<ConjunctionPeriod> conjunctionWBListIter(ptrDataProvider->GetWBConjunctionPeriodList());
    ptrConjunctionPeriod = conjunctionNBListIter.First();

    while (ptrConjunctionPeriod != NULL) {
        PrintConjunctionPeriod(*ptrConjunctionPeriod);
        outfileStream_a << "     NB" << NEW_LINE;
        ptrConjunctionPeriod = conjunctionNBListIter.Next();
    }

    ptrConjunctionPeriod = conjunctionWBListIter.First();

    while (ptrConjunctionPeriod != NULL) {
        PrintConjunctionPeriod(*ptrConjunctionPeriod);
        outfileStream_a << "     WB" << NEW_LINE;
        ptrConjunctionPeriod = conjunctionWBListIter.Next();
    }

    return;
}

/* ************************************************************************** */

void
ConjunctionReport::PrintConjunctionPeriod(const ConjunctionPeriod &period)
{
    outfileStream_a << setw(8) << period.GetReceiverDesignator() << " ";
    outfileStream_a << setw(8) << period.GetSender1Designator() << " ";
    outfileStream_a << setw(8) << period.GetSender2Designator() << " ";
    TimePiece::PrintIndex(outfileStream_a, period.GetStartTime());
    outfileStream_a << " ";
    TimePiece::PrintIndex(outfileStream_a, period.GetEndTime());
    outfileStream_a << " " << setw(8) << period.GetNumberOfSimultaneousSenders();
    return;
}

/* ************************************************************************** */
ConjunctionReport::~ConjunctionReport()
{
    // Do nothing
}
