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
 * DataProvider.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "DataProvider.h"
#include "mural/MURALTypes.h"
#include "mural/antenna/Antenna.h"
#include "mural/modules/lg/LinkCriteria.h"
#include "mural/io/MURALInputProcessor.h"
#include "mural/io/extract/JourneyExtractor.h"
#include "mural/modules/pg/Trek.h"
#include "mural/modules/pg/Orbit.h"
#include "mural/sensor/LongWaveSensor.h"
#include "mural/sensor/SARSensor.h"
#include "mural/sensor/SpectralSensor.h"
#include "mural/sensor/VisibleSensor.h"
#include "mural/sensor/WideFOVSensor.h"

#include "general/array/ListIterator.h"
#include "general/data/InputData.h"
#include "general/data/TimePeriod.h"
#include "general/time/TimePiece.h"
#include "general/exception/UpdateException.h"
#include "general/math/SimpleMath.h"
#include "general/math/SpaceVector.h"
#include "general/utility/StringEditor.h"


//#include "optimizer/Constraint.h"
//#include "optimizer/SolutionIndex.h"

//----------------------------------------------------------------------
//
//  Method Name: DataProvider::DataProvider
//
//  Purpose:
//    Constructor for the DataProvider class.
//
//  Calling Method:
//     DataProvider();
//
//  Calling Parameters
//  Parameters      i/o  description
//  ----------      ---  -----------
//
//  Returns:
//     None
//
//  Limitations:
//     None
//
//----------------------------------------------------------------------

DataProvider::DataProvider()
    : Element(UNKNOWN_ELEMENT_TYPE), minimumProviderUseIndex_a(INVALID_SOLUTION_INDEX),
      ptrOPSDesignator_a(NULL), ptrType_a(NULL), ptrSystem_a(NULL), ptrProbability_a(NULL),
      ptrLinkCriteria_a(NULL), ptrSensorList_a(NULL),
      ptrNBConjunctionPeriodList_a(NULL), ptrWBConjunctionPeriodList_a(NULL)
{
    return;
}

DataProvider&
DataProvider::operator = (const DataProvider &copyMe)
{
    minimumProviderUseIndex_a = INVALID_SOLUTION_INDEX;
    ptrDesignator_a = copyMe.ptrDesignator_a;
    ptrOPSDesignator_a = shared_ptr<string>(new string(*(copyMe.ptrOPSDesignator_a)));
    ptrType_a = shared_ptr<string>(new string(*(copyMe.ptrType_a)));
    ptrSystem_a = shared_ptr<string>(new string(*(copyMe.ptrSystem_a)));
    return(*this);
}

bool
DataProvider::operator == (const DataProvider &compareMe) const
{
    return( (*ptrDesignator_a == *(compareMe.ptrDesignator_a)) &&
            (*ptrType_a == *(compareMe.ptrType_a)) &&
            (*ptrSystem_a == *(compareMe.ptrSystem_a)) );
}

