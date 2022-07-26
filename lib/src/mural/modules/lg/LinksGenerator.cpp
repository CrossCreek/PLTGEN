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
 * LinksGenerator.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>
#include <sstream>

#include "mural/Region.h"
#include "mural/Earth.h"
#include "LinksGenerator.h"
#include "AzimuthAngleData.h"
#include "ConjunctionPeriod.h"
#include "mural/io/MURALInputProcessor.h"
#include "mural/io/FileNames.h"
#include "mural/antenna/GroundPointingAntenna.h"
#include "mural/antenna/ReceiveFacilityAntenna.h"
#include "mural/antenna/RelayPointingAntenna.h"
#include "mural/antenna/UserTransmitAntenna.h"
#include "mural/antenna/UserPointingAntenna.h"
#include "mural/element/UserVehicle.h"
#include "mural/element/RelaySatellite.h"
#include "mural/element/GroundStation.h"
#include "mural/element/DataProviderCollection.h"
#include "mural/modules/pg/PositionGenerator.h"
#include "mural/modules/pg/PositionUtility.h"

#include "general/array/Array3D.h"
#include "general/array/ListIterator.h"
#include "general/data/UnitData.h"
#include "general/parse/ParseUtility.h"
#include "general/math/SimpleMath.h"
#include "general/math/SpaceVector.h"
#include "general/utility/StringEditor.h"

double  LinksGenerator::additionalCheckSeconds_s         = INVALID_SECONDS;

LinksGenerator::LinksGenerator()
    : ptrDataProviderCollection_a(NULL)
{
    return;
}

LinksGenerator::LinksGenerator(const LinksGenerator &copyMe)
    : ptrDataProviderCollection_a(copyMe.ptrDataProviderCollection_a)
{
    return;
}

LinksGenerator&
LinksGenerator::operator = (const LinksGenerator &copyMe)
{
    ptrDataProviderCollection_a    = copyMe.ptrDataProviderCollection_a;
    return(*this);
}

bool
LinksGenerator::operator == (const LinksGenerator &compareMe) const
{
    return( ptrDataProviderCollection_a == compareMe.ptrDataProviderCollection_a);
}

void
LinksGenerator::ExtractLinksGeneratorInfo(const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData> ptrInputData = NULL;

    try {
        if ( inputDataList != NULL ) {
            for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
                ptrInputData = (*inputDataIter);

                if (ptrInputData->LabelContainsString("Additional Check Seconds") == true) {
                    additionalCheckSeconds_s = ptrInputData->GetDoubleValue();
                }
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ExtractLinksGeneratorInfo(InputDataElement*)");
        throw;
    }

    if (additionalCheckSeconds_s <= 0.0) {
        throw new InputException(GetClassName(),
                                 "ExtractLinksGeneratorInfo(InputDataElement*)",
                                 "Invalid 'Additional Check Seconds' input");
    }

    return;
}

void
LinksGenerator::PrintAttributes(ofstream &outfile)
{
    outfile << "<LINK_GENERATION_START>" << NEW_LINE;
    outfile << "   Additional Check Seconds  : " << additionalCheckSeconds_s << NEW_LINE;
    outfile << "<LINK_GENERATION_END>" << NEW_LINE;
    return;
}

void
LinksGenerator::GenerateLinks(const shared_ptr<DataProviderCollection> &ptrDataProviderCollection)
{
    ofstream detailedLinksFile;
    ofstream detailedConjFile;
    // set the data proivder collection attribute
    ptrDataProviderCollection_a = ptrDataProviderCollection;

    try {
        if (FileNames::HasFileNameDefined(FileNames::LINKS_DEBUGFILE) == true) {
            ParseUtility::OpenOutputFile(detailedLinksFile, FileNames::GetFileName(FileNames::LINKS_DEBUGFILE));
            detailedLinksFile << "                             AZIM ANGLE      ELEV ANGLE ";
            detailedLinksFile << "      GRAZ                 SUN ANGLE" << NEW_LINE;
            detailedLinksFile << "   TIME    SEND   RECV       SEND    RECV    SEND    RECV";
            detailedLinksFile << "       ALT      RANGE    SEND    RECV    TIME    STATUS" << NEW_LINE;
        }

        if (FileNames::HasFileNameDefined(FileNames::CONJ_DEBUGFILE) == true) {
            ParseUtility::OpenOutputFile(detailedConjFile, FileNames::GetFileName(FileNames::CONJ_DEBUGFILE));
            detailedConjFile << "   TIME  RECEIVR  SENDER1  SENDER2  TYPE   ANGLE    CONJ   TIME" << NEW_LINE;
        }

        // Need to get downlinks before determining crosslinks
        DetermineRelayLinks(detailedLinksFile);
        DetermineUserLinks(detailedLinksFile);

        if (ptrDataProviderCollection_a->GetRelayCount() > 0 ) {
            DetermineConjunctionUPA(detailedConjFile);
            DetermineConjunctionRF(detailedConjFile);
        }

        if (FileNames::HasFileNameDefined(FileNames::LINKS_DEBUGFILE) == true) {
            detailedLinksFile.close();
        }

        if (FileNames::HasFileNameDefined(FileNames::CONJ_DEBUGFILE) == true) {
            detailedConjFile.close();
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),"GenerateLinks(LinkList<DataProvider>*)");
        throw;
    }

    return;
}

