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
 * MRLReport.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef MRL_REPORT_H
#define MRL_REPORT_H "MRLReport"

#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "mural/MURALTypes.h"

using namespace std;

template<class Type> class LinkList;
template<class Type> class ArrayOfLinkLists;
template<class Type> class Array2D;
template<class Type> class Array;
class Antenna;
class UserVehicle;
class RelaySatellite;
class GroundStation;
class RevisitGroup;
class ElementCollection;

/**
 * Class to write the mrl.  This is the main output file of mural and contains an overview
 * of all the data that mural generated during the run.  It is a large file with lots of specific
 * sections with their own formats.
 *
 * @author Brian Fowler
 * @date 09/12/11
 */
class MRLReport
{
public:

    MRLReport(
        const string &outFile,
        const shared_ptr<MURALTypes::UserVector> &userList,
        const shared_ptr<MURALTypes::RelayVector> &relayList,
        const shared_ptr<MURALTypes::GroundVector> &groundList,
        const shared_ptr<MURALTypes::ElementCollectionVector> &ptrUserGroupList,
        const shared_ptr<MURALTypes::ElementCollectionVector> &ptrUserTypeList,
        const shared_ptr<MURALTypes::ElementCollectionVector> &ptrUserSystemList
    );

    ~MRLReport();

    /**
    * Creates the report (the mrl file).  If there are > maximum number of users
    * per one mrl file then this method will create multiple mrls.  A sub list of
    * users is created (print users) and is passed to the output sub routines to write to
    * the current mrl.
    *
    * This break out capability is typically never used.
    *
    * @param executableName the exe for reporting
    */
    void CreateReport(const string &executableName);

    static const int     MAXIMUM_PRINT_USERS_s;

protected:

private:
    static inline string GetClassName();

    void CreateRFIdentificationArray();

    /**
     * Method which sets the structure of the mrl.  Creates the ofstream for the current set of users.
     *
     * @param &executableName the executable used (to be printed in mrl)
     * @param &ptrOutputFileName the current output mrl name (for breaking up mrl if > maximum number of users)
     * @param LinkList<UserVehicle>* the current user list to print (for breaking up mrl if > maximum number of users)
     * @param iteration the current mrl iteration (for breaking up mrl if > maximum number of users)
     */
    void PrintMainOutput(
        const string &executableName,
        const string *ptrOutputFileName,
        MURALTypes::UserVector &printUserList,
        int iteration
    );

    void PrintOutputHeader(
        ofstream &outfile,
        const string &executableName,
        int iteration
    );

    /**
     * Writes the Input section of the mrl
     *
     * @param &outfile the output file stream
     * @param LinkList<UserVehicle>* the user list to print
     */
    void PrintInputData(
        ofstream &outfile,
        MURALTypes::UserVector &printUserList
    );

    /**
     * Writes the MURALLP section
     *
     * @param &outfile the output file stream
     * @param LinkList<UserVehicle>* the user list to print
     */
    void PrintLinkPaths(ofstream &outfile, MURALTypes::UserVector &printUserList);

    void PrintLinkPaths(
        ofstream &outfile,
        const UserVehicle &user
    ) const;

    void PrintLinkPathInviews(
        ofstream &outfile,
        const UserVehicle &user,
        const int &timeIndex,
        const bool &hasTransmitSRA
    ) const;

    void PrintLinkPathResourceNumbers(
        ofstream &outfile,
        const UserVehicle &user,
        const Array<int> *ptrDirectDownlinkSpaces,
        const Array<int> *ptrCrosslinkSpaces,
        const Array<int> *ptrSelfRelayLinkSpaces
    ) const;

    void PrintLinkPathUnderline(
        ofstream &outfile,
        const Array<int> *ptrDirectDownlinkSpaces,
        const Array<int> *ptrCrosslinkSpaces,
        const Array<int> *ptrSelfRelayLinkSpaces
    ) const;

    int PrintLinkPathHeader(
        ofstream &outfile,
        const UserVehicle &user,
        const int &numberOfScores,
        const bool &hasCrisis
    ) const;

    int PrintLinkPathResources(
        ofstream &outfile,
        const UserVehicle &user,
        Array<int> *&ptrDirectDownlinkSpaces,
        Array<int> *&ptrCrosslinkSpaces,
        Array<int> *&ptrSelfRelayLinkSpaces
    ) const;

    Array2D<double>* DetermineLinkPathScoreProfile(const UserVehicle &user, const int &numberOfValues) const;

    /**
     * Writes the MURALFORWARD section
     *
     * @param &outfile the output file stream
     * @param LinkList<UserVehicle>* the user list to print
     */
    void PrintForwardResourceActivityData(ofstream &outfile, MURALTypes::UserVector &printUserList);

    void PrintUserTransmitAntennaUsage(
        ofstream &outfile,
        const UserVehicle &user,
        const int &timeIndex
    ) const;

    /**
     * Writes the MURALRETURN section
     *
     * @param &outfile the output file stream
     * @param LinkList<UserVehicle>* the user list to print
     */
    void PrintReturnResourceActivityData(ofstream &outfile, MURALTypes::UserVector &printUserList);

    /**
     * Writes the MURALMBB section
     *
     * @param &outfile the output file stream
     * @param LinkList<UserVehicle>* the user list to print
     */
    void PrintMakeBeforeBreakData(ofstream &outfile, MURALTypes::UserVector &printUserList);

    /**
     * This method displays the output footer containing:
     *    UserName, Computer Name, PID, Directory, Start/End/Duration times
     *
     * @param outfile the output file stream
     */
    void PrintOutputFooter(ofstream &outfile);

    string outputFileName_a;

    int minimumPrintUserNumber_a;
    int maximumPrintUserNumber_a;

    shared_ptr<MURALTypes::UserVector> ptrUserList_a;
    shared_ptr<MURALTypes::RelayVector> ptrRelayList_a;
    shared_ptr<MURALTypes::GroundVector> ptrGroundList_a;
    shared_ptr<MURALTypes::RevisitVector> ptrRevisitGroupList_a;

    shared_ptr<MURALTypes::ElementCollectionVector> ptrUserGroupList_a;
    shared_ptr<MURALTypes::ElementCollectionVector> ptrUserTypeList_a;
    shared_ptr<MURALTypes::ElementCollectionVector> ptrUserSystemList_a;
};

inline string MRLReport::GetClassName()
{
    return(MRL_REPORT_H);
}

#endif
