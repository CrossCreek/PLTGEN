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
 * ConjunctionReport.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef CONJ_REPORT_H
#define CONJ_REPORT_H "ConjunctionReport"

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
class ConjunctionPeriod;

using namespace std;

/**
 * Creates the Conjunction report for both UPA conjunction and RF conjunction
 *
 * Example Header:
 *   GROUND   RELAY1   RELAY2   START     END  SENDERS   TYPE
 *
 * @author Brian Fowler
 * @date 11/18/11
 */
class ConjunctionReport
{
public:



    explicit ConjunctionReport(const std::string &outFile);

    /**
     * Destructor
     */
    ~ConjunctionReport();

    /**
     * Writes the UPA conjunction report.
     */
    void CreateUPAConjReport(const shared_ptr<MURALTypes::RelayVector> &ptrRelayList);

    /**
     * Writes the RF conjunction report.
     */
    void CreateRFConjReport(const shared_ptr<MURALTypes::GroundVector> &ptrGroundList);
protected:

private:




    /**
     * Generically write conjunction periods
     */
    void   PrintConjunctionPeriods(const shared_ptr<DataProvider> &ptrDataProvider);
    void   PrintConjunctionPeriod(const ConjunctionPeriod &period);

    std::string outputFileName_a;
    std::ofstream outfileStream_a;
};

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
std::string
ConjunctionReport::GetClassName()
{
    return(CONJ_REPORT_H);
}
*/

#endif
