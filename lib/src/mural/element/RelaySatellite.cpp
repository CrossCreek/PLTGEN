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
 * RelaySatellite.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include <algorithm>

#include "UserVehicle.h"
#include "RelaySatellite.h"
#include "GroundStation.h"
#include "mural/io/MURALInputProcessor.h"
#include "mural/io/OutputGenerator.h"
#include "mural/io/FileNames.h"
#include "mural/antenna/GroundPointingAntenna.h"
#include "mural/antenna/ReceiveFacilityAntenna.h"
#include "mural/antenna/RelayPointingAntenna.h"
#include "mural/antenna/UserPointingAntenna.h"
#include "mural/modules/lg/LinkCriteria.h"
#include "mural/modules/lg/LinksGenerator.h"
#include "mural/modules/lg/OverheadData.h"
#include "mural/coorframe/ElementCoordinateFrame.h"
#include "general/exception/InputException.h"
#include "general/exception/ClassCastException.h"
#include "general/array/ListIterator.h"
#include "general/utility/Utility.h"
#include "general/utility/StringEditor.h"
#include "general/math/SimpleMath.h"
#include "general/math/CoordinateFrame.h"

bool           RelaySatellite::anyStateOfHealthRelays_s = false;
const char     RelaySatellite::RELAY_USE_TIME_STEP_ID_s = 'r';
Array<string>* RelaySatellite::ptrIdentificationArray_s = NULL;

//------------------------------------------------------------------------------
//
//  Method Name: RelaySatellite::RelaySatellite
//
//  Purpose:
//    Constructor for the RelaySatellite class.
//
//  Calling Method:
//     RelaySatellite();
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
//------------------------------------------------------------------------------

RelaySatellite::RelaySatellite()
    : DataProvider(), bodyTiltAngle_a(0.0),
      ptrUPAList_a(NULL), ptrGPAList_a(NULL),
      ptrRPAList_a(NULL), ptrStateOfHealthPeriodList_a(NULL)
{
    elementType_a = RELAY_SATELLITE;
    ptrUPAList_a = shared_ptr<MURALTypes::UPAVector>(new MURALTypes::UPAVector());
    ptrRPAList_a = shared_ptr<MURALTypes::RPAVector>(new MURALTypes::RPAVector());
    ptrGPAList_a = shared_ptr<MURALTypes::GPAVector>(new MURALTypes::GPAVector());
    return;
}

RelaySatellite&
RelaySatellite::operator = (const RelaySatellite &copyMe)
{
    DataProvider::operator = (copyMe);
    bodyTiltAngle_a = copyMe.bodyTiltAngle_a;

    ptrUPAList_a = NULL;

    if (copyMe.ptrUPAList_a != NULL) {
        ptrUPAList_a = copyMe.ptrUPAList_a;
    }

    ptrGPAList_a = NULL;

    if (copyMe.ptrGPAList_a != NULL) {
        ptrGPAList_a = copyMe.ptrGPAList_a;
    }

    ptrRPAList_a = NULL;

    if (copyMe.ptrRPAList_a != NULL) {
        ptrRPAList_a = copyMe.ptrRPAList_a;
    }

    ptrStateOfHealthPeriodList_a = NULL;

    if (copyMe.ptrStateOfHealthPeriodList_a != NULL) {
        ptrStateOfHealthPeriodList_a
            = new LinkList<TimePeriod>(*(copyMe.ptrStateOfHealthPeriodList_a));
    }

    return(*this);
}

bool
RelaySatellite::operator == (const RelaySatellite &compareMe) const
{
    return( (DataProvider::operator == (compareMe)) &&
            (bodyTiltAngle_a == compareMe.bodyTiltAngle_a) );
}

void
RelaySatellite::ResetIdentification()
{
    if (ptrIdentificationArray_s != NULL) {
        delete ptrIdentificationArray_s;
        ptrIdentificationArray_s = NULL;
    }
}

void
RelaySatellite::SetIdentification(const Array<string> *ptrIdentification)
{
    int       satelliteIndex = 0;
    const int numberOfRelays = ptrIdentification->Size();
    string     designator = "";

    if (ptrIdentificationArray_s == NULL) {
        ptrIdentificationArray_s = new Array<string>();
    }

    while (satelliteIndex < numberOfRelays) {
        designator = ptrIdentification->ElementAt(satelliteIndex);

        if (ptrIdentificationArray_s->ValueExists(designator) == true) {
            throw new InputException(GetClassName(),
                                     "SetIdentification(const Array<string>*)",
                                     "Can not have multiple Relay Satellites with the same Designator!");
        }

        ptrIdentificationArray_s->AddElement(designator);
        ++satelliteIndex;
    }

    return;
}

void
RelaySatellite::SortIdentificationArray()
{
    Array<string> *ptrSortedIdentificationArray;

    if(ptrIdentificationArray_s != NULL) {
        int arrayIndex = 0;
        const int arraySize = ptrIdentificationArray_s->Size();
        ptrSortedIdentificationArray = new Array<string>();

        while (arrayIndex < arraySize) {
            ptrSortedIdentificationArray->InsertElement(ptrIdentificationArray_s->ElementAt(arrayIndex));
            ++arrayIndex;
        }

        delete ptrIdentificationArray_s;
        ptrIdentificationArray_s = ptrSortedIdentificationArray;
        ptrSortedIdentificationArray = NULL;
    }
}

void
RelaySatellite::PrintAttributes(ostream &outfile) const
{
    MURALTypes::UPAVector::iterator upaIter;
    MURALTypes::GPAVector::iterator gpaIter;
    MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<UserPointingAntenna>   ptrUPA = NULL;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;
    outfile << "<RELAY_SATELLITE_OBJECT_START>" << NEW_LINE;
    PrintAllNames(outfile);

    if (bodyTiltAngle_a != 0.0) {
        outfile << "   Body Tilt Angle                      : ";
        outfile << UnitData::GetInputAngle(bodyTiltAngle_a) << NEW_LINE;
    }

    for(upaIter = ptrUPAList_a->begin(); upaIter != ptrUPAList_a->end(); ++upaIter) {
        ptrUPA = (*upaIter);
        outfile << NEW_LINE;
        ptrUPA->PrintAttributes(outfile);
    }

    for(rpaIter = ptrRPAList_a->begin(); rpaIter != ptrRPAList_a->end(); ++rpaIter) {
        ptrRPA = (*rpaIter);
        outfile << NEW_LINE;
        ptrRPA->PrintAttributes(outfile);
    }

    for(gpaIter = ptrGPAList_a->begin(); gpaIter != ptrGPAList_a->end(); ++gpaIter) {
        ptrGPA = (*gpaIter);
        outfile << NEW_LINE;
        ptrGPA->PrintAttributes(outfile);
    }

    DataProvider::PrintAttributes(outfile);

    outfile << "<RELAY_SATELLITE_OBJECT_END>" << NEW_LINE;
    return;
}