void
LinksGenerator::DetermineUserLinks(ofstream &detailedLinksFile)
{
    shared_ptr<UserVehicle>              ptrTrailVehicle = NULL;
    shared_ptr<UserVehicle>              ptrUserVehicle = NULL;
    shared_ptr<MURALTypes::UserVector>   ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();

    try {
        MURALTypes::UserVector::iterator userIter;

        for ( userIter = ptrUserList->begin(); userIter != ptrUserList->end(); ++userIter) {
            ptrUserVehicle = (*userIter);
            ptrTrailVehicle = ptrUserVehicle->GetTrailVehicle();
            DetermineUserVehicleLinks(ptrUserVehicle, ptrTrailVehicle, detailedLinksFile);
            ptrUserVehicle->CheckTransmitLinkStatus();

            if (ptrTrailVehicle != NULL) {
                DetermineUserVehicleLinks(ptrTrailVehicle, ptrUserVehicle, detailedLinksFile);
                ptrTrailVehicle->CheckTransmitLinkStatus();
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "DetermineUserLinks(ofstream&)");
        throw;
    }

    return;
}

void
LinksGenerator::DetermineUserVehicleLinks(const shared_ptr<UserVehicle> &ptrUserVehicle,
        const shared_ptr<UserVehicle> &ptrClusterUser,
        ofstream &detailedLinksFile)
{
    const string                      designator = ptrUserVehicle->GetDesignator();
    shared_ptr<SelfRelayAntenna>      ptrTransmitSRA = NULL;
    shared_ptr<MURALTypes::SRAVector> ptrSRAList = NULL;
    MURALTypes::SRAVector::iterator   sraIter;

    try {
        if (ptrUserVehicle->ReadLinkPaths() == true) {
            if (ptrUserVehicle->IsACrosslinkVehicle() == true) {
                cout << "LinksGenerator: Retrieving Crosslinks For " << designator << NEW_LINE;
                RetrieveCrosslinks(ptrUserVehicle);
            }

            if (ptrUserVehicle->IsADirectDownlinkVehicle() == true) {
                cout << "LinksGenerator: Retrieving Direct Downlinks For " << designator << NEW_LINE;
                RetrieveDirectDownlinks(ptrUserVehicle);
            }

            if (ptrUserVehicle->IsATransmitSelfRelayingVehicle() == true) {
                cout << "LinksGenerator: Retrieving Self Relaying Links For " << designator << NEW_LINE;
                ptrSRAList = ptrUserVehicle->GetSelfRelayAntennaList();

                for( sraIter = ptrSRAList->begin(); sraIter != ptrSRAList->end(); ++sraIter) {
                    ptrTransmitSRA = (*sraIter);

                    if (ptrTransmitSRA->IsTransmittingAntenna() == true) {
                        RetrieveSelfRelayingLinks(ptrUserVehicle, ptrTransmitSRA);
                    }
                }
            }
        } else {
            if (ptrUserVehicle->IsACrosslinkVehicle() == true) {
                cout << "LinksGenerator: Determining Crosslinks For " << designator << NEW_LINE;
                DetermineCrosslinks(ptrUserVehicle, ptrClusterUser, detailedLinksFile);
            }

            if (ptrUserVehicle->IsADirectDownlinkVehicle() == true) {
                cout << "LinksGenerator: Determining Direct Downlinks For " << designator << NEW_LINE;
                DetermineDirectDownlinks(ptrUserVehicle, detailedLinksFile);
            }

            if (ptrUserVehicle->IsATransmitSelfRelayingVehicle() == true) {
                cout << "LinksGenerator: Determining Self Relaying Links For " << designator << NEW_LINE;
                DetermineSelfRelayingLinks(ptrUserVehicle, detailedLinksFile);
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "DetermineUserVehicleLinks(UserVehicle*, ofstream&)");
        throw;
    }

    return;
}

void
LinksGenerator::DetermineCrosslinks(const shared_ptr<UserVehicle> &ptrUserVehicle,
                                    const shared_ptr<UserVehicle> &ptrClusterUser,
                                    ofstream &detailedLinksFile)
{
    Array<int>                          *ptrStatus = NULL;
    const bool                           checkDownlinkSupport = true;
    string                               relayDesignator = "";
    const Antenna::FREQUENCY_BAND        frequencyBand = ptrUserVehicle->GetTransmitFrequencyBand();
    Link                                *ptrLink = NULL;
    shared_ptr<UserTransmitAntenna>      ptrUserAntenna = ptrUserVehicle->GetUserTransmitAntenna();
    shared_ptr<UserPointingAntenna>      ptrUPA = NULL;
    shared_ptr<RelaySatellite>           ptrRelaySatellite = NULL;
    shared_ptr<MURALTypes::UPAVector>    ptrUPAList = NULL;
    shared_ptr<MURALTypes::RelayVector>  ptrRelayList = ptrDataProviderCollection_a->GetRelayListPtr();
    MURALTypes::UPAVector::iterator      upaIter;
    MURALTypes::RelayVector::iterator    relayIter;

    for( relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
        ptrRelaySatellite = (*relayIter);
        ptrUPAList = ptrRelaySatellite->GetUserPointingAntennaList();
        relayDesignator = ptrRelaySatellite->GetDesignator();

        for(upaIter = ptrUPAList->begin(); upaIter != ptrUPAList->end(); ++upaIter) {
            ptrUPA = (*upaIter);

            try {
                cout << "                Crosslink With " << relayDesignator
                     << ", UPA " << ptrUPA->GetAntennaDesignator();

                if (ptrUPA->GetFrequencyBand() == frequencyBand) {
                    ptrStatus = DetermineLinkStatusArray(ptrUserVehicle, ptrUserAntenna,
                                                         ptrRelaySatellite, ptrUPA,
                                                         detailedLinksFile, checkDownlinkSupport,
                                                         ptrClusterUser);

                    if (ptrStatus->ValueExists(Link::LINK) == true) {
                        ptrLink = new Link(ptrUserAntenna, ptrUPA, ptrStatus,
                                           ptrUserVehicle->GetLinkOverheadData(relayDesignator));
                        ptrUserAntenna->AppendLink(ptrLink);
                        ptrUPA->AppendLink(ptrLink);
                        ptrLink = NULL;
                        cout << " - Created" << NEW_LINE;
                    } else {
                        cout << " - No Inview" << NEW_LINE;
                    }

                    delete ptrStatus;
                    ptrStatus = NULL;
                } else {
                    cout << " - Mis-matching UPA Frequency Bands" << NEW_LINE;
                }
            } catch (Exception *ptrError) {
                cout << NEW_LINE;
                ptrError->AddMethod(GetClassName(), "DetermineCrosslinks(UserVehicle*, ofstream&)");
                throw;
            }
        }
    }

    return;
}

void
LinksGenerator::DetermineDirectDownlinks(const shared_ptr<UserVehicle> &ptrUserVehicle,
        ofstream &detailedLinksFile)
{
    Array<int>                                   *ptrStatus = NULL;
    string                                        groundDesignator = "";
    const Antenna::FREQUENCY_BAND                 frequencyBand = ptrUserVehicle->GetDirectDownlinkFrequencyBand();
    Link                                         *ptrLink = NULL;
    shared_ptr<DirectDownlinkAntenna>             ptrDirectDownlinkAntenna = ptrUserVehicle->GetDirectDownlinkAntenna();
    shared_ptr<ReceiveFacilityAntenna>            ptrRF = NULL;
    shared_ptr<GroundStation>                     ptrGround = NULL;
    shared_ptr<MURALTypes::RFAVector>             ptrRFList = NULL;
    shared_ptr<MURALTypes::GroundVector>          ptrGroundList = ptrDataProviderCollection_a->GetGroundListPtr();
    MURALTypes::RFAVector::iterator rfaIter;
    MURALTypes::GroundVector::iterator groundIter;

    for( groundIter = ptrGroundList->begin(); groundIter != ptrGroundList->end(); ++groundIter) {
        ptrGround = (*groundIter);
        ptrRFList = ptrGround->GetRFList();
        groundDesignator = ptrGround->GetDesignator();

        for(rfaIter = ptrRFList->begin(); rfaIter != ptrRFList->end(); ++rfaIter) {
            ptrRF = (*rfaIter);

            try {
                cout << "                Direct Downlink With " << groundDesignator
                     << ", RFA " << ptrRF->GetAntennaDesignator();

                if (ptrRF->GetFrequencyBand() == frequencyBand) {
                    ptrStatus = DetermineLinkStatusArray(ptrUserVehicle, ptrDirectDownlinkAntenna,
                                                         ptrGround, ptrRF,
                                                         detailedLinksFile);

                    if (ptrStatus->ValueExists(Link::LINK) == true) {
                        ptrLink = new Link(ptrDirectDownlinkAntenna, ptrRF, ptrStatus,
                                           ptrUserVehicle->GetLinkOverheadData(groundDesignator));
                        ptrDirectDownlinkAntenna->AppendLink(ptrLink);
                        ptrRF->AppendLink(ptrLink);
                        ptrLink = NULL;
                        cout << " - Created" << NEW_LINE;
                    } else {
                        cout << " - No Inview" << NEW_LINE;
                    }

                    delete ptrStatus;
                    ptrStatus = NULL;
                } else {
                    cout << " - Mis-matching RFA Frequency Bands" << NEW_LINE;
                }
            } catch (Exception *ptrError) {
                cout << NEW_LINE;
                ptrError->AddMethod(GetClassName(),
                                    "DetermineDirectDownlinks(UserVehicle*, ofstream&)");
                throw;
            }
        }
    }
}

void
LinksGenerator::DetermineSelfRelayingLinks(const shared_ptr<UserVehicle> &ptrFromUserVehicle,
        ofstream &detailedLinksFile)
{
    Array<int>                         *ptrStatus = NULL;
    string                              toUserDesignator = "";
    const string                        fromUserDesignator = ptrFromUserVehicle->GetDesignator();
    Link                               *ptrLink = NULL;
    Antenna::FREQUENCY_BAND             frequencyBand = Antenna::UNKNOWN_BAND;
    shared_ptr<UserVehicle>             ptrToUserVehicle = NULL;
    shared_ptr<SelfRelayAntenna>        ptrToSRA = NULL;
    shared_ptr<SelfRelayAntenna>        ptrFromSRA = NULL;
    shared_ptr<MURALTypes::SRAVector>   ptrFromSRAList = NULL;
    shared_ptr<MURALTypes::SRAVector>   ptrToSRAList = NULL;
    shared_ptr<MURALTypes::UserVector>  ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();
    MURALTypes::SRAVector::iterator     fromSRAIter;
    MURALTypes::SRAVector::iterator     toSRAIter;
    ptrFromSRAList = ptrFromUserVehicle->GetSelfRelayAntennaList();

    for( fromSRAIter = ptrFromSRAList->begin(); fromSRAIter != ptrFromSRAList->end(); ++fromSRAIter) {
        ptrFromSRA = (*fromSRAIter);

        if (ptrFromSRA->IsTransmittingAntenna() == true) {
            frequencyBand = ptrFromSRA->GetFrequencyBand();
            MURALTypes::UserVector::iterator userIter;

            for( userIter = ptrUserList->begin(); userIter != ptrUserList->end(); ++userIter) {
                ptrToUserVehicle = (*userIter);
                toUserDesignator = ptrToUserVehicle->GetDesignator();

                if (toUserDesignator != fromUserDesignator) {
                    ptrToSRAList = ptrToUserVehicle->GetSelfRelayAntennaList();

                    for( toSRAIter = ptrToSRAList->begin(); toSRAIter != ptrToSRAList->end(); ++toSRAIter) {
                        ptrToSRA = (*toSRAIter);

                        if (ptrToSRA->IsReceiveAntenna() == true) {
                            try {
                                cout << "                Self Relaying Link With " << toUserDesignator
                                     << ", SRA " << ptrToSRA->GetAntennaDesignator();

                                if (ptrToSRA->GetFrequencyBand() == frequencyBand) {
                                    ptrStatus = DetermineLinkStatusArray(ptrFromUserVehicle, ptrFromSRA,
                                                                         ptrToUserVehicle, ptrToSRA,
                                                                         detailedLinksFile);

                                    if (ptrStatus->ValueExists(Link::LINK) == true) {
                                        ptrLink = new Link(ptrFromSRA, ptrToSRA, ptrStatus,
                                                           ptrFromUserVehicle->GetLinkOverheadData(toUserDesignator));
                                        ptrFromSRA->AppendLink(ptrLink);
                                        ptrToSRA->AppendLink(ptrLink);
                                        ptrLink = NULL;
                                        cout << " - Created" << NEW_LINE;
                                    } else {
                                        cout << " - No Inview" << NEW_LINE;
                                    }

                                    delete ptrStatus;
                                    ptrStatus = NULL;
                                } else {
                                    cout << " - Mis-matching SRA Frequency Bands" << NEW_LINE;
                                }
                            } catch (Exception *ptrError) {
                                cout << NEW_LINE;
                                ptrError->AddMethod(GetClassName(),
                                                    "DetermineSelfRelayingLinks(UserVehicle*, ofstream&)");
                                throw;
                            }
                        }
                    }
                }
            }
        }
    }
}

void
LinksGenerator::DetermineRelayLinks(ofstream &detailedLinksFile)
{
    shared_ptr<GroundPointingAntenna>       ptrGPA = NULL;
    shared_ptr<RelaySatellite>              ptrRelaySatellite = NULL;
    shared_ptr<MURALTypes::GPAVector>       ptrGPAList = NULL;
    shared_ptr<MURALTypes::RelayVector>     ptrRelayList = ptrDataProviderCollection_a->GetRelayListPtr();
    MURALTypes::GPAVector::iterator         gpaIter;

    try {
        // Determine downlinks for gateway relays first
        MURALTypes::RelayVector::iterator relayIter;

        if (ptrRelayList != NULL) {
            for (relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
                ptrRelaySatellite = (*relayIter);

                if (ptrRelaySatellite->GetGroundPointingAntennaList() != NULL) {
                    ptrGPAList = ptrRelaySatellite->GetGroundPointingAntennaList();

                    for (gpaIter = ptrGPAList->begin(); gpaIter != ptrGPAList->end(); ++gpaIter) {
                        ptrGPA = (*gpaIter);

                        if (ptrRelaySatellite->ReadLinkPaths() == true) {
                            cout << "LinksGenerator: Retrieving Downlinks For " << ptrRelaySatellite->GetDesignator()
                                 << ", GPA " << ptrGPA->GetAntennaDesignator() << NEW_LINE;
                            RetrieveDownlinks(ptrRelaySatellite, ptrGPA);
                        } else {
                            cout << "LinksGenerator: Determining Downlinks For " << ptrRelaySatellite->GetDesignator()
                                 << ", GPA " << ptrGPA->GetAntennaDesignator() << NEW_LINE;
                            DetermineDownlinks(ptrRelaySatellite, ptrGPA, detailedLinksFile);
                        }
                    }
                }
            }

            // Now determine multihop links for any multihop relay(s)
            for (relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
                ptrRelaySatellite = (*relayIter);

                if (ptrRelaySatellite->HasTransmitRPA() == true) {
                    if (ptrRelaySatellite->ReadLinkPaths() == true) {
                        cout << "LinksGenerator: Retrieving Multihop Links For "
                             << ptrRelaySatellite->GetDesignator() << NEW_LINE;
                        RetrieveMultihopLinks(ptrRelaySatellite);
                    } else {
                        cout << "LinksGenerator: Determining Multihop Links For "
                             << ptrRelaySatellite->GetDesignator() << NEW_LINE;
                        DetermineMultihopLinks(ptrRelaySatellite, detailedLinksFile);
                    }
                }
            }

            // Now check that each relay's GPA(s) and/or RPA has at least one link
            for (relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
                ptrRelaySatellite = (*relayIter);
                ptrRelaySatellite->CheckTransmitLinkStatus();
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "DetermineRelayLinks(ofstream&)");
        throw;
    }

    return;
}

void
LinksGenerator::DetermineDownlinks(const shared_ptr<RelaySatellite> &ptrRelaySatellite,
                                   const shared_ptr<GroundPointingAntenna> &ptrGPA,
                                   ofstream &detailedLinksFile)
{
    Array<int>                           *ptrStatus = NULL;
    string                                groundDesignator = "";
    const Antenna::FREQUENCY_BAND         frequencyBand = ptrGPA->GetFrequencyBand();
    Link                                 *ptrLink = NULL;
    shared_ptr<ReceiveFacilityAntenna>    ptrRF = NULL;
    shared_ptr<GroundStation>             ptrGround = NULL;
    shared_ptr<MURALTypes::RFAVector>     ptrRFList = NULL;
    shared_ptr<MURALTypes::GroundVector>  ptrGroundList = ptrDataProviderCollection_a->GetGroundListPtr();
    MURALTypes::RFAVector::iterator       rfaIter;
    MURALTypes::GroundVector::iterator groundIter;

    for( groundIter = ptrGroundList->begin(); groundIter != ptrGroundList->end(); ++groundIter) {
        ptrGround = (*groundIter);
        groundDesignator = ptrGround->GetDesignator();
        ptrRFList = ptrGround->GetRFList();

        for(rfaIter = ptrRFList->begin(); rfaIter != ptrRFList->end(); ++rfaIter) {
            ptrRF = (*rfaIter);

            try {
                cout << "                Downlink With " << groundDesignator
                     << ", RFA " << ptrRF->GetAntennaDesignator();

                if (ptrRF->GetFrequencyBand() == frequencyBand) {
                    ptrStatus = DetermineLinkStatusArray(ptrRelaySatellite, ptrGPA,
                                                         ptrGround, ptrRF,
                                                         detailedLinksFile);

                    if (ptrStatus->ValueExists(Link::LINK) == true) {
                        ptrLink = new Link(ptrGPA, ptrRF, ptrStatus,
                                           ptrRelaySatellite->GetLinkOverheadData(groundDesignator));
                        ptrGPA->AppendLink(ptrLink);
                        ptrRF->AppendLink(ptrLink);
                        ptrLink = NULL;
                        cout << " - Created" << NEW_LINE;
                    } else {
                        cout << " - No Inview" << NEW_LINE;
                    }

                    delete ptrStatus;
                    ptrStatus = NULL;
                } else {
                    cout << " - Mis-matching RFA Technology" << NEW_LINE;
                }
            } catch (Exception *ptrError) {
                cout << NEW_LINE;
                ptrError->AddMethod(GetClassName(),
                                    "DetermineDownlinks(RelaySatellite*, GroundPointingAntenna*, ofstream&)");
                throw;
            }
        }
    }
}

void
LinksGenerator::DetermineRequiredDownlinkOverheadTime(const shared_ptr<RelaySatellite> &ptrRelaySatellite) const
{
    const int                         numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    Array<bool>                      *ptrRequiredOverheadArray = ptrRelaySatellite->GetRequiredDownlinkOverheadTimeArray();
    Link                             *ptrDownlink = NULL;
    shared_ptr<GroundPointingAntenna> ptrGPA = NULL;
    shared_ptr<MURALTypes::GPAVector> ptrGPAList = ptrRelaySatellite->GetGroundPointingAntennaList();
    MURALTypes::GPAVector::iterator   gpaIter;

    if (ptrRequiredOverheadArray != NULL) {
        int timeIndex = 0;

        for(gpaIter = ptrGPAList->begin(); gpaIter != ptrGPAList->end(); ++gpaIter) {
            ptrGPA = (*gpaIter);
            ListIterator<Link> downlinkListIter(ptrGPA->GetLinksList());
            ptrDownlink = downlinkListIter.First();

            while (ptrDownlink != NULL) {
                timeIndex = 0;

                while (timeIndex < numberOfTimeSteps) {
                    if ( (ptrDownlink->IsInview(timeIndex) == true) &&
                            (ptrRequiredOverheadArray->ElementAt(timeIndex) == true) ) {
                        ptrDownlink->SetLinkStatus(timeIndex, Link::REQUIRED_DOWNLINK_OVERHEAD);
                    }

                    ++timeIndex;
                }

                ptrDownlink = downlinkListIter.Next();
            }
        }

        delete ptrRequiredOverheadArray;
        ptrRequiredOverheadArray = NULL;
    }
}

void
LinksGenerator::DetermineMultihopLinks(const shared_ptr<RelaySatellite> &ptrFromRelaySatellite,
                                       ofstream &detailedLinksFile)
{
    Array<int>                         *ptrStatus = NULL;
    string                              toRelayDesignator = "";
    const string                        fromRelayDesignator = ptrFromRelaySatellite->GetDesignator();
    Antenna::FREQUENCY_BAND             frequencyBand = Antenna::UNKNOWN_BAND;
    Link                               *ptrLink = NULL;
    shared_ptr<RelayPointingAntenna>    ptrToRPA = NULL;
    shared_ptr<RelayPointingAntenna>    ptrFromRPA = NULL;
    shared_ptr<RelaySatellite>          ptrToRelaySatellite = NULL;
    shared_ptr<MURALTypes::RPAVector>   ptrToRPAList = NULL;
    shared_ptr<MURALTypes::RPAVector>   ptrFromRPAList = ptrFromRelaySatellite->GetRelayPointingAntennaList();
    MURALTypes::RPAVector::iterator     toRPAIter;
    MURALTypes::RPAVector::iterator     fromRPAIter;
    shared_ptr<MURALTypes::RelayVector> ptrRelayList = ptrDataProviderCollection_a->GetRelayListPtr();

    for(fromRPAIter = ptrFromRPAList->begin(); fromRPAIter != ptrFromRPAList->end(); ++fromRPAIter) {
        ptrFromRPA = (*fromRPAIter);

        if (ptrFromRPA->IsTransmittingAntenna() == true) {
            frequencyBand = ptrFromRPA->GetFrequencyBand();
            MURALTypes::RelayVector::iterator relayIter;

            for( relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
                ptrToRelaySatellite = (*relayIter);
                toRelayDesignator = ptrToRelaySatellite->GetDesignator();

                if (toRelayDesignator != fromRelayDesignator) {
                    ptrToRPAList = ptrToRelaySatellite->GetRelayPointingAntennaList();

                    for(toRPAIter = ptrToRPAList->begin(); toRPAIter != ptrToRPAList->end(); ++toRPAIter) {
                        ptrToRPA = (*toRPAIter);

                        if (ptrToRPA->IsReceiveAntenna() == true) {
                            try {
                                cout << "                Multihop Link With " << toRelayDesignator
                                     << ", RPA " << ptrToRPA->GetAntennaDesignator();

                                if (ptrToRPA->GetFrequencyBand() == frequencyBand) {
                                    ptrStatus = DetermineLinkStatusArray(ptrFromRelaySatellite, ptrFromRPA,
                                                                         ptrToRelaySatellite, ptrToRPA,
                                                                         detailedLinksFile);

                                    if (ptrStatus->ValueExists(Link::LINK) == true) {
                                        ptrLink = new Link(ptrFromRPA, ptrToRPA, ptrStatus,
                                                           ptrFromRelaySatellite->GetLinkOverheadData(toRelayDesignator));
                                        ptrFromRPA->AppendLink(ptrLink);
                                        ptrToRPA->AppendLink(ptrLink);
                                        ptrLink = NULL;
                                        cout << " - Created" << NEW_LINE;
                                    } else {
                                        cout << " - No Inview" << NEW_LINE;
                                    }

                                    delete ptrStatus;
                                    ptrStatus = NULL;
                                } else {
                                    cout << " - Mis-matching RRA Frequency Bands" << NEW_LINE;
                                }
                            } catch (Exception *ptrError) {
                                cout << NEW_LINE;
                                ptrError->AddMethod(GetClassName(), "DetermineMultihopLinks(RelaySatellite*, ofstream)");
                                throw;
                            }
                        }
                    }
                }
            }
        }
    }

    return;
}

void
LinksGenerator::RetrieveCrosslinks(const shared_ptr<UserVehicle> &ptrUserVehicle)
{
    Array2D<int>                       *ptrRelayStatus = NULL;
    Array3D<int>                       *ptrStatus = NULL;
    string                              inputString = "";
    string                              errorString = "";
    string                              userDesignator = "";
    string                              userAntennaDesignator = "";
    string                              relayDesignator = "";
    string                              relayAntennaDesignator = "";
    ifstream                            inputFile;
    Link                               *ptrLink = NULL;
    shared_ptr<UserPointingAntenna>     ptrUPA = NULL;
    shared_ptr<RelaySatellite>          ptrRelaySatellite = NULL;
    shared_ptr<UserTransmitAntenna>     ptrUserAntenna = ptrUserVehicle->GetUserTransmitAntenna();
    shared_ptr<MURALTypes::UPAVector>   ptrUPAList = NULL;
    shared_ptr<MURALTypes::RelayVector> ptrRelayList = ptrDataProviderCollection_a->GetRelayListPtr();
    MURALTypes::UPAVector::iterator     upaIter;
    const int                           numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    const int                           unknownLink = Link::UNKNOWN;
    const int                           numberOfRelays = ptrDataProviderCollection_a->GetRelayCount();
    const string                        currentUserDesignator = ptrUserVehicle->GetDesignator();
    const string                        currentUserAntennaDesignator = ptrUserAntenna->GetAntennaDesignator();
    const Antenna::FREQUENCY_BAND       frequencyBand = ptrUserAntenna->GetFrequencyBand();

    try {
        int linkStatus = 0;
        int timeIndex = 0;
        int relayIndex = 0;
        int upaIndex = 0;
        double inputTime = 0.0;
        ParseUtility::OpenInputFile(inputFile, FileNames::GetFileName(FileNames::CROSSLINK_INPUTFILE));
        ptrStatus = new Array3D<int>(numberOfRelays);
        MURALTypes::RelayVector::iterator relayIter;

        for( relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
            ptrRelaySatellite = (*relayIter);
            ptrRelayStatus = new Array2D<int>(ptrRelaySatellite->GetNumberOfUPAs(),
                                              numberOfTimeSteps, unknownLink);
            ptrStatus->AddArray(ptrRelaySatellite->GetIndex(), ptrRelayStatus);
            ptrRelayStatus = NULL;
        }

        // get the header line first
        getline(inputFile, inputString, NEW_LINE);

        while (getline(inputFile, inputString, NEW_LINE)) {
            istringstream inputStream(inputString);
            inputStream >> userDesignator >> userAntennaDesignator;

            if ( (userDesignator == currentUserDesignator) &&
                    (userAntennaDesignator == currentUserAntennaDesignator) ) {
                inputStream >> relayDesignator >> relayAntennaDesignator >> inputTime >> linkStatus;
                ptrRelaySatellite = ptrDataProviderCollection_a->FindRelay(relayDesignator);

                if ( (TimePiece::ConvertToStoreTimeIndex(inputTime, timeIndex) == true) && (ptrRelaySatellite != NULL) ) {
                    ptrUPA = ptrRelaySatellite->GetUPA(relayAntennaDesignator, upaIndex);

                    if (ptrUPA != NULL) {
                        if (ptrUPA->GetFrequencyBand() == frequencyBand) {
                            if (linkStatus == Link::LINK) {
                                if ( (ptrUPA->IsAnOutageTimeStep(timeIndex, userDesignator) == true) ||
                                        (ptrUserAntenna->IsAnOutageTimeStep(timeIndex, relayDesignator) == true) ) {
                                    linkStatus = Link::OUTAGE_TIME_STEP;
                                } else if (ptrUPA->IsNonFullyDedicatedTransmitter(timeIndex, userDesignator) == true) {
                                    linkStatus = Link::FULLY_DEDICATED_TIME_STEP;
                                } else if (MissingRequiredDownlink(ptrRelaySatellite, timeIndex) == true) {
                                    linkStatus = Link::NO_SUPPORTING_DOWNLINK;
                                }
                            }

                            relayIndex = ptrRelaySatellite->GetIndex();
                            (*ptrStatus)[relayIndex][upaIndex][timeIndex] = linkStatus;
                        }
                    }
                }
            }
        }

        for( relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
            ptrRelaySatellite = (*relayIter);
            ptrUPAList = ptrRelaySatellite->GetUserPointingAntennaList();
            relayDesignator = ptrRelaySatellite->GetDesignator();
            relayIndex = ptrRelaySatellite->GetIndex();
            upaIndex = 0;

            for(upaIter = ptrUPAList->begin(); upaIter != ptrUPAList->end(); ++upaIter) {
                ptrUPA = (*upaIter);
                cout << "                Crosslink With " << relayDesignator
                     << ", UPA " << ptrUPA->GetAntennaDesignator();

                if (ptrUPA->GetFrequencyBand() == frequencyBand) {
                    if ((ptrStatus->ElementAt(relayIndex, upaIndex))->ValueExists(unknownLink) == true) {
                        int timeIndex = (ptrStatus->ElementAt(relayIndex, upaIndex))->FindIndex(unknownLink);
                        cout << NEW_LINE;
                        errorString = "Missing crosslink status(es) between ";
                        errorString += currentUserDesignator;
                        errorString += " and ";
                        errorString += ptrRelaySatellite->GetDesignator();
                        errorString += ", UPA ";
                        errorString += ptrUPA->GetAntennaDesignator();
                        errorString += " at time ";
                        errorString += StringEditor::ConvertToString(timeIndex + 1);
                        throw new InputException(GetClassName(), "RetrieveCrosslinks(UserVehicle*)",
                                                 errorString);
                    } else if ((ptrStatus->ElementAt(relayIndex, upaIndex))->ValueExists(Link::LINK) == true) {
                        ptrLink = new Link(ptrUserAntenna, ptrUPA, ptrStatus->ElementAt(relayIndex, upaIndex),
                                           ptrUserVehicle->GetLinkOverheadData(relayDesignator));
                        ptrUserAntenna->AppendLink(ptrLink);
                        ptrUPA->AppendLink(ptrLink);
                        ptrLink = NULL;
                        cout << " - Retrieved" << NEW_LINE;
                    } else {
                        cout << " - No Inview" << NEW_LINE;
                    }
                } else {
                    cout << " - Mis-matching Frequency Bands" << NEW_LINE;
                }

                ++upaIndex;
            }
        }

        delete ptrStatus;
        ptrStatus = NULL;
        inputFile.close();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "RetrieveCrosslinks(UserVehicle*)");
        throw;
    }

    return;
}

void
LinksGenerator::RetrieveDirectDownlinks(const shared_ptr<UserVehicle> &ptrUserVehicle)
{
    Array2D<int>                        *ptrGroundStatus = NULL;
    Array3D<int>                        *ptrStatus = NULL;
    string                               inputString = "";
    string                               errorString = "";
    string                               userDesignator = "";
    string                               ddlAntennaDesignator = "";
    string                               groundDesignator = "";
    string                               rfAntennaDesignator = "";
    ifstream                             inputFile;
    Link                                *ptrLink = NULL;
    shared_ptr<DirectDownlinkAntenna>    ptrDirectDownlinkAntenna = ptrUserVehicle->GetDirectDownlinkAntenna();
    shared_ptr<ReceiveFacilityAntenna>   ptrRF = NULL;
    shared_ptr<GroundStation>            ptrGroundStation = NULL;
    shared_ptr<MURALTypes::RFAVector>    ptrRFList = NULL;
    shared_ptr<MURALTypes::GroundVector> ptrGroundList = ptrDataProviderCollection_a->GetGroundListPtr();
    const int                            unknownLink = Link::UNKNOWN;
    const int                            numberOfGroundStations = ptrDataProviderCollection_a->GetGroundCount();
    const int                            numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    const string                         currentUserDesignator = ptrUserVehicle->GetDesignator();
    const string                         currentDDLAntennaDesignator = ptrDirectDownlinkAntenna->GetAntennaDesignator();
    const Antenna::FREQUENCY_BAND        frequencyBand = ptrDirectDownlinkAntenna->GetFrequencyBand();
    MURALTypes::RFAVector::iterator      rfaIter;

    try {
        int linkStatus = 0;
        int timeIndex = 0;
        int groundIndex = 0;
        int rfIndex = 0;
        double inputTime = 0.0;
        ParseUtility::OpenInputFile(inputFile, FileNames::GetFileName(FileNames::DIRECTDOWNLINK_INPUTFILE));
        ptrStatus = new Array3D<int>(numberOfGroundStations);
        MURALTypes::GroundVector::iterator groundIter;

        for( groundIter = ptrGroundList->begin(); groundIter != ptrGroundList->end(); ++groundIter) {
            ptrGroundStation = (*groundIter);
            ptrGroundStatus = new Array2D<int>(ptrGroundStation->GetNumberOfRFs(),
                                               numberOfTimeSteps, unknownLink);
            ptrStatus->AddArray(ptrGroundStation->GetIndex(), ptrGroundStatus);
            ptrGroundStatus = NULL;
        }

        // get the header line first
        getline(inputFile, inputString, NEW_LINE);

        while (getline(inputFile, inputString, NEW_LINE)) {
            istringstream inputStream(inputString);
            inputStream >> userDesignator >> ddlAntennaDesignator;

            if ( (userDesignator == currentUserDesignator) &&
                    (ddlAntennaDesignator == currentDDLAntennaDesignator) ) {
                inputStream >> groundDesignator >> rfAntennaDesignator >> inputTime >> linkStatus;
                ptrGroundStation = ptrDataProviderCollection_a->FindGround(groundDesignator);

                if ( (TimePiece::ConvertToStoreTimeIndex(inputTime, timeIndex) == true) && (ptrGroundStation != NULL) ) {
                    ptrRF = ptrGroundStation->GetRF(rfAntennaDesignator, rfIndex);

                    if (ptrRF != NULL) {
                        if (ptrRF->GetFrequencyBand() == frequencyBand) {
                            if (linkStatus == Link::LINK) {
                                if ( (ptrRF->IsAnOutageTimeStep(timeIndex, userDesignator) == true) ||
                                        (ptrDirectDownlinkAntenna->IsAnOutageTimeStep(timeIndex, groundDesignator) == true) ) {
                                    linkStatus = Link::OUTAGE_TIME_STEP;
                                } else if (ptrRF->IsNonFullyDedicatedTransmitter(timeIndex, userDesignator) == true) {
                                    linkStatus = Link::FULLY_DEDICATED_TIME_STEP;
                                }
                            }

                            groundIndex = ptrGroundStation->GetIndex();
                            (*ptrStatus)[groundIndex][rfIndex][timeIndex] = linkStatus;
                        }
                    }
                }
            }
        }

        for( groundIter = ptrGroundList->begin(); groundIter != ptrGroundList->end(); ++groundIter) {
            ptrGroundStation = (*groundIter);
            ptrRFList = ptrGroundStation->GetRFList();
            groundDesignator = ptrGroundStation->GetDesignator();
            groundIndex = ptrGroundStation->GetIndex();
            rfIndex = 0;

            for(rfaIter = ptrRFList->begin(); rfaIter != ptrRFList->end(); ++rfaIter) {
                ptrRF = (*rfaIter);
                cout << "                Direct Downlink With " << groundDesignator
                     << ", RF " << ptrRF->GetAntennaDesignator();

                if (ptrRF->GetFrequencyBand() == frequencyBand) {
                    if ((ptrStatus->ElementAt(groundIndex, rfIndex))->ValueExists(unknownLink) == true) {
                        cout << NEW_LINE;
                        errorString = "Missing one or more crosslink status(es) between ";
                        errorString += currentUserDesignator;
                        errorString += " and ";
                        errorString += ptrGroundStation->GetDesignator();
                        throw new InputException(GetClassName(),
                                                 "RetrieveDirectDownlinks(UserVehicle*)",
                                                 errorString);
                    } else if ((ptrStatus->ElementAt(groundIndex, rfIndex))->ValueExists(Link::LINK) == true) {
                        ptrLink = new Link(ptrDirectDownlinkAntenna, ptrRF, ptrStatus->ElementAt(groundIndex, rfIndex),
                                           ptrUserVehicle->GetLinkOverheadData(groundDesignator));
                        ptrDirectDownlinkAntenna->AppendLink(ptrLink);
                        ptrRF->AppendLink(ptrLink);
                        ptrLink = NULL;
                        cout << " - Retrieved" << NEW_LINE;
                    } else {
                        cout << " - No Inview" << NEW_LINE;
                    }
                } else {
                    cout << " - Mis-matching Frequency Band" << NEW_LINE;
                }

                ++rfIndex;
            }
        }

        delete ptrStatus;
        ptrStatus = NULL;
        inputFile.close();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "RetrieveDirectDownlinks(UserVehicle*)");
        throw;
    }

    return;
}

void
LinksGenerator::RetrieveSelfRelayingLinks(const shared_ptr<UserVehicle> &ptrFromUserVehicle,
        const shared_ptr<SelfRelayAntenna> &ptrFromSRA)
{
    Array2D<int>                       *ptrStatus = NULL;
    string                              inputString = "";
    string                              errorString = "";
    string                              fromUserDesignator = "";
    string                              fromAntennaDesignator = "";
    string                              toUserDesignator = "";
    string                              toAntennaDesignator = "";
    ifstream                            inputFile;
    Link                               *ptrLink = NULL;
    shared_ptr<SelfRelayAntenna>        ptrSRA = NULL;
    shared_ptr<UserVehicle>             ptrToUserVehicle = NULL;
    shared_ptr<MURALTypes::SRAVector>   ptrSRAList = NULL;
    shared_ptr<MURALTypes::UserVector>  ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();
    MURALTypes::SRAVector::iterator     sraIter;
    const int                           numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    const int                           unknownLink = Link::UNKNOWN;
    const int                           currentUserIndex = ptrFromUserVehicle->GetIndex();
    const int                           numberOfUsers = ptrDataProviderCollection_a->GetUserCount();
    const string                        currentUserDesignator = ptrFromUserVehicle->GetDesignator();
    const string                        currentAntennaDesignator = ptrFromSRA->GetAntennaDesignator();
    const Antenna::FREQUENCY_BAND       frequencyBand = ptrFromSRA->GetFrequencyBand();

    try {
        int linkStatus = 0;
        int timeIndex = 0;
        int toUserIndex = 0;
        double inputTime = 0.0;
        ParseUtility::OpenInputFile(inputFile, FileNames::GetFileName(FileNames::SELFRELAY_INPUTFILE));
        ptrStatus = new Array2D<int>(numberOfUsers, numberOfTimeSteps, unknownLink);
        // get the header line first
        getline(inputFile, inputString, NEW_LINE);

        while (getline(inputFile, inputString, NEW_LINE)) {
            istringstream inputStream(inputString);
            inputStream >> fromUserDesignator >> fromAntennaDesignator;

            if ( (fromUserDesignator == currentUserDesignator) &&
                    (fromAntennaDesignator == currentAntennaDesignator) ) {
                inputStream >> toUserDesignator >> toAntennaDesignator >> inputTime >> linkStatus;
                ptrToUserVehicle = ptrDataProviderCollection_a->FindUser(toUserDesignator);

                if ( (TimePiece::ConvertToStoreTimeIndex(inputTime, timeIndex) == true) && (ptrToUserVehicle != NULL) ) {
                    toUserIndex = ptrToUserVehicle->GetIndex();

                    if (toUserIndex != currentUserIndex) {
                        ptrSRA = ptrToUserVehicle->FindSelfRelayAntenna(toAntennaDesignator);

                        if (ptrSRA != NULL) {
                            if ( (ptrSRA->IsReceiveAntenna() == true) &&
                                    (ptrSRA->GetFrequencyBand() == frequencyBand) ) {
                                if (linkStatus == Link::LINK) {
                                    if ( (ptrFromSRA->IsAnOutageTimeStep(timeIndex, toUserDesignator) == true) ||
                                            (ptrSRA->IsAnOutageTimeStep(timeIndex, fromUserDesignator) == true) ) {
                                        linkStatus = Link::OUTAGE_TIME_STEP;
                                    }
                                }

                                (*ptrStatus)[toUserIndex][timeIndex] = linkStatus;
                            }
                        }
                    }
                }
            }
        }

        MURALTypes::UserVector::iterator userIter;

        for( userIter = ptrUserList->begin(); ((userIter != ptrUserList->end()) && (errorString.size() == 0)); ++userIter) {
            ptrToUserVehicle = (*userIter);
            toUserIndex               = ptrToUserVehicle->GetIndex();
            toUserDesignator          = ptrToUserVehicle->GetDesignator();
            ptrSRAList = ptrToUserVehicle->GetSelfRelayAntennaList();

            for( sraIter = ptrSRAList->begin(); sraIter != ptrSRAList->end(); ++sraIter) {
                ptrSRA = (*sraIter);

                if (ptrSRA->IsReceiveAntenna() == true) {
                    if (toUserIndex != currentUserIndex) {
                        cout << "                Self Relaying Link With " << toUserDesignator;

                        if (ptrSRA->GetFrequencyBand() == frequencyBand) {
                            if ((ptrStatus->ElementAt(toUserIndex))->ValueExists(unknownLink) == true) {
                                errorString = "Missing self relaying link status(es) between ";
                                errorString += currentUserDesignator;
                                errorString += " and ";
                                errorString += ptrToUserVehicle->GetDesignator();
                                cout << NEW_LINE;
                            } else if (ptrStatus->ValueExists(Link::LINK) == true) {
                                ptrLink = new Link(ptrFromSRA, ptrSRA,
                                                   ptrStatus->ElementAt(toUserIndex),
                                                   ptrFromUserVehicle->GetLinkOverheadData(toUserDesignator));
                                ptrFromSRA->AppendLink(ptrLink);
                                ptrSRA->AppendLink(ptrLink);
                                ptrLink = NULL;
                                cout << " - Retrieved" << NEW_LINE;
                            } else {
                                cout << " - No Inview" << NEW_LINE;
                            }
                        } else {
                            cout << " - Mis-matching Frequency Bands" << NEW_LINE;
                        }
                    }
                }
            }
        }

        delete ptrStatus;
        ptrStatus = NULL;
        inputFile.close();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "RetrieveSelfRelayingLinks(UserVehicle*)");
        throw;
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "RetrieveSelfRelayingLinks(UserVehicle*)",
                                 errorString);
    }

    return;
}

