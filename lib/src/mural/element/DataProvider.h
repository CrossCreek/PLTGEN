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
 * DataProvider.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef DATA_PROVIDER_H
#define DATA_PROVIDER_H "DataProvider"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "mural/element/Element.h"
#include "mural/modules/lg/ConjunctionPeriod.h"

#include "general/array/Array2D.h"
#include "general/array/ListIterator.h"
#include "general/utility/Constant.h"

using namespace std;

class Antenna;
class InputDataElement;
class Journey;
class Link;
class LinkCriteria;
class OverheadData;
class Revolution;
class Sensor;
class SpaceVector;
class TimePeriod;

/**
 * Child class of Element (Element contains the position/trek data) and base class
 * to UserVehicle/RelaySatellite/GroundStation.  This class contains common
 * "resource allocation" methods and data shared by the children such as allocation
 * status and constraint generation.
 *
 * Class Attribute Descriptions
 *
 * Name                       Description (units)
 * -------------              --------------------------------------------------
 * minimumProviderUseIndex_a  starting time step solution index for this
 *                              satellite (DDL for user or relay use for relay)
 * ptrOPSDesignator_a*        string containing the assoc. ops designator
 * ptrType_a*                 string containing type info
 * ptrSystem_a*               string containing system type info
 * ptrAllocationStatus_a*     array of ALLOCATION_STATUS's at each time step
 * ptrAllocationDataRate_a*   array of total allocated data rate at each time step
 * ptrProbability_a*          probability data provider is allocated by time
 *                              and resource (collector) or technology (relay)
 *                              used only if running the probabilitic allocator
 * ptrLinkCriteria_a*         pointer to criteria for generating link paths
 * ptrSensorList_a*           list of sensors on this data provider (if any)
 * ptrNBConjunctionPeriodList_a*  list of ConjunctionPeriod instances
 *                                  containing all NB conjunction periods
 * ptrWBConjunctionPeriodList_a*  list of ConjunctionPeriod instances
 *                                  containing all WB conjunction periods
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class DataProvider : public Element
{
public:

    DataProvider();
    virtual ~DataProvider();

    DataProvider&   operator =  (const DataProvider &copyMe);
    bool            operator == (const DataProvider &compareMe) const;
    bool            operator != (const DataProvider &compareMe) const;

    virtual int  GetMinimumNumberOfHops(Array<int> *ptrVisitedRelayIndicies = NULL) const;

    virtual double GetTotalAllocatedDataRate(int timeIndex) const;

    /**
     * Virtual method to return the list which conjucntion constraints should be computed
     * for (UPA's for relays, and RF's for grounds).
     *
     * @return shared_ptr<MURALTypes::AntennaVector>
     */
    virtual shared_ptr<MURALTypes::AntennaVector> GetConjunctionAntennaList();


    void   AddActiveSensors(Array<bool> *&ptrActiveSensor) const;

    void   InitializeProbabilityArray(int secondDimension);
    void   PrintAllNames(ostream &outfile) const;
    void   PrintAttributes(ostream &outfile) const;
    void   PrintUpdatedOrbitFile(ofstream &updatedOrbitFile);
    void   PrintProbabilityArray(ofstream &debugFile);

    bool   ConjunctionProblem(const ConjunctionPeriod::CONJUNCTION_TYPE &type,
                              int timeIndex,
                              const string &transmitDesignator);
    bool   HasNarrowbandConjunctionData() const;
    bool   HasWidebandConjunctionData() const;
    bool   HasSimultaneousConjunctionSenders(int simultaneousSenders) const;
    bool   IsStationaryPosition() const;
    bool   IsWidebandConjunctionTimeStep(int timeIndex,
                                         const string &sender1Designator,
                                         const string &sender2Designator) const;
    bool   IsWidebandConjunctionTimeStep(int startTimeIndex,
                                         int endTimeIndex,
                                         const string &sender1Designator,
                                         const string &sender2Designator) const;

    bool   ReadLinkPaths() const;
    bool   ReadConjunctionData() const;
    bool   ShouldCheckConjunction(const string &sender1Designator,
                                  const string &sender2Designator,
                                  int timeIndex);

    virtual inline bool  BodyTiltsForCollection() const;

    inline bool   HasNarrowbandConjunctionPeriod() const;
    inline bool   HasWidebandConjunctionPeriod() const;
    inline bool   IsAPrecludedPathReceiver(const Array<string>
                                           *ptrCurrentPrecludedDesignators) const;

    /**
     * Getters
     */
    shared_ptr<LinkCriteria> GetLinkCriteria() const;
    int  GetMaximumManeuverTime() const;
    int  GetMaximumSimultaneousConjunctionSenders() const;

    shared_ptr<vector<int>> GetRelayCoverageContourArray() const;

    double GetProbability(int timeIndex) const;
    double GetProbability(int timeIndex,
                          int secondIndex) const;
    double GetMaximumSensorDataRate() const;
    double GetMinimumSensorDataRate() const;

    double GetNarrowbandConjunctionAngle(const string &nearDesignator,
                                         const string &farDesignator,
                                         int simultaneousSenders) const;
    double GetWidebandConjunctionAngle(const string &nearDesignator,
                                       const string &farDesignator,
                                       int simultaneousSenders) const;
    double GetMinimumICCLSolarIntrusionAngle() const;

    const OverheadData* GetLinkOverheadData(const string &designator) const;
    Array2D<double> *GetProbability() const;

    Sensor*       GetSensor(int sensorNumber) const;

    LinkList<Revolution>* GenerateSouthPointRevolutionList() const;
    const LinkList<Revolution>* GetRevolutionList() const;

    inline int  GetNumber() const;
    inline int  GetNumberOfSensors() const;

    inline string GetOPSDesignator() const;
    inline string GetType() const;
    inline string GetSystemType() const;

    inline LinkList<Sensor>* GetSensorList() const;

    inline LinkList<ConjunctionPeriod>* GetConjunctionPeriodList(const ConjunctionPeriod::CONJUNCTION_TYPE &type) const;
    inline LinkList<ConjunctionPeriod>* GetNBConjunctionPeriodList() const;
    inline LinkList<ConjunctionPeriod>* GetWBConjunctionPeriodList() const;

    virtual inline const LinkList<Link>* GetTransmitLinksList() const;
    virtual inline int GetStartWidebandPaddingTime() const;
    virtual inline int GetEndWidebandPaddingTime() const;
    virtual inline int GetIndex() const;

    /**
     * Setters
     */
    void   SetProbability(int timeIndex,
                          int secondIndex,
                          double probability);
    void   SetProbability(int timeIndex,
                          double probability);

    void   SetConjunctionPeriod(int startTime,
                                int endTime,
                                int simultaneousSenders,
                                const string &receiverDesignator,
                                const string &sender1Designator,
                                const string &sender2Designator,
                                const ConjunctionPeriod::CONJUNCTION_TYPE &type);
    void   SetConjunctionPeriods(const Array<bool> *ptrConjunctionTimeStep,
                                 int simultaneousSenders,
                                 const string &sender1Designator,
                                 const string &sender2Designator,
                                 const ConjunctionPeriod::CONJUNCTION_TYPE &type);

    void SetLinkCriteria(const shared_ptr<LinkCriteria> &ptrLinkCriteria);
    void SetSensorList(LinkList<Sensor> *ptrSensorList);
    void SetType(shared_ptr<string> ptrType);
    void SetSystem(shared_ptr<string> ptrSystem);
    void SetOPSDesignator(shared_ptr<string> ptrOPSDesignator);