void
RelaySatellite::SetMinimumNumberOfTransmitChannels(const bool &constrainMaximumThroughput)
{
    MURALTypes::GPAVector::iterator gpaIter;
    MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); gpaIter != ptrGPAList_a->end(); ++gpaIter) {
        ptrGPA = (*gpaIter);

        if (constrainMaximumThroughput == false) {
            ptrGPA->ResetNumberOfTransmitChannels();
        } else {
            ptrGPA->SetMinimumNumberOfTransmitChannels();
        }
    }

    for(rpaIter = ptrRPAList_a->begin(); rpaIter != ptrRPAList_a->end(); ++rpaIter) {
        ptrRPA = (*rpaIter);

        if ((constrainMaximumThroughput == false) || (ptrRPA->IsTransmittingAntenna() == false)) {
            ptrRPA->ResetNumberOfTransmitChannels();
        } else {
            ptrRPA->SetMinimumNumberOfTransmitChannels();
        }
    }
}

void
RelaySatellite::CheckTransmitLinkStatus()
{
    bool                                foundValidLink = false;
    Link                               *ptrLink = NULL;
    MURALTypes::GPAVector::iterator gpaIter;
    MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); gpaIter != ptrGPAList_a->end(); ++gpaIter) {
        ptrGPA = (*gpaIter);
        ListIterator<Link> downlinksListIter(ptrGPA->GetLinksList());
        foundValidLink = false;
        ptrLink = downlinksListIter.First();

        while (ptrLink != NULL) {
            if (ptrLink->IsPrecluded() == false) {
                foundValidLink = true;
                ptrLink = NULL;
            } else {
                ptrLink = downlinksListIter.Next();
            }
        }

        if (foundValidLink == false) {
            throw new InputException(GetClassName(), "CheckTransmitLinkStatus()",
                                     (GetDesignator() + " has no valid link for ground pointing antenna " + ptrGPA->GetAntennaDesignator()));
        }
    }

    for(rpaIter = ptrRPAList_a->begin(); rpaIter != ptrRPAList_a->end(); ++rpaIter) {
        ptrRPA = (*rpaIter);

        if (ptrRPA->IsTransmittingAntenna() == true) {
            ListIterator<Link> multiHopLinksListIter(ptrRPA->GetLinksList());
            foundValidLink = false;
            ptrLink = multiHopLinksListIter.First();

            while (ptrLink != NULL) {
                if (ptrLink->IsPrecluded() == false) {
                    foundValidLink = true;
                    ptrLink = NULL;
                } else {
                    ptrLink = multiHopLinksListIter.Next();
                }
            }

            if (foundValidLink == false) {
                throw new InputException(GetClassName(), "CheckTransmitLinkStatus()",
                                         (GetDesignator() + " has no valid link found for transmit relay pointing antenna"));
            }
        }
    }

    return;
}

int
RelaySatellite::GetGroundPointingAntennaCapacity() const
{
    int      totalCapacity = 0;
    MURALTypes::GPAVector::iterator gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); gpaIter != ptrGPAList_a->end(); ++gpaIter) {
        ptrGPA = (*gpaIter);
        totalCapacity += ptrGPA->GetCapacity();
    }

    return(totalCapacity);
}

int
RelaySatellite::GetTransmitRelayPointingAntennaCapacity() const
{
    int      totalCapacity = 0;
    MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;

    for(rpaIter = ptrRPAList_a->begin(); rpaIter != ptrRPAList_a->end(); ++rpaIter) {
        ptrRPA = (*rpaIter);

        if (ptrRPA->IsTransmittingAntenna() == true) {
            totalCapacity += ptrRPA->GetCapacity();
        }
    }

    return(totalCapacity);
}

int
RelaySatellite::GetMinimumDownlinkAcquisitionTime() const
{
    int                  minimumDownlinkAcquisitionTime = 0;
    MURALTypes::GPAVector::iterator gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    if (ptrGPAList_a->size() > 0) {
        minimumDownlinkAcquisitionTime = SHRT_MAX;

        for(gpaIter = ptrGPAList_a->begin(); gpaIter != ptrGPAList_a->end(); ++gpaIter) {
            ptrGPA = (*gpaIter);
            minimumDownlinkAcquisitionTime = std::min(minimumDownlinkAcquisitionTime,
                                             ptrGPA->GetMinimumAcquisitionTime());
        }
    }

    return(minimumDownlinkAcquisitionTime);
}

int
RelaySatellite::GetMinimumDownlinkDroplinkTime() const
{
    int                  minimumDownlinkDroplinkTime = 0;
    MURALTypes::GPAVector::iterator gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    if (ptrGPAList_a->size() > 0) {
        minimumDownlinkDroplinkTime = SHRT_MAX;

        for(gpaIter = ptrGPAList_a->begin(); gpaIter != ptrGPAList_a->end(); ++gpaIter) {
            ptrGPA = (*gpaIter);
            minimumDownlinkDroplinkTime = std::min(minimumDownlinkDroplinkTime,
                                                   ptrGPA->GetMinimumDroplinkTime());
        }
    }

    return(minimumDownlinkDroplinkTime);
}

int
RelaySatellite::GetMinimumNumberOfHops(Array<int> *ptrVisitedRelayIndicies) const
{
    int                 minimumNumberOfHops = SHRT_MAX;
    bool                  initalizedArray = false;
    const string          relayDesignator = GetDesignator();
    Link                 *ptrLink = NULL;
    MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;

    if (ptrVisitedRelayIndicies == NULL) {
        ptrVisitedRelayIndicies = new Array<int>();
        initalizedArray = true;
    }

    if (ptrGPAList_a->size() > 0) {
        minimumNumberOfHops = 1;
    } else {
        int numberOfHops = 0;
        int receiveRelayIndex = 0;

        for(rpaIter = ptrRPAList_a->begin(); rpaIter != ptrRPAList_a->end(); ++rpaIter) {
            ptrRPA = (*rpaIter);

            if (ptrRPA->IsTransmittingAntenna() == true) {
                ListIterator<Link> multiHopLinksListIter(ptrRPA->GetLinksList());
                ptrLink = multiHopLinksListIter.First();

                while (ptrLink != NULL) {
                    if (ptrLink->GetTransmitDesignator() == relayDesignator) {
                        receiveRelayIndex = ptrLink->GetReceiveIndex();

                        if (ptrVisitedRelayIndicies->ValueExists(receiveRelayIndex) == false) {
                            ptrVisitedRelayIndicies->AddElement(receiveRelayIndex);
                            const shared_ptr<RelaySatellite> ptrReceiveRelay =
                                dynamic_pointer_cast<RelaySatellite>(ptrLink->GetReceiveProvider());
                            numberOfHops = 1 + ptrReceiveRelay->GetMinimumNumberOfHops(ptrVisitedRelayIndicies);
                            minimumNumberOfHops = std::min(minimumNumberOfHops, numberOfHops);
                        }
                    }

                    ptrLink = multiHopLinksListIter.Next();
                }
            }
        }
    }

    if (initalizedArray == true) {
        delete ptrVisitedRelayIndicies;
        ptrVisitedRelayIndicies = NULL;
    }

    return(minimumNumberOfHops);
}