bool
DataProvider::operator != (const DataProvider &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
DataProvider::PrintAllNames(ostream &outfile) const
{
    if (ptrDesignator_a != NULL) {
        outfile << "   Data Provider Designator             : ";
        outfile << *ptrDesignator_a << NEW_LINE;
    }

    if (ptrOPSDesignator_a != NULL) {
        outfile << "   Operational Designator               : ";
        outfile << *ptrOPSDesignator_a << NEW_LINE;
    }

    if (ptrType_a != NULL) {
        outfile << "   Data Provider Type                   : ";
        outfile << *ptrType_a << NEW_LINE;
    }

    if (ptrSystem_a != NULL) {
        outfile << "   Data Provider System                 : ";
        outfile << *ptrSystem_a << NEW_LINE;
    }

    return;
}

void
DataProvider::PrintAttributes(ostream &outfile) const
{
    ListIterator<Sensor> sensorListIter(ptrSensorList_a);
    Sensor              *ptrSensor = sensorListIter.First();

    while (ptrSensor != NULL) {
        outfile << NEW_LINE;
        ptrSensor->PrintAttributes(outfile);
        ptrSensor = sensorListIter.Next();
    }

    if (ptrJourney_a != NULL) {
        outfile << NEW_LINE;
        ptrJourney_a->PrintAttributes(outfile);
    }

    if (ptrLinkCriteria_a != NULL) {
        outfile << NEW_LINE;
        ptrLinkCriteria_a->PrintAttributes(outfile);
    }

    return;
}

void
DataProvider::SetConjunctionPeriod(int startTime,
                                   int endTime,
                                   int simultaneousSenders,
                                   const string &receiverDesignator,
                                   const string &sender1Designator,
                                   const string &sender2Designator,
                                   const ConjunctionPeriod::CONJUNCTION_TYPE &type)
{
    ConjunctionPeriod *ptrConjunctionPeriod = new ConjunctionPeriod(startTime, endTime,
            simultaneousSenders,
            receiverDesignator,
            sender1Designator,
            sender2Designator,
            type);
    AppendConjunctionPeriod(ptrConjunctionPeriod, type);
    ptrConjunctionPeriod = NULL;
    return;
}

void
DataProvider::SetConjunctionPeriods(const Array<bool> *ptrConjunctionTimeStep,
                                    int simultaneousSenders,
                                    const string &sender1Designator,
                                    const string &sender2Designator,
                                    const ConjunctionPeriod::CONJUNCTION_TYPE &type)
{
    const string          receiverDesignator = GetDesignator();
    TimePeriod           *ptrPeriod = NULL;
    ConjunctionPeriod    *ptrConjunctionPeriod = NULL;
    LinkList<TimePeriod> *ptrPeriodList = TimePeriod::GroupIntoPeriods(ptrConjunctionTimeStep);

    if (ptrPeriodList != NULL) {
        //JOE:
        ptrPeriod = ptrPeriodList->Retrieve();

        while (ptrPeriod != NULL) {
            ptrConjunctionPeriod = new ConjunctionPeriod(ptrPeriod->GetStartTime(),
                    ptrPeriod->GetEndTime(),
                    simultaneousSenders,
                    receiverDesignator, sender1Designator,
                    sender2Designator, type);
            AppendConjunctionPeriod(ptrConjunctionPeriod, type);
            ptrConjunctionPeriod = NULL;
            delete ptrPeriod;
            ptrPeriod = NULL;
            ptrPeriod = ptrPeriodList->Retrieve();
        }

        delete ptrPeriodList;
        ptrPeriodList = NULL;
    }

    return;
}

void
DataProvider::AppendConjunctionPeriod(ConjunctionPeriod *ptrPeriod,
                                      const ConjunctionPeriod::CONJUNCTION_TYPE &type)
{
    if (type == ConjunctionPeriod::NB) {
        if (ptrNBConjunctionPeriodList_a == NULL) {
            ptrNBConjunctionPeriodList_a = new LinkList<ConjunctionPeriod>();
        }

        ptrNBConjunctionPeriodList_a->Append(ptrPeriod);
    } else if (type == ConjunctionPeriod::WB) {
        if (ptrWBConjunctionPeriodList_a == NULL) {
            ptrWBConjunctionPeriodList_a = new LinkList<ConjunctionPeriod>();
        }

        ptrWBConjunctionPeriodList_a->Append(ptrPeriod);
    }

    return;
}

Sensor*
DataProvider::GetSensor(int sensorNumber) const
{
    bool                 foundSensor = false;
    ListIterator<Sensor> sensorListIter(ptrSensorList_a);
    Sensor              *ptrSensor = sensorListIter.First();

    while ((ptrSensor != NULL) && (foundSensor == false)) {
        if (ptrSensor->GetSensorNumber() == sensorNumber) {
            foundSensor = true;
        } else {
            ptrSensor = sensorListIter.Next();
        }
    }

    return(ptrSensor);
}

double
DataProvider::GetMaximumSensorDataRate() const
{
    double               returnDataRate = 0.00;
    ListIterator<Sensor> sensorIter(ptrSensorList_a);
    Sensor              *ptrSensor = sensorIter.First();

    while (ptrSensor != NULL) {
        returnDataRate = std::max(returnDataRate,
                                  ptrSensor->GetDataRate());
        ptrSensor = sensorIter.Next();
    }

    return(returnDataRate);
}

double
DataProvider::GetMinimumSensorDataRate() const
{
    double  returnDataRate = 0.0;
    Sensor *ptrSensor = NULL;

    if (ptrSensorList_a != NULL) {
        ListIterator<Sensor> sensorIter(ptrSensorList_a);
        returnDataRate = DBL_MAX;
        ptrSensor = sensorIter.First();

        while (ptrSensor != NULL) {
            returnDataRate = std::min(returnDataRate,
                                      ptrSensor->GetDataRate());
            ptrSensor = sensorIter.Next();
        }
    }

    return(returnDataRate);
}

void
DataProvider::AddActiveSensors(Array<bool> *&ptrActiveSensor) const
{
    ListIterator<Sensor> sensorListIter(ptrSensorList_a);
    Sensor              *ptrSensor = sensorListIter.First();

    if (ptrActiveSensor == NULL) {
        ptrActiveSensor = new Array<bool>(Sensor::GetNumberOfTargetSensors(), false);
    }

    while (ptrSensor != NULL) {
        (*ptrActiveSensor)[ptrSensor->GetSensorIndex()] = true;
        ptrSensor = sensorListIter.Next();
    }

    return;
}


void
DataProvider::InitializeProbabilityArray(int secondDimension)
{
    const double initialValue = -1.0;

    if (ptrProbability_a == NULL) {
        ptrProbability_a = new Array2D<double>(TimePiece::GetNumberOfTimeSteps(),
                                               secondDimension, initialValue);
    } else {
        ptrProbability_a->ResetValues(initialValue);
    }

    return;
}

Array2D<double>*
DataProvider::GetProbability() const
{
    return ptrProbability_a;
}

double
DataProvider::GetProbability(int timeIndex) const
{
    double probability = -1.0;

    if (ptrProbability_a != NULL) {
        try {
            int secondIndex = 0;
            int secondDimension = 0;
            secondDimension = ptrProbability_a->GetSize(timeIndex);

            while (secondIndex < secondDimension) {
                if (ptrProbability_a->ElementAt(timeIndex, secondIndex) > 0.00) {
                    probability += ptrProbability_a->ElementAt(timeIndex, secondIndex);
                }

                ++secondIndex;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "GetProbability(int)");
            throw;
        }
    }

    return(probability);
}

double
DataProvider::GetProbability(int timeIndex,
                             int secondIndex) const
{
    double probability = -1.0;

    if (ptrProbability_a != NULL) {
        try {
            probability = ptrProbability_a->ElementAt(timeIndex, secondIndex);
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "GetProbability(int, int)");
            throw;
        }
    }

    return(probability);
}

