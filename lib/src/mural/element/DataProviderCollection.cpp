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
 * DataProviderCollection.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "DataProviderCollection.h"

#include "mural/element/DataProvider.h"
#include "mural/element/UserVehicle.h"
#include "mural/element/RelaySatellite.h"
#include "mural/element/GroundStation.h"
#include "general\utility\Utility.h"

DataProviderCollection::DataProviderCollection()
{
    if ( ptrDataProviderList_a == NULL ) {
        ptrDataProviderList_a = shared_ptr<MURALTypes::DataProviderVector>(
                                    new MURALTypes::DataProviderVector() );
    }

    return;
}

/* ******************************************************************************* */

void
DataProviderCollection::AppendUser(shared_ptr<UserVehicle> ptrUser)
{
    if ( ptrUserVehicleList_a == NULL ) {
        ptrUserVehicleList_a = shared_ptr<MURALTypes::UserVector>(
                                   new MURALTypes::UserVector() );
    }

    ptrUserVehicleList_a->push_back(ptrUser);
    ptrDataProviderList_a->push_back(ptrUser);
    string group  = ptrUser->GetGroupName();
    string type   = ptrUser->GetType();
    string system = ptrUser->GetSystemType();

    if(userGroupMap_a == NULL) {
        userGroupMap_a = shared_ptr<map<string, shared_ptr<MURALTypes::UserVector> > >(new map<string,shared_ptr<MURALTypes::UserVector> >);
    }

    if(userGroupMap_a->find(group) == userGroupMap_a->end()) {
        userGroupMap_a->insert(std::make_pair(group, shared_ptr<MURALTypes::UserVector>(new MURALTypes::UserVector() ) ) );
    }

    if(userTypeMap_a == NULL) {
        userTypeMap_a = shared_ptr<map<string, shared_ptr<MURALTypes::UserVector> > >(new map<string,shared_ptr<MURALTypes::UserVector> >);
    }

    if(userTypeMap_a->find(group) == userTypeMap_a->end()) {
        userTypeMap_a->insert(std::make_pair(type, shared_ptr<MURALTypes::UserVector>(new MURALTypes::UserVector() ) ) );
    }

    if(userSystemMap_a == NULL) {
        userSystemMap_a = shared_ptr<map<string, shared_ptr<MURALTypes::UserVector> > >(new map<string,shared_ptr<MURALTypes::UserVector> >);
    }

    if(userSystemMap_a->find(group) == userSystemMap_a->end()) {
        userSystemMap_a->insert(std::make_pair(system, shared_ptr<MURALTypes::UserVector>(new MURALTypes::UserVector() ) ) );
    }

    (userGroupMap_a->at(group))->push_back(ptrUser);
    (userTypeMap_a->at(type))->push_back(ptrUser);
    (userSystemMap_a->at(system))->push_back(ptrUser);
}

/* ******************************************************************************* */

void
DataProviderCollection::AppendRelay(shared_ptr<RelaySatellite> ptrRelay)
{
    if ( ptrRelaySatelliteList_a == NULL ) {
        ptrRelaySatelliteList_a = shared_ptr<MURALTypes::RelayVector>(
                                      new MURALTypes::RelayVector() );
    }

    ptrRelaySatelliteList_a->push_back(ptrRelay);
    ptrDataProviderList_a->push_back(ptrRelay);
}

/* ******************************************************************************* */

void
DataProviderCollection::AppendGround(shared_ptr<GroundStation> ptrGround)
{
    if ( ptrGroundStationList_a == NULL ) {
        ptrGroundStationList_a = shared_ptr<MURALTypes::GroundVector>(
                                     new MURALTypes::GroundVector() );
    }

    ptrGroundStationList_a->push_back(ptrGround);
    ptrDataProviderList_a->push_back(ptrGround);
}

/* ******************************************************************************* */


int
DataProviderCollection::GetUserCount()
{
    //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
    return Utility::CastSizetToInt(ptrUserVehicleList_a->size());
}

/* ******************************************************************************* */

int
DataProviderCollection::GetRelayCount()
{
    //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
    return  Utility::CastSizetToInt(ptrRelaySatelliteList_a->size());
}

/* ******************************************************************************* */


int
DataProviderCollection::GetGroundCount()
{
    //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
    return  Utility::CastSizetToInt(ptrGroundStationList_a->size());
}

/* ******************************************************************************* */


int
DataProviderCollection::GetDataProviderCount()
{
    //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
    return Utility::CastSizetToInt(ptrDataProviderList_a->size());
}

/* ******************************************************************************* */


int
DataProviderCollection::GetGroupUserCount(const string& groupDesignator)
{
    if(userGroupMap_a->find(groupDesignator) == userGroupMap_a->end()) {
        return 0;
    } else {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        return Utility::CastSizetToInt(userGroupMap_a->at(groupDesignator)->size());
    }
}


int
DataProviderCollection::GetTypeUserCount(const string& typeDesignator)
{
    if(userTypeMap_a->find(typeDesignator) == userTypeMap_a->end()) {
        return 0;
    } else {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        return Utility::CastSizetToInt(userTypeMap_a->at(typeDesignator)->size());
    }
}


int
DataProviderCollection::GetSystemUserCount(const string& systemDesignator)
{
    if(userSystemMap_a->find(systemDesignator) == userSystemMap_a->end()) {
        return 0;
    } else {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        return Utility::CastSizetToInt(userSystemMap_a->at(systemDesignator)->size());
    }
}
/* ******************************************************************************* */

shared_ptr<MURALTypes::UserVector>
DataProviderCollection::GetUserListPtr()
{
    return ptrUserVehicleList_a;
}

