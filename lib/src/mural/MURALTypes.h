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
 * MURALTypes.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef MURAL_TYPES_H
#define MURAL_TYPES_H "MURALTypes"

#include <list>
#include <vector>
#include <memory>

class Element;
class DataProvider;
class UserVehicle;
class RelaySatellite;
class GroundStation;
class RevisitGroup;
class ElementCollection;
class SATSIMData;
class PositionState;
class UserGroupConstraint;
class RelayGroupConstraint;
class Antenna;
class SelfRelayAntenna;
class UserPointingAntenna;
class RelayPointingAntenna;
class GroundPointingAntenna;
class ReceiveFacilityAntenna;
class AzimuthAngleData;
class LinkConstraintData;
class Requirement;
class Link;

using namespace std;

/**
 * Holds types (typedefs) common in the MURAL libraries
 *
 * @author Brian Fowler
 * @date   11/17/11
 */
class MURALTypes
{
public:

    typedef vector<shared_ptr<Element>>               ElementVector;
    typedef vector<shared_ptr<DataProvider>>          DataProviderVector;
    typedef vector<shared_ptr<UserVehicle>>           UserVector;
    typedef vector<shared_ptr<RelaySatellite>>        RelayVector;
    typedef vector<shared_ptr<GroundStation>>         GroundVector;

    typedef vector<shared_ptr<RevisitGroup>>          RevisitVector;

    typedef vector<shared_ptr<ElementCollection>>     ElementCollectionVector;

    typedef vector<shared_ptr<SATSIMData>>            SatsimDataVector;

    typedef vector<shared_ptr<UserGroupConstraint>>   UserGroupConstraintVector;
    typedef vector<shared_ptr<RelayGroupConstraint>>  RelayGroupConstraintVector;

    typedef vector<shared_ptr<PositionState>>         PositionStateVector;

    typedef vector<shared_ptr<Antenna>>                AntennaVector;
    typedef vector<shared_ptr<SelfRelayAntenna>>       SRAVector;
    typedef vector<shared_ptr<UserPointingAntenna>>    UPAVector;
    typedef vector<shared_ptr<RelayPointingAntenna>>   RPAVector;
    typedef vector<shared_ptr<GroundPointingAntenna>>  GPAVector;
    typedef vector<shared_ptr<ReceiveFacilityAntenna>> RFAVector;

    typedef vector<shared_ptr<AzimuthAngleData>>       AzimuthAngleDataVector;

    typedef shared_ptr<LinkConstraintData>             LinkConstraintDataPtr;

    typedef vector<shared_ptr<Requirement>>            RequirementVector;

    typedef vector<shared_ptr<Link>>                   LinkVector;

    typedef list<shared_ptr<string>>                   FileList;

protected:

private:
    MURALTypes();
};

#endif