void
DataProvider::SetProbability(int timeIndex,
                             int secondIndex,
                             double probability)
{
    try {
        (*ptrProbability_a)[timeIndex][secondIndex]= probability;
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "SetProbability(int, int, const double&)");
        throw;
    }

    return;
}

void
DataProvider::SetProbability(int timeIndex,
                             double probability)
{
    try {
        int secondIndex = 0;
        int secondDimension = 0;
        secondDimension = ptrProbability_a->GetSize(timeIndex);

        while (secondIndex < secondDimension) {
            (*ptrProbability_a)[timeIndex][secondIndex] = probability;
            ++secondIndex;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "SetProbability(int, const double&)");
        throw;
    }

    return;
}

void
DataProvider::PrintProbabilityArray(ofstream &debugFile)
{
    if (ptrProbability_a != NULL) {
        ptrProbability_a->Print(debugFile);
    } else {
        debugFile << GetDesignator() << " : is empty!" << NEW_LINE;
    }
}


bool
DataProvider::ConjunctionProblem(const ConjunctionPeriod::CONJUNCTION_TYPE &type,
                                 int timeIndex,
                                 const string &transmitDesignator)
{
    bool                            problem = false;
    Link                           *ptrLink = NULL;
    ListIterator<ConjunctionPeriod> periodListIter(GetConjunctionPeriodList(type));
    ConjunctionPeriod              *ptrPeriod = periodListIter.First();
    MURALTypes::AntennaVector::iterator antIter;
    shared_ptr<Antenna> ptrAntenna = NULL;
    shared_ptr<MURALTypes::AntennaVector> ptrAntennaList = GetConjunctionAntennaList();

    while ((ptrPeriod != NULL) && (problem == false)) {
        if (ptrPeriod->IsDuringTimePeriod(timeIndex) == true) {
            for(antIter = ptrAntennaList->begin(); (antIter != ptrAntennaList->end()) && (problem == false); ++antIter) {
                ptrAntenna = (*antIter);

                if (ptrAntenna->IsConjunctionAntenna() == true) {
                    ptrLink = NULL;

                    if (ptrPeriod->GetSender1Designator() == transmitDesignator) {
                        ptrLink = ptrAntenna->GetLinkFrom(ptrPeriod->GetSender2Designator());
                    } else if (ptrPeriod->GetSender2Designator() == transmitDesignator) {
                        ptrLink = ptrAntenna->GetLinkFrom(ptrPeriod->GetSender1Designator());
                    }

                    if (ptrLink != NULL) {
                        if (type == ConjunctionPeriod::WB) {
                            problem = ptrLink->IsAllocatedWideband(timeIndex);
                        } else {
                            problem = ptrLink->IsAllocated(timeIndex);
                        }
                    }
                }
            }
        }

        ptrPeriod = periodListIter.Next();
    }

    return(problem);
}

