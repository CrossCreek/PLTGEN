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
 * DataProviderCollection.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef DATA_PROVIDER_COLLECTION_H
#define DATA_PROVIDER_COLLECTION_H "DataProviderCollection"

#include <memory>
#include <vector>
#include <map>

#include "mural/MURALTypes.h"

using namespace std;

class UserVehicle;
class RelaySatellite;
class GroundStation;
class DataProvider;

/**
 * A collection class which contains lists of all the users/relays/ground in
 * the MURAL run.  It has methods to add/remove/get these lists.
 *
 * This gets passed to each module of mural (pg, lg, vtg, rg, ra,ts)
 *
 * Name                       Description (units)
 * -------------              --------------------------------------------------
 * ptrDataProviderList_a*     List of All the Data Providers (as the Base) for convienence
 * ptrUserVehicleList_a*      List of User Vehicles from the run control
 * ptrRelaySatelliteList_a*   List of Relay Satellites from the run control
 * ptrGroundStationList_a*    List of Ground Stations from the run control
 *
 * One note is that trail vehicle DO NOT get added to the ptrUserVehicleList_a/ptrDataProviderList_a.
 * (see the MURALInputProcessor::CreateDataProviderList).  Any need for the trail vehicle needs
 * to be obtained via the lead.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class DataProviderCollection
{
public:

    DataProviderCollection();
    ~DataProviderCollection();

    /**
     * Methods to append vehicles to their perspective lists.  The data
     * provider list is also appended in addition to the specific vehicle
     * list
     */
    void AppendUser(shared_ptr<UserVehicle> ptrUser);
    void AppendRelay(shared_ptr<RelaySatellite> ptrRelay);
    void AppendGround(shared_ptr<GroundStation> ptrGround);

    /**
     * Methods to return the count of vehicles on the lists
     */

    int GetUserCount();
    int GetRelayCount();
    int GetGroundCount();
    int GetDataProviderCount();


    int GetGroupUserCount(const string& groupDesignator);

    int GetTypeUserCount(const string& typeDesignator);

    int GetSystemUserCount(const string& systemDesignator);

    /**
     * Getters to find a specific vehicle based on the given designator
     */
    /**
     * Finds the specified user. If the returnLead is true then a valid trail vehicle designator
     * will return the "lead" vehicle instead of the trail.  It is defaulted to false (to
     * return the actual "trail" vehicle.
     */
    shared_ptr<UserVehicle>    FindUser(const string& designator, bool returnLead = false);
    shared_ptr<RelaySatellite> FindRelay(const string& designator);
    shared_ptr<GroundStation>  FindGround(const string& designator);
    shared_ptr<DataProvider>   FindDataProvider(const string& designator);

    /**
     * Returns the shared pointer to the vehicle list
     */
    shared_ptr<MURALTypes::UserVector>           GetUserListPtr();
    shared_ptr<MURALTypes::RelayVector>          GetRelayListPtr();
    shared_ptr<MURALTypes::GroundVector>         GetGroundListPtr();
    shared_ptr<MURALTypes::DataProviderVector>   GetDataProviderListPtr();

    shared_ptr<MURALTypes::UserVector>           GetGroupUserListPtr(const string& groupDesignator);
    shared_ptr<MURALTypes::UserVector>           GetTypeUserListPtr(const string& typeDesignator);
    shared_ptr<MURALTypes::UserVector>           GetSystemUserListPtr(const string& systemDesignator);

protected:

private:




    shared_ptr<MURALTypes::DataProviderVector>  ptrDataProviderList_a;
    shared_ptr<MURALTypes::UserVector>          ptrUserVehicleList_a;
    shared_ptr<MURALTypes::RelayVector>         ptrRelaySatelliteList_a;
    shared_ptr<MURALTypes::GroundVector>        ptrGroundStationList_a;

    shared_ptr<map<string, shared_ptr<MURALTypes::UserVector> > > userGroupMap_a;
    shared_ptr<map<string, shared_ptr<MURALTypes::UserVector> > > userTypeMap_a;
    shared_ptr<map<string, shared_ptr<MURALTypes::UserVector> > > userSystemMap_a;
};

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
DataProviderCollection::GetClassName()
{
    return(DATA_PROVIDER_COLLECTION_H);
}
*/

#endif
