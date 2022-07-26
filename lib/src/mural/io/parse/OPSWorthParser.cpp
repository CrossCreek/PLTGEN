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
 * OPSWorthParser.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "OPSWorthParser.h"
#include "mural/element/UserVehicle.h"
#include "mural/element/RelaySatellite.h"
#include "mural/modules/lg/Link.h"

#include "general/math/SimpleMath.h"
#include "general/parse/ParseUtility.h"
#include "general/utility/StringEditor.h"
#include "general/data/TimePeriod.h"
#include "general/time/TimePiece.h"

string OPSWorthParser::ANY_RESOURCE = "ANY";
string OPSWorthParser::T10_RESOURCE = "T10";
string OPSWorthParser::T11_RESOURCE = "T11";

OPSWorthParser::OPSWorthParser(const string &worthFile) : worthFileName_a(worthFile)
{
}

/**
 * Determines if the line is a header line in the format
 * <date> <user> <id> <system> <oftimestart> <resource>
 *
 * It checks this by looking for more than two tokens
 */
bool
OPSWorthParser::IsHeaderLine(std::string inputString)
{
    bool isHeader = true;
    istringstream inputStream(inputString);
    string testtoken;
    inputStream >> testtoken;
    inputStream >> testtoken;

    if ( inputStream.eof() ) {
        isHeader = false;
    }

    return isHeader;
}

/**
 * Determines if the line is a worth bin line in the format
 * <seconds> <bin worth>
 *
 * It checks this by looking for exactly two tokens
 */
bool
OPSWorthParser::IsWorthBinLine(std::string inputString)
{
    bool isWorthBin = false;
    istringstream inputStream(inputString);
    string token1, token2;
    inputStream >> token1;
    inputStream >> token2;

    if ( (inputStream.eof()) && (token1.size() > 0) && (token2.size() > 0)) {
        isWorthBin = true;
    }

    return isWorthBin;
}

/**
 * Parses the Worth from the file
 */
void
OPSWorthParser::ParseWorth()
{
    const int numberOfTimesteps = TimePiece::GetNumberOfTimeSteps();
    ifstream worthFile;
    string errorString = "";
    string inputString = "";
    string dateStr;
    string user;
    string requestID;
    string mnemonicPriToken;
    string resource;
    const double simStartOfTime = TimePiece::GetOFTime(0);
    const double simEndOfTime = TimePiece::GetOFTime(numberOfTimesteps);
    Array<double> timeIndexOFTimes(numberOfTimesteps);

    // Initialize an array of oftimes based on time index for reference later
    for(int index = 0; index < numberOfTimesteps; index++) {
        timeIndexOFTimes[index] = TimePiece::ConvertJ2000ToOF(TimePiece::GetJ2000Time(index));
    }

    // Main Processing loop
    try {
        bool validProfile;
        double secondDuration = 0.0;
        double opsOftimeStart = 0.0;
        double opsBinWorth = 0.0;
        double nextOfTime = 0.0;
        double ofIndex = 0.0;
        int timeIndex = 0;
        int nextTimeIndex = 0;
        // Open the input file
        ParseUtility::OpenInputFile(worthFile, worthFileName_a);

        while (getline(worthFile, inputString, NEW_LINE) && (errorString.size() == 0)) {
            istringstream inputStream(inputString);

            // Process the Header line
            if ( IsHeaderLine(inputString) == true ) {
                inputStream >> dateStr;
                inputStream >> user;
                inputStream >> requestID;
                inputStream >> mnemonicPriToken;
                inputStream >> opsOftimeStart;
                inputStream >> resource;
                // Set the ofIndex to the of time start
                ofIndex = opsOftimeStart;
                // Check for a valid mnemonic and do not consider slow class resources
                validProfile = IsValidMnemonic(mnemonicPriToken) && (resource != T10_RESOURCE);

                // A T11 resource should become an "ANY" resource because the slow class relays
                // no longer fly
                if ( resource == T11_RESOURCE ) {
                    resource = ANY_RESOURCE;
                }

                // If a valid Mnemonic and resource is for a specific asset then add it to the list
                // of users with specific resource profiles
                usersWithSpecificProfiles_a.AddUniqueElement(user);
            }
            // Process a worth line
            else if ( (IsWorthBinLine(inputString) == true) && (validProfile == true) &&
                      (ofIndex >= simStartOfTime) && (ofIndex <= simEndOfTime) ) {
                inputStream >> secondDuration;
                inputStream >> opsBinWorth;
                double worthPerSec = opsBinWorth / secondDuration;
                double secondCountdown = secondDuration;
                double ofDifference = 0.0;

                while ( (secondCountdown > 0) && (ofIndex >= simStartOfTime) && (ofIndex <= simEndOfTime)) {
                    timeIndex = TimePiece::GetIndexTime(TimePiece::ConvertOFToJ2000(ofIndex));
                    nextTimeIndex = timeIndex + 1;

                    if ( nextTimeIndex < numberOfTimesteps ) {
                        nextOfTime = timeIndexOFTimes[nextTimeIndex];
                        ofDifference = nextOfTime - ofIndex;

                        if ( ofDifference <= secondCountdown ) {
                            // apply the worth remaining for this timestep (ofDifference)
                            (*GetWorthProfile(user, resource))[timeIndex] += (worthPerSec * ofDifference);
                            // add the ofDifference into the of index time counter
                            ofIndex += ofDifference;
                            // adjust the worth bin's second count (secondCountdown)
                            secondCountdown -= ofDifference;
                        } else {
                            // Add all remaining worth in this bin (secondCountdown)
                            (*GetWorthProfile(user, resource))[timeIndex] += (worthPerSec * secondCountdown);
                            // add the ofDifference into the of Index for the next parsed worth bin
                            ofIndex += secondCountdown;
                            // We used all the remaining worth in this bin
                            secondCountdown = 0;
                        }
                    } else {
                        // break the loop
                        secondCountdown = 0;
                    }
                }
            }
        }

        worthFile.close();
        // Fill in Base (ANY) worth profile
        FillBaseProfile();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "ParseWorth()");
        throw;
    }
}

