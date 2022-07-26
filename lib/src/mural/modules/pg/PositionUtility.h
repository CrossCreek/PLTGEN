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
 * PositionUtility.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef POSITION_UTILITY_H
#define POSITION_UTILITY_H "PositionUtility"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <memory>

#include "general/array/LinkList.h"

using namespace std;

class DataProvider;
class Journey;
class Orbit;
class PositionState;
class SpaceVector;
class Trek;
class TrekNode;

/**
 * This is a utility class that has static methods to do the orbit propigation.  It propigates
 * all the orbits for the data providers. It's methods
 * were removed from PositionGenerator and given their own class (PositionUtility)
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class PositionUtility
{
public:

    PositionUtility();
    PositionUtility(const PositionUtility &copyMe);
    virtual ~PositionUtility();

    static double MakeAngleBetweenZeroAndTwoPI(double currentAngle);

    static Orbit       UpdateOrbit(const Orbit &currentOrbit,
                                   double newEpochTime);

    static SpaceVector DetermineInterimPosition(const SpaceVector &startPosition,
            const SpaceVector &endPosition,
            double totalSeconds,
            double currentSeconds);

    /**
     * Propagates the orbit state.  Based on General Engineering's SpatialMJ2J4::UpdateState() method
     *
     * @param Orbit the orbit to get the classical elements
     * @param j2000Time for converting to ECF and the store frame
     */
    static shared_ptr<PositionState> GeneratePositionState(shared_ptr<Orbit> ptrOrbit,
            double j2000Time);

    /**
     * Retrieves the position Data from a MURAL formatted position file
     * (maybe make this its own class in future?)
     *
     * @param &positionFile position file to use
     * @param &designator the designaotr of the vehilce
     * @param *ptrJourney the pointer of the journey to populate
     */
    static void   RetrievePositionData(const string &positionFile,
                                       const string &designator,
                                       shared_ptr<Journey> ptrJourney);

    /**
     * Retrieves the position Data from a MURAL P470 formatted position file
     * (maybe make this its own class in future?)
     *
     * @param &positionFile position file to use
     * @param &designator the designaotr of the vehilce
     * @param *ptrJourney the pointer of the journey to populate
     */
    static void   RetrieveP470PositionData(const string &positionFile,
                                           const string &designator,
                                           shared_ptr<Journey> ptrJourney);

    /**
     * Determines Interim Trek Nodes
     */
    static void   DetermineInterimTrekNodes(shared_ptr<Trek> ptrTrek,
                                            TrekNode *ptrStartNode,
                                            TrekNode *ptrEndNode);

    static void   GenerateStationaryTrek(shared_ptr<Trek> ptrTrek);
    static void   GenerateMovingTrek(shared_ptr<Trek> ptrTrek);

protected:

private:

    static SpaceVector ConvertPQWToECI(const SpaceVector &pqwVector,
                                       double inclination,
                                       double rightAscension,
                                       double argumentOfPerigee);




    static inline string GetClassName();
};

inline
string
PositionUtility::GetClassName()
{
    return(POSITION_UTILITY_H);
}

#endif
