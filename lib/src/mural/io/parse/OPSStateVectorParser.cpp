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
 * OPSStateVectorParser.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "OPSStateVectorParser.h"


#include "general/parse/ParseUtility.h"
#include "general/time/Calendar.h"
#include "general/utility/StringEditor.h"
#include "general/time/TimePiece.h"
#include "general/math/SpaceVector.h"

shared_ptr<OPSStateVectorParser> OPSStateVectorParser::ptrOPSStateVectorParser_s    = NULL;

/* ********************************************************************************* */

OPSStateVectorParser::OPSStateVectorParser(const string &fileName)
{
    ptrStateVectorFileName_a = shared_ptr<string>(new string(fileName));
    ptrEpochJ2000Map_a = shared_ptr<map<string, double>>(new map<string, double>());
    ptrPositionMap_a = shared_ptr<map<string, SpaceVector>>(new std::map<string, SpaceVector>());
    ptrVelocityMap_a = shared_ptr<map<string, SpaceVector>>(new std::map<string, SpaceVector>());
}

/* ********************************************************************************* */

string
OPSStateVectorParser::FindEpochDate()
{
    string         epochDateString = "";
    string         inputString = "";
    string         dateString = "";
    string         dayString = "";
    string         monthString = "";
    string         yearString = "";
    string         designator = "";
    string         ofTimeString = "";
    string         timeString = "";
    string         dateStringFormatted = "";
    ifstream       stateVectorFile;
    Calendar       *ptrCalendar = NULL;
    Calendar       *ptrTargetCalendar = NULL;

    try {
        bool foundEpoch = false;
        double j2000Time = 0;
        // Find the target epoch day ( the mean day of the start and end)
        j2000Time  = (TimePiece::GetEndCalendar().GetJ2000Time() - TimePiece::GetStartCalendar().GetJ2000Time()) / 2;
        j2000Time += TimePiece::GetStartCalendar().GetJ2000Time();
        ptrTargetCalendar = new Calendar(j2000Time);
        // Open the input file
        ParseUtility::OpenInputFile(stateVectorFile, *ptrStateVectorFileName_a);

        while ( (getline(stateVectorFile, inputString, NEW_LINE)) && (foundEpoch == false) ) {
            if ( inputString.size() > 0 ) {
                istringstream inputStream(inputString);
                // Get the date string and the parameters of it
                inputStream >> dateString;
                // Get the date string parameters
                dayString = dateString.substr(0,2);
                monthString = dateString.substr(2,3);
                yearString = "20" + dateString.substr(5,2);
                dateStringFormatted  = yearString + "/";
                dateStringFormatted += StringEditor::ConvertToString(Date::ConvertMonth(monthString,Date::SHORT_FORM)) + "/";
                dateStringFormatted += dayString;
                inputStream >> designator >> ofTimeString >> timeString;
                ptrCalendar = new Calendar(dateStringFormatted, timeString);
                epochDateString = dateString;

                if ( (ptrCalendar->GetDay() == ptrTargetCalendar->GetDay()) &&
                        (ptrCalendar->GetMonth() == ptrTargetCalendar->GetMonth()) &&
                        (ptrCalendar->GetYear() == ptrTargetCalendar->GetYear())) {
                    foundEpoch = true;
                }

                delete ptrCalendar;
                ptrCalendar = NULL;
            }
        }

        delete ptrTargetCalendar;
        ptrTargetCalendar = NULL;
        stateVectorFile.close();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ParseStateVectorFile()");
        throw;
    }

    return (epochDateString);
}

/* ********************************************************************************* */