void
LinksGenerator::RetrieveDownlinks(const shared_ptr<RelaySatellite> &ptrRelaySatellite,
                                  const shared_ptr<GroundPointingAntenna> &ptrGPA)
{
    Array2D<int>                            *ptrGroundStatus = NULL;
    Array3D<int>                            *ptrStatus = NULL;
    string                                   inputString = "";
    string                                   errorString = "";
    string                                   relayDesignator = "";
    string                                   gpaDesignator = "";
    string                                   groundDesignator = "";
    string                                   rfDesignator = "";
    ifstream                                 inputFile;
    Link                                    *ptrLink = NULL;
    shared_ptr<ReceiveFacilityAntenna>       ptrRF = NULL;
    shared_ptr<MURALTypes::RFAVector>        ptrRFList = NULL;
    shared_ptr<GroundStation>                ptrGroundStation = NULL;
    shared_ptr<MURALTypes::GroundVector>     ptrGroundList = ptrDataProviderCollection_a->GetGroundListPtr();
    const int                                unknownLink = Link::UNKNOWN;
    const int                                numberOfGroundStations = ptrDataProviderCollection_a->GetGroundCount();
    const int                                numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    const string                             currentRelayDesignator = ptrRelaySatellite->GetDesignator();
    const string                             currentGPADesignator = ptrGPA->GetAntennaDesignator();
    const Antenna::FREQUENCY_BAND            frequencyBand = ptrGPA->GetFrequencyBand();
    MURALTypes::RFAVector::iterator          rfaIter;

    try {
        int linkStatus = 0;
        int timeIndex = 0;
        int groundIndex = 0;
        int rfIndex = 0;
        double inputTime = 0.0;
        ParseUtility::OpenInputFile(inputFile, FileNames::GetFileName(FileNames::DOWNLINK_INPUTFILE));
        ptrStatus = new Array3D<int>(numberOfGroundStations);
        MURALTypes::GroundVector::iterator groundIter;

        for( groundIter = ptrGroundList->begin(); groundIter != ptrGroundList->end(); ++groundIter) {
            ptrGroundStation = (*groundIter);
            ptrGroundStatus = new Array2D<int>(ptrGroundStation->GetNumberOfRFs(),
                                               numberOfTimeSteps, unknownLink);
            ptrStatus->AddArray(ptrGroundStation->GetIndex(), ptrGroundStatus);
            ptrGroundStatus = NULL;
        }

        // get the header line first
        getline(inputFile, inputString, NEW_LINE);

        while (getline(inputFile, inputString, NEW_LINE)) {
            istringstream inputStream(inputString);
            inputStream >> relayDesignator >> gpaDesignator;

            if ( (relayDesignator == currentRelayDesignator) &&
                    (gpaDesignator == currentGPADesignator) ) {
                inputStream >> groundDesignator >> rfDesignator >> inputTime >> linkStatus;
                ptrGroundStation = ptrDataProviderCollection_a->FindGround(groundDesignator);

                if ( (TimePiece::ConvertToStoreTimeIndex(inputTime, timeIndex) == true) && (ptrGroundStation != NULL) ) {
                    ptrRF = ptrGroundStation->GetRF(rfDesignator, rfIndex);

                    if (ptrRF != NULL) {
                        if (ptrRF->GetFrequencyBand() == frequencyBand) {
                            if (linkStatus == Link::LINK) {
                                if ( (ptrRF->IsAnOutageTimeStep(timeIndex, relayDesignator) == true) ||
                                        (ptrGPA->IsAnOutageTimeStep(timeIndex, groundDesignator) == true) ) {
                                    linkStatus = Link::OUTAGE_TIME_STEP;
                                } else if (ptrRF->IsNonFullyDedicatedTransmitter(timeIndex, relayDesignator) == true) {
                                    linkStatus = Link::FULLY_DEDICATED_TIME_STEP;
                                }
                            }

                            groundIndex = ptrGroundStation->GetIndex();
                            (*ptrStatus)[groundIndex][rfIndex][timeIndex] = linkStatus;
                        }
                    }
                }
            }
        }

        for( groundIter = ptrGroundList->begin(); groundIter != ptrGroundList->end(); ++groundIter) {
            ptrGroundStation = (*groundIter);
            ptrRFList = ptrGroundStation->GetRFList();
            groundDesignator = ptrGroundStation->GetDesignator();
            groundIndex = ptrGroundStation->GetIndex();
            rfIndex = 0;

            for(rfaIter = ptrRFList->begin(); rfaIter != ptrRFList->end(); ++rfaIter) {
                ptrRF = (*rfaIter);
                cout << "                Downlink With " << groundDesignator
                     << ", RF " << ptrRF->GetAntennaDesignator();

                if (ptrRF->GetFrequencyBand() == frequencyBand) {
                    if ((ptrStatus->ElementAt(groundIndex, rfIndex))->ValueExists(unknownLink) == true) {
                        cout << NEW_LINE;
                        errorString = "Missing downlink status(es) between ";
                        errorString += currentRelayDesignator;
                        errorString += " and ";
                        errorString += groundDesignator;
                        throw new InputException(GetClassName(),
                                                 "RetrieveDownlinks(RelaySatellite*)",
                                                 errorString);
                    } else if ((ptrStatus->ElementAt(groundIndex, rfIndex))->ValueExists(Link::LINK) == true) {
                        ptrLink = new Link(ptrGPA, ptrRF, ptrStatus->ElementAt(groundIndex, rfIndex),
                                           ptrRelaySatellite->GetLinkOverheadData(groundDesignator));
                        ptrGPA->AppendLink(ptrLink);
                        ptrRF->AppendLink(ptrLink);
                        ptrLink = NULL;
                        cout << " - Retrieved" << NEW_LINE;
                    } else {
                        cout << " - No Inview" << NEW_LINE;
                    }
                } else {
                    cout << " - Mis-matching Frequency Bands" << NEW_LINE;
                }

                ++rfIndex;
            }
        }

        delete ptrStatus;
        ptrStatus = NULL;
        inputFile.close();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "RetrieveDownlinks(RelaySatellite*)");
        throw;
    }

    return;
}