int
RelaySatellite::GetTotalTransmitCapacity() const
{
    return(GetGroundPointingAntennaCapacity() + GetTransmitRelayPointingAntennaCapacity());
}

int
RelaySatellite::GetUserPointingAntennaCapacity() const
{
    int      totalCapacity = 0;
    MURALTypes::UPAVector::iterator upaIter;
    shared_ptr<UserPointingAntenna>   ptrUPA = NULL;

    for(upaIter = ptrUPAList_a->begin(); upaIter != ptrUPAList_a->end(); ++upaIter) {
        ptrUPA = (*upaIter);
        totalCapacity += ptrUPA->GetCapacity();
    }

    return(totalCapacity);
}

int
RelaySatellite::GetMaximumPreparationTimeUPA() const
{
    int                              maxPreparationTime = 0;
    MURALTypes::UPAVector::iterator upaIter;
    shared_ptr<UserPointingAntenna>   ptrUPA = NULL;

    for(upaIter = ptrUPAList_a->begin(); upaIter != ptrUPAList_a->end(); ++upaIter) {
        ptrUPA = (*upaIter);
        maxPreparationTime = std::max(maxPreparationTime,
                                      ptrUPA->GetPreparationTime());
    }

    return(maxPreparationTime);
}

int
RelaySatellite::GetNumberOfReceiveAntenna() const
{
    int                                numberOfReceiveAntenna = GetNumberOfUPAs();
    MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;

    for(rpaIter = ptrRPAList_a->begin(); rpaIter != ptrRPAList_a->end(); ++rpaIter) {
        ptrRPA = (*rpaIter);

        if (ptrRPA->IsReceiveAntenna() == true) {
            ++numberOfReceiveAntenna;
        }
    }

    return(numberOfReceiveAntenna);
}

int
RelaySatellite::GetTotalNumberOfTransmitChannels() const
{
    int      totalTransmitChannels = 0;
    MURALTypes::GPAVector::iterator gpaIter;
    MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); gpaIter != ptrGPAList_a->end(); ++gpaIter) {
        ptrGPA = (*gpaIter);
        totalTransmitChannels += ptrGPA->GetNumberOfTransmitChannels();
    }

    for(rpaIter = ptrRPAList_a->begin(); rpaIter != ptrRPAList_a->end(); ++rpaIter) {
        ptrRPA = (*rpaIter);

        if (ptrRPA->IsTransmittingAntenna() == true) {
            totalTransmitChannels += ptrRPA->GetNumberOfTransmitChannels();
        }
    }

    return(totalTransmitChannels);
}

Array<double>*
RelaySatellite::GetChannelDataRates() const
{
    int          channelIndex = 0;
    int          numberOfChannels = 0;
    int          totalNumberOfChannels = 0;
    double         totalDataRate = 0.0;
    Array<double> *ptrDataRates = NULL;
    MURALTypes::GPAVector::iterator gpaIter;
    MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); gpaIter != ptrGPAList_a->end(); ++gpaIter) {
        ptrGPA = (*gpaIter);
        numberOfChannels = ptrGPA->GetNumberOfTransmitChannels();

        if (numberOfChannels > 0) {
            if (ptrDataRates == NULL) {
                ptrDataRates = new Array<double>();
            }

            channelIndex = 0;

            while (channelIndex < numberOfChannels) {
                ptrDataRates->AddElement(ptrGPA->GetDataRatePerChannel());
                ++channelIndex;
            }

            totalNumberOfChannels += numberOfChannels;
            totalDataRate += ptrGPA->GetDataRate();
        }
    }

    for(rpaIter = ptrRPAList_a->begin(); rpaIter != ptrRPAList_a->end(); ++rpaIter) {
        ptrRPA = (*rpaIter);

        if (ptrRPA->IsTransmittingAntenna() == true) {
            numberOfChannels = ptrRPA->GetNumberOfTransmitChannels();

            if (numberOfChannels > 0) {
                ptrDataRates = new Array<double>();
                channelIndex = 0;

                while (channelIndex < numberOfChannels) {
                    ptrDataRates->AddElement(ptrRPA->GetDataRatePerChannel());
                    ++channelIndex;
                }

                totalNumberOfChannels += numberOfChannels;
                totalDataRate += ptrRPA->GetDataRate();
            }
        }
    }

    if (totalNumberOfChannels > 1) {
        ptrDataRates->AddElement(totalDataRate);
    }

    return(ptrDataRates);
}

shared_ptr<UserPointingAntenna>
RelaySatellite::GetUPA(const string &antennaDesignator, int &upaIndex) const
{
    bool                              foundUPA = false;
    MURALTypes::UPAVector::iterator upaIter;
    shared_ptr<UserPointingAntenna> ptrUPA = NULL;
    upaIndex = 0;

    for(upaIter = ptrUPAList_a->begin(); (upaIter != ptrUPAList_a->end()) && (foundUPA == false); ++upaIter) {
        ptrUPA = (*upaIter);

        if (ptrUPA->GetAntennaDesignator() == antennaDesignator) {
            foundUPA = true;
        } else {
            ++upaIndex;
        }
    }

    return(ptrUPA);
}

bool
RelaySatellite::HasConjunctionGPA() const
{
    bool                                hasConjunctionGPA = false;
    MURALTypes::GPAVector::iterator gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); (gpaIter != ptrGPAList_a->end()) && (hasConjunctionGPA == false); ++gpaIter) {
        ptrGPA = (*gpaIter);
        hasConjunctionGPA = ptrGPA->IsConjunctionAntenna();
    }

    return(hasConjunctionGPA);
}

bool
RelaySatellite::HasReceiveRPA() const
{
    bool hasReceiveRPA = false;
    MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;

    for(rpaIter = ptrRPAList_a->begin(); (rpaIter != ptrRPAList_a->end()) && (hasReceiveRPA == false); ++rpaIter) {
        ptrRPA = (*rpaIter);
        hasReceiveRPA = ptrRPA->IsReceiveAntenna();
    }

    return(hasReceiveRPA);
}

bool
RelaySatellite::HasTransmitRPA() const
{
    bool hasTransmitRPA = false;
    MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;

    for(rpaIter = ptrRPAList_a->begin(); (rpaIter != ptrRPAList_a->end()) && (hasTransmitRPA == false); ++rpaIter) {
        ptrRPA = (*rpaIter);
        hasTransmitRPA = ptrRPA->IsTransmittingAntenna();
    }

    return(hasTransmitRPA);
}

