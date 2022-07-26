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
 * TrekNode.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef TREK_NODE_H
#define TREK_NODE_H "TrekNode"

#include <iostream>
#include <string>


#include "general/data/UnitData.h"

using namespace std;

class SpaceVector;
class PositionState;

/**
 * This class represents an individual element position in a trek
 *
 *
 * Name                       Description (units)
 * -------------              --------------------------------------------------
 * arrivalTimeStep_a          time step arrived at this node
 * latitude_a                 latitude of this node
 * longitude_a                longitude of this node
 * altitude_a                 altitude of this node
 * numberOfLayoverTimeSteps_a number of time steps waiting at this node
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class TrekNode
{
public:

    TrekNode();
    TrekNode(const int   &arrivalTimeStep,
             double latitude,
             double longitude,
             double altitude,
             const int  &layoverTimeSteps);
    TrekNode(const TrekNode &copyMe);
    virtual ~TrekNode();

    TrekNode&       operator =  (const TrekNode &copyMe);
    bool            operator == (const TrekNode &compareMe);

    SpaceVector   GetPositionVector(int timeIndex,
                                    REFERENCE_FRAME frame
                                    = UnitData::GetInternalReferenceFrame()) const;
    SpaceVector   GetVelocityVector(int timeIndex,
                                    REFERENCE_FRAME frame
                                    = UnitData::GetInternalReferenceFrame()) const;

    PositionState GetPositionState(int timeIndex,
                                   REFERENCE_FRAME frame
                                   = UnitData::GetInternalReferenceFrame()) const;

    inline void   SetNumberOfLayoverTimeSteps(int layoverTimeSteps);

    inline int   GetNumberOfLayoverTimeSteps() const;
    inline int   GetArrivalTimeStep() const;
    inline int   GetDepartureTimeStep() const;

    inline double GetLatitude() const;
    inline double GetLongitude() const;
    inline double GetAltitude() const;

protected:

private:













    static inline string GetClassName();

    int     numberOfLayoverTimeSteps_a;
    int     arrivalTimeStep_a;
    double   latitude_a;
    double   longitude_a;
    double   altitude_a;
};

inline
double
TrekNode::GetLatitude() const
{
    return(latitude_a);
}

inline
double
TrekNode::GetLongitude() const
{
    return(longitude_a);
}

inline
double
TrekNode::GetAltitude() const
{
    return(altitude_a);
}

inline
int
TrekNode::GetArrivalTimeStep() const
{
    return(arrivalTimeStep_a);
}

inline
int
TrekNode::GetDepartureTimeStep() const
{
    return(arrivalTimeStep_a + numberOfLayoverTimeSteps_a);
}

inline
int
TrekNode::GetNumberOfLayoverTimeSteps() const
{
    return(numberOfLayoverTimeSteps_a);
}

//CPPCHECK_RELATED
//Removed SetArrivalTimeStep() since it wasn't called in code base
/*
inline
void
TrekNode::SetArrivalTimeStep(int arrivalTimeStep)
{
    arrivalTimeStep_a = arrivalTimeStep;
}
*/


//CPPCHECK_RELATED
//Removed SetLatitude() since it wasn't called in code base
/*
inline
void
TrekNode::SetLatitude(double latitude)
{
    latitude_a = latitude;
}
*/


//CPPCHECK_RELATED
//Removed SetLongitude() since it wasn't called in code base
/*
inline
void
TrekNode::SetLongitude(double longitude)
{
    longitude_a = longitude;
}
*/


//CPPCHECK_RELATED
//Removed SetAltitude() since it wasn't called in code base
/*
inline
void
TrekNode::SetAltitude(double altitude)
{
    altitude_a = altitude;
}
*/

inline
void
TrekNode::SetNumberOfLayoverTimeSteps(int layoverTimeSteps)
{
    numberOfLayoverTimeSteps_a = layoverTimeSteps;
}

inline
string
TrekNode::GetClassName()
{
    return(TREK_NODE_H);
}

#endif
