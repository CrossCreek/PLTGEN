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
 * RegionInViewReport.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef REGION_IN_VIEW_REPORT_H
#define REGION_IN_VIEW_REPORT_H "RegionInViewReport"

#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "mural/MURALTypes.h"

using namespace std;

template<class Type> class LinkList;
template<class Type> class Array;
class PositionState;
class AccessPattern;
class UserVehicle;

/**
 * Class to write the regions in view at a given time in an Access Pattern.  This version
 * is the MURAL specific version that writes additional data other than geometric inviews
 *
 * Example Header:
 *     USER    TIME  SEN   MODE   TYPE REGIONS
 *
 * It writes the following "MODE"s
 *   INVIEW - Geometric inviews at each timestep
 *   REQ_IN - Requested inviews (only csr timesteps are written).  list is filtered by
 *            regions which have score (valid requirements)
 *   REQ_MV - The most valueable requested region (only 1 region in the list)
 *   ALL_IN - Allocated inviews (only rsp timesteps are written).  list is filtered by
 *            regions with field of regard issues
 *   ALL_MV - The most valueable allocated region (only 1 region in the list)
 *
 * @author Brian Fowler
 * @date 09/12/11
 */
class RegionInViewReport
{
public:

    RegionInViewReport(const string &outFile,
                       const shared_ptr<MURALTypes::UserVector> &ptrUserList);
    ~RegionInViewReport();

    /**
     * Creates the report
     */
    void CreateReport();

protected:

private:
    static inline string GetClassName();

    void   PrintAllRegionsInView(const shared_ptr<UserVehicle> &ptrUser);
    void   PrintRequestedRegionsInView(const shared_ptr<UserVehicle> &ptrUser);
    void   PrintAllocatedRegionsInView(const shared_ptr<UserVehicle> &ptrUser);

    /**
     * Prints an individual entry in the regions in view report
     *
     * @param userDesignator a string representing the user
     * @param &timeIndex the time index to report
     * @param &sensorString the sensor to report
     * @param &modeString the mode to report (csrs, rsps)
     * @param &typeString the type to report (real, stor, dump)
     * @param ptrRegionNumbers the region numbers to list
     */
    void PrintRegionsInView(const string &userDesignator,
                            const int &timeIndex,
                            const string &sensorString,
                            const string &modeString,
                            const string &typeString,
                            const Array<double> *ptrRegionNumbers);

    /**
     * Prints an individual entry in the regions in view report for the most valuable region
     *
     * @param userDesignator a string representing the user
     * @param &timeIndex the time index to report
     * @param &sensorString the sensor to report
     * @param &modeString the mode to report (csrs, rsps)
     * @param &typeString the type to report (real, stor, dump)
     * @param regionNumber the most valuable region
     */
    void PrintMostValuableRegion(const string &userDesignator,
                                 const int &timeIndex,
                                 const string &sensorString,
                                 const string &modeString,
                                 const string &typeString,
                                 const double &regionNumber);

    shared_ptr<MURALTypes::UserVector> ptrUserList_a;

    string outputFileName_a;
    ofstream outfileStream_a;
};

inline
string
RegionInViewReport::GetClassName()
{
    return(REGION_IN_VIEW_REPORT_H);
}

#endif