/**
 * Because some ops requests may not specify a worth profile for 'ANY', this method "fills in"
 * the 'ANY' profile from the max of all the specific profiles provided.  This only happens if
 * 'ANY' is not defined ( = 0.0)
 */
void
OPSWorthParser::FillBaseProfile()
{
    int timeIndex = 0;
    int userIndex = 0;
    int relayIndex = 0;
    int numberOfTimesteps = TimePiece::GetNumberOfTimeSteps();
    int numberOfUsers = userSet_a.Size();
    int numberOfRelays = relaySet_a.Size();
    double maxProfile = 0.0;
    string resource = "";
    string user = "";

    for(userIndex = 0; userIndex < numberOfUsers; userIndex++) {
        user = userSet_a[userIndex];

        for(timeIndex = 0; timeIndex < numberOfTimesteps; timeIndex++) {
            double baseProfile = GetBaseScore(user, timeIndex);

            if ( baseProfile == 0.0 ) {
                maxProfile = 0.0;

                for(relayIndex = 0; relayIndex < numberOfRelays; relayIndex++) {
                    resource = relaySet_a[relayIndex];
                    maxProfile = std::max(maxProfile, GetScore(user, resource, timeIndex));
                }

                (*GetWorthProfile(user, ANY_RESOURCE))[timeIndex] = maxProfile;
            }
        }
    }
}


/**
 * Method to determine if a resource designator has a worth array from the ops parser.
 * true if the designator is in the map
 *
 * @param mapkey the mapkey to look up
 * @return bool
 */
bool
OPSWorthParser::HasAWorthProfile(string user, string resource)
{
    bool hasProfile = false;
    const string mapkey = user + resource;

    if ( worthArrayMap_a.find(mapkey) != worthArrayMap_a.end()) {
        hasProfile = true;
    }

    return ( hasProfile );
}

/**
 * Gets the base score for the user at the given time step.  The resource is ANY_RESOURCE because
 * this is the base worth profile.  It the user get concatenated with "ANY" for the map key
 *
 * @param resource the resource to look up
 * @return Array<double>*
 */
double
OPSWorthParser::GetBaseScore(string user, int timeIndex)
{
    return ( GetScore(user, ANY_RESOURCE, timeIndex) );
}

/**
 * Gets the score for the user, resouce at the given time step.
 *
 * @param resource the resource to look up
 * @return Array<double>*
 */
double
OPSWorthParser::GetScore(string user, string resource, int timeIndex)
{
    double worth = 0.0;
    const string mapkey = user + resource;

    if ( worthArrayMap_a.find(mapkey) != worthArrayMap_a.end()) {
        Array<double>*ptrWorthArray = worthArrayMap_a.find(mapkey)->second;
        worth = (*ptrWorthArray)[timeIndex];
    }

    return ( worth );
}

/**
 * Gets the worth profile list from the map based on the resource designator.  If the list does not exist
 * the method creates a new Array for the worth profile
 *
 * @param resource the resource to look up
 * @return Array<double>*
 */