void
LinksGenerator::RetrieveMultihopLinks(const shared_ptr<RelaySatellite> &ptrFromRelaySatellite)
{
    int                                           linkStatus = 0;
    Array<int>                                   *ptrStatus = NULL;
    int                                           timeIndex = 0;
    double                                        inputTime = 0.0;
    string                                        inputString = "";
    string                                        errorString = "";
    string                                        transmitRelayDesignator = "";
    string                                        transmitAntennaDesignator = "";
    string                                        toRelayDesignator = "";
    string                                        toAntennaDesignator = "";
    string                                        receiveRelayDesignator = "";
    string                                        receiveAntennaDesignator = "";
    string                                        fromAntennaDesignator = "";
    ifstream                                      inputFile;
    Antenna::FREQUENCY_BAND                       frequencyBand = Antenna::UNKNOWN_BAND;
    Link                                         *ptrLink = NULL;
    shared_ptr<RelayPointingAntenna>              ptrToRPA = NULL;
    shared_ptr<RelayPointingAntenna>              ptrFromRPA = NULL;
    shared_ptr<MURALTypes::RPAVector>             ptrToList = NULL;
    shared_ptr<MURALTypes::RPAVector>             ptrFromList = NULL;
    shared_ptr<RelaySatellite>                    ptrToRelay = NULL;
    MURALTypes::RPAVector::iterator               fromRPAIter;
    MURALTypes::RPAVector::iterator               toRPAIter;
    shared_ptr<MURALTypes::RelayVector>           ptrRelayList = ptrDataProviderCollection_a->GetRelayListPtr();
    const int                        unknownLink = Link::UNKNOWN;
    const string                       fromRelayDesignator = ptrFromRelaySatellite->GetDesignator();
    ptrFromList = ptrFromRelaySatellite->GetRelayPointingAntennaList();

    for(fromRPAIter = ptrFromList->begin(); fromRPAIter != ptrFromList->end(); ++fromRPAIter) {
        ptrFromRPA = (*fromRPAIter);

        if (ptrFromRPA->IsTransmittingAntenna() == true) {
            fromAntennaDesignator = ptrFromRPA->GetAntennaDesignator();
            frequencyBand = ptrFromRPA->GetFrequencyBand();
            MURALTypes::RelayVector::iterator relayIter;

            for( relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
                ptrToRelay = (*relayIter);
                toRelayDesignator = ptrToRelay->GetDesignator();

                if (toRelayDesignator != fromRelayDesignator) {
                    ptrToList = ptrToRelay->GetRelayPointingAntennaList();

                    for(toRPAIter = ptrToList->begin(); toRPAIter != ptrToList->end(); ++toRPAIter) {
                        ptrToRPA = (*toRPAIter);

                        if ( (ptrToRPA->IsReceiveAntenna() == true) &&
                                (ptrToRPA->GetFrequencyBand() == frequencyBand) ) {
                            try {
                                ParseUtility::OpenInputFile(inputFile, FileNames::GetFileName(FileNames::MULTIHOP_INPUTFILE));
                                ptrStatus = new Array<int>(TimePiece::GetNumberOfTimeSteps(), unknownLink);
                                toAntennaDesignator = ptrToRPA->GetAntennaDesignator();
                                cout << "                Multihop Link With " << toRelayDesignator
                                     << ", RPA " << toAntennaDesignator;
                                // get the header line first
                                getline(inputFile, inputString, NEW_LINE);

                                while (getline(inputFile, inputString, NEW_LINE)) {
                                    istringstream inputStream(inputString);
                                    inputStream >> transmitRelayDesignator >> transmitAntennaDesignator;

                                    if ( (fromRelayDesignator == transmitRelayDesignator) &&
                                            (fromAntennaDesignator == transmitAntennaDesignator) ) {
                                        inputStream >> receiveRelayDesignator >> receiveAntennaDesignator >> inputTime;

                                        if ( (toRelayDesignator == receiveRelayDesignator) &&
                                                (toAntennaDesignator == receiveAntennaDesignator) &&
                                                (TimePiece::ConvertToStoreTimeIndex(inputTime, timeIndex) == true) ) {
                                            inputStream >> linkStatus;

                                            if (linkStatus == Link::LINK) {
                                                if ( (ptrFromRPA->IsAnOutageTimeStep(timeIndex, toRelayDesignator) == true) ||
                                                        (ptrToRPA->IsAnOutageTimeStep(timeIndex, fromRelayDesignator) == true) ) {
                                                    linkStatus = Link::OUTAGE_TIME_STEP;
                                                }
                                            }

                                            (*ptrStatus)[timeIndex] = linkStatus;
                                        }
                                    }
                                }

                                if (ptrStatus->ValueExists(unknownLink) == true) {
                                    cout << NEW_LINE;
                                    delete ptrStatus;
                                    ptrStatus = NULL;
                                    errorString = "Missing multihop link status(es) between ";
                                    errorString += fromRelayDesignator;
                                    errorString += " and ";
                                    errorString += toRelayDesignator;
                                    throw new InputException(GetClassName(),
                                                             "RetrieveMultihopLinks(RelaySatellite*)",
                                                             errorString);
                                } else if (ptrStatus->ValueExists(Link::LINK) == true) {
                                    ptrLink = new Link(ptrFromRPA, ptrToRPA, ptrStatus,
                                                       ptrFromRelaySatellite->GetLinkOverheadData(toRelayDesignator));
                                    ptrFromRPA->AppendLink(ptrLink);
                                    ptrToRPA->AppendLink(ptrLink);
                                    ptrLink = NULL;
                                    cout << " - Retrieved" << NEW_LINE;
                                } else {
                                    cout << " - No Inview" << NEW_LINE;
                                }

                                delete ptrStatus;
                                ptrStatus = NULL;
                                inputFile.close();
                            } catch (Exception *ptrError) {
                                ptrError->AddMethod(GetClassName(), "RetrieveMultihopLinks(RelaySatellite*)");
                                throw;
                            }
                        }
                    }
                }
            }
        }
    }

    return;
}

