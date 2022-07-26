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
 * OutputGenerator.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef OUTPUT_GENERATOR_H
#define OUTPUT_GENERATOR_H "OutputGenerator"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "mural/io/satsim/SATSIMData.h"
#include "mural/MURALTypes.h"

#include "general/array/Array2D.h"
#include "general/array/ArrayOfLinkLists.h"
#include "general/utility/Constant.h"
#include "mural/io/reports/MRLReport.h"

class Antenna;
class CollectionElement;
class DataProvider;
class GroundStation;
class InputDataElement;
class RelaySatellite;
class RevisitGroup;
class UserVehicle;
class DataProviderCollection;
class ElementCollection;

/**
 * Class which handles the writing of output.  The methods on this class use the "report"
 * classes to write out output to text.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class OutputGenerator
{
public:

    OutputGenerator();

    explicit OutputGenerator(const shared_ptr<DataProviderCollection> &ptrDataProviderList);

    OutputGenerator(const OutputGenerator &copyMe);
    virtual ~OutputGenerator();

    OutputGenerator& operator =  (const OutputGenerator &copyMe);
    bool             operator == (const OutputGenerator &compareMe) const;
    bool             operator != (const OutputGenerator &compareMe) const;

    static void   ExtractOutputParameterInfo(const shared_ptr<InputDataElement> &ptrElement);

    /**
     * Method to display the common Satisfaction header used in the mrl MURALSAT section and the
     * multiple day averaging files.
     *
     * @param scoreWorthStr A description string on whether score or worth is being reported
     * @param outfile the out stream to write to.
     */
	static void   PrintCrisisData(ofstream &outfile,MURALTypes::UserVector &printUserList,
		                          int userVehicleCount);
    static void   PrintCrisisDataNumbers(ofstream &outfile,
                                         int inview);
    static void   SetOutputFileNames(const shared_ptr<InputDataElement> &ptrInputDataElement);

    void   GenerateOrbitOutput();
    void   GenerateLinkOutput();
    void   GenerateValueTimelineOutput();
    void   GenerateMainOutput(const string &executableName);
    void   GenerateRegionsInViewOutputFile();

    static void   PrintAttributes(ofstream &outfile);

    static inline char    GetUserCharacter(int userNumber, int minimumPrintUserNumber = 1, int maximumPrintUserNumber = MRLReport::MAXIMUM_PRINT_USERS_s);
    static inline char    GetRelayCharacter(int relayNumber);

protected:

private:

    static void   PrintOutputFiles(ofstream &outfile);

    void  GenerateCrosslinkOutputFile();
    void  GenerateDownlinkOutputFile();
    void  GenerateDirectDownlinkOutputFile();
    void  GenerateMultihopLinkOutputFile();
    void  GenerateSelfRelayLinkOutputFile();
    void  GenerateVehiclePositionOutputFile();
    void  GenerateUPAConjunctionOutputFile();
    void  GenerateRFConjunctionOutputFile();
    void  GenerateValueTimelineOutputFile();
    void  GenerateRevolutionTimeOutputFile();
    void  GenerateUpdatedOrbitOutputFile();

    void  SetupDataProviderLists(const shared_ptr<DataProviderCollection> &ptrDataProviderCollection);

    static inline string GetClassName();

    shared_ptr<MURALTypes::ElementCollectionVector> ptrUserGroupList_a;
    shared_ptr<MURALTypes::ElementCollectionVector> ptrUserTypeList_a;
    shared_ptr<MURALTypes::ElementCollectionVector> ptrUserSystemList_a;
    shared_ptr<DataProviderCollection> ptrDataProviderCollection_a;
};

inline
char
OutputGenerator::GetUserCharacter(int userNumber, int minimumPrintUserNumber, int maximumPrintUserNumber)
{
    char userCharacter = '@';

    if ((userNumber >= minimumPrintUserNumber) && (userNumber <= maximumPrintUserNumber)) {
        int tempUserNumber = 0;
        tempUserNumber = (userNumber - minimumPrintUserNumber + 1);

        if (tempUserNumber < 10) {
            userCharacter = tempUserNumber + '0';
        } else {
            userCharacter = tempUserNumber + '7';
        }
    }

    return(userCharacter);
}

inline
char
OutputGenerator::GetRelayCharacter(int relayNumber)
{
    char relayCharacter = BLANK_CHARACTER;

    if (relayNumber < 10) {
        relayCharacter = relayNumber + '0';
    } else {
        relayCharacter = relayNumber + 'W';
    }

    return(relayCharacter);
}

inline
string
OutputGenerator::GetClassName()
{
    return(OUTPUT_GENERATOR_H);
}

#endif
