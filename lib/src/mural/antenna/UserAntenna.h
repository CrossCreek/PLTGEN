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
 * UserAntenna.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef USER_ANTENNA_H
#define USER_ANTENNA_H "UserAntenna"

#include "Antenna.h"

using namespace std;

class UserVehicle;

/**
 *  UserAntenna is the parent class created and inherited by either the
 *     UserTransmitAntenna or SelfRelayAntenna child classes.  It is a child
 *     of the Antenna class.  This maintains the data that is common to the
 *     children classes.
 *
 * Class Attribute Descriptions
 *
 * Name                       Description (units)
 * -------------              --------------------------------------------------
 * ptrUserVehicle_a*          pointer to the user that owns this antenna
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class UserAntenna : public Antenna
{
public:

    UserAntenna();
    UserAntenna(shared_ptr<UserVehicle> ptrUser,
                int previousCapacity = 0);
    virtual ~UserAntenna();

    UserAntenna&    operator =  (const UserAntenna &copyMe);
    bool            operator == (const UserAntenna &compareMe);
    bool            operator != (const UserAntenna &compareMe);


    void   PrintAttributes(ostream &outfile) const;

    int  GetNumberOfAssetsModeled() const;
    int  GetDataProviderIndex() const;
    int  GetDataProviderNumber() const;

    double GetCollectionDataRate(int timeIndex) const;
    double GetMaximumCollectionDataRate() const;
    double GetTransmitDataRate(int timeIndex,
                               int resourceIndex) const;

    virtual inline void DetermineTransmitLink(const string &transmitDesignator);

    virtual inline bool AcquisitionIsWideband() const;
    virtual inline bool StartUpIsWideband() const;
    virtual inline bool DroplinkIsWideband() const;
    virtual inline bool IsTransmittingAntenna() const;
    virtual inline bool IsReceiveAntenna() const;

    /**
     * Method cast the element on Antenna base to a UserVehicle
     * (for MURAL compatability)
     *
     * @return shared_ptr<UserVehicle>
     */
    shared_ptr<UserVehicle>  GetUser() const;

protected:

private:



};

inline
void
UserAntenna::DetermineTransmitLink(const string &transmitDesignator)
{
    return;
}

inline
bool
UserAntenna::AcquisitionIsWideband() const
{
    return(false);
}

inline
bool
UserAntenna::StartUpIsWideband() const
{
    return(false);
}

inline
bool
UserAntenna::DroplinkIsWideband() const
{
    return(false);
}

inline
bool
UserAntenna::IsTransmittingAntenna() const
{
    return(false);
}

inline
bool
UserAntenna::IsReceiveAntenna() const
{
    return(false);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
UserAntenna::GetClassName()
{
    return(USER_ANTENNA_H);
}
*/

#endif