shared_ptr<GroundPointingAntenna>
RelaySatellite::GetGPA(const string &antennaDesignator) const
{
    bool                              foundGPA = false;
    MURALTypes::GPAVector::iterator   gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); (gpaIter != ptrGPAList_a->end()) && (foundGPA == false); ++gpaIter) {
        ptrGPA = (*gpaIter);

        if (ptrGPA->GetAntennaDesignator() == antennaDesignator) {
            foundGPA = true;
        }
    }

    return(ptrGPA);
}

double
RelaySatellite::GetMaximumTransmitDataRate() const
{
    double                              maximumTransmitDataRate = 0.0;
    MURALTypes::GPAVector::iterator gpaIter;
	MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); gpaIter != ptrGPAList_a->end(); ++gpaIter) {
        ptrGPA = (*gpaIter);
        maximumTransmitDataRate += (ptrGPA->GetDataRate() * (double)ptrGPA->GetCapacity());
    }

    for(rpaIter = ptrRPAList_a->begin(); rpaIter != ptrRPAList_a->end(); ++rpaIter) {
        ptrRPA = (*rpaIter);

        if (ptrRPA->IsTransmittingAntenna() == true) {
            maximumTransmitDataRate += (ptrRPA->GetDataRate() * (double)ptrRPA->GetCapacity());
        }
    }

    return(maximumTransmitDataRate);
}

double
RelaySatellite::GetMaximumReceiveDataRate() const
{
    double                              maximumReceiveDataRate = 0.0;
    MURALTypes::UPAVector::iterator upaIter;
    MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<UserPointingAntenna>   ptrUPA = NULL;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;

    for(upaIter = ptrUPAList_a->begin(); upaIter != ptrUPAList_a->end(); ++upaIter) {
        ptrUPA = (*upaIter);
        maximumReceiveDataRate += (ptrUPA->GetDataRate() * (double)ptrUPA->GetCapacity());
    }

    for(rpaIter = ptrRPAList_a->begin(); rpaIter != ptrRPAList_a->end(); ++rpaIter) {
        ptrRPA = (*rpaIter);

        if (ptrRPA->IsReceiveAntenna() == true) {
            maximumReceiveDataRate += (ptrRPA->GetDataRate() * (double)ptrRPA->GetCapacity());
        }
    }

    return(maximumReceiveDataRate);
}

bool
RelaySatellite::DownlinkingNarrowband(int timeIndex) const
{
    bool                   downlinkingNarrowband = false;
    MURALTypes::GPAVector::iterator gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); (gpaIter != ptrGPAList_a->end()) && (downlinkingNarrowband == false); ++gpaIter) {
        ptrGPA = (*gpaIter);

        if ( (ptrGPA->IsAllocatedAcquisition(timeIndex) == true) ||
                (ptrGPA->IsAllocatedDroplink(timeIndex) == true) ||
                (ptrGPA->IsAllocatedTransmitPrep(timeIndex) == true) ) {
            downlinkingNarrowband = true;
        }
    }

    return(downlinkingNarrowband);
}

bool
RelaySatellite::DownlinkingWideband(int timeIndex) const
{
    bool                   downlinkingWideband = false;
    MURALTypes::GPAVector::iterator gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); (gpaIter != ptrGPAList_a->end()) && (downlinkingWideband == false); ++gpaIter) {
        ptrGPA = (*gpaIter);

        if ( (ptrGPA->IsAllocatedMission(timeIndex) == true) ) {
            downlinkingWideband = true;
        }
    }

    return(downlinkingWideband);
}

bool
RelaySatellite::DownlinkIsAllocatedAcquisition(int timeIndex) const
{
    bool                  allocatedAcquisition = false;
    MURALTypes::GPAVector::iterator gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); (gpaIter != ptrGPAList_a->end()) && (allocatedAcquisition == false); ++gpaIter) {
        ptrGPA = (*gpaIter);
        allocatedAcquisition = ptrGPA->IsAllocatedAcquisition(timeIndex);
    }

    return(allocatedAcquisition);
}

bool
RelaySatellite::DownlinkIsAllocatedDroplink(int timeIndex) const
{
    bool                     allocatedDroplink = false;
    MURALTypes::GPAVector::iterator gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); (gpaIter != ptrGPAList_a->end()) && (allocatedDroplink == false); ++gpaIter) {
        ptrGPA = (*gpaIter);
        allocatedDroplink = ptrGPA->IsAllocatedDroplink(timeIndex);
    }

    return(allocatedDroplink);
}

bool
RelaySatellite::DownlinkIsAllocatedStateOfHealth(int timeIndex) const
{
    bool                     allocatedDroplink = false;
    MURALTypes::GPAVector::iterator gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); (gpaIter != ptrGPAList_a->end()) && (allocatedDroplink == false); ++gpaIter) {
        ptrGPA = (*gpaIter);
        allocatedDroplink = ptrGPA->IsAllocatedStateOfHealth(timeIndex);
    }

    return(allocatedDroplink);
}

bool
RelaySatellite::DownlinkIsAllocated(int timeIndex) const
{
    bool                     isAllocated = false;
    MURALTypes::GPAVector::iterator gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); (gpaIter != ptrGPAList_a->end()) && (isAllocated == false); ++gpaIter) {
        ptrGPA = (*gpaIter);
        isAllocated = ptrGPA->IsAllocated(timeIndex);
    }

    return(isAllocated);
}

bool
RelaySatellite::HasDownlink(int timeIndex) const
{
    bool                              hasDownlink = false;
    MURALTypes::GPAVector::iterator   gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); (gpaIter != ptrGPAList_a->end()) && (hasDownlink == false); ++gpaIter) {
        ptrGPA = (*gpaIter);
        hasDownlink = ptrGPA->IsThereAnyLink(timeIndex);
    }

    return(hasDownlink);
}

bool
RelaySatellite::HasAnyGroundStationInView(int timeIndex) const
{
    bool                              groundStationInView = false;
    MURALTypes::GPAVector::iterator   gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); (gpaIter != ptrGPAList_a->end()) && (groundStationInView == false); ++gpaIter) {
        ptrGPA = (*gpaIter);
        groundStationInView = ptrGPA->IsThereAnyLink(timeIndex);
    }

    return(groundStationInView);
}

