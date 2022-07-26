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
 * GroundPointingAntenna.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef GROUND_POINTING_ANTENNA_H
#define GROUND_POINTING_ANTENNA_H "GroundPointingAntenna"

#include "RelayAntenna.h"

using namespace std;

class AllocationCriteria;
class IntervalTraversal;

/**
 * Child Antenna Class to represent a Ground Pointing Antenna.  This is an antenna on
 * a relay satellite and is the transmitter to a Receive Facility Antenna for a
 * traditional downlink.
 *
 * GroundPointingAntenna is created and owned by an instance of the
 * RelaySatellite class and is a child of the RelayAntenna class.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class GroundPointingAntenna : public RelayAntenna
{
public:

    GroundPointingAntenna();
    GroundPointingAntenna(shared_ptr<RelaySatellite> ptrRelay,
                          int previousCapacity);
    virtual ~GroundPointingAntenna();

    GroundPointingAntenna& operator =  (const GroundPointingAntenna &copyMe);
    bool                   operator == (const GroundPointingAntenna &compareMe);
    bool                   operator != (const GroundPointingAntenna &compareMe);

    void   MakeDownlinkAssignment(int startAllocation,
                                  int endAllocation);

    bool   MakingContactForDiss(int timeIndex) const;

    void   PrintAttributes(ostream &outfile) const;

    void   RegenerateIntervalTraversal(const shared_ptr<AllocationCriteria> &ptrAllocationCriteria) const;

    inline bool IsTransmittingAntenna() const;
    inline bool IsReceiveAntenna() const;

protected:

private:


};

inline
bool
GroundPointingAntenna::IsTransmittingAntenna() const
{
    return(true);
}

inline
bool
GroundPointingAntenna::IsReceiveAntenna() const
{
    return(false);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
GroundPointingAntenna::GetClassName()
{
    return(GROUND_POINTING_ANTENNA_H);
}
*/

#endif
