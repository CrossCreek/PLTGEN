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
 * UserPointingAntenna.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef USER_POINTING_ANTENNA_H
#define USER_POINTING_ANTENNA_H "RelayAntenna"

#include "RelayAntenna.h"

using namespace std;

class IntervalTraversal;
class UserVehicle;

/**
 * Child class of RelayAntenna.  Represents the receiveing antenna on a typical
 * user to relay crosslink.
 *
 * UserPointingAntenna is created and owned by an instance of the
 * RelaySatellite class and is a child of the RelayAntenna class.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class UserPointingAntenna : public RelayAntenna
{
public:

    UserPointingAntenna();
    UserPointingAntenna(shared_ptr<RelaySatellite> ptrRelay,
                        int previousCapacity);
    virtual ~UserPointingAntenna();

    UserPointingAntenna& operator =  (const UserPointingAntenna &copyMe);
    bool                 operator == (const UserPointingAntenna &compareMe);
    bool                 operator != (const UserPointingAntenna &compareMe);

    void   PrintAttributes(ostream &outfile) const;

    Link*  GetCrosslink(const string &userDesignator) const;

    inline bool IsTransmittingAntenna() const;
    inline bool IsReceiveAntenna() const;

protected:

private:



};

inline
bool
UserPointingAntenna::IsTransmittingAntenna() const
{
    return(false);
}

inline
bool
UserPointingAntenna::IsReceiveAntenna() const
{
    return(true);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
UserPointingAntenna::GetClassName()
{
    return(USER_POINTING_ANTENNA_H);
}
*/

#endif