Array<int>*
LinksGenerator::DetermineLinkStatusArray(const shared_ptr<DataProvider> &ptrFromProvider,
        const shared_ptr<Antenna> &ptrFromAntenna,
        const shared_ptr<DataProvider> &ptrToProvider,
        const shared_ptr<Antenna> &ptrToAntenna,
        ofstream &detailedLinksFile,
        bool checkToDownlinkSupport,
        const shared_ptr<UserVehicle> &ptrClusterUser)
{
    int              timeIndex = 0;
    const int        numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    const int        endIndexTime = TimePiece::GetEndIndexTime();
    Array<int>     *ptrStatusArray = new Array<int>(numberOfTimeSteps, Link::NO_LINK);
    double            elevationAngleFrom = 0.0;
    double            elevationAngleTo = 0.0;
    double            grazingAltitude = 0.0;
    double            range = 0.0;
    double            separationAngle = 0.0;
    double            sunAngleFrom = 0.0;
    double            sunAngleTo = 0.0;
    double            azimuthFrom = UNDEFINED_ANGLE;
    double            azimuthTo = UNDEFINED_ANGLE;
    double            minimumAzimuthFrom = UNDEFINED_ANGLE;
    double            maximumAzimuthFrom = -UNDEFINED_ANGLE;
    double            minimumAzimuthTo = UNDEFINED_ANGLE;
    double            maximumAzimuthTo = -UNDEFINED_ANGLE;
    double            minimumElevationFrom = UNDEFINED_ANGLE;
    double            maximumElevationFrom = -UNDEFINED_ANGLE;
    double            minimumElevationTo = UNDEFINED_ANGLE;
    double            maximumElevationTo = -UNDEFINED_ANGLE;
    double            solarIntrusionAngle = UNDEFINED_ANGLE;
    const string      fromDesignator = ptrFromProvider->GetDesignator();
    const string      toDesignator = ptrToProvider->GetDesignator();
    double            minElevationFrom = -UNDEFINED_ANGLE;
    double            maxElevationFrom = UNDEFINED_ANGLE;
    double            minElevationTo = -UNDEFINED_ANGLE;
    double            maxElevationTo = UNDEFINED_ANGLE;
    const double      secondsPerTimeStep = (double)TimePiece::GetSecondsPerTimeStep();
    const double      quarterCircle = UnitData::GetStoreQuarterCircle();
    const double      earthRadius = Earth::GetStoreRadius();
    const double      minGrazing = std::min(ptrFromAntenna->GetMinimumGrazingAltitude(toDesignator),
                                            ptrToAntenna->GetMinimumGrazingAltitude(fromDesignator));
    const double      maxRange = ptrFromAntenna->GetMaximumRange(toDesignator);
    const double      minRange = ptrFromAntenna->GetMinimumRange(toDesignator);
    const double      maxSunFrom = ptrFromAntenna->GetMaximumSunSeparationAngle(toDesignator);
    const double      minSunFrom = ptrFromAntenna->GetMinimumSunSeparationAngle(toDesignator);
    const double      maxSunTo = ptrToAntenna->GetMaximumSunSeparationAngle(fromDesignator);
    const double      minSunTo = ptrToAntenna->GetMinimumSunSeparationAngle(fromDesignator);
    const double      minICCLSolarAngle = ptrFromProvider->GetMinimumICCLSolarIntrusionAngle();
    bool              fromAzimuthBasedAngles = false;
    bool              toAzimuthBasedAngles = false;
    Link::LINK_STATUS linkStatus = Link::NO_LINK;
    SpaceVector       fromVehiclePosition;
    SpaceVector       toVehiclePosition;
    SpaceVector       lineOfSight;
    SpaceVector       lineOfSightNegated;
    SpaceVector       interimSunPosition;
    SpaceVector       sunPosition;
    SpaceVector       fromVehicleToSun;
    SpaceVector       toVehicleToSun;
    SpaceVector       clusterLOS;
    shared_ptr<UserVehicle>      ptrFromUser = dynamic_pointer_cast<UserVehicle>(ptrFromProvider);
    shared_ptr<RelaySatellite>   ptrToRelay = NULL;

    if (checkToDownlinkSupport == true) {
        ptrToRelay = dynamic_pointer_cast<RelaySatellite>(ptrToProvider);
    }

    // if the body does not tilt for collection, then use the appropriate min and
    // max elevation angles to check for link status
    if (ptrFromProvider->BodyTiltsForCollection() == false) {
        if (ptrFromAntenna->HasAzimuthBasedElevationAngles() == false) {
            minElevationFrom = ptrFromAntenna->GetMinimumElevationAngle(toDesignator);
            maxElevationFrom = ptrFromAntenna->GetMaximumElevationAngle(toDesignator);
        } else {
            fromAzimuthBasedAngles = true;
        }
    }

    // if the body does not tilt for collection, then use the appropriate min and
    // max elevation angles to check for link status
    if (ptrToProvider->BodyTiltsForCollection() == false) {
        if (ptrToAntenna->HasAzimuthBasedElevationAngles() == false) {
            minElevationTo = ptrToAntenna->GetMinimumElevationAngle(fromDesignator);
            maxElevationTo = ptrToAntenna->GetMaximumElevationAngle(fromDesignator);
        } else {
            toAzimuthBasedAngles = true;
        }
    }

    while (timeIndex < numberOfTimeSteps) {
        double additionalSeconds;
        additionalSeconds = 0.0;
        linkStatus = Link::LINK;
        sunPosition = Sun::GetPosition(timeIndex);

        while ((additionalSeconds < secondsPerTimeStep) && (linkStatus == Link::LINK)) {
            fromVehiclePosition = ptrFromProvider->GetInterimPosition(timeIndex, additionalSeconds);
            toVehiclePosition = ptrToProvider->GetInterimPosition(timeIndex, additionalSeconds);
            lineOfSight = toVehiclePosition - fromVehiclePosition;
            lineOfSightNegated = lineOfSight.Negate();
            ptrFromAntenna->DetermineAzElAngles(timeIndex, lineOfSight, additionalSeconds,
                                                azimuthFrom, elevationAngleFrom);

            if (fromAzimuthBasedAngles == true) {
                ptrFromAntenna->GetAzimuthBasedElevationAngles(azimuthFrom, toDesignator,
                        minElevationFrom, maxElevationFrom);
            }

            ptrToAntenna->DetermineAzElAngles(timeIndex, lineOfSightNegated, additionalSeconds,
                                              azimuthTo, elevationAngleTo);

            if (toAzimuthBasedAngles == true) {
                ptrToAntenna->GetAzimuthBasedElevationAngles(azimuthTo, fromDesignator,
                        minElevationTo, maxElevationTo);
            }

            if (fromVehiclePosition.GetMagnitude() <= toVehiclePosition.GetMagnitude()) {
                separationAngle = lineOfSight.GetSeparationAngle(fromVehiclePosition.Negate());

                if (separationAngle < quarterCircle) {
                    grazingAltitude = SimpleMath::Abs(fromVehiclePosition.GetMagnitude() * sin(separationAngle));
                } else {
                    grazingAltitude = fromVehiclePosition.GetMagnitude();
                }
            } else {
                separationAngle = lineOfSightNegated.GetSeparationAngle(toVehiclePosition.Negate());

                if (separationAngle < quarterCircle) {
                    grazingAltitude = SimpleMath::Abs(toVehiclePosition.GetMagnitude() * sin(separationAngle));
                } else {
                    grazingAltitude = toVehiclePosition.GetMagnitude();
                }
            }

            grazingAltitude -= earthRadius;
            range = lineOfSight.GetMagnitude();

            if (additionalSeconds == 0.0) {
                interimSunPosition = sunPosition;
            } else {
                interimSunPosition = PositionUtility::DetermineInterimPosition(sunPosition, Sun::GetPosition(timeIndex + 1),
                                     secondsPerTimeStep, additionalSeconds);
            }

            fromVehicleToSun = interimSunPosition - fromVehiclePosition;
            toVehicleToSun = interimSunPosition - toVehiclePosition;
            sunAngleFrom = fromVehicleToSun.GetSeparationAngle(lineOfSight);
            sunAngleTo = toVehicleToSun.GetSeparationAngle(lineOfSightNegated);

            if ((ptrClusterUser != NULL) && (minICCLSolarAngle != UNDEFINED_ANGLE)) {
                clusterLOS = ptrClusterUser->GetInterimPosition(timeIndex, additionalSeconds) - fromVehiclePosition;
                solarIntrusionAngle = clusterLOS.GetSeparationAngle(fromVehicleToSun);
            }

            if ((minGrazing != UNDEFINED_DISTANCE) && (grazingAltitude < minGrazing)) {
                linkStatus = Link::MIN_GRAZING_ALTITUDE;
            } else if ((maxRange != UNDEFINED_DISTANCE) && (range > maxRange)) {
                linkStatus = Link::MAX_RANGE_TRANSMIT;
            } else if ((minRange != UNDEFINED_DISTANCE) && (range < minRange)) {
                linkStatus = Link::MIN_RANGE_TRANSMIT;
            } else if ((maxSunFrom != UNDEFINED_ANGLE) && (sunAngleFrom > maxSunFrom)) {
                linkStatus = Link::MAX_SUN_SEPARATION_TRANSMIT;
            } else if ((minSunFrom != UNDEFINED_ANGLE) && (sunAngleFrom < minSunFrom)) {
                linkStatus = Link::MIN_SUN_SEPARATION_TRANSMIT;
            } else if ((maxSunTo != UNDEFINED_ANGLE) && (sunAngleTo > maxSunTo)) {
                linkStatus = Link::MAX_SUN_SEPARATION_RECEIVE;
            } else if ((minSunTo != UNDEFINED_ANGLE) && (sunAngleTo < minSunTo)) {
                linkStatus = Link::MIN_SUN_SEPARATION_RECEIVE;
            } else if ((maxElevationTo != UNDEFINED_ANGLE) && (elevationAngleTo > maxElevationTo)) {
                linkStatus = Link::MAX_ELEVATION_RECEIVE;
            } else if ((minElevationTo != UNDEFINED_ANGLE) && (elevationAngleTo < minElevationTo)) {
                linkStatus = Link::MIN_ELEVATION_RECEIVE;
            } else if ((maxElevationFrom != UNDEFINED_ANGLE) && (elevationAngleFrom > maxElevationFrom)) {
                linkStatus = Link::MAX_ELEVATION_TRANSMIT;
            } else if ((minElevationFrom != UNDEFINED_ANGLE) && (elevationAngleFrom < minElevationFrom)) {
                linkStatus = Link::MIN_ELEVATION_TRANSMIT;
            } else if ((minICCLSolarAngle != UNDEFINED_ANGLE) && (solarIntrusionAngle < minICCLSolarAngle)) {
                linkStatus = Link::MIN_SOLAR_INTRUSION;
            } else if (additionalSeconds == 0.0) {
                if ( (ptrToAntenna->IsAnOutageTimeStep(timeIndex, fromDesignator) == true) ||
                        (ptrFromAntenna->IsAnOutageTimeStep(timeIndex, toDesignator) == true) ) {
                    linkStatus = Link::OUTAGE_TIME_STEP;
                } else if (ptrToAntenna->IsNonFullyDedicatedTransmitter(timeIndex, fromDesignator) == true) {
                    linkStatus = Link::FULLY_DEDICATED_TIME_STEP;
                } else if (MissingRequiredDownlink(ptrToRelay, timeIndex) == true) {
                    linkStatus = Link::NO_SUPPORTING_DOWNLINK;
                }
            }

            if (FileNames::HasFileNameDefined(FileNames::LINKS_DEBUGFILE) == true) {
                if (additionalSeconds == 0.0) {
                    TimePiece::PrintIndex(detailedLinksFile, timeIndex);
                    detailedLinksFile << " " << setw(8) << fromDesignator;
                    detailedLinksFile << " " << setw(8) << toDesignator;
                } else {
                    detailedLinksFile << "                         ";
                }

                PrintOutputAngle(detailedLinksFile, azimuthFrom);
                PrintOutputAngle(detailedLinksFile, azimuthTo);
                PrintOutputAngle(detailedLinksFile, elevationAngleFrom);
                PrintOutputAngle(detailedLinksFile, elevationAngleTo);
                detailedLinksFile << " " << setw(9) << setprecision(2)
                                  << UnitData::GetOutputDistance(grazingAltitude);
                detailedLinksFile << " " << setw(10) << setprecision(2)
                                  << UnitData::GetOutputDistance(range);
                PrintOutputAngle(detailedLinksFile, sunAngleFrom);
                PrintOutputAngle(detailedLinksFile, sunAngleTo);
                detailedLinksFile << "  " << TimePiece::GetClockTime(timeIndex, additionalSeconds);
                detailedLinksFile << "  " << Link::GetLinkStatusString(linkStatus) << NEW_LINE;

                if ( (linkStatus != Link::MIN_GRAZING_ALTITUDE) &&
                        (linkStatus != Link::NO_SUPPORTING_DOWNLINK) ) {
                    minimumAzimuthFrom = std::min(minimumAzimuthFrom, azimuthFrom);
                    maximumAzimuthFrom = std::max(maximumAzimuthFrom, azimuthFrom);
                    minimumElevationFrom = std::min(minimumElevationFrom, elevationAngleFrom);
                    maximumElevationFrom = std::max(maximumElevationFrom, elevationAngleFrom);
                    minimumAzimuthTo = std::min(minimumAzimuthTo, azimuthTo);
                    maximumAzimuthTo = std::max(maximumAzimuthTo, azimuthTo);
                    minimumElevationTo = std::min(minimumElevationTo, elevationAngleTo);
                    maximumElevationTo = std::max(maximumElevationTo, elevationAngleTo);
                }
            }

            if (timeIndex < endIndexTime) {
                additionalSeconds += additionalCheckSeconds_s;
            } else {
                additionalSeconds += secondsPerTimeStep;
            }
        }

        (*ptrStatusArray)[timeIndex] = linkStatus;
        ++timeIndex;
    }

    if (FileNames::HasFileNameDefined(FileNames::LINKS_DEBUGFILE) == true) {
        detailedLinksFile << "Minimum Angles :         ";
        PrintOutputAngle(detailedLinksFile, minimumAzimuthFrom);
        PrintOutputAngle(detailedLinksFile, minimumAzimuthTo);
        PrintOutputAngle(detailedLinksFile, minimumElevationFrom);
        PrintOutputAngle(detailedLinksFile, minimumElevationTo);
        detailedLinksFile << NEW_LINE;
        detailedLinksFile << "Maximum Angles :         ";
        PrintOutputAngle(detailedLinksFile, maximumAzimuthFrom);
        PrintOutputAngle(detailedLinksFile, maximumAzimuthTo);
        PrintOutputAngle(detailedLinksFile, maximumElevationFrom);
        PrintOutputAngle(detailedLinksFile, maximumElevationTo);
        detailedLinksFile << NEW_LINE;
    }

    return(ptrStatusArray);
}

