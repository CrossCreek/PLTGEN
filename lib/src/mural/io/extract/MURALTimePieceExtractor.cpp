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
 * MURALTimePieceExtractor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iostream>
#include <fstream>
#include "MURALTimePieceExtractor.h"
#include "general/parse/ParseUtility.h"
#include "general/data/InputDataElement.h"
#include "general/data/UnitData.h"
#include "general/time/TimePiece.h"
#include "general/array/ListIterator.h"

/* ***************************************************************************** */

MURALTimePieceExtractor::MURALTimePieceExtractor()
{
    return;
}

/* ***************************************************************************** */

MURALTimePieceExtractor::MURALTimePieceExtractor(const MURALTimePieceExtractor &copyMe)
{
    return;
}

/* ***************************************************************************** */

void
MURALTimePieceExtractor::ExtractTimePieceInfo(shared_ptr<InputDataElement> ptrInputDataElement)
{
    int                     secondsPerTimeStep = 0;
    double                  dayBoundaryStart = 0.0;
    double                  dayBoundaryEnd = 0.0;
    string                  startDateString = "";
    string                  startTimeString = "";
    string                  endDateString = "";
    string                  endTimeString = "";
    string                  errorString = "";
    string*                 ptrLeapSecondFileName = NULL;
    Calendar* ptrSimStartCalendar = NULL;
    Calendar* ptrSimEndCalendar = NULL;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>   ptrInputData = NULL;

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        // Parse data from the block
        if (ptrInputData->LabelContainsString("Start Date") == true) {
            startDateString = ptrInputData->GetStringValue();
        } else if (ptrInputData->LabelContainsString("Start Time") == true) {
            startTimeString = ptrInputData->GetStringValue();
        } else if (ptrInputData->LabelContainsString("End Date") == true) {
            endDateString = ptrInputData->GetStringValue();
        } else if (ptrInputData->LabelContainsString("End Time") == true) {
            endTimeString = ptrInputData->GetStringValue();
        } else if (ptrInputData->LabelContainsString("Seconds Per Time Step") == true) {
            secondsPerTimeStep = ptrInputData->GetIntValue();
        } else if (ptrInputData->LabelContainsString("Day Boundary Offset Start") == true) {
            dayBoundaryStart = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Day Boundary Offset End") == true) {
            dayBoundaryEnd = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Leap Second File Name") == true) {
            ptrLeapSecondFileName = new string(ptrInputData->GetStringValue());
        }
    }

    // Error Checking
    if (startDateString.size() == 0) {
        errorString += "Missing 'Start Date' input in TIME_PIECE block";
    }

    if (startTimeString.size() == 0) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString = "Missing 'Start Time' input in TIME_PIECE block";
    }

    if (endDateString.size() == 0) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing 'End Date' input in TIME_PIECE block";
    }

    if (endTimeString.size() == 0) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing 'End Time' input in TIME_PIECE block";
    }

    // Set the calendars
    if (errorString.size() == 0) {
        try {
            ptrSimStartCalendar = new Calendar(startDateString, startTimeString);
            ptrSimEndCalendar = new Calendar(endDateString, endTimeString);
            // Set the values on time piece
            TimePiece::SetSimStartCalendar(ptrSimStartCalendar);
            TimePiece::SetSimEndCalendar(ptrSimEndCalendar);
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "ExtractTimePieceInfo(InputDataElement*)");
            throw;
        }
    }

    if (secondsPerTimeStep < 1) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Seconds Per Time Step' input in TIME_PIECE block";
    } else if ((ptrSimStartCalendar != NULL) && (ptrSimEndCalendar != NULL)) {
        // Compute and set the number of time steps
        int numberOfTimeSteps = (int)((ptrSimEndCalendar->GetJ2000Time() - ptrSimStartCalendar->GetJ2000Time() + (double)secondsPerTimeStep)
                                      / (double)secondsPerTimeStep);
        // Set the values on time piece
        TimePiece::SetSecondsPerTimeStep(secondsPerTimeStep);
        TimePiece::SetNumberOfTimesteps(numberOfTimeSteps);
        // Extract Day boundary data
        //RAGUSA 4/30/2018 - Replaced 'double' with 'int' to avoid possible loss of data warning
        int dayBoundaryStartTimeSteps = UnitData::GetInternalTimeDuration(dayBoundaryStart);
        int dayBoundaryEndTimeSteps = UnitData::GetInternalTimeDuration(dayBoundaryEnd);

        if (dayBoundaryStartTimeSteps < 0) {
            if (errorString.size() > 0) {
                errorString += "\n        ";
            }

            errorString += "Missing/invalid 'Day Boundary Offset Start' input in TIME_PIECE block";
        }

        if (dayBoundaryEndTimeSteps < 0) {
            if (errorString.size() > 0) {
                errorString += "\n        ";
            }

            errorString += "Missing/invalid 'Day Boundary Offset End' input in TIME_PIECE block";
        }

        if (numberOfTimeSteps < 1) {
            if (errorString.size() > 0) {
                errorString += "\n        ";
            }

            errorString += "Zero or negative simulation duration not allowed";
        }

        TimePiece::SetDayBoundaryStart(dayBoundaryStartTimeSteps);
        TimePiece::SetDayBoundaryEnd(dayBoundaryEndTimeSteps);
        //  Extract the Leap second data
        TimePiece::SetLeapSecondFileName(ptrLeapSecondFileName);

        if (ParseUtility::FileExists(*ptrLeapSecondFileName) == true) {
            ExtractLeapSecondFile(*ptrLeapSecondFileName);
        }
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "ExtractTimePieceInfo(InputDataElement*)",
                                 errorString);
    }

    return;
}

/* ***************************************************************************** */

void
MURALTimePieceExtractor::ExtractLeapSecondFile(const std::string &leapSecondFileName)
{
    ifstream                leapSecondFile;
    string                  inputLine;
    int                    leapSecondOF = 0;
    ParseUtility::OpenInputFile(leapSecondFile, leapSecondFileName);
    Array<int> *ptrLeapSecondsArray = new Array<int>();

    while (getline(leapSecondFile, inputLine, NEW_LINE)) {
        istringstream inputStream(inputLine);
        inputStream >> leapSecondOF;
        ptrLeapSecondsArray->AddElement(leapSecondOF);
    }

    if (ptrLeapSecondsArray->Size() == 0) {
        delete ptrLeapSecondsArray;
        ptrLeapSecondsArray = NULL;
    }
    // Set the leap second array on TimePiece
    else {
        TimePiece::SetLeapSecondArray(ptrLeapSecondsArray);
    }
}

/* ***************************************************************************** */

MURALTimePieceExtractor::~MURALTimePieceExtractor()
{
    return;
}