bool
RelaySatellite::HasAnyResouceInView(int timeIndex,
                                    Array<string> *ptrCheckedRelayArray) const
{
    bool                  resourceInView = HasAnyGroundStationInView(timeIndex);
    const string          relayDesignator = GetDesignator();
    Link                 *ptrLink = NULL;
    MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;

    if ( (resourceInView == false) && (HasTransmitRPA() == true) &&
            (ptrCheckedRelayArray->ValueExists(relayDesignator) == false) ) {
        ptrCheckedRelayArray->AddElement(relayDesignator);

        for(rpaIter = ptrRPAList_a->begin(); (rpaIter != ptrRPAList_a->end()) && (resourceInView == false); ++rpaIter) {
            ptrRPA = (*rpaIter);

            if (ptrRPA->IsTransmittingAntenna() == true) {
                ListIterator<Link> multiHopLinksListIter(ptrRPA->GetLinksList());
                ptrLink = multiHopLinksListIter.First();

                while ((ptrLink != NULL) && (resourceInView == false)) {
                    if ( (ptrLink->GetTransmitDesignator() == relayDesignator) &&
                            (ptrCheckedRelayArray->ValueExists(ptrLink->GetReceiveDesignator()) == false) ) {
                        const shared_ptr<RelaySatellite> ptrReceiveRelay =
                            dynamic_pointer_cast<RelaySatellite>(ptrLink->GetReceiveProvider());
                        resourceInView = ptrReceiveRelay->HasAnyResouceInView(timeIndex, ptrCheckedRelayArray);
                    }

                    ptrLink = multiHopLinksListIter.Next();
                }
            }
        }
    }

    return(resourceInView);
}

bool
RelaySatellite::HasAnyUserInView(int timeIndex) const
{
    bool                             userInView = false;
    MURALTypes::UPAVector::iterator  upaIter;
    shared_ptr<UserPointingAntenna>  ptrUPA = NULL;

    for(upaIter = ptrUPAList_a->begin(); (upaIter != ptrUPAList_a->end()) && (userInView == false); ++upaIter) {
        ptrUPA = (*upaIter);
        userInView = ptrUPA->IsThereAnyLink(timeIndex);
    }

    return(userInView);
}

bool
RelaySatellite::HasUserAllocated(int timeIndex) const
{
    bool                   userAllocated = false;
    MURALTypes::UPAVector::iterator  upaIter;
    shared_ptr<UserPointingAntenna>  ptrUPA = NULL;

    for(upaIter = ptrUPAList_a->begin(); (upaIter != ptrUPAList_a->end()) && (userAllocated == false); ++upaIter) {
        ptrUPA = (*upaIter);
        userAllocated = ptrUPA->IsAllocatedNonPrepTime(timeIndex);
    }

    return(userAllocated);
}

bool
RelaySatellite::IsAllocatedStateOfHealth(int timeIndex) const
{
    bool isAllocatedSOH = false;
    MURALTypes::GPAVector::iterator gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); (gpaIter != ptrGPAList_a->end()) && (isAllocatedSOH == false); ++gpaIter) {
        ptrGPA = (*gpaIter);
        isAllocatedSOH = ptrGPA->IsAllocatedStateOfHealth(timeIndex);
    }

    return(isAllocatedSOH);
}

LinkList<Link>*
RelaySatellite::GetCrosslinks(const string &userDesignator) const
{
    Link                             *ptrCrosslink = NULL;
    LinkList<Link>                   *ptrCrosslinkList = NULL;
    MURALTypes::UPAVector::iterator upaIter;
    shared_ptr<UserPointingAntenna>   ptrUPA = NULL;

    for(upaIter = ptrUPAList_a->begin(); upaIter != ptrUPAList_a->end(); ++upaIter) {
        ptrUPA = (*upaIter);
        ptrCrosslink = ptrUPA->GetLinkFrom(userDesignator);

        if (ptrCrosslink != NULL) {
            if (ptrCrosslinkList == NULL) {
                ptrCrosslinkList = new LinkList<Link>();
            }

            ptrCrosslinkList->Append(ptrCrosslink);
            ptrCrosslink = NULL;
        }
    }

    return(ptrCrosslinkList);
}

Link*
RelaySatellite::GetAllocatedDownlink(int timeIndex) const
{
    Link                  *ptrLink = NULL;
    Link                  *ptrAllocatedDownlink = NULL;
    MURALTypes::GPAVector::iterator gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); (gpaIter != ptrGPAList_a->end()) && (ptrAllocatedDownlink == NULL); ++gpaIter) {
        ptrGPA = (*gpaIter);
        ListIterator<Link> linksListIter(ptrGPA->GetLinksList());
        ptrLink = linksListIter.First();

        while ((ptrLink != NULL) && (ptrAllocatedDownlink == NULL)) {
            if (ptrLink->IsAllocated(timeIndex) == true) {
                ptrAllocatedDownlink = ptrLink;
            } else {
                ptrLink = linksListIter.Next();
            }
        }
    }

    return(ptrAllocatedDownlink);
}

bool
RelaySatellite::ConvertDesignatorToIndex(const string &relayDesignator,
        int &index)
{
    index = -1;

    if (ptrIdentificationArray_s != NULL) {
        index = ptrIdentificationArray_s->FindIndex(relayDesignator);
    }

    return(index >= 0);
}

bool
RelaySatellite::ConvertIndexToDesignator(int index,
        string &relayDesignator)
{
    bool validDesignator = false;

    if (ptrIdentificationArray_s != NULL) {
        if ((index >= 0) && (index < ptrIdentificationArray_s->Size())) {
            relayDesignator = ptrIdentificationArray_s->ElementAt(index);
            validDesignator = true;
        }
    }

    return(validDesignator);
}

bool
RelaySatellite::IsRequiredDownlinkOverheadTime(int timeIndex) const
{
    bool                              requiredDownlinkOverheadTime = false;
    MURALTypes::GPAVector::iterator   gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;

    for(gpaIter = ptrGPAList_a->begin(); (gpaIter != ptrGPAList_a->end()) && (requiredDownlinkOverheadTime == false); ++gpaIter) {
        ptrGPA = (*gpaIter);
        requiredDownlinkOverheadTime = ptrGPA->IsRequiredDownlinkOverheadTime(timeIndex);
    }

    return(requiredDownlinkOverheadTime);
}

double
RelaySatellite::GetTotalAllocatedDataRate(int timeIndex) const
{
    double       allocatedDataRate = 0.0;
    const string relayDesignator = GetDesignator();
    Link        *ptrLink = NULL;
    MURALTypes::UPAVector::iterator upaIter;
    MURALTypes::RPAVector::iterator rpaIter;
    shared_ptr<UserPointingAntenna>   ptrUPA = NULL;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;

    // add all the crosslink allocated wideband data rates
    for(upaIter = ptrUPAList_a->begin(); upaIter != ptrUPAList_a->end(); ++upaIter) {
        ptrUPA = (*upaIter);
        ListIterator<Link> crosslinkListIter(ptrUPA->GetLinksList());
        ptrLink = crosslinkListIter.First();

        while (ptrLink != NULL) {
            if (ptrLink->IsAllocatedWideband(timeIndex) == true) {
                allocatedDataRate += ptrLink->GetTransmitDataRate();
            }

            ptrLink = crosslinkListIter.Next();
        }
    }

    // if this is a gateway, add in the allocated data rate being received
    // from other relays
    for(rpaIter = ptrRPAList_a->begin(); rpaIter != ptrRPAList_a->end(); ++rpaIter) {
        ptrRPA = (*rpaIter);

        if (ptrRPA->IsReceiveAntenna() == true) {
            ListIterator<Link> multiHopLinksListIter(ptrRPA->GetLinksList());
            ptrLink = multiHopLinksListIter.First();

            while (ptrLink != NULL) {
                if (ptrLink->GetReceiveDesignator() == relayDesignator) {
                    allocatedDataRate += (ptrLink->GetTransmitProvider())->GetTotalAllocatedDataRate(timeIndex);
                }

                ptrLink = multiHopLinksListIter.Next();
            }
        }
    }

    return(allocatedDataRate);
}