void
OPSStateVectorParser::ParseStateVectorFile()
{
    string         inputString = "";
    string         dateString = "";
    string         designator = "";
    string         ofTimeString = "";
    string         timeString = "";
    string         nextToken = "";
    string         dayString = "";
    string         monthString = "";
    string         yearString = "";
    string         dateStringFormatted = "";
    ifstream       stateVectorFile;
    SpaceVector*   ptrPosition = NULL;
    SpaceVector*   ptrVelocity = NULL;
    Calendar*      ptrEpoch = NULL;
    const ANGLE_UNIT      inputAngle = UnitData::GetInputAngleUnits();
    const DISTANCE_UNIT   inputDistance = UnitData::GetInputDistanceUnits();

    try {
        double positionX = 0.0;
        double positionY = 0.0;
        double positionZ = 0.0;
        double velocityX = 0.0;
        double velocityY = 0.0;
        double velocityZ = 0.0;
        double j2000Time = 0.0;
        // Find the Epoch we are looking for
        string epochString = FindEpochDate();
        // Open the input file
        ParseUtility::OpenInputFile(stateVectorFile, *ptrStateVectorFileName_a);

        while ( getline(stateVectorFile, inputString, NEW_LINE) ) {
            if ( inputString.size() > 0 ) {
                istringstream inputStream(inputString);
                // Get the date string and the parameters of it
                inputStream >> dateString;

                if ( epochString == dateString ) {
                    inputStream >> designator >> ofTimeString >> timeString;
                    // Get the epoch parameters and get j2000 time
                    dayString = dateString.substr(0,2);
                    monthString = dateString.substr(2,3);
                    yearString = "20" + dateString.substr(5,2);
                    dateStringFormatted  = yearString + "/";
                    dateStringFormatted += StringEditor::ConvertToString(Date::ConvertMonth(monthString,Date::SHORT_FORM)) + "/";
                    dateStringFormatted += dayString;
                    ptrEpoch = new Calendar(dateStringFormatted, timeString);
                    j2000Time = ptrEpoch->GetJ2000Time();
                    // Read the position and velocity data
                    inputStream >> nextToken;
                    positionX = StringEditor::ConvertToDouble(nextToken);
                    inputStream >> nextToken;
                    positionY = StringEditor::ConvertToDouble(nextToken);
                    inputStream >> nextToken;
                    positionZ = StringEditor::ConvertToDouble(nextToken);
                    inputStream >> nextToken;
                    velocityX = StringEditor::ConvertToDouble(nextToken);
                    inputStream >> nextToken;
                    velocityY = StringEditor::ConvertToDouble(nextToken);
                    inputStream >> nextToken;
                    velocityZ = StringEditor::ConvertToDouble(nextToken);
                    ptrPosition =  new SpaceVector(positionX, positionY, positionZ, EARTH_CENTER_INERTIAL, inputAngle, inputDistance);
                    ptrVelocity =  new SpaceVector(velocityX, velocityY, velocityZ, EARTH_CENTER_INERTIAL, inputAngle, inputDistance);
                    // Poulate the maps
                    ptrEpochJ2000Map_a->insert( std::pair<string, double>(designator, j2000Time) );
                    ptrPositionMap_a->insert( std::pair<string, SpaceVector>(designator, (*ptrPosition)) );
                    ptrVelocityMap_a->insert( std::pair<string, SpaceVector>(designator, (*ptrVelocity)) );
                    // Delete the temporary pointers
                    delete ptrEpoch;
                    delete ptrPosition;
                    delete ptrVelocity;
                }
            }
        }

        stateVectorFile.close();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ParseStateVectorFile()");
        throw;
    }
}

/* ********************************************************************************* */

SpaceVector
OPSStateVectorParser::GetPosition(const string &designator) const
{
    SpaceVector position;

    if ( ptrPositionMap_a->find(designator) != ptrPositionMap_a->end()) {
        position = ptrPositionMap_a->find(designator)->second;
    }

    return ( position );
}

/* ********************************************************************************* */

SpaceVector
OPSStateVectorParser::GetVelocity(const string &designator) const
{
    SpaceVector velocity;

    if ( ptrVelocityMap_a->find(designator) != ptrVelocityMap_a->end()) {
        velocity = ptrVelocityMap_a->find(designator)->second;
    }

    return( velocity );
}

/* ********************************************************************************* */

double
OPSStateVectorParser::GetJ2000Epoch(const string &designator) const
{
    double returnValue = -1.0;

    if ( ptrEpochJ2000Map_a->find(designator) != ptrEpochJ2000Map_a->end()) {
        returnValue = ptrEpochJ2000Map_a->find(designator)->second;
    }

    return( returnValue );
}

/* ********************************************************************************* */

OPSStateVectorParser::~OPSStateVectorParser()
{
    // Do nothing, all shared_ptrs
}
