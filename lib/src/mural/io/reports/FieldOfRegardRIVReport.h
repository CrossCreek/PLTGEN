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
 * FieldOfRegardRIVReport.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef FIELD_OF_REGARD_RIV_REPORT_H
#define FIELD_OF_REGARD_RIV_REPORT_H "FieldOfRegardRIVReport"

#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "mural/MURALTypes.h"


#include "general/array/Array3D.h"
#include "general/array/ArrayOfLinkLists.h"
#include "general/array/ArrayOfPointers.h"

using namespace std;

class UserVehicle;

/**
 * Class to write the Field of Regard Region in view file which details the regions
 * in view data for each relay/cable wrap/az offset combo.  It is primarily used by ATOM to
 * compute geo-views for field of regard users
 *
 * @author Brian Fowler
 * @date 09/12/11
 */
class FieldOfRegardRIVReport
{
public:

    FieldOfRegardRIVReport(const string &outFile,
                           const shared_ptr<MURALTypes::UserVector> &ptrUserList);
    ~FieldOfRegardRIVReport();

    void CreateReport();

protected:

private:
    static inline string GetClassName();

    void DetermineFORRegionsInviewDebug(const shared_ptr<UserVehicle> &ptrUser, ofstream &forDebugFile);

    shared_ptr<MURALTypes::UserVector> ptrUserList_a;
    string outputFileName_a;
};

inline
string
FieldOfRegardRIVReport::GetClassName()
{
    return(FIELD_OF_REGARD_RIV_REPORT_H);
}

#endif

