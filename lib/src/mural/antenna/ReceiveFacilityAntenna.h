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
 * ReceiveFacilityAntenna.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef RECEIVE_FACILITY_ANTENNA_H
#define RECEIVE_FACILITY_ANTENNA_H "ReceiveFacilityAntenna"

#include "Antenna.h"

using namespace std;

class GroundStation;
class PooledEquipmentPeriod;
class UserToGroundPath;

/**
 * A child Antenna class which represents a receive antenna on the ground station.
 * It is the receiver of the traditional "downlink"
 *
 * ReceiveFacilityAntenna is created and owned by an instance of the
 * GroundStation class and is a child of the Antenna class.
 *
 *
 * Class Attribute Descriptions
 *
 * Name                       Description (units)
 * -------------              --------------------------------------------
 * totalNumberOfRFs_s         total RFs across all ground stations
 * ptrIdentificationArray_s*  array of characters identifying each RF by GS and
 *                              capacity index
 * ptrPrecludedUserArray_a*   array of precluded user designators requiring the
 *                              generation of UserToGroundPath instances
 * ptrGroundStation_a*        pointer to the ground station this RF resides at
 * ptrPooledEquipmentList_a*  pointer to a list of limiting PooledEquipment
 *                              instance requiring the generation of
 *                              UserToGroundPath instances
 * ptrUserToGroundPathList_a* list of UserToGroundPath instances used for
 *                              limiting and/or precluding certain user to relay
 *                              to ground paths from being allocated
 *
 * userConstrained_a          This boolean was added for 9.3 and the User Capacity
 *                              constraint.  It flags the RF as being used as in
 *                              global user capacity constraint.  Mainly needed to
 *                              flag MURAL to generate the UserToGround paths and assign
 *                              WB solution indices.
 *
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class ReceiveFacilityAntenna : public Antenna
{
public:

    ReceiveFacilityAntenna();
    ReceiveFacilityAntenna(shared_ptr<GroundStation> ptrGround,
                           int previousCapacity);
    virtual ~ReceiveFacilityAntenna();

    ReceiveFacilityAntenna& operator =  (const ReceiveFacilityAntenna &copyMe);
    bool                  operator == (const ReceiveFacilityAntenna &compareMe);
    bool                  operator != (const ReceiveFacilityAntenna &compareMe);

    static void CreateIdentificationArray(const Array<int> *ptrCapacityArray);

    static char GetIdentification(int groundIndex,
                                  int rfIndex);

    void   AddRFCountToTotal(int capacity);


    void   PrintAttributes(ostream &outfile) const;

    void   SetUserConstrained(const bool userConstrained);
    void   SetPooledEquipmentList(LinkList<PooledEquipmentPeriod>  *ptrPooledEquipmentList);
    void   SetPrecludedUserArray(Array<string>* ptrPrecludedUserArray);

    int  GetDataProviderIndex() const;
    int  GetDataProviderNumber() const;

    static int GetTotalNumberOfRFs();

    void  SetRequiredNumberOfRFs();

    /**
     * This method returns weather the RF has a precluded user
     * list
     *
     * @return bool
     */
    bool  HasPrecludedUser() const;

    /**
     * This method returns weather the RF has user to ground paths
     *
     * @return bool
     */
    bool  HasUserToGroundPaths() const;

    /**
     * This method returns weather the RF has a pooled equipment list
     *
     * @return bool
     */
    bool  HasPooledEquipment() const;

    /**
     * This method returns weather the user designator passed is a part of the
     * precluded user array.
     *
     * @param userDesignator user designator to test
     * @return bool
     */
    bool  IsPrecludedUser(const string &userDesignator) const;
    bool  IsTransmittingAntenna() const;
    bool  IsReceiveAntenna() const;

    /**
     * This method returns the boolean signaling that this RF is a part of
     * a global user capacity constrinat (9.3)
     */
    bool  IsUserConstrained() const;

    /**
     * Method cast the element on Antenna base to a GroundStation
     * (for MURAL compatability)
     *
     * @return shared_ptr<GroundStation>
     */
    shared_ptr<GroundStation> GetGround() const;

    LinkList<UserToGroundPath>* GetUserToGroundPathList() const;

protected:

private:

    static inline string GetClassName();

    static int                totalNumberOfRFs_s;
    static Array2D<char>     *ptrIdentificationArray_s;

    bool                              userConstrained_a;

    Array<string>                    *ptrPrecludedUserArray_a;
    LinkList<PooledEquipmentPeriod>  *ptrPooledEquipmentList_a;
    LinkList<UserToGroundPath>       *ptrUserToGroundPathList_a;
};



inline
string
ReceiveFacilityAntenna::GetClassName()
{
    return(RECEIVE_FACILITY_ANTENNA_H);
}

#endif
