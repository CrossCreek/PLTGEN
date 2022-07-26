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
 * UserTransmitAntenna.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef USER_TRANSMIT_ANTENNA_H
#define USER_TRANSMIT_ANTENNA_H "UserTransmitAntenna"

#include "UserAntenna.h"

using namespace std;

/**
 * Child class of UserAntenna.  Represents the transmitting antenna on a typical
 * user to relay crosslink.
 *
 * UserTransmitAntenna is created and owned by an instance of the
 *     UserVehicle class and is a child of the UserAntenna class.
 *
 * Class Attribute Descriptions
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------------
 * fixedGimbal_a                indicates if the antenna has free travel or is
 *                                in a fixed position
 * ptrCableWrapOffsetAngles_a*  an array of specific cable wrap positions the
 *                                antenna centers around when using the
 *                                minimumGimbalStopAngle_a and
 *                                maximumGimbalStopAngle_a for field-of-view
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class UserTransmitAntenna : public UserAntenna
{
public:

    UserTransmitAntenna();



    explicit UserTransmitAntenna(shared_ptr<UserVehicle> ptrUser);

    virtual ~UserTransmitAntenna();

    UserTransmitAntenna& operator =  (const UserTransmitAntenna &copyMe);
    bool                 operator == (const UserTransmitAntenna &compareMe);
    bool                 operator != (const UserTransmitAntenna &compareMe);

    void   PrintAttributes(ostream &outfile) const;

    bool   IsThereAnyCrosslink(int timeIndex) const;

    // Setters
    void   SetFixedGimbal(bool fixedGimbal);
    void   SetCableWrapOffsetAngles(Array<double>* ptrCableWrapOffsetAngles);


    inline bool IsFixedGimbal() const;
    inline bool IsTransmittingAntenna() const;
    inline bool IsReceiveAntenna() const;

    inline Array<double>* GetCableWrapOffsetAngles() const;

protected:

private:




    bool                fixedGimbal_a;
    Array<double>      *ptrCableWrapOffsetAngles_a;
};

inline
bool
UserTransmitAntenna::IsFixedGimbal() const
{
    return(fixedGimbal_a);
}

inline
bool
UserTransmitAntenna::IsTransmittingAntenna() const
{
    return(true);
}

inline
bool
UserTransmitAntenna::IsReceiveAntenna() const
{
    return(false);
}

inline
Array<double>*
UserTransmitAntenna::GetCableWrapOffsetAngles() const
{
    return(ptrCableWrapOffsetAngles_a);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
UserTransmitAntenna::GetClassName()
{
    return(USER_TRANSMIT_ANTENNA_H);
}
*/

#endif