void
LinksGenerator::PrintOutputAngle(ofstream &outfile,
                                 double angle) const
{
    if ((angle > -UNDEFINED_ANGLE) && (angle < UNDEFINED_ANGLE)) {
        outfile << " " << setw(7) << setprecision(2)
                << UnitData::GetOutputAngle(angle);
    } else {
        outfile << "        ";
    }
}

void
LinksGenerator::DetermineConjunctionUPA(ofstream &detailedConjFile)
{
    const int                           numberOfSenders = ptrDataProviderCollection_a->GetUserCount();
    shared_ptr<UserVehicle>             ptrSender1 = NULL;
    shared_ptr<UserVehicle>             ptrSender2 = NULL;
    shared_ptr<MURALTypes::UserVector>  ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();
    shared_ptr<MURALTypes::RelayVector> ptrRelayList = ptrDataProviderCollection_a->GetRelayListPtr();
    shared_ptr<RelaySatellite>          ptrReceiver = NULL;
	const string                        upaInputFileName = FileNames::GetFileName(FileNames::UPA_CONJ_INPUTFILE);

    try {
        int sender1Index = 0;
        int sender2Index = 0;
        int simultaneousSenders = 0;
        int maximumSimultaneousSenders = 0;
        MURALTypes::RelayVector::iterator relayIter;

        for( relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
            double previousAngleNB = 0.0;
            double previousAngleWB = 0.0;
            ptrReceiver = (*relayIter);

            if (ptrReceiver->ReadConjunctionData() == true) {
                cout << "LinksGenerator: Retrieving UPA Conjunction For " << ptrReceiver->GetDesignator() << NEW_LINE;
                RetrieveConjunctionPeriods(ptrReceiver, &upaInputFileName);
            } else {
                maximumSimultaneousSenders = ptrReceiver->GetMaximumSimultaneousConjunctionSenders();

                if (maximumSimultaneousSenders > 1) {
                    previousAngleNB;
                    previousAngleWB;
                    simultaneousSenders = 2;

                    while (simultaneousSenders <= maximumSimultaneousSenders) {
                        if (ptrReceiver->HasSimultaneousConjunctionSenders(simultaneousSenders) == true) {
                            cout << "LinksGenerator: Determining UPA Conjunction For " << ptrReceiver->GetDesignator();

                            if (simultaneousSenders > 2) {
                                cout << " For " << simultaneousSenders << " Simultaneous Senders";
                            }

                            cout << NEW_LINE;
                            sender1Index = 0;

                            while (sender1Index < numberOfSenders) {
                                ptrSender1 = ptrUserList->at(sender1Index);
                                sender2Index = sender1Index;

                                if (simultaneousSenders > 2) {
                                    ++sender2Index;
                                }

                                while (sender2Index < numberOfSenders) {
                                    ptrSender2 = ptrUserList->at(sender2Index);
                                    DetermineUPAConjunctionPeriods(ptrReceiver, ptrSender1, ptrSender2,
                                                                   simultaneousSenders, detailedConjFile);
                                    // if sender 2 has a trail vehicle then determine conjunction periods with sender 1
                                    ptrSender2 = ptrSender2->GetTrailVehicle();

                                    if (ptrSender2 != NULL) {
                                        DetermineUPAConjunctionPeriods(ptrReceiver, ptrSender1, ptrSender2,
                                                                       simultaneousSenders, detailedConjFile);
                                    }

                                    ++sender2Index;
                                }

                                // if sender 1 has a trail vehicle then determine it's conjunction periods (same way as lead)
                                ptrSender1 = ptrSender1->GetTrailVehicle();

                                if (ptrSender1 != NULL) {
                                    sender2Index = sender1Index;

                                    if (simultaneousSenders > 2) {
                                        ++sender2Index;
                                    }

                                    while (sender2Index < numberOfSenders) {
                                        ptrSender2 = ptrUserList->at(sender2Index);
                                        DetermineUPAConjunctionPeriods(ptrReceiver, ptrSender1, ptrSender2,
                                                                       simultaneousSenders, detailedConjFile);
                                        // if sender 2 has a trail vehicle then determine conjunction periods with sender 1
                                        ptrSender2 = ptrSender2->GetTrailVehicle();

                                        if (ptrSender2 != NULL) {
                                            DetermineUPAConjunctionPeriods(ptrReceiver, ptrSender1, ptrSender2,
                                                                           simultaneousSenders, detailedConjFile);
                                        }

                                        ++sender2Index;
                                    }
                                }

                                ++sender1Index;
                            }
                        }

                        ++simultaneousSenders;
                    }
                }
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "DetermineConjunctionUPA(ofstream&)");
        throw;
    }

    return;
}

void
LinksGenerator::DetermineUPAConjunctionPeriods(const shared_ptr<RelaySatellite> &ptrReceiver,
        const shared_ptr<UserVehicle> &ptrSender1,
        const shared_ptr<UserVehicle> &ptrSender2,
        int simultaneousSenders,
        ofstream &detailedConjFile)
{
    try {
        if ( (Antenna::IsConjunctionFrequency(ptrSender1->GetTransmitFrequencyBand()) == true) &&
                (Antenna::IsConjunctionFrequency(ptrSender2->GetTransmitFrequencyBand()) == true) ) {
            double previousAngleNB = -1.0;
            double previousAngleWB = -1.0;

            if (simultaneousSenders > 2) {
                double previousAngle = 0.0;
                previousAngle = ptrReceiver->GetNarrowbandConjunctionAngle(
                                    ptrSender1->GetDesignator(),
                                    ptrSender2->GetDesignator(),
                                    (simultaneousSenders - 1)
                                );

                if (previousAngle != UNDEFINED_ANGLE) {
                    previousAngleNB = previousAngle;
                }

                previousAngle = ptrReceiver->GetWidebandConjunctionAngle(
                                    ptrSender1->GetDesignator(),
                                    ptrSender2->GetDesignator(),
                                    (simultaneousSenders - 1)
                                );

                if (previousAngle != UNDEFINED_ANGLE) {
                    previousAngleWB = previousAngle;
                }
            }

            DetermineConjunction(ptrReceiver, ptrSender1, ptrSender2, simultaneousSenders,
                                 previousAngleNB, previousAngleWB, detailedConjFile);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "DetermineUPAConjunctionPeriods(RelaySatellite*, UserVehicle*, UserVehicle*, ...)");
        throw;
    }
}

void
LinksGenerator::RetrieveConjunctionPeriods(shared_ptr<DataProvider> ptrReceiver,
        const string *ptrConjunctionFileName)
{
    string            inputString = "";
    string            receiverDesignator = "";
    string            sender1Designator = "";
    string            sender2Designator = "";
    string            conjunctionTypeString = "";
    const string      currentReceiverDesignator = ptrReceiver->GetDesignator();
    ifstream          inputFile;
    ConjunctionPeriod::CONJUNCTION_TYPE type = ConjunctionPeriod::UNKNOWN;

    try {
        int simultaneousSenders = 0;
        int startTimeIndex = 0;
        int endTimeIndex = 0;
        double startTime = 0.0;
        double endTime = 0.0;
        ParseUtility::OpenInputFile(inputFile, ptrConjunctionFileName);
        // get the header line first
        getline(inputFile, inputString, NEW_LINE);

        while (getline(inputFile, inputString, NEW_LINE)) {
            istringstream inputStream(inputString);
            inputStream >> receiverDesignator;

            if (receiverDesignator == currentReceiverDesignator) {
                inputStream >> sender1Designator >> sender2Designator;
                inputStream >> startTime >> endTime >> simultaneousSenders >> conjunctionTypeString;

                if (conjunctionTypeString == "WB") {
                    type = ConjunctionPeriod::WB;
                } else if (conjunctionTypeString == "NB") {
                    type = ConjunctionPeriod::NB;
                } else {
                    type = ConjunctionPeriod::UNKNOWN;
                }

                if ( (MURALInputProcessor::ValidTransmitter(sender1Designator) == true) &&
                        (MURALInputProcessor::ValidTransmitter(sender2Designator) == true) &&
                        (TimePiece::ConvertToStoreTimeIndex(startTime, startTimeIndex) == true) &&
                        (TimePiece::ConvertToStoreTimeIndex(endTime, endTimeIndex) == true) &&
                        (type != ConjunctionPeriod::UNKNOWN) &&
                        (simultaneousSenders > 1) ) {
                    ptrReceiver->SetConjunctionPeriod(startTimeIndex, endTimeIndex, simultaneousSenders,
                                                      currentReceiverDesignator, sender1Designator,
                                                      sender2Designator, type);
                }
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "RetrieveConjunctionPeriods(DataProvider*)");
        throw;
    }

    return;
}

void
LinksGenerator::DetermineConjunctionRF(ofstream &detailedConjFile)
{
    const int                             numberOfRSSenders = ptrDataProviderCollection_a->GetRelayCount();
    const int                             numberOfDDUSenders = ptrDataProviderCollection_a->GetUserCount();
    const int                             totalNumberOfSenders = numberOfRSSenders + numberOfDDUSenders;
    shared_ptr<MURALTypes::UserVector>    ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();
    shared_ptr<MURALTypes::RelayVector>   ptrRelayList = ptrDataProviderCollection_a->GetRelayListPtr();
    shared_ptr<MURALTypes::GroundVector>  ptrGroundList = ptrDataProviderCollection_a->GetGroundListPtr();
    shared_ptr<RelaySatellite>            ptrRSSender1  = NULL;
    shared_ptr<RelaySatellite>            ptrRSSender2  = NULL;
    shared_ptr<UserVehicle>               ptrDDUSender1 = NULL;
    shared_ptr<UserVehicle>               ptrDDUSender2 = NULL;
    shared_ptr<GroundStation>             ptrReceiver = NULL;
	const string                          rfInputFileName = FileNames::GetFileName(FileNames::RF_CONJ_INPUTFILE);

    // Must Determine RF Conjuction between all the following combinations:
    // a) RS1 & RS
    // b) RS & DDU
    // c) DDU & RS
    // d) DDU & DDU
    try {
        int sender1Index = 0;
        int sender2Index = 0;
        int simultaneousSenders = 0;
        int maximumSimultaneousSenders = 0;
        double previousAngle = 0.0;
        double previousAngleNB = 0.0;
        double previousAngleWB = 0.0;
        MURALTypes::GroundVector::iterator groundIter;

        for( groundIter = ptrGroundList->begin(); groundIter != ptrGroundList->end(); ++groundIter) {
            ptrReceiver = (*groundIter);

            if (ptrReceiver->ReadConjunctionData() == true) {
                cout << "LinksGenerator: Retrieving RF Conjunction For " << ptrReceiver->GetDesignator() << NEW_LINE;
                RetrieveConjunctionPeriods(ptrReceiver, &rfInputFileName);
            } else {
                maximumSimultaneousSenders = ptrReceiver->GetMaximumSimultaneousConjunctionSenders();

                if (maximumSimultaneousSenders > 1) {
                    previousAngleNB = -1.0;
                    previousAngleWB = -1.0;
                    simultaneousSenders = 2;

                    while (simultaneousSenders <= maximumSimultaneousSenders) {
                        if (ptrReceiver->HasSimultaneousConjunctionSenders(simultaneousSenders) == true) {
                            cout << "LinksGenerator: Determining RF Conjunction For " << ptrReceiver->GetDesignator();

                            if (simultaneousSenders > 2) {
                                cout << " For " << simultaneousSenders << " Simultaneous Senders";
                            }

                            cout << NEW_LINE;
                            sender1Index = 0;

                            while (sender1Index < totalNumberOfSenders) {
                                if (sender1Index < numberOfRSSenders) {
                                    ptrRSSender1 = ptrRelayList->at(sender1Index);

                                    if (ptrRSSender1->HasConjunctionGPA() == true) {
                                        sender2Index = (sender1Index + 1);

                                        while (sender2Index < totalNumberOfSenders) {
                                            if (sender2Index < numberOfRSSenders) {
                                                ptrRSSender2 = ptrRelayList->at(sender2Index);

                                                if (ptrRSSender2->HasConjunctionGPA() == true) {
                                                    DetermineConjunction(ptrReceiver, ptrRSSender1, ptrRSSender2, simultaneousSenders,
                                                                         previousAngleNB, previousAngleWB, detailedConjFile);
                                                }
                                            } else {
                                                ptrDDUSender2 = ptrUserList->at(sender2Index - numberOfRSSenders);

                                                if ( (ptrDDUSender2->IsADirectDownlinkVehicle() == true) &&
                                                        (Antenna::IsConjunctionFrequency(ptrDDUSender2->GetDirectDownlinkFrequencyBand()) == true) ) {
                                                    DetermineConjunction(ptrReceiver, ptrRSSender1, ptrDDUSender2, simultaneousSenders,
                                                                         previousAngleNB, previousAngleWB, detailedConjFile);
                                                }
                                            }

                                            ++sender2Index;
                                        }
                                    }
                                } else {
                                    ptrDDUSender1 = ptrUserList->at(sender1Index - numberOfRSSenders);

                                    if ( (ptrDDUSender1->IsADirectDownlinkVehicle() == true) &&
                                            (Antenna::IsConjunctionFrequency(ptrDDUSender1->GetDirectDownlinkFrequencyBand()) == true) ) {
                                        sender2Index = (sender1Index + 1);

                                        while (sender2Index < totalNumberOfSenders) {
                                            if (sender2Index < numberOfRSSenders) {
                                                ptrRSSender2 = ptrRelayList->at(sender2Index);

                                                if (ptrRSSender2->HasConjunctionGPA() == true) {
                                                    DetermineConjunction(ptrReceiver, ptrDDUSender1, ptrRSSender2, simultaneousSenders,
                                                                         previousAngleNB, previousAngleWB, detailedConjFile);
                                                }
                                            } else {
                                                ptrDDUSender2 = ptrUserList->at(sender2Index - numberOfRSSenders);

                                                if ( (ptrDDUSender2->IsADirectDownlinkVehicle() == true) &&
                                                        (Antenna::IsConjunctionFrequency(ptrDDUSender2->GetDirectDownlinkFrequencyBand()) == true) ) {
                                                    DetermineConjunction(ptrReceiver, ptrDDUSender1, ptrDDUSender2, simultaneousSenders,
                                                                         previousAngleNB, previousAngleWB, detailedConjFile);
                                                }
                                            }

                                            ++sender2Index;
                                        }
                                    }
                                }

                                ++sender1Index;
                            }

                            if ((previousAngle = ptrReceiver->GetNarrowbandConjunctionAngle(DEFAULT_STRING,
                                                 DEFAULT_STRING, simultaneousSenders)) != UNDEFINED_ANGLE) {
                                previousAngleNB = previousAngle;
                            }

                            if ((previousAngle = ptrReceiver->GetWidebandConjunctionAngle(DEFAULT_STRING,
                                                 DEFAULT_STRING, simultaneousSenders)) != UNDEFINED_ANGLE) {
                                previousAngleWB = previousAngle;
                            }
                        }

                        ++simultaneousSenders;
                    }
                }
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "DetermineConjunctionRF(ofstream&)");
        throw;
    }

    return;
}

void
LinksGenerator::DetermineConjunction(shared_ptr<DataProvider> ptrReceiver,
                                     shared_ptr<DataProvider> ptrSender1,
                                     shared_ptr<DataProvider> ptrSender2,
                                     int simultaneousSenders,
                                     double previousAngleNB,
                                     double previousAngleWB,
                                     ofstream &detailedConjFile)
{
    const int     numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    const int     endIndexTime = TimePiece::GetEndIndexTime();
    const double   secondsPerTimeStep = (double)TimePiece::GetSecondsPerTimeStep();
    Array<bool>   *ptrConjunctionTimeStepNB = new Array<bool>(numberOfTimeSteps, false);
    Array<bool>   *ptrConjunctionTimeStepWB = new Array<bool>(numberOfTimeSteps, false);
    const string   receiverDesignator = ptrReceiver->GetDesignator();
    const string   sender1Designator = ptrSender1->GetDesignator();
    const string   sender2Designator = ptrSender2->GetDesignator();
    SpaceVector    receiverPosition;
    SpaceVector    receiverToSender1;
    SpaceVector    receiverToSender2;

    try {
        double conjunctionAngleNB1;
        double conjunctionAngleNB2;
        double conjunctionAngleWB1;
        double conjunctionAngleWB2;
        conjunctionAngleNB1 = ptrReceiver->GetNarrowbandConjunctionAngle(sender1Designator,
                              sender2Designator,
                              simultaneousSenders);
        conjunctionAngleNB2 = ptrReceiver->GetNarrowbandConjunctionAngle(sender2Designator,
                              sender1Designator,
                              simultaneousSenders);
        conjunctionAngleWB1 = ptrReceiver->GetWidebandConjunctionAngle(sender1Designator,
                              sender2Designator,
                              simultaneousSenders);
        conjunctionAngleWB2 = ptrReceiver->GetWidebandConjunctionAngle(sender2Designator,
                              sender1Designator,
                              simultaneousSenders);

        if ( ((conjunctionAngleNB1 > 0.0) && (conjunctionAngleNB1 != UNDEFINED_ANGLE)) ||
                ((conjunctionAngleNB2 > 0.0) && (conjunctionAngleNB2 != UNDEFINED_ANGLE)) ||
                ((conjunctionAngleWB1 > 0.0) && (conjunctionAngleWB1 != UNDEFINED_ANGLE)) ||
                ((conjunctionAngleWB2 > 0.0) && (conjunctionAngleWB2 != UNDEFINED_ANGLE)) ) {
            int timeIndex = 0;
            double conjunctionAngle = 0.0;
            double differenceAngle = 0.0;
            double additionalSeconds = 0.0;

            while (timeIndex < numberOfTimeSteps) {
                if (ptrReceiver->ShouldCheckConjunction(sender1Designator, sender2Designator, timeIndex) == true) {
                    if ( ((conjunctionAngleNB1 > 0.0) && (conjunctionAngleNB1 != UNDEFINED_ANGLE)) ||
                            ((conjunctionAngleNB2 > 0.0) && (conjunctionAngleNB2 != UNDEFINED_ANGLE)) ) {
                        additionalSeconds = 0.0;

                        while ( (additionalSeconds < secondsPerTimeStep) &&
                                (ptrConjunctionTimeStepNB->ElementAt(timeIndex) == false) ) {
                            receiverPosition = ptrReceiver->GetInterimPosition(timeIndex, additionalSeconds);
                            receiverToSender1 = ptrSender1->GetInterimPosition(timeIndex, additionalSeconds) - receiverPosition;
                            receiverToSender2 = ptrSender2->GetInterimPosition(timeIndex, additionalSeconds) - receiverPosition;
                            differenceAngle = receiverToSender1.GetSeparationAngle(receiverToSender2);

                            if (receiverToSender1.GetMagnitude() <= receiverToSender2.GetMagnitude()) {
                                conjunctionAngle = conjunctionAngleNB1;
                            } else {
                                conjunctionAngle = conjunctionAngleNB2;
                            }

                            // check for narrowband conjunction problem
                            if ( (conjunctionAngle != UNDEFINED_ANGLE) &&
                                    (differenceAngle > previousAngleNB) &&
                                    (differenceAngle <= conjunctionAngle) ) {
                                (*ptrConjunctionTimeStepNB)[timeIndex] = true;

                                if (FileNames::HasFileNameDefined(FileNames::CONJ_DEBUGFILE) == true) {
                                    TimePiece::PrintIndex(detailedConjFile, timeIndex);
                                    detailedConjFile << " " << setw(8) << receiverDesignator;
                                    detailedConjFile << " " << setw(8) << sender1Designator;
                                    detailedConjFile << " " << setw(8) << sender2Designator << "    NB";
                                    detailedConjFile << " " << setw(7) << setprecision(2)
                                                     << UnitData::GetOutputAngle(differenceAngle);
                                    detailedConjFile << " " << setw(7) << setprecision(2)
                                                     << UnitData::GetOutputAngle(conjunctionAngle);
                                    detailedConjFile << "   " << TimePiece::GetClockTime(timeIndex, additionalSeconds) << NEW_LINE;
                                }
                            }

                            if (timeIndex < endIndexTime) {
                                additionalSeconds += additionalCheckSeconds_s;
                            } else {
                                additionalSeconds = secondsPerTimeStep;
                            }
                        }
                    }

                    if ( ((conjunctionAngleWB1 > 0.0) && (conjunctionAngleWB1 != UNDEFINED_ANGLE)) ||
                            ((conjunctionAngleWB2 > 0.0) && (conjunctionAngleWB2 != UNDEFINED_ANGLE)) ) {
                        additionalSeconds = 0.0;

                        while ( (additionalSeconds < secondsPerTimeStep) &&
                                (ptrConjunctionTimeStepWB->ElementAt(timeIndex) == false) ) {
                            receiverPosition = ptrReceiver->GetInterimPosition(timeIndex, additionalSeconds);
                            receiverToSender1 = ptrSender1->GetInterimPosition(timeIndex, additionalSeconds) - receiverPosition;
                            receiverToSender2 = ptrSender2->GetInterimPosition(timeIndex, additionalSeconds) - receiverPosition;
                            differenceAngle = receiverToSender1.GetSeparationAngle(receiverToSender2);

                            if (receiverToSender1.GetMagnitude() <= receiverToSender2.GetMagnitude()) {
                                conjunctionAngle = conjunctionAngleWB1;
                            } else {
                                conjunctionAngle = conjunctionAngleWB2;
                            }

                            // check for wideband conjunction problem
                            if ( (conjunctionAngle != UNDEFINED_ANGLE) &&
                                    (differenceAngle > previousAngleWB) &&
                                    (differenceAngle <= conjunctionAngle) ) {
                                (*ptrConjunctionTimeStepWB)[timeIndex] = true;

                                if (FileNames::HasFileNameDefined(FileNames::CONJ_DEBUGFILE) == true) {
                                    TimePiece::PrintIndex(detailedConjFile, timeIndex);
                                    detailedConjFile << " " << setw(8) << receiverDesignator;
                                    detailedConjFile << " " << setw(8) << sender1Designator;
                                    detailedConjFile << " " << setw(8) << sender2Designator << "    WB";
                                    detailedConjFile << " " << setw(7) << setprecision(2)
                                                     << UnitData::GetOutputAngle(differenceAngle);
                                    detailedConjFile << " " << setw(7) << setprecision(2)
                                                     << UnitData::GetOutputAngle(conjunctionAngle);
                                    detailedConjFile << "   " << TimePiece::GetClockTime(timeIndex, additionalSeconds) << NEW_LINE;
                                }
                            }

                            if (timeIndex < endIndexTime) {
                                additionalSeconds += additionalCheckSeconds_s;
                            } else {
                                additionalSeconds = secondsPerTimeStep;
                            }
                        }
                    }
                }

                ++timeIndex;
            }

            if (ptrConjunctionTimeStepNB->ValueExists(true) == true) {
                ptrReceiver->SetConjunctionPeriods(ptrConjunctionTimeStepNB, simultaneousSenders,
                                                   sender1Designator, sender2Designator,
                                                   ConjunctionPeriod::NB);
            }

            delete ptrConjunctionTimeStepNB;
            ptrConjunctionTimeStepNB = NULL;

            if (ptrConjunctionTimeStepWB->ValueExists(true) == true) {
                ptrReceiver->SetConjunctionPeriods(ptrConjunctionTimeStepWB, simultaneousSenders,
                                                   sender1Designator, sender2Designator,
                                                   ConjunctionPeriod::WB);
            }

            delete ptrConjunctionTimeStepWB;
            ptrConjunctionTimeStepWB = NULL;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "DetermineConjunction(DataProvider*, DataProvider*, DataProvider*, ...)");
        throw;
    }

    return;
}

double
LinksGenerator::CalculateGrazingAltitude(const SpaceVector &fromVehiclePosition,
        double elevationAngleFrom,
        const SpaceVector &toVehiclePosition,
        double elevationAngleTo,
        double quarterCircle) const
{
    double       grazingAltitude = 0.0;
    const double fromMagnitude = fromVehiclePosition.GetMagnitude();
    const double toMagnitude = toVehiclePosition.GetMagnitude();

    if (fromMagnitude <= toMagnitude) {
        if (elevationAngleFrom < 0.0) {
            grazingAltitude = fabs((fromMagnitude * sin(elevationAngleFrom + quarterCircle)));
        } else {
            grazingAltitude = fromMagnitude;
        }
    } else if (elevationAngleTo < 0.0) {
        grazingAltitude = fabs((toMagnitude * sin(elevationAngleTo + quarterCircle)));
    } else {
        grazingAltitude = toMagnitude;
    }

    grazingAltitude -= Earth::GetStoreRadius();
    return(grazingAltitude);
}

bool
LinksGenerator::CheckAdditionalSeconds()
{
    return(additionalCheckSeconds_s < INVALID_SECONDS);
}

bool
LinksGenerator::MissingRequiredDownlink(const shared_ptr<RelaySatellite> &ptrToRelay,
                                        int timeIndex) const
{
    bool           missingRequiredDownlink = false;
    Array<string> *ptrCheckedRelayArray = NULL;

    if (ptrToRelay != NULL) {
        ptrCheckedRelayArray = new Array<string>();

        if (ptrToRelay->HasAnyResouceInView(timeIndex, ptrCheckedRelayArray) == false) {
            missingRequiredDownlink = true;
        }

        delete ptrCheckedRelayArray;
        ptrCheckedRelayArray = NULL;
    }

    return(missingRequiredDownlink);
}

LinksGenerator::~LinksGenerator()
{
    return;
}

