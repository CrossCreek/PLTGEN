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
 * RevolutionReport.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "RevolutionReport.h"
#include "mural/element/UserVehicle.h"
#include "mural/modules/pg/Journey.h"
#include "mural/modules/pg/Orbit.h"
#include "mural/modules/pg/Revolution.h"


#include "general/array/Array.h"
#include "general/array/ListIterator.h"
#include "general/utility/StringEditor.h"
#include "general/data/UnitData.h"
#include "general/time/TimePiece.h"
#include "general/parse/ParseUtility.h"

RevolutionReport::RevolutionReport(const std::string &outFile,
                                   const shared_ptr<MURALTypes::UserVector> &ptrUserList) :
    outputFileName_a(outFile), ptrUserList_a(ptrUserList)
{
    return;
}

/* ************************************************************************** */

void
RevolutionReport::CreateReport()
{
    shared_ptr<UserVehicle> ptrUser = NULL;
    shared_ptr<UserVehicle> ptrTrailVehicle = NULL;
    MURALTypes::UserVector::iterator userIter;
    ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
    outfileStream_a << "    USER   START     END  REV  ZERO CRS" << NEW_LINE;

    for( userIter = ptrUserList_a->begin(); userIter != ptrUserList_a->end(); ++userIter) {
        ptrUser = (*userIter);
        PrintRevTimesFile(*ptrUser);
        ptrTrailVehicle = ptrUser->GetTrailVehicle();

        if (ptrTrailVehicle != NULL) {
            PrintRevTimesFile(*ptrTrailVehicle);
        }
    }

    outfileStream_a.close();
    return;
}

/* ************************************************************************** */

void
RevolutionReport::CreateGPLReport()
{
    int         totalNumberOfRevs = 0;
    shared_ptr<UserVehicle> ptrUserVehicle = NULL;
    shared_ptr<UserVehicle> ptrTrailVehicle = NULL;
    MURALTypes::UserVector::iterator userIter;
    ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);

    for( userIter = ptrUserList_a->begin(); userIter != ptrUserList_a->end(); ++userIter) {
        ptrUserVehicle = (*userIter);
        totalNumberOfRevs += ptrUserVehicle->GetNumberOfRevs();
        ptrTrailVehicle = ptrUserVehicle->GetTrailVehicle();

        if (ptrTrailVehicle != NULL) {
            totalNumberOfRevs += ptrTrailVehicle->GetNumberOfRevs();
        }
    }

    outfileStream_a << "####Number of Constraints in message:" << NEW_LINE;
    outfileStream_a << setw(5) << totalNumberOfRevs << NEW_LINE;

    for( userIter = ptrUserList_a->begin(); userIter != ptrUserList_a->end(); ++userIter) {
        ptrUserVehicle = (*userIter);
        PrintRevGPLFile(*ptrUserVehicle);
        ptrTrailVehicle = ptrUserVehicle->GetTrailVehicle();

        if (ptrTrailVehicle != NULL) {
            PrintRevGPLFile(*ptrTrailVehicle);
        }
    }

    outfileStream_a.close();
}

/* ************************************************************************** */

void
RevolutionReport::PrintRevGPLFile(const UserVehicle &user)
{
    string                   revLabel = "";
    const string             userDesignator = user.GetDesignator();
    const string             userType = user.GetType();
    Revolution              *ptrRevolution = NULL;
    ListIterator<Revolution> revolutionListIter(user.GetRevolutionList());

    if (userType.find("P") != string::npos) {
        revLabel = "PREVBND";
    } else if (userType.find("H") != string::npos) {
        revLabel = "HREV";
    } else {
        revLabel = "REV";
    }

    ptrRevolution = revolutionListIter.First();

    while (ptrRevolution != NULL) {
        outfileStream_a << "#" << NEW_LINE;
        outfileStream_a << revLabel << "," << userType << "," << userDesignator;
        outfileStream_a << ",0,0,";
        outfileStream_a << setprecision(6) << TimePiece::GetOFTime(ptrRevolution->GetStartTime()) << ",";
        outfileStream_a << setprecision(6) << TimePiece::GetOFTime(ptrRevolution->GetEndTime() + 1) << ",";
        outfileStream_a << setw(5) << ptrRevolution->GetRevNumber() << ",0" << NEW_LINE;
        ptrRevolution = revolutionListIter.Next();
    }

    /*
    BTF - I do not think this recursion is needed because the calling method handles the trail

    shared_ptr<UserVehicle>  ptrTrailVehicle = NULL;
    if (ptrTrailVehicle != NULL)
    {
      PrintRevGPLFile(*ptrTrailVehicle);
    }*/
    return;
}

/* ************************************************************************** */

void
RevolutionReport::PrintRevTimesFile(const UserVehicle &user)
{
    shared_ptr<Orbit>        ptrOrbit = dynamic_pointer_cast<Orbit>(user.GetJourney());

    // Make sure it is an orbit with revolutions!
    if ( ptrOrbit != NULL ) {
        ListIterator<Revolution> revolutionListIter(ptrOrbit->GetRevolutionList());
        Revolution              *ptrRevolution = revolutionListIter.First();

        while (ptrRevolution != NULL) {
            outfileStream_a << setw(8) << user.GetDesignator() << " ";
            PrintRevolution(*ptrRevolution);
            ptrRevolution = revolutionListIter.Next();
        }
    }

    return;
}

/* ************************************************************************** */

void
RevolutionReport::PrintRevolution(const Revolution &revolution)
{
    TimePiece::PrintIndex(outfileStream_a, revolution.GetStartTime());
    outfileStream_a << " ";
    TimePiece::PrintIndex(outfileStream_a, revolution.GetEndTime());
    outfileStream_a << " ";
    outfileStream_a << setw(4) << revolution.GetRevNumber() << " ";
    outfileStream_a << setw(9) << setprecision(2)
                    << UnitData::GetOutputAngle(revolution.GetAscendingZeroCrossingLongitude()) << NEW_LINE;
    return;
}

/* *************************************************************************** */

RevolutionReport::~RevolutionReport()
{
    // Do nothing
}