bool
DataProvider::IsWidebandConjunctionTimeStep(int timeIndex,
        const string &sender1Designator,
        const string &sender2Designator) const
{
    bool isWidebandConjunctionTimeStep = false;
    ListIterator<ConjunctionPeriod> periodListIter(ptrWBConjunctionPeriodList_a);
    ConjunctionPeriod              *ptrPeriod = periodListIter.First();

    while ((ptrPeriod != NULL) && (isWidebandConjunctionTimeStep == false)) {
        if (ptrPeriod->IsDuringTimePeriod(timeIndex) == true) {
            if ( ((ptrPeriod->GetSender1Designator() == sender1Designator) &&
                    (ptrPeriod->GetSender2Designator() == sender2Designator)) ||
                    ((ptrPeriod->GetSender1Designator() == sender2Designator) &&
                     (ptrPeriod->GetSender2Designator() == sender1Designator)) ) {
                isWidebandConjunctionTimeStep = true;
            }
        }

        ptrPeriod = periodListIter.Next();
    }

    return(isWidebandConjunctionTimeStep);
}

bool
DataProvider::IsWidebandConjunctionTimeStep(int startTimeIndex,
        int endTimeIndex,
        const string &sender1Designator,
        const string &sender2Designator) const
{
    bool        isWidebandConjunctionTimeStep = false;
    TimePeriod *ptrCheckTimePeriod = new TimePeriod(startTimeIndex, endTimeIndex);
    ListIterator<ConjunctionPeriod> periodListIter(ptrWBConjunctionPeriodList_a);
    ConjunctionPeriod              *ptrPeriod = periodListIter.First();

    while ((ptrPeriod != NULL) && (isWidebandConjunctionTimeStep == false)) {
        if (ptrPeriod->OverlapsTimePeriod(ptrCheckTimePeriod) == true) {
            if ( ((ptrPeriod->GetSender1Designator() == sender1Designator) &&
                    (ptrPeriod->GetSender2Designator() == sender2Designator)) ||
                    ((ptrPeriod->GetSender1Designator() == sender2Designator) &&
                     (ptrPeriod->GetSender2Designator() == sender1Designator)) ) {
                isWidebandConjunctionTimeStep = true;
            }
        }

        ptrPeriod = periodListIter.Next();
    }

    delete ptrCheckTimePeriod;
    ptrCheckTimePeriod = NULL;
    return(isWidebandConjunctionTimeStep);
}

