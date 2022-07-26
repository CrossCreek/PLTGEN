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
 * VehiclePositionReport.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef VEHICLE_POSITION_REPORT_H
#define VEHICLE_POSITION_REPORT_H "VehiclePositionReport"

#include <fstream>
#include <string>
#include <map>
#include <list>
#include <memory>

#include "mural/modules/pg/Journey.h"

template<class Type> class LinkList;
template<class Type> class Array;
class Element;

using namespace std;

/**
 * Class to write the position.out file based on the list of journey objects
 *
 * Example Header:
 * DATA PRV    TIME       POSITION X      POSITION Y      POSITION Z      VELOCITY X      VELOCITY Y      VELOCITY Z       DATE     TIME
 *
 * @author Brian Fowler
 * @date 09/30/11
 */
class VehiclePositionReport
{
public:

    VehiclePositionReport(const std::string &outFile,
                          const shared_ptr<MURALTypes::ElementVector> &ptrElementList );
    /**
     * Destructor
     */
    ~VehiclePositionReport();

    /**
     * Creates the report
     */
    void CreateReport();

protected:

private:
    static inline std::string GetClassName();

    /**
     * Prints an individual Journey for the Elment
     *
     * @param ptrJourney the Journey to print
     * @designator
     */
    void PrintPositions(const Element &element);

    shared_ptr<MURALTypes::ElementVector> ptrElementList_a;

    std::string outputFileName_a;
    std::ofstream outfileStream_a;
};

inline
std::string
VehiclePositionReport::GetClassName()
{
    return(VEHICLE_POSITION_REPORT_H);
}
#endif

