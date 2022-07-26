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
 * DirectDownlinkAntenna.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef DIRECT_DOWNLINK_ANTENNA_H
#define DIRECT_DOWNLINK_ANTENNA_H "DirectDownlinkAntenna"

#include "UserAntenna.h"

using namespace std;

/**
 * Child Antenna Class to represent a Direct Downlink Antenna for direct downlink
 * capability (non-standard)
 *
 * DirectDownlinkAntenna is created and owned by an instance of the
 * UserVehicle class and is a child of the UserAntenna class.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class DirectDownlinkAntenna : public UserAntenna
{
public:

    DirectDownlinkAntenna();



    explicit DirectDownlinkAntenna(shared_ptr<UserVehicle> ptrUser);

    virtual ~DirectDownlinkAntenna();

    DirectDownlinkAntenna& operator =  (const DirectDownlinkAntenna &copyMe);
    bool              operator == (const DirectDownlinkAntenna &compareMe);
    bool              operator != (const DirectDownlinkAntenna &compareMe);

    void   PrintAttributes(ostream &outfile) const;

    inline bool IsTransmittingAntenna() const;
    inline bool IsReceiveAntenna() const;

protected:

private:



};

inline
bool
DirectDownlinkAntenna::IsTransmittingAntenna() const
{
    return(true);
}

inline
bool
DirectDownlinkAntenna::IsReceiveAntenna() const
{
    return(false);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
DirectDownlinkAntenna::GetClassName()
{
    return(DIRECT_DOWNLINK_ANTENNA_H);
}
*/

#endif
