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
 * LinksGenerator.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef LINKS_GENERATOR_H
#define LINKS_GENERATOR_H "LinksGenerator"

#include <fstream>
#include <iostream>

#include "mural/modules/lg/ConjunctionPeriod.h"
#include "mural/modules/lg/Link.h"
#include "mural/Sun.h"

#include "general/array/LinkList.h"
#include "general/array/Array.h"

class Antenna;
class DataProvider;
class EarthBoundVehicle;
class GroundPointingAntenna;
class GroundStation;
class Region;
class RelaySatellite;
class Satellite;
class UserPointingAntenna;
class UserTransmitAntenna;
class SelfRelayAntenna;
class UserVehicle;
class DataProviderCollection;

using namespace std;

/**
 * Main module of MURAL which computes links between the data provider.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class LinksGenerator
{
public:

    LinksGenerator();
    LinksGenerator(const LinksGenerator &copyMe);
    virtual ~LinksGenerator();

    LinksGenerator& operator =  (const LinksGenerator &copyMe);
    bool            operator == (const LinksGenerator &compareMe) const;

    static void ExtractLinksGeneratorInfo(const shared_ptr<InputDataElement> &ptrInputDataElement);
    static void PrintAttributes(ofstream &outfile);

    static bool CheckAdditionalSeconds();

    void GenerateLinks(const shared_ptr<DataProviderCollection> &ptrDataProvdierCollection);

protected:

private:

    void   DetermineConjunctionUPA(ofstream &detailedConjFile);
    void   DetermineConjunctionRF(ofstream &detailedConjFile);
    void   DetermineConjunction(shared_ptr<DataProvider> ptrReceiver,
                                shared_ptr<DataProvider> ptrSender1,
                                shared_ptr<DataProvider> ptrSender2,
                                int simultaneousSenders,
                                double previousAngleNB,
                                double previousAngleWB,
                                ofstream &detailedConjFile);
    void   DetermineRelayLinks(ofstream &detailedLinksFile);
    void   DetermineDownlinks(const shared_ptr<RelaySatellite> &ptrRelaySatellite,
                              const shared_ptr<GroundPointingAntenna> &ptrGPA,
                              ofstream &detailedLinksFile);
    void   DetermineRequiredDownlinkOverheadTime(const shared_ptr<RelaySatellite> &ptrRelaySatellite) const;
    void   DetermineMultihopLinks(const shared_ptr<RelaySatellite> &ptrRelaySatellite,
                                  ofstream &detailedLinksFile);
    void   DetermineUserLinks(ofstream &detailedLinksFile);
    void   DetermineUserVehicleLinks(const shared_ptr<UserVehicle> &ptrUserVehicle,
                                     const shared_ptr<UserVehicle> &ptrClusterUser,
                                     ofstream &detailedLinksFile);
    void   DetermineCrosslinks(const shared_ptr<UserVehicle> &ptrUserVehicle,
                               const shared_ptr<UserVehicle> &ptrClusterUser,
                               ofstream &detailedLinksFile);
    void   DetermineDirectDownlinks(const shared_ptr<UserVehicle> &ptrUserVehicle,
                                    ofstream &logfile);
    void   DetermineSelfRelayingLinks(const shared_ptr<UserVehicle> &ptrUserVehicle,
                                      ofstream &detailedLinksFile);
    void   DetermineUPAConjunctionPeriods(const shared_ptr<RelaySatellite> &ptrReceiver,
                                          const shared_ptr<UserVehicle> &ptrSender1,
                                          const shared_ptr<UserVehicle> &ptrSender2,
                                          int simultaneousSenders,
                                          ofstream &detailedConjFile);
    void   PrintOutputAngle(ofstream &outfile,
                            double angle) const;
    void   RetrieveConjunctionPeriods(shared_ptr<DataProvider> ptrReceiver,
                                      const string *ptrConjunctionFileName);
    void   RetrieveDownlinks(const shared_ptr<RelaySatellite> &ptrRelaySatellite,
                             const shared_ptr<GroundPointingAntenna> &ptrGPA);
    void   RetrieveMultihopLinks(const shared_ptr<RelaySatellite> &ptrRelaySatellite);
    void   RetrieveCrosslinks(const shared_ptr<UserVehicle> &ptrUserVehicle);
    void   RetrieveDirectDownlinks(const shared_ptr<UserVehicle> &ptrUserVehicle);
    void   RetrieveSelfRelayingLinks(const shared_ptr<UserVehicle> &ptrFromUserVehicle,
                                     const shared_ptr<SelfRelayAntenna> &ptrFromSRA);
    //void   SetupDataProviderLists(LinkList<DataProvider> *ptrCurrentList);

    double CalculateGrazingAltitude(const SpaceVector &fromVehicleECI,
                                    double elevationAngleFrom,
                                    const SpaceVector &toVehicleECI,
                                    double elevationAngleTo,
                                    double angleOffset) const;

    bool   MissingRequiredDownlink(const shared_ptr<RelaySatellite> &ptrToRelay,
                                   int timeIndex) const;

    Array<int>* DetermineLinkStatusArray(const shared_ptr<DataProvider> &ptrFromProvider,
                                         const shared_ptr<Antenna> &ptrFromAntenna,
                                         const shared_ptr<DataProvider> &ptrToProvider,
                                         const shared_ptr<Antenna> &ptrToAntenna,
                                         ofstream &detailedLinksFile,
                                         bool checkToDownlinkSupport = false,
                                         const shared_ptr<UserVehicle> &ptrClusterUser = nullptr);

    static inline string GetClassName();

    static double          additionalCheckSeconds_s;

    shared_ptr<DataProviderCollection> ptrDataProviderCollection_a;
};

inline
string
LinksGenerator::GetClassName()
{
    return(LINKS_GENERATOR_H);
}

#endif
