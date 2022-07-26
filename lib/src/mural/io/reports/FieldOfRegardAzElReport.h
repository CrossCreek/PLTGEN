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
 * FieldOfRegardAzElReport.h
 *
 * MURAL - Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef FIELD_OF_REGARD_AZEL_REPORT_H
#define FIELD_OF_REGARD_AZEL_REPORT_H "FieldOfRegardAzElReport"

#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <vector>

#include "mural/MURALTypes.h"


#include "general/array/Array3D.h"
#include "general/array/ArrayOfLinkLists.h"
#include "general/array/ArrayOfPointers.h"

using namespace std;

class UserVehicle;

/**
 * Creates the Field of Regard Azimuth and Elevation report.  THis is a large debug
 * report which details the azimuth/elevation values for each UTA->UPA for each relay/cbl wrap/az offset
 *
 * @author Brian Fowler
 * @date 11/18/11
 */
class FieldOfRegardAzElReport
{
public:

    FieldOfRegardAzElReport(const string &outFile,
                            const shared_ptr<MURALTypes::UserVector> &ptrUserList);
    ~FieldOfRegardAzElReport();

    void CreateReport();

protected:

private:
    static inline string GetClassName();

    Array<double>* DetermineImagingRegions(const shared_ptr<UserVehicle> &ptrUser) const;
    Array<double>* DetermineAzConfiguration(const shared_ptr<UserVehicle> &ptrUser) const;
    Array<string>* DetermineRealTimeAllocatedRelays(const shared_ptr<UserVehicle> &ptrUser) const;

    void PrintHeader(const shared_ptr<UserVehicle> &ptrUser,ofstream &forDebugFile);
    void DetermineAzElAngles(const shared_ptr<UserVehicle> &ptrUser, ofstream &forDebugFile);

    shared_ptr<MURALTypes::UserVector> ptrUserList_a;
    string outputFileName_a;
};

inline
string
FieldOfRegardAzElReport::GetClassName()
{
    return(FIELD_OF_REGARD_AZEL_REPORT_H);
}

#endif

