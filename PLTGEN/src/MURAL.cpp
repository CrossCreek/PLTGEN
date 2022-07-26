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
 * MURAL.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iostream>
#include <new>
#include <string>

#include "general/exception/CommandLineException.h"

#include "mural/element/DataProvider.h"
#include "mural/element/DataProviderCollection.h"
#include "mural/io/MURALInputProcessor.h"
#include "mural/io/OutputGenerator.h"
#include "mural/modules/pg/PositionGenerator.h"
#include "mural/modules/lg/LinksGenerator.h"
#include "mural/modules/vtg/ValueTimelineGenerator.h"

using namespace std;

/**
 * MURAL is the main operation that controls the command line arguments,
 *   creates all of the inputs (MURALInputProcessor), sets up CPLEX
 *   (Optimizer) if needed, creates and executes all other desired modules
 *   (PositionGenerator, LinksGenerator, ...), creates the output
 *   (OutputGenerator).
 *
 * It contains the main entry point which references the murallib.lib static
 * libary.  This was done to allow for easier reuse of MURAL's componenets in
 * other software and to structure it similar to HASRD.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
int
main(const int argc, const char *argv[])
{
    enum COMMAND_LINE {
        EXECUTABLE_NAME = 0,
        INPUT_FILE_NAME = 1,
        NUMBER_OF_ARGUMENTS = 2
    };
    int									returnStatus = 0;
    MURALInputProcessor					*ptrInputProcessor = NULL;
    OutputGenerator						*ptrOutputGenerator = NULL;
    PositionGenerator					*ptrPositionGenerator = NULL;
    LinksGenerator						*ptrLinksGenerator = NULL;
    ValueTimelineGenerator				*ptrTimelineGenerator = NULL;
    shared_ptr<DataProviderCollection>	ptrDataProviderCollection = NULL;

    try {
        if (argc != NUMBER_OF_ARGUMENTS) {
            throw new CommandLineException(argc, NUMBER_OF_ARGUMENTS);
        }

        TimePiece::SetSystemStartTime();
        ptrInputProcessor = new MURALInputProcessor(string(argv[INPUT_FILE_NAME]));
        ptrDataProviderCollection = ptrInputProcessor->CreateDataProviderList();

        ptrOutputGenerator = new OutputGenerator(ptrDataProviderCollection);
        ptrPositionGenerator = new PositionGenerator();
        ptrPositionGenerator->GeneratePositions(ptrDataProviderCollection);
        ptrOutputGenerator->GenerateOrbitOutput();

        if (MURALInputProcessor::RunLinkPathGenerator() == true) {
            ptrLinksGenerator = new LinksGenerator();
            ptrLinksGenerator->GenerateLinks(ptrDataProviderCollection);
            ptrOutputGenerator->GenerateLinkOutput();

            if (MURALInputProcessor::RunTimelineGenerator() == true) {
                ptrTimelineGenerator = new ValueTimelineGenerator();
                ptrTimelineGenerator->GenerateValueTimelines(ptrDataProviderCollection);
                ptrOutputGenerator->GenerateValueTimelineOutput();

            } //END: if (MURALInputProcessor::RunTimelineGenerator() == true)
        } //END: if (MURALInputProcessor::RunLinkPathGenerator() == true)

        ptrOutputGenerator->GenerateMainOutput(string(argv[EXECUTABLE_NAME]));
        ptrOutputGenerator->GenerateRegionsInViewOutputFile();
    } catch (Exception *ptrError) {
        ptrError->AddMethod("MURAL", "main(const int, const char**)");
        returnStatus = ptrError->ReportError();
        delete ptrError;
        ptrError = NULL;
    } catch (bad_alloc& error) {
        cout << "ERROR  : Out Of Memory!" << NEW_LINE;
        returnStatus = -1;
    }

    if (ptrPositionGenerator != NULL) {
        delete ptrPositionGenerator;
        ptrPositionGenerator = NULL;
    }

    if (ptrLinksGenerator != NULL) {
        delete ptrLinksGenerator;
        ptrLinksGenerator = NULL;
    }

    if (ptrTimelineGenerator != NULL) {
        delete ptrTimelineGenerator;
        ptrTimelineGenerator = NULL;
    }

    if (ptrOutputGenerator != NULL) {
        delete ptrOutputGenerator;
        ptrOutputGenerator = NULL;
    }

    if (ptrInputProcessor != NULL) {
        delete ptrInputProcessor;
        ptrInputProcessor = NULL;
    }

    return(returnStatus);
}
