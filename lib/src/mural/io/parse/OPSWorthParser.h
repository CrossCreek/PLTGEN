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
 * OPSWorthParser.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef OPS_WORTH_PARSER_H
#define OPS_WORTH_PARSER_H "OPSWorthParser"

#include <fstream>
#include <string>
#include <map>


#include "general/array/Array2D.h"
#include "general/array/ArrayOfLinkLists.h"
#include "general/array/ArrayOfPointers.h"

using namespace std;

class CommunicationServiceRequest;
class ConstrainedUserSet;
class Calendar;
class DataProvider;
class InputDataElement;
class Optimizer;
class TimePeriod;
class UserVehicle;
class RelaySatellite;

/**
 * Class which parses an operational worth file to associate with an operational
 * request file for each user.  It computes and overrides the value timeline with the
 * operational worth.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class OPSWorthParser
{
public:



    explicit OPSWorthParser(const string &worthFile);

    ~OPSWorthParser();

    void ParseWorth();
    void PrintDebugFile(const string &fileName);

    bool HasAWorthProfile(string user, string resource);
    bool HasSpecificResources(string user);

    double GetBaseScore(string user, int timeIndex);
    double GetScore(string user, string resource, int timeIndex);

    Array2D<double>* GetResourceFactorArray(string user, int startTimeIndex, int endTimeIndex);
    Array<double>* GetWorthProfile(string user, string resource);


protected:

private:
    static inline string GetClassName();

    void FillBaseProfile();

    bool IsHeaderLine(string inputString);
    bool IsWorthBinLine(string inputString);
    bool IsValidMnemonic(string mnemonicPriority);

    string worthFileName_a;
    std::map<string, Array<double>*> worthArrayMap_a;

    Array<string> userSet_a;
    Array<string> relaySet_a;
    Array<string> usersWithSpecificProfiles_a;

    static string ANY_RESOURCE;
    static string T10_RESOURCE;
    static string T11_RESOURCE;
};

inline
string
OPSWorthParser::GetClassName()
{
    return(OPS_WORTH_PARSER_H);
}

#endif

