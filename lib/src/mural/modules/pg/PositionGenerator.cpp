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
 * PositionGenerator.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "PositionGenerator.h"
#include "mural/Earth.h"
#include "mural/io/FileNames.h"
#include "mural/element/UserVehicle.h"
#include "mural/element/DataProviderCollection.h"
#include "mural/modules/pg/PositionUtility.h"
#include "mural/modules/pg/Trek.h"
#include "mural/modules/pg/Orbit.h"


#include "general/math/SimpleMath.h"
#include "general/math/SpaceVector.h"
#include "general/array/ListIterator.h"
#include "general/parse/ParseUtility.h"
#include "general/data/PositionState.h"
#include "general/utility/Constant.h"

PositionGenerator::PositionGenerator()
{
    return;
}

PositionGenerator::PositionGenerator(const PositionGenerator &copyMe)
{
    return;
}

void
PositionGenerator::GeneratePositions(const shared_ptr<DataProviderCollection> &ptrDataProviderCollection)
{
    string                                      designator = "";
    shared_ptr<Journey>                         ptrJourney = NULL;
    Orbit                                      *ptrOrbit = NULL;
    shared_ptr<MURALTypes::DataProviderVector>  ptrDataProviderList = ptrDataProviderCollection->GetDataProviderListPtr();
    shared_ptr<DataProvider>                    ptrDataProvider = NULL;
    shared_ptr<UserVehicle>                     ptrTrailVehicle = NULL;

    try {
        vector<shared_ptr<DataProvider>>::iterator dataProviderIter;

        for( dataProviderIter = ptrDataProviderList->begin(); dataProviderIter != ptrDataProviderList->end(); ++dataProviderIter) {
            bool dataCollector;
            ptrDataProvider = (*dataProviderIter);
            ptrJourney = ptrDataProvider->GetJourney();
            designator = ptrDataProvider->GetDesignator();
            dataCollector = false;
            shared_ptr<UserVehicle> ptrUserVehicle(dynamic_pointer_cast<UserVehicle>(ptrDataProvider));

            if (ptrUserVehicle != NULL) {
                dataCollector = true;
            }

            DeterminePositionData(ptrJourney, designator, dataCollector);

            // if this is an orbiting data collector, determine it's revolutions
            if (ptrUserVehicle != NULL) {
                ptrTrailVehicle = ptrUserVehicle->GetTrailVehicle();

                if (ptrTrailVehicle != NULL) {
                    ptrJourney = ptrTrailVehicle->GetJourney();
                    designator = ptrTrailVehicle->GetDesignator();
                    DeterminePositionData(ptrJourney, designator, dataCollector);
                }
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GenerateOrbits(LinkList<DataProvider>*)");
        throw;
    }

    return;
}

void
PositionGenerator::DeterminePositionData(const shared_ptr<Journey> &ptrJourney,
        const string &designator,
        const bool &dataCollector)
{
    shared_ptr<Orbit> ptrOrbit(dynamic_pointer_cast<Orbit>(ptrJourney));
    shared_ptr<Trek>  ptrTrek(dynamic_pointer_cast<Trek>(ptrJourney));

    try {
        if (ptrJourney->GetGenerationMethod() == Journey::READ_MURAL) {
            cout << "PositionGenerator: Retrieving Position Data For " << designator << NEW_LINE;
            PositionUtility::RetrievePositionData(FileNames::GetFileName(FileNames::POSITION_INPUTFILE), designator, ptrJourney);
            ptrJourney->CheckPositionStates(designator);

            // if this is an Orbit and positions are READ in, determine the classical elements
            // so they are accurate when determining coordinate frames (for a HEO RelaySatellite)
            if (ptrOrbit != NULL) {
                ptrOrbit->CalculateClassicalElements();
            }
        } else if (ptrJourney->GetGenerationMethod() == Journey::READ_P470) {
            cout << "PositionGenerator: Retrieving P470 Position Data For " << designator << NEW_LINE;
            PositionUtility::RetrieveP470PositionData(FileNames::GetFileName(FileNames::POSITION_P470_INPUTFILE), designator, ptrJourney);
            ptrJourney->CheckPositionStates(designator);

            // if this is an Orbit and positions are READ in, determine the classical elements
            // so they are accurate when determining coordinate frames (for a HEO RelaySatellite)
            if (ptrOrbit != NULL) {
                ptrOrbit->CalculateClassicalElements();
            }
        } else if (ptrOrbit != NULL) {
            cout << "PositionGenerator: Generating Ephemeris For " << designator << NEW_LINE;
            // The Calculate Perturbations used to be done during parsing and constructing of the orbit
            ptrOrbit->CalculatePerturbations();
            GenerateEphemeris(ptrOrbit);
        } else if (ptrTrek != NULL) {
            // This method retrieves the trek from the trek file (moving)
            // or sets the trek's node for each timestep (stationary)
            ptrTrek->CreateTrek();

            // Populate the PositionStates for the Trek
            if (ptrTrek->GetGenerationMethod() == Journey::STATIONARY) {
                cout << "PositionGenerator: Generating Stationary Trek For " << designator << NEW_LINE;
                PositionUtility::GenerateStationaryTrek(ptrTrek);
            } else if (ptrTrek->GetGenerationMethod() == Journey::MOVING) {
                cout << "PositionGenerator: Generating Moving Trek For " << designator << NEW_LINE;
                PositionUtility::GenerateMovingTrek(ptrTrek);
            }
        }

        // if this is an orbiting data collector, determine it's revolutions
        if ( (dataCollector == true) && (ptrOrbit != NULL) ) {
            ptrOrbit->DetermineRevolutionTimes();
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "DeterminePositionData(Journey*, const string&)");
        throw;
    }

    return;
}


void
PositionGenerator::GenerateEphemeris(const shared_ptr<Orbit> &ptrOrbit)
{
    const int                 numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    shared_ptr<PositionState> ptrPositionState = NULL;

    try {
        int timeIndex = 0;

        while (timeIndex < numberOfTimeSteps) {
            double j2000Time;
            j2000Time = TimePiece::GetJ2000Time(timeIndex);
            ptrPositionState = PositionUtility::GeneratePositionState(ptrOrbit, j2000Time);
            ptrOrbit->SetPositionState(timeIndex, ptrPositionState);
            ptrPositionState = NULL;
            ++timeIndex;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GenerateEphemeris(Orbit*)");
        throw;
    }

    return;
}

/* **************************************************************************************** */

PositionGenerator::~PositionGenerator()
{
    return;
}