Array<double>*
OPSWorthParser::GetWorthProfile(string user, string resource)
{
    Array<double>* ptrWorthArray;
    const string mapkey = user + resource;

    if ( worthArrayMap_a.find(mapkey) != worthArrayMap_a.end()) {
        ptrWorthArray = worthArrayMap_a.find(mapkey)->second;
    } else {
        int numberOfTimesteps = TimePiece::GetNumberOfTimeSteps();
        // Init the worth array and store the keys.
        ptrWorthArray = new Array<double>(numberOfTimesteps);

        for(int index = 0; index < numberOfTimesteps; index++) {
            (*ptrWorthArray)[index] = 0.0;
        }

        userSet_a.AddUniqueElement(user);
        relaySet_a.AddUniqueElement(resource);
        worthArrayMap_a.insert(std::pair<string, Array<double>*>(mapkey, ptrWorthArray) );
    }

    return ( ptrWorthArray );
}

/**
 * Method returns whether the user has specific resource profile
 */
bool
OPSWorthParser::HasSpecificResources(std::string user)
{
    return ( usersWithSpecificProfiles_a.ValueExists(user) );
}

/**
 * Computes and gets the resource factor array for the given start/stop times
 *
 * @param user the user designator
 * @param startTimeIndex the start index (request start)
 * @param endTimeIndex the end index (request end)
 *
 * @param Array2D the resource factor array
 */
Array2D<double>*
OPSWorthParser::GetResourceFactorArray(string user, int startTimeIndex, int endTimeIndex)
{
    int duration = endTimeIndex - startTimeIndex + 1;
    int numberOfResources = relaySet_a.Size();
    int timeIndex = 0;
    int relayIndex = 0;
    Array2D<double>* ptrResourceFactor = NULL;
    ptrResourceFactor = new Array2D<double>(duration, numberOfResources, 0.0);

    for(timeIndex = startTimeIndex; timeIndex <= endTimeIndex; timeIndex++) {
        double baseScore = GetBaseScore(user, timeIndex);

        for( relayIndex = 0; relayIndex < numberOfResources; relayIndex++) {
            string resource = relaySet_a[relayIndex];
            double resourceScore = GetScore(user, resource, timeIndex);

            if ( baseScore == 0.0 ) {
                // do not alter the base score
                (*ptrResourceFactor)[timeIndex - startTimeIndex][relayIndex] = 1.0;
            } else {
                (*ptrResourceFactor)[timeIndex - startTimeIndex][relayIndex] = resourceScore / baseScore;
            }
        }
    }

    return (ptrResourceFactor);
}

/**
 * Debug method to dump output to debug file
 */
void
OPSWorthParser::PrintDebugFile(const string &fileName)
{
    int numberOfTimesteps = TimePiece::GetNumberOfTimeSteps();
    double ofTime = 0.0;
    // Open the file
    ofstream worthDebugFile;
    ParseUtility::OpenOutputFile(worthDebugFile, &fileName);
    worthDebugFile << "Total Users  Found : " << userSet_a.Size() << endl;
    worthDebugFile << "Total Relays Found : " << relaySet_a.Size() << endl;
    // Header
    worthDebugFile << "   USER   TIME       OFTIME                   ANY";

    for(int keyIndex = 0; keyIndex < relaySet_a.Size(); keyIndex++) {
        string resource = relaySet_a[keyIndex];

        if ( resource != ANY_RESOURCE ) {
            worthDebugFile << setw(15) << resource;
        }
    }

    worthDebugFile << endl;

    // Data
    for(int userIndex = 0; userIndex < userSet_a.Size(); userIndex++) {
        string user = userSet_a.ElementAt(userIndex);

        for(int index = 0; index < numberOfTimesteps; index++) {
            ofTime = TimePiece::ConvertJ2000ToOF(TimePiece::GetJ2000Time(index));
            worthDebugFile << setw(7) << user;
            worthDebugFile << setw(7) << index;
            worthDebugFile << setw(20) << setprecision(2) << ofTime;
            worthDebugFile << setprecision(3) << setw(15) << (*GetWorthProfile(user, ANY_RESOURCE))[index];

            for(int rsIndex = 0; rsIndex < relaySet_a.Size(); rsIndex++) {
                string relay = relaySet_a.ElementAt(rsIndex);

                if ( relay != ANY_RESOURCE ) {
                    worthDebugFile << setprecision(3) << setw(15) << (*GetWorthProfile(user, relay))[index];
                }
            }

            worthDebugFile << endl;
        }
    }

    worthDebugFile.close();
}

/**
 * Destructor
 */
OPSWorthParser::~OPSWorthParser()
{
    // Delete all the worth arrays from the map
    for(int userIndex = 0; userIndex < userSet_a.Size(); userIndex++) {
        string user = userSet_a.ElementAt(userIndex);

        for(int rsIndex = 0; rsIndex < relaySet_a.Size(); rsIndex++) {
            string relay = relaySet_a.ElementAt(rsIndex);
            delete GetWorthProfile(user, relay);
        }
    }
}
