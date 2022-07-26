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
 * BasicRegionInViewReport.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef BASIC_REGION_IN_VIEW_REPORT_H
#define BASIC_REGION_IN_VIEW_REPORT_H "BasicRegionInViewReport"

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <memory>

template<class Type> class LinkList;
template<class Type> class Array;
class PositionState;
class AccessPattern;

/**
 * Class to generically write the regions in view at a given time for the given an Access Pattern.
 *
 * Example Header:
 *     USER    TIME  SEN   MODE   TYPE REGIONS
 *
 * @author Brian Fowler
 * @date 09/12/11
 */

class BasicRegionInViewReport
{
public:

    BasicRegionInViewReport(const std::string &outFile,
                            const std::shared_ptr<AccessPattern> &ptrAccessPattern,
                            const std::shared_ptr<std::vector<PositionState>> &ptrPositionStates );
    ~BasicRegionInViewReport();

    /**
     * Creates the report
     */
    void CreateReport();

protected:

private:
    static inline std::string GetClassName();

    void   PrintRegionsInView();

    /**
     * Prints an individual entry in the regions in view report
     *
     * @param name a string representing the access pattern
     * @param &timeIndex the time index to report
     * @param &sensorString the sensor to report
     * @param &modeString the mode to report (csrs, rsps)
     * @param &typeString the type to report (real, stor, dump)
     * @param ptrRegionNumbers the region numbers to list
     */
    void PrintRegionsInView(const std::string &name,
                            const int &timeIndex,
                            const std::string &sensorString,
                            const std::string &modeString,
                            const std::string &typeString,
                            const Array<double> *ptrRegionNumbers);


    std::shared_ptr<AccessPattern> ptrAccessPattern_a;
    std::shared_ptr<std::vector<PositionState>> ptrPositionStates_a;

    std::string outputFileName_a;
    std::ofstream outfileStream_a;
};

inline
std::string
BasicRegionInViewReport::GetClassName()
{
    return(BASIC_REGION_IN_VIEW_REPORT_H);
}

#endif