void
RelaySatellite::PrintGroundPointingAntennaUsage(ofstream &outfile,
        int timeIndex) const
{
    const int totalCapacity = GetGroundPointingAntennaCapacity();
    char        rfCharacter = BLANK_CHARACTER;
    string      groundDesignator = "";
    MURALTypes::GPAVector::iterator gpaIter;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;
    Link                               *ptrDownlink = NULL;
    Link                               *ptrAllocatedDownlink = NULL;

    try {
        int rfIndex = 0;
        int capacityIndex = 9;
        int allocatedGround = 0;
        int capacityNumber = 0;
        int groundIndex = 0;

        while (capacityIndex > totalCapacity) {
            outfile << " ";
            --capacityIndex;
        }

        for(gpaIter = ptrGPAList_a->begin(); gpaIter != ptrGPAList_a->end(); ++gpaIter) {
            int capacitySize = 0;
            ptrGPA = (*gpaIter);
            capacityIndex = 0;
            capacitySize = ptrGPA->GetCapacity();

            while (capacityIndex < capacitySize) {
                allocatedGround = ptrGPA->GetAllocatedAssetNumber(timeIndex, capacityIndex);

                if (allocatedGround == ERROR_ASSIGNMENT) {
                    rfCharacter = ERROR_CHARACTER;
                } else if (allocatedGround == STATE_OF_HEALTH_ASSIGNMENT) {
                    rfCharacter = STATE_OF_HEALTH_CHARACTER;
                } else if (ptrGPA->IsRequiredDownlinkOverheadTime(timeIndex) == true) {
                    rfCharacter = REQUIRED_DOWNLINK_OVERHEAD_CHARACTER;
                } else if (allocatedGround != 0) {
                    rfCharacter = ERROR_CHARACTER;
                    groundIndex = SimpleMath::Abs(allocatedGround) - 1;

                    if (GroundStation::ConvertIndexToDesignator(groundIndex, groundDesignator) == true) {
                        ListIterator<Link> downlinkListIter(ptrGPA->GetLinksList());
                        ptrAllocatedDownlink = NULL;
                        ptrDownlink = downlinkListIter.First();

                        while ((ptrDownlink != NULL) && (ptrAllocatedDownlink == NULL)) {
                            if ( (ptrDownlink->GetReceiveDesignator() == groundDesignator) &&
                                    (ptrDownlink->IsAllocated(timeIndex) == true) ) {
                                ptrAllocatedDownlink = ptrDownlink;
                            }

                            ptrDownlink = downlinkListIter.Next();
                        }

                        if (ptrAllocatedDownlink != NULL) {
                            if (ptrAllocatedDownlink->IsAllocatedTransmitPrep(timeIndex) == true) {
                                rfCharacter = RELAY_PREP_CHARACTER;
                            } else if (ptrAllocatedDownlink->IsAllocatedReceivePrep(timeIndex) == true) {
                                rfCharacter = UNALLOCATED_CHARACTER;
                            } else if (ptrAllocatedDownlink->IsAcquiringOrDroplinking(timeIndex) == true) {
                                rfCharacter = '#';
                            } else if (ptrAllocatedDownlink->IsAllocatedStateOfHealth(timeIndex) == true) {
                                rfCharacter = STATE_OF_HEALTH_CHARACTER;
                            } else {
                                capacityNumber = (ptrAllocatedDownlink->GetReceiveAntenna())
                                                 ->GetAllocatedCapacity(timeIndex, GetNumber());

                                if (capacityNumber > 0) {
                                    rfIndex = (capacityNumber - 1);
                                    rfCharacter = ReceiveFacilityAntenna::GetIdentification(groundIndex, rfIndex);

                                    if (HasUserAllocated(timeIndex) == false) {
                                        rfCharacter = tolower(rfCharacter);
                                    }
                                }
                            }
                        }
                    }
                } else {
                    rfCharacter = EMPTY_CAPACITY_CHARACTER;
                }

                outfile << rfCharacter;
                ++capacityIndex;
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "PrintGroundPointingAntennaUsage(ofstream&, int)");
        throw;
    }

    return;
}

void
RelaySatellite::PrintUserPointingAntennaUsage(ofstream &outfile,
        int timeIndex) const
{
    int      allocatedUser = 0;
    char       userCharacter = BLANK_CHARACTER;
    string     userDesignator = "";
    Link      *ptrAllocatedLink = NULL;
    MURALTypes::UPAVector::iterator upaIter;
    shared_ptr<UserPointingAntenna>   ptrUPA = NULL;

    for(upaIter = ptrUPAList_a->begin(); upaIter != ptrUPAList_a->end(); ++upaIter) {
        int capacityIndex = 0;
        int capacitySize = 0;
        ptrUPA = (*upaIter);
        capacityIndex = 0;
        capacitySize = ptrUPA->GetCapacity();

        while (capacityIndex < capacitySize) {
            allocatedUser = ptrUPA->GetAllocatedAssetNumber(timeIndex, capacityIndex);

            if (allocatedUser == ERROR_ASSIGNMENT) {
                userCharacter = ERROR_CHARACTER;
            } else if (allocatedUser != 0) {
                allocatedUser = SimpleMath::Abs(allocatedUser);
                userCharacter = OutputGenerator::GetUserCharacter(allocatedUser);

                if (UserVehicle::ConvertIndexToDesignator((allocatedUser - 1), userDesignator) == true) {
                    ptrAllocatedLink = ptrUPA->GetLinkFrom(userDesignator);

                    if (ptrAllocatedLink != NULL) {
                        if (ptrAllocatedLink->IsAllocatedReceivePrep(timeIndex) == true) {
                            userCharacter = RELAY_PREP_CHARACTER;
                        } else if (ptrAllocatedLink->IsAllocatedTransmitPrep(timeIndex) == true) {
                            userCharacter = EMPTY_CAPACITY_CHARACTER;
                        }
                    }
                }
            } else {
                userCharacter = EMPTY_CAPACITY_CHARACTER;
            }

            outfile << userCharacter;
            ++capacityIndex;
        }
    }

    return;
}

void
RelaySatellite::PrintUserPointingAntennaUsage(ofstream &outfile,
        int timeIndex,
        const Antenna::FREQUENCY_BAND &frequencyBand) const
{
    int      allocatedUser = 0;
    char       userCharacter = BLANK_CHARACTER;
    const char mismatchedTechnologyID = '-';
    string     userDesignator = "";
    Link      *ptrAllocatedLink = NULL;
    MURALTypes::UPAVector::iterator upaIter;
    shared_ptr<UserPointingAntenna>   ptrUPA = NULL;

    for(upaIter = ptrUPAList_a->begin(); upaIter != ptrUPAList_a->end(); ++upaIter) {
        int capacityIndex = 0;
        int capacitySize = 0;
        ptrUPA = (*upaIter);
        capacityIndex = 0;
        capacitySize = ptrUPA->GetCapacity();

        while (capacityIndex < capacitySize) {
            if (ptrUPA->GetFrequencyBand() == frequencyBand) {
                allocatedUser = ptrUPA->GetAllocatedAssetNumber(timeIndex, capacityIndex);

                if (allocatedUser == ERROR_ASSIGNMENT) {
                    userCharacter = ERROR_CHARACTER;
                } else if (allocatedUser != 0) {
                    allocatedUser = SimpleMath::Abs(allocatedUser);
                    userCharacter = OutputGenerator::GetUserCharacter(allocatedUser);

                    if (UserVehicle::ConvertIndexToDesignator((allocatedUser - 1), userDesignator) == true) {
                        ptrAllocatedLink = ptrUPA->GetLinkFrom(userDesignator);

                        if (ptrAllocatedLink != NULL) {
                            if (ptrAllocatedLink->IsAllocatedReceivePrep(timeIndex) == true) {
                                userCharacter = RELAY_PREP_CHARACTER;
                            } else if (ptrAllocatedLink->IsAllocatedTransmitPrep(timeIndex) == true) {
                                userCharacter = UNALLOCATED_CHARACTER;
                            }
                        }
                    }
                } else {
                    userCharacter = UNALLOCATED_CHARACTER;
                }
            } else {
                userCharacter = mismatchedTechnologyID;
            }

            outfile << userCharacter;
            ++capacityIndex;
        }
    }

    return;
}

/**
 * Method to print the Excess Time to the mrl file.  Excess time is any time
 * where the upa has no allocation to a user and, if the parameter is set to true,
 * the relay is not aquiring or droplinking or in transmit prep.
 * Narrowband contact is counted as excess time.
 *
 * @param outfile The pointer to the out file stream
 * @param countNarrowbandDownlinkTime weather or not to count the downlink activities
 */
void
RelaySatellite::PrintExcessTime(ofstream &outfile,
                                const bool &countNarrowbandDownlinkTime) const
{
    int         totalExcessTime = 0;
    int         timeIndex = 0;
    const int   startReportTime = TimePiece::GetReportStartIndexTime();
    const int   endReportTime = TimePiece::GetReportEndIndexTime();
    const int   numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    Array<bool> *ptrExcessTime = NULL;
    Array<bool> *ptrOverallExcessTime = NULL;
    const string relayDesignator = GetDesignator();
    TimePeriod                       *ptrExcessPeriod = NULL;
    LinkList<TimePeriod>             *ptrExcessPeriodList = NULL;
    MURALTypes::UPAVector::iterator upaIter;
    shared_ptr<UserPointingAntenna>   ptrUPA = NULL;
    outfile << " MURALEXCESS" << NEW_LINE << NEW_LINE;
    outfile << " RELAY " << relayDesignator << NEW_LINE << NEW_LINE;
    // Init the overall array to true for the reporting period
    ptrOverallExcessTime = new Array<bool>(numberOfTimeSteps, false);
    timeIndex = startReportTime;

    while (timeIndex <= endReportTime) {
        (*ptrOverallExcessTime)[timeIndex] = true;
        timeIndex++;
    }

    // Process the individual excess time periods
    if (GetUserPointingAntennaCapacity() > 1) {
        int capacityNumber = 0;

        for(upaIter = ptrUPAList_a->begin(); upaIter != ptrUPAList_a->end(); ++upaIter) {
            int capacityIndex = 0;
            ptrUPA = (*upaIter);
            capacityIndex = 0;

            while (capacityIndex < ptrUPA->GetCapacity()) {
                ptrExcessTime = new Array<bool>(numberOfTimeSteps, false);
                timeIndex = startReportTime;

                while (timeIndex <= endReportTime) {
                    if (ptrUPA->GetAllocatedAssetNumber(timeIndex, capacityIndex) == 0) {
                        if ( HasAnyGroundStationInView(timeIndex) == true ) {
                            if ( (countNarrowbandDownlinkTime == false) ||
                                    (DownlinkingNarrowband(timeIndex) == false) ) {
                                (*ptrExcessTime)[timeIndex] = true;
                            }
                        }
                    }

                    // Compute overall excess array using the and operator becuase overall a relay is in
                    // excess time if all the capacities on the relay are empty and the relay is not in
                    // aquisition or drop or transmit prep.
                    (*ptrOverallExcessTime)[timeIndex] = (*ptrOverallExcessTime)[timeIndex] && (*ptrExcessTime)[timeIndex];
                    ++timeIndex;
                }

                // Output the excess time
                outfile << "  UPA " << ++capacityNumber << NEW_LINE;
                outfile << "    START     END     DUR" << NEW_LINE;
                totalExcessTime = 0;
                ptrExcessPeriodList = TimePeriod::GroupIntoPeriods(ptrExcessTime);

                if (ptrExcessPeriodList != NULL) {
                    //JOE:
                    ptrExcessPeriod = ptrExcessPeriodList->Retrieve();

                    while (ptrExcessPeriod != NULL) {
                        outfile << "  ";
                        TimePiece::PrintIndex(outfile, ptrExcessPeriod->GetStartTime());
                        outfile << " ";
                        TimePiece::PrintIndex(outfile, ptrExcessPeriod->GetEndTime());
                        outfile << " ";
                        TimePiece::PrintDuration(outfile, ptrExcessPeriod->GetDuration());
                        outfile << NEW_LINE;
                        totalExcessTime += ptrExcessPeriod->GetDuration();
                        delete ptrExcessPeriod;
                        ptrExcessPeriod = NULL;
                        ptrExcessPeriod = ptrExcessPeriodList->Retrieve();
                    }

                    delete ptrExcessPeriodList;
                    ptrExcessPeriodList = NULL;
                }

                outfile << NEW_LINE << "  UPA " << setw(2) << capacityNumber << "  TOTAL:  ";
                TimePiece::PrintDuration(outfile, totalExcessTime);
                outfile << NEW_LINE << NEW_LINE;
                delete ptrExcessTime;
                ptrExcessTime = NULL;
                ++capacityIndex;
            }
        }
    }

    // Print the overall excess time array
    outfile << "   RELAY" << NEW_LINE;
    outfile << "    START     END     DUR" << NEW_LINE;
    totalExcessTime = 0;
    ptrExcessPeriodList = TimePeriod::GroupIntoPeriods(ptrOverallExcessTime);

    if (ptrExcessPeriodList != NULL) {
        //JOE:
        ptrExcessPeriod = ptrExcessPeriodList->Retrieve();

        while (ptrExcessPeriod != NULL) {
            outfile << "  ";
            TimePiece::PrintIndex(outfile, ptrExcessPeriod->GetStartTime());
            outfile << " ";
            TimePiece::PrintIndex(outfile, ptrExcessPeriod->GetEndTime());
            outfile << " ";
            TimePiece::PrintDuration(outfile, ptrExcessPeriod->GetDuration());
            outfile << NEW_LINE;
            totalExcessTime += ptrExcessPeriod->GetDuration();
            delete ptrExcessPeriod;
            ptrExcessPeriod = NULL;
            ptrExcessPeriod = ptrExcessPeriodList->Retrieve();
        }

        delete ptrExcessPeriodList;
        ptrExcessPeriodList = NULL;
    }

    outfile << NEW_LINE << setw(8) << relayDesignator;
    outfile << "  TOTAL:  ";
    TimePiece::PrintDuration(outfile, totalExcessTime);
    outfile << NEW_LINE << NEW_LINE << NEW_LINE;
    delete ptrOverallExcessTime;
    ptrOverallExcessTime = NULL;
    return;
}

Array<bool>*
RelaySatellite::GetRequiredDownlinkOverheadTimeArray() const
{
    int         timeIndex = 0;
    const int   startIndexTime = TimePiece::GetStartIndexTime();
    const int   endIndexTime = TimePiece::GetEndIndexTime();
    const int   numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    const int   minimumAcqTime = GetMinimumDownlinkAcquisitionTime();
    const int   minimumDropTime = GetMinimumDownlinkDroplinkTime();
    Array<bool> *ptrRequiredOverheadArray = NULL;
    Array<bool> *ptrDownlinkInviewArray = new Array<bool>(numberOfTimeSteps);
    TimePeriod  *ptrDownlinkPeriod = NULL;
    LinkList<TimePeriod> *ptrDownlinkPeriodList = NULL;

    while (timeIndex < numberOfTimeSteps) {
        (*ptrDownlinkInviewArray)[timeIndex] = HasDownlink(timeIndex);
        ++timeIndex;
    }

    ptrDownlinkPeriodList = TimePeriod::GroupIntoPeriods(ptrDownlinkInviewArray);

    if (ptrDownlinkPeriodList != NULL) {
        int endPeriod = 0;
        //JOE:
        ptrDownlinkPeriod = ptrDownlinkPeriodList->Retrieve();

        while (ptrDownlinkPeriod != NULL) {
            int startPeriod = 0;
            startPeriod = ptrDownlinkPeriod->GetStartTime();

            if (startPeriod > startIndexTime) {
                if (ptrRequiredOverheadArray == NULL) {
                    ptrRequiredOverheadArray = new Array<bool>(numberOfTimeSteps, false);
                }

                endPeriod = std::min((startPeriod + minimumAcqTime - 1), endIndexTime);
                timeIndex = startPeriod;

                while (timeIndex <= endPeriod) {
                    (*ptrRequiredOverheadArray)[timeIndex] = true;
                    ++timeIndex;
                }
            }

            endPeriod = ptrDownlinkPeriod->GetEndTime();

            if (endPeriod < endIndexTime) {
                if (ptrRequiredOverheadArray == NULL) {
                    ptrRequiredOverheadArray = new Array<bool>(numberOfTimeSteps, false);
                }

                startPeriod = std::max((endPeriod - minimumDropTime + 1), startIndexTime);
                timeIndex = startPeriod;

                while (timeIndex <= endPeriod) {
                    (*ptrRequiredOverheadArray)[timeIndex] = true;
                    ++timeIndex;
                }
            }

            delete ptrDownlinkPeriod;
            ptrDownlinkPeriod = NULL;
            ptrDownlinkPeriod = ptrDownlinkPeriodList->Retrieve();
        }

        delete ptrDownlinkPeriodList;
        ptrDownlinkPeriodList = NULL;
    }

    delete ptrDownlinkInviewArray;
    ptrDownlinkInviewArray = NULL;
    return(ptrRequiredOverheadArray);
}

string
RelaySatellite::GetDesignatorString(int index)
{
    string designator = "";

    try {
        designator = ptrIdentificationArray_s->ElementAt(index);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetDesignator(int)");
        throw;
    }

    return(designator);
}

bool
RelaySatellite::IsHighlyElliptical() const
{
    return(Utility::IsHighlyElliptical(GetEccentricity()));
}

/* ************************************************************************************** */

shared_ptr<MURALTypes::AntennaVector>
RelaySatellite::GetConjunctionAntennaList()
{
    shared_ptr<MURALTypes::AntennaVector> ptrAntennaVector = NULL;
    MURALTypes::UPAVector::iterator upaIter;
    shared_ptr<UserPointingAntenna> ptrUPA = NULL;
    shared_ptr<Antenna>             ptrAntenna = NULL;
    ptrAntennaVector = shared_ptr<MURALTypes::AntennaVector>( new MURALTypes::AntennaVector() );

    for( upaIter = ptrUPAList_a->begin(); upaIter != ptrUPAList_a->end(); ++upaIter) {
        ptrUPA = (*upaIter);
        ptrAntenna = dynamic_pointer_cast<Antenna>(ptrUPA);

        if ( ptrAntenna != NULL ) {
            ptrAntennaVector->push_back(ptrAntenna);
        }
    }

    return(ptrAntennaVector);
}

/* ************************************************************************************** */

void
RelaySatellite::SetBodyTiltAngle(double bodyTiltAngle)
{
    bodyTiltAngle_a = bodyTiltAngle;
}

/* ************************************************************************************** */

void
RelaySatellite::SetUPAList(const shared_ptr<MURALTypes::UPAVector> &ptrUPAList)
{
    ptrUPAList_a = ptrUPAList;
}

/* ************************************************************************************** */

void
RelaySatellite::SetRPAList(const shared_ptr<MURALTypes::RPAVector> &ptrRPAList)
{
    ptrRPAList_a = ptrRPAList;
}

/* ************************************************************************************** */

void
RelaySatellite::SetGPAList(const shared_ptr<MURALTypes::GPAVector> &ptrGPAList)
{
    ptrGPAList_a = ptrGPAList;
}

/* ************************************************************************************** */

RelaySatellite::~RelaySatellite()
{
    if (ptrIdentificationArray_s != NULL) {
        ptrIdentificationArray_s->RemoveElement(GetDesignator());

        if (ptrIdentificationArray_s->Size() == 0) {
            delete ptrIdentificationArray_s;
            ptrIdentificationArray_s = NULL;
        }
    }

    if (ptrStateOfHealthPeriodList_a != NULL) {
        ptrStateOfHealthPeriodList_a->Delete();
        delete ptrStateOfHealthPeriodList_a;
        ptrStateOfHealthPeriodList_a = NULL;
    }

    return;
}
