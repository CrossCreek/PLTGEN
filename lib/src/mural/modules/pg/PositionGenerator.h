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
 * PositionGenerator.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef POSITION_GENERATOR_H
#define POSITION_GENERATOR_H "PositionGenerator"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <memory>

using namespace std;

class DataProvider;
class Journey;
class Orbit;
class PositionState;
class SpaceVector;
class Trek;
class TrekNode;
class DataProviderCollection;

/**
 * This class is the main module for generating positions in MURAL
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class PositionGenerator
{
public:

    PositionGenerator();
    PositionGenerator(const PositionGenerator &copyMe);
    virtual ~PositionGenerator();

    void   GeneratePositions(const shared_ptr<DataProviderCollection> &ptrDataProviderCollection);

protected:

private:

    void   DeterminePositionData(const shared_ptr<Journey> &ptrJourney,
                                 const string &designator,
                                 const bool &dataCollector);
    void   GenerateEphemeris(const shared_ptr<Orbit> &ptrOrbit);

    static inline string GetClassName();
};

inline
string
PositionGenerator::GetClassName()
{
    return(POSITION_GENERATOR_H);
}

#endif