protected:

    int                          minimumProviderUseIndex_a;
    shared_ptr<string>           ptrOPSDesignator_a;
    shared_ptr<string>           ptrType_a;
    shared_ptr<string>           ptrSystem_a;
    Array2D<double>             *ptrProbability_a;
    LinkList<Sensor>            *ptrSensorList_a;
    LinkList<ConjunctionPeriod> *ptrNBConjunctionPeriodList_a;
    LinkList<ConjunctionPeriod> *ptrWBConjunctionPeriodList_a;

    shared_ptr<LinkCriteria>     ptrLinkCriteria_a;

private:

    void AppendConjunctionPeriod(ConjunctionPeriod *ptrPeriod,
                                 const ConjunctionPeriod::CONJUNCTION_TYPE &type);

    static inline string GetClassName();
};

inline
bool
DataProvider::HasNarrowbandConjunctionPeriod() const
{
    return(ptrNBConjunctionPeriodList_a != NULL);
}

inline
bool
DataProvider::HasWidebandConjunctionPeriod() const
{
    return(ptrWBConjunctionPeriodList_a != NULL);
}

inline
bool
DataProvider::IsAPrecludedPathReceiver(const Array<string> *ptrCurrentPrecludedDesignators) const
{
    return( ptrCurrentPrecludedDesignators->ValueExists(GetDesignator()) );
}

inline
int
DataProvider::GetStartWidebandPaddingTime() const
{
    return(SHORT_ZERO);
}

inline
int
DataProvider::GetEndWidebandPaddingTime() const
{
    return(SHORT_ZERO);
}

inline
int
DataProvider::GetIndex() const
{
    return(-1);
}

inline
bool
DataProvider::BodyTiltsForCollection() const
{
    return(false);
}

inline
int
DataProvider::GetNumber() const
{
    return(GetIndex() + 1);
}

inline
int
DataProvider::GetNumberOfSensors() const
{
    int numberOfSensors = 0;

    if (ptrSensorList_a != NULL) {
        numberOfSensors = ptrSensorList_a->Size();
    }

    return(numberOfSensors);
}

inline
string
DataProvider::GetOPSDesignator() const
{
    return(*ptrOPSDesignator_a);
}

inline
string
DataProvider::GetType() const
{
    return(*ptrType_a);
}

inline
string
DataProvider::GetSystemType() const
{
    return(*ptrSystem_a);
}

inline
LinkList<Sensor>*
DataProvider::GetSensorList() const
{
    return(ptrSensorList_a);
}

inline
LinkList<ConjunctionPeriod>*
DataProvider::GetConjunctionPeriodList(const ConjunctionPeriod::CONJUNCTION_TYPE &type) const
{
    LinkList<ConjunctionPeriod> *ptrConjunctionList = NULL;

    if (type == ConjunctionPeriod::NB) {
        ptrConjunctionList = ptrNBConjunctionPeriodList_a;
    } else if (type == ConjunctionPeriod::WB) {
        ptrConjunctionList = ptrWBConjunctionPeriodList_a;
    }

    return(ptrConjunctionList);
}

inline
LinkList<ConjunctionPeriod>*
DataProvider::GetNBConjunctionPeriodList() const
{
    return(ptrNBConjunctionPeriodList_a);
}

inline
LinkList<ConjunctionPeriod>*
DataProvider::GetWBConjunctionPeriodList() const
{
    return(ptrWBConjunctionPeriodList_a);
}

inline
const LinkList<Link>*
DataProvider::GetTransmitLinksList() const
{
    return(NULL);
}

inline
string
DataProvider::GetClassName()
{
    return(DATA_PROVIDER_H);
}

#endif