bool
DataProvider::ShouldCheckConjunction(const string &sender1Designator,
                                     const string &sender2Designator,
                                     int timeIndex)
{
    bool   shouldCheck1 = false;
    bool   shouldCheck2 = false;
    Link  *ptrLink1 = NULL;
    Link  *ptrLink2 = NULL;
    MURALTypes::AntennaVector::iterator antIter;
    shared_ptr<Antenna> ptrAntenna = NULL;
    shared_ptr<MURALTypes::AntennaVector> ptrAntennaList = GetConjunctionAntennaList();

    for(antIter = ptrAntennaList->begin(); (antIter != ptrAntennaList->end()) && ((shouldCheck1 == false) || (shouldCheck2 == false)); ++antIter) {
        ptrAntenna = (*antIter);

        if (ptrAntenna->IsConjunctionAntenna() == true) {
            if (ptrAntenna->ShouldCheckConjunction(sender1Designator, timeIndex) == true) {
                shouldCheck1 = true;
            }

            if (ptrAntenna->ShouldCheckConjunction(sender2Designator, timeIndex) == true) {
                shouldCheck2 = true;
            }
        }
    }

    return((shouldCheck1 == true) && (shouldCheck2 == true));
}

LinkList<Revolution>*
DataProvider::GenerateSouthPointRevolutionList() const
{
    return(ptrJourney_a->GenerateSouthPointRevolutionList());
}

void
DataProvider::PrintUpdatedOrbitFile(ofstream &updatedOrbitFile)
{
    ptrJourney_a->PrintUpdatedOrbitFile(updatedOrbitFile, *ptrDesignator_a);
    return;
}

bool
DataProvider::IsStationaryPosition() const
{
    return(ptrJourney_a->IsStationaryPosition());
}

const LinkList<Revolution>*
DataProvider::GetRevolutionList() const
{
    return(ptrJourney_a->GetRevolutionList());
}

int
DataProvider::GetMaximumManeuverTime() const
{
    int maximumManeuverTime = 0;

    if (ptrLinkCriteria_a != NULL) {
        maximumManeuverTime = ptrLinkCriteria_a->GetMaximumManeuverTime();
    }

    return(maximumManeuverTime);
}

int
DataProvider::GetMaximumSimultaneousConjunctionSenders() const
{
    int maximumSenders = 0;

    if (ptrLinkCriteria_a != NULL) {
        maximumSenders = ptrLinkCriteria_a->GetMaximumSimultaneousConjunctionSenders();
    }

    return(maximumSenders);
}

shared_ptr<vector<int>>
                     DataProvider::GetRelayCoverageContourArray() const
{
    shared_ptr<vector<int>> ptrContourValueArray = NULL;

    if (ptrLinkCriteria_a != NULL) {
        ptrContourValueArray = ptrLinkCriteria_a->GetRelayCoverageContourArray();
    }

    return(ptrContourValueArray);
}

int
DataProvider::GetMinimumNumberOfHops(Array<int> *ptrVisitedRelayIndicies) const
{
    return(SHORT_ZERO);
}

double
DataProvider::GetNarrowbandConjunctionAngle(const string &nearDesignator,
        const string &farDesignator,
        int simultaneousSenders) const
{
    double angle = UNDEFINED_ANGLE;

    if (ptrLinkCriteria_a != NULL) {
        angle = ptrLinkCriteria_a->GetNarrowbandConjunctionAngle(nearDesignator, farDesignator,
                simultaneousSenders);
    }

    return(angle);
}

double
DataProvider::GetWidebandConjunctionAngle(const string &nearDesignator,
        const string &farDesignator,
        int simultaneousSenders) const
{
    double angle = UNDEFINED_ANGLE;

    if (ptrLinkCriteria_a != NULL) {
        angle = ptrLinkCriteria_a->GetWidebandConjunctionAngle(nearDesignator, farDesignator,
                simultaneousSenders);
    }

    return(angle);
}

double
DataProvider::GetMinimumICCLSolarIntrusionAngle() const
{
    double angle = UNDEFINED_ANGLE;

    if (ptrLinkCriteria_a != NULL) {
        angle = ptrLinkCriteria_a->GetMinimumICCLSolarIntrusionAngle();
    }

    return(angle);
}