/* ******************************************************************************* */

shared_ptr<MURALTypes::RelayVector>
DataProviderCollection::GetRelayListPtr()
{
    return ptrRelaySatelliteList_a;
}

/* ******************************************************************************* */

shared_ptr<MURALTypes::GroundVector>
DataProviderCollection::GetGroundListPtr()
{
    return ptrGroundStationList_a;
}

/* ******************************************************************************* */

shared_ptr<MURALTypes::DataProviderVector>
DataProviderCollection::GetDataProviderListPtr()
{
    return ptrDataProviderList_a;
}

shared_ptr<MURALTypes::UserVector>
DataProviderCollection::GetGroupUserListPtr(const string& groupDesignator)
{
    if(userGroupMap_a->find(groupDesignator) == userGroupMap_a->end()) {
        return shared_ptr<MURALTypes::UserVector>(new MURALTypes::UserVector() ) ;
    } else {
        return userGroupMap_a->at(groupDesignator);
    }
}

shared_ptr<MURALTypes::UserVector>
DataProviderCollection::GetTypeUserListPtr(const string& typeDesignator)
{
    if(userTypeMap_a->find(typeDesignator) == userTypeMap_a->end()) {
        return shared_ptr<MURALTypes::UserVector>(new MURALTypes::UserVector() ) ;
    } else {
        return userTypeMap_a->at(typeDesignator);
    }
}

shared_ptr<MURALTypes::UserVector>
DataProviderCollection::GetSystemUserListPtr(const string& systemDesignator)
{
    if(userSystemMap_a->find(systemDesignator) == userSystemMap_a->end()) {
        return shared_ptr<MURALTypes::UserVector>(new MURALTypes::UserVector() ) ;
    } else {
        return userSystemMap_a->at(systemDesignator);
    }
}

/* ******************************************************************************* */

shared_ptr<UserVehicle>
DataProviderCollection::FindUser(const string &designator, bool returnLead)
{
    shared_ptr<UserVehicle>       ptrReturnVehicle = NULL;
    shared_ptr<UserVehicle>       ptrUserVehicle = NULL;
    shared_ptr<UserVehicle>       ptrTrail = NULL;
    MURALTypes::UserVector::iterator userIter;

    for( userIter = ptrUserVehicleList_a->begin(); ((userIter != ptrUserVehicleList_a->end()) && (ptrReturnVehicle == NULL)); ++userIter) {
        ptrUserVehicle = (*userIter);

        // Does the designator match the lead?
        if (ptrUserVehicle->GetDesignator() == designator) {
            ptrReturnVehicle = ptrUserVehicle;
        }
        // Does the designator the trail (if the lead has a trail)?
        else {
            ptrTrail = ptrUserVehicle->GetTrailVehicle();

            if ( ptrTrail != NULL ) {
                if ( ptrTrail->GetDesignator() == designator) {
                    // if returnLead == true, then we want the lead vehicle of the trail
                    if ( returnLead == true ) {
                        ptrReturnVehicle = ptrUserVehicle;
                    }
                    // if returnLead == false then we want the trail directly
                    else {
                        ptrReturnVehicle = ptrTrail;
                    }
                }
            }
        }
    }

    return(ptrReturnVehicle);
}

/* ******************************************************************************* */

shared_ptr<RelaySatellite>
DataProviderCollection::FindRelay(const string &designator)
{
    shared_ptr<RelaySatellite>       ptrReturnRelay = NULL;
    shared_ptr<RelaySatellite>       ptrRelaySatellite = NULL;
    MURALTypes::RelayVector::iterator relayIter;

    for( relayIter = ptrRelaySatelliteList_a->begin(); ((relayIter != ptrRelaySatelliteList_a->end()) && (ptrReturnRelay == NULL)); ++relayIter) {
        ptrRelaySatellite = (*relayIter);

        if (ptrRelaySatellite->GetDesignator() == designator) {
            ptrReturnRelay = ptrRelaySatellite;
        }
    }

    return(ptrReturnRelay);
}

/* ******************************************************************************* */

shared_ptr<GroundStation>
DataProviderCollection::FindGround(const string &designator)
{
    shared_ptr<GroundStation>        ptrReturnGround = NULL;
    shared_ptr<GroundStation>        ptrGroundStation = NULL;
    MURALTypes::GroundVector::iterator groundIter;

    for( groundIter = ptrGroundStationList_a->begin(); ((groundIter != ptrGroundStationList_a->end()) && (ptrReturnGround == NULL)); ++groundIter) {
        ptrGroundStation = (*groundIter);

        if (ptrGroundStation->GetDesignator() == designator) {
            ptrReturnGround = ptrGroundStation;
        }
    }

    return(ptrReturnGround);
}

/* ******************************************************************************* */

shared_ptr<DataProvider>
DataProviderCollection::FindDataProvider(const string &designator)
{
    shared_ptr<DataProvider>    ptrReturnProvider = NULL;
    shared_ptr<DataProvider>    ptrDataProvider = NULL;
    MURALTypes::DataProviderVector::iterator providerIter;

    for( providerIter = ptrDataProviderList_a->begin(); ((providerIter != ptrDataProviderList_a->end()) && (ptrReturnProvider == NULL)); ++providerIter) {
        ptrDataProvider = (*providerIter);

        if (ptrDataProvider->GetDesignator() == designator) {
            ptrReturnProvider = ptrDataProvider;
        }
    }

    return(ptrReturnProvider);
}

/* ******************************************************************************* */

DataProviderCollection::~DataProviderCollection()
{
    return;
}
