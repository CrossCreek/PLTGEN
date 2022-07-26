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
 * RevolutionReport.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef REV_REPORT_H
#define REV_REPORT_H "RevolutionReport"

#include <fstream>
#include <string>
#include <map>
#include <list>
#include <memory>

#include "mural/MURALTypes.h"

template<class Type> class LinkList;
template<class Type> class Array;

class UserVehicle;
class Revolution;

using namespace std;

/**
 * Creates the Revolution report detailing the time periods for the calculated revs
 *
 * Example Header:
 *     USER   START     END  REV  ZERO CRS
 *
 * @author Brian Fowler
 * @date 11/18/11
 */
class RevolutionReport
{
public:

    RevolutionReport(const std::string &outFile,
                     const shared_ptr<MURALTypes::UserVector> &ptrUserList);
    /**
     * Destructor
     */
    ~RevolutionReport();

    /**
     * Creates the "standard" revolution report
     */
    void  CreateReport();

    /**
     * Creates the revolution report in a GPL format
     */
    void  CreateGPLReport();
protected:

private:




    void PrintRevGPLFile(const UserVehicle &user);
    void PrintRevTimesFile(const UserVehicle &user);
    void PrintRevolution(const Revolution &revolution);

    shared_ptr<MURALTypes::UserVector> ptrUserList_a;

    std::string outputFileName_a;
    std::ofstream outfileStream_a;
};

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
std::string
RevolutionReport::GetClassName()
{
    return(REV_REPORT_H);
}
*/

#endif