bool
DataProvider::HasNarrowbandConjunctionData() const
{
    bool hasNarrowbandConjunction = false;

    if (ptrLinkCriteria_a != NULL) {
        hasNarrowbandConjunction = ptrLinkCriteria_a->HasNarrowbandConjunctionData();
    }

    return(hasNarrowbandConjunction);
}

bool
DataProvider::HasWidebandConjunctionData() const
{
    bool hasWidebandConjunction = false;

    if (ptrLinkCriteria_a != NULL) {
        hasWidebandConjunction = ptrLinkCriteria_a->HasWidebandConjunctionData();
    }

    return(hasWidebandConjunction);
}

bool
DataProvider::HasSimultaneousConjunctionSenders(int simultaneousSenders) const
{
    bool hasSenders = false;

    if (ptrLinkCriteria_a != NULL) {
        hasSenders = ptrLinkCriteria_a->HasSimultaneousConjunctionSenders(simultaneousSenders);
    }

    return(hasSenders);
}

bool
DataProvider::ReadLinkPaths() const
{
    bool readLinkPaths = false;

    if (ptrLinkCriteria_a != NULL) {
        readLinkPaths = ptrLinkCriteria_a->ReadLinkPaths();
    }

    return(readLinkPaths);
}

bool
DataProvider::ReadConjunctionData() const
{
    bool readConjunctionData = false;

    if (ptrLinkCriteria_a != NULL) {
        readConjunctionData = ptrLinkCriteria_a->ReadConjunctionData();
    }

    return(readConjunctionData);
}

/* ****************************************************************************** */

const OverheadData*
DataProvider::GetLinkOverheadData(const string &designator) const
{
    const OverheadData *ptrOverheadData = NULL;

    if (ptrLinkCriteria_a != NULL) {
        ptrOverheadData = ptrLinkCriteria_a->GetLinkOverheadData(designator);
    }

    return(ptrOverheadData);
}

/* ****************************************************************************** */

shared_ptr<LinkCriteria>
DataProvider::GetLinkCriteria() const
{
    return (ptrLinkCriteria_a);
}

/* ****************************************************************************** */

void
DataProvider::SetLinkCriteria(const shared_ptr<LinkCriteria> &ptrLinkCriteria)
{
    ptrLinkCriteria_a = ptrLinkCriteria;
}

/* ****************************************************************************** */

void
DataProvider::SetSensorList(LinkList<Sensor> *ptrSensorList)
{
    ptrSensorList_a = ptrSensorList;
}

/* ****************************************************************************** */

void
DataProvider::SetType(shared_ptr<string> ptrType)
{
    ptrType_a = ptrType;
}

/* ****************************************************************************** */

void
DataProvider::SetSystem(shared_ptr<string> ptrSystem)
{
    ptrSystem_a = ptrSystem;
}

/* ****************************************************************************** */

void
DataProvider::SetOPSDesignator(shared_ptr<string> ptrOPSDesignator)
{
    ptrOPSDesignator_a = ptrOPSDesignator;
}

/* ****************************************************************************** */

shared_ptr<MURALTypes::AntennaVector>
DataProvider::GetConjunctionAntennaList()
{
    return(NULL);
}

/* ****************************************************************************** */

double
DataProvider::GetTotalAllocatedDataRate(int timeIndex) const
{
    return(0.0);
}

DataProvider::~DataProvider()
{

    if (ptrProbability_a != NULL) {
        delete ptrProbability_a;
        ptrProbability_a = NULL;
    }

    if (ptrSensorList_a != NULL) {
        ptrSensorList_a->Delete();
        delete ptrSensorList_a;
        ptrSensorList_a = NULL;
    }

    if (ptrNBConjunctionPeriodList_a != NULL) {
        ptrNBConjunctionPeriodList_a->Delete();
        delete ptrNBConjunctionPeriodList_a;
        ptrNBConjunctionPeriodList_a = NULL;
    }

    if (ptrWBConjunctionPeriodList_a != NULL) {
        ptrWBConjunctionPeriodList_a->Delete();
        delete ptrWBConjunctionPeriodList_a;
        ptrWBConjunctionPeriodList_a = NULL;
    }

    return;
}

