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
 * GroundStation.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef GROUND_STATION_H
#define GROUND_STATION_H "GroundStation"

#include <iostream>

#include "mural/element/DataProvider.h"
#include "general/array/Array.h"
#include "general\utility\Utility.h"

using namespace std;

class CoordinateFrame;
class Link;
class ReceiveFacilityAntenna;

/**
 * This is a child of DataProvider which represnts a ground station.  It holds
 * ReceieveFacilityAntennas which are the "ending" point of the user->relay->ground
 * allocation algorithm.
 *
 * Class Attribute Descriptions
 *
 * Name                       Description (units)
 * -------------              --------------------------------------------------
 * ptrIdentificationArray_s*  array of ground station designations
 * ptrAllocationCriteria_a*   pointer to the resource allocation criteria for
 *                              this ground station
 * ptrRFList_a*               list of ReceiveFacilityAntenna
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class GroundStation : public DataProvider
{
public:

    GroundStation();
    virtual ~GroundStation();

    GroundStation&  operator =  (const GroundStation &copyMe);
    bool            operator == (const GroundStation &compareMe) const;
    bool            operator != (const GroundStation &compareMe) const;

    /**
     * Override of virtual method on DataProvider to return the RF list, (the
     * list to perform conjunction on)
     *
     * @return AntennaVector
     */
    shared_ptr<MURALTypes::AntennaVector> GetConjunctionAntennaList();

    static void ResetIdentification();
    static void SetIdentification(const Array<string> *ptrIdentification);
    static void SortIdentificationArray();

    static bool ConvertDesignatorToIndex(const string &groundDesignator,
                                         int &index);
    static bool ConvertIndexToDesignator(int index,
                                         string &groundDesignator);

    static string GetDesignatorString(int index);


    void   PrintAttributes(ostream &outfile) const;

    void   SetRequiredNumberOfRFs();

    int  GetTotalRFCapacity() const;
    int  GetNumberOfDedicatedTransmitters() const;

    double GetMaximumReceiveDataRate() const;

    bool   HasDownlinkInview(const string &relayDesignator,
                             int timeIndex) const;
    bool   HasPooledEquipment() const;
    bool   HasPrecludedUser() const;

    static inline int  GetGroundStationCount();

    static inline bool   ValidGroundStation(const string &designator);

    static inline const Array<string>* GetIdentificationArray();

    inline int   GetIndex() const;

    inline int   GetNumberOfRFs() const;

    shared_ptr<ReceiveFacilityAntenna>   GetRF(const string &antennaDesignator, int &rfIndex) const;

    inline shared_ptr<MURALTypes::RFAVector>     GetRFList() const;

    /**
     * Setters
     */
    void SetRFList(const shared_ptr<MURALTypes::RFAVector> &ptrRFList);


protected:

private:

    static inline string GetClassName();

    static Array<string>              *ptrIdentificationArray_s;

    shared_ptr<MURALTypes::RFAVector>      ptrRFList_a;
};

inline
int
GroundStation::GetIndex() const
{
    return(ptrIdentificationArray_s->FindIndex(GetDesignator()));
}

inline

int
GroundStation::GetNumberOfRFs() const
{
    int numberOfRFs = 0;

    if (ptrRFList_a != NULL) {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        numberOfRFs = Utility::CastSizetToInt(ptrRFList_a->size());
    }

    return(numberOfRFs);
}

inline
int
GroundStation::GetGroundStationCount()
{
    int numberOfGroundStations = 0;

    if (ptrIdentificationArray_s != NULL) {
        numberOfGroundStations = ptrIdentificationArray_s->Size();
    }

    return(numberOfGroundStations);
}

inline
bool
GroundStation::ValidGroundStation(const string &designator)
{
    bool validGround = false;

    if (ptrIdentificationArray_s != NULL) {
        validGround = ptrIdentificationArray_s->ValueExists(designator);
    }

    return(validGround);
}

inline
const Array<string>*
GroundStation::GetIdentificationArray()
{
    return(ptrIdentificationArray_s);
}

inline
shared_ptr<MURALTypes::RFAVector>
GroundStation::GetRFList() const
{
    return(ptrRFList_a);
}

inline
string
GroundStation::GetClassName()
{
    return(GROUND_STATION_H);
}

#endif
