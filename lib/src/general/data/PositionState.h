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
 * PositionState.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef POSITION_STATE_H
#define POSITION_STATE_H "PositionState"

#include <memory>
#include <fstream>

#include "general/utility/Constant.h"

using namespace std;

class SpaceVector;

/**
 * PositionState is the class that represents a  a position and a
 * velocity for an object at a given time index.
 *
 * ptrPosition_a*               the position (SpaceVector*)
 * ptrVelocity_a*               the velocity (SpaceVector*)
 *
 * @author Brian Fowler
 * @date 09/09/2011
 */
class PositionState
{
public:

    PositionState();
    PositionState(const SpaceVector &position,
                  const SpaceVector &velocity);
    PositionState(double xPosition,
                  double yPosition,
                  double zPosition,
                  double xVelocity,
                  double yVelocity,
                  double zVelocity,
                  const REFERENCE_FRAME &frame);
    PositionState(const PositionState &copyMe);
    virtual ~PositionState();

    PositionState& operator =  (const PositionState &copyMe);
    bool           operator == (const PositionState &compareMe) const;
    bool           operator != (const PositionState &compareMe) const;

    void PrintPositionState(ofstream &outfile,
                            int timeIndex) const;

    SpaceVector GetPosition(const REFERENCE_FRAME &frame,
                            int timeIndex = -1) const;
    SpaceVector GetVelocity(const REFERENCE_FRAME &frame,
                            int timeIndex = -1) const;

    /**
     * This method is a pss through to the SpaceVector's conversion method which correctly handles position
     * vectors transformations.  This method is more for convience and consistancy with the velocity based
     * one.
     *
     * @param j2000Time the j2000 time for the Greenwich Sidereal Angle
     *
     * @return SpaceVector in ECI
     */
    SpaceVector GetPositionECI(double j2000Time) const;

    /**
     * This method does account for the angular velocity component of an ECF to ECI coordinate transformation.
     * It uses The SpaceVector's base conversion method to handle LLA->ECI.  If the base frame is ECF then this
     * method also accounts for the angular velocity component for a more accurate ECF to ECI transformation.
     *
     * If the base frame is LLA, then it is ignored
     *
     * @param j2000Time the j2000 time for the Greenwich Sidereal Angle
     *
     * @return SpaceVector in ECI
     */
    SpaceVector GetVelocityECI(double j2000Time) const;

    inline const SpaceVector* GetPosition() const;
    inline const SpaceVector* GetVelocity() const;

protected:

private:

    static inline string GetClassName();

    SpaceVector        *ptrPosition_a;
    SpaceVector        *ptrVelocity_a;
};

inline
const SpaceVector*
PositionState::GetPosition() const
{
    return(ptrPosition_a);
}

inline
const SpaceVector*
PositionState::GetVelocity() const
{
    return(ptrVelocity_a);
}

inline
string
PositionState::GetClassName()
{
    return(POSITION_STATE_H);
}

#endif
