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
 * LinkReport.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef LINK_REPORT_H
#define LINK_REPORT_H "LinkReport"

#include <fstream>
#include <string>
#include <map>
#include <list>
#include <memory>

#include "mural/MURALTypes.h"

template<class Type> class LinkList;
template<class Type> class Array;

class UserVehicle;
class ValueTimelineElement;

using namespace std;

/**
 * Class to write basic reports for links between elements.  This can writes all
 * the various types of links files with the cooresponding method.  This archetecture is
 * a bit different from the other reports but it was the best I could come up with
 * in the limited time I could devote to this
 *
 * "Common link files"
 * the crosslink.out file containing basic information about the user to relay links and
 * the downlink.out file containing basic information about the relay to ground links
 *
 * "Other link files"
 * the directDownlink.out file containing basic information about the user to ground links
 * the selfRelayLinks.out file containing basic information about the user to user links
 * the multihopLinks.out file containing basic information about the relay to relay links
 *
 * Example Header:
 *     USER      UTA    RELAY      UPA    TIME LINK
 *
 * @author Brian Fowler
 * @date 11/18/11
 */
class LinkReport
{
public:



    explicit LinkReport(const std::string &outFile);

    /**
     * Destructor
     */
    ~LinkReport();

    /**
     * Creates the crosslink.out report
     */
    void CreateCrosslinkReport(const shared_ptr<MURALTypes::UserVector> &ptrUserList);

    /**
     * Creates the directDownlink.out report
     */
    void CreateDirectDownlinkReport(const shared_ptr<MURALTypes::UserVector> &ptrUserList);

    /**
     * Creates the selfRelayLinks.out report
     */
    void CreateUserSelfRelayReport(const shared_ptr<MURALTypes::UserVector> &ptrUserList);

    /**
     * Creates the downlinks.out report
     */
    void CreateDownlinkReport(const shared_ptr<MURALTypes::RelayVector> &ptrRelayList);

    /**
     * Creates the multihopLinks.out report
     */
    void CreateRelayMultiHopReport(const shared_ptr<MURALTypes::RelayVector> &ptrRelayList);

protected:

private:




    void PrintLinks(const shared_ptr<Antenna> &antenna);

    std::string outputFileName_a;
    std::ofstream outfileStream_a;
};

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
std::string
LinkReport::GetClassName()
{
    return(LINK_REPORT_H);
}
*/

#endif
