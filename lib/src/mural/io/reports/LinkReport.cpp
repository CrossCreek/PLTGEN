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
 * LinkReport.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "LinkReport.h"
#include "mural/element/UserVehicle.h"
#include "mural/element/RelaySatellite.h"
#include "mural/antenna/UserTransmitAntenna.h"
#include "mural/antenna/RelayPointingAntenna.h"
#include "mural/antenna/GroundPointingAntenna.h"
#include "mural/modules/lg/Link.h"


#include "general/array/Array.h"
#include "general/array/ListIterator.h"
#include "general/utility/StringEditor.h"
#include "general/data/UnitData.h"
#include "general/time/TimePiece.h"
#include "general/parse/ParseUtility.h"

LinkReport::LinkReport(const std::string &outFile ) :
    outputFileName_a(outFile)
{
    return;
}

/* ************************************************************************** */

void
LinkReport::CreateCrosslinkReport(const shared_ptr<MURALTypes::UserVector> &ptrUserList)
{
    bool                 openedFile = false;
    shared_ptr<UserTransmitAntenna> ptrTransmitAntenna = NULL;
    shared_ptr<UserVehicle>    ptrUserVehicle = NULL;
    shared_ptr<UserVehicle>    ptrTrailVehicle = NULL;
    MURALTypes::UserVector::iterator userIter;

    for( userIter = ptrUserList->begin(); userIter != ptrUserList->end(); ++userIter) {
        ptrUserVehicle = (*userIter);
        ptrTransmitAntenna = ptrUserVehicle->GetUserTransmitAntenna();

        if (ptrTransmitAntenna != NULL) {
            if (openedFile == false) {
                ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
                outfileStream_a << "    USER      UTA    RELAY      UPA    TIME LINK" << NEW_LINE;
                openedFile = true;
            }

            PrintLinks(ptrTransmitAntenna);
        }

        ptrTrailVehicle = ptrUserVehicle->GetTrailVehicle();

        if (ptrTrailVehicle != NULL) {
            ptrTransmitAntenna = ptrTrailVehicle->GetUserTransmitAntenna();

            if (ptrTransmitAntenna != NULL) {
                if (openedFile == false) {
                    ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
                    outfileStream_a << "    USER      UTA    RELAY      UPA    TIME LINK" << NEW_LINE;
                    openedFile = true;
                }

                PrintLinks(ptrTransmitAntenna);
            }
        }
    }

    if (openedFile == true) {
        outfileStream_a.close();
    }
}

/* *********************************************************************************** */

void
LinkReport::CreateDirectDownlinkReport(const shared_ptr<MURALTypes::UserVector> &ptrUserList)
{
    bool         openedFile = false;
    ofstream     outfile;
    shared_ptr<UserVehicle> ptrUser = NULL;
    shared_ptr<UserVehicle> ptrTrailVehicle = NULL;
    MURALTypes::UserVector::iterator userIter;

    for( userIter = ptrUserList->begin(); userIter != ptrUserList->end(); ++userIter) {
        ptrUser = (*userIter);

        if (ptrUser->IsADirectDownlinkVehicle() == true) {
            if (openedFile == false) {
                ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
                outfileStream_a << "    USER      UTA   GROUND      RFA    TIME LINK" << NEW_LINE;
                openedFile = true;
            }

            PrintLinks(ptrUser->GetDirectDownlinkAntenna());
        }

        ptrTrailVehicle = ptrUser->GetTrailVehicle();

        if (ptrTrailVehicle != NULL) {
            if (ptrTrailVehicle->IsADirectDownlinkVehicle() == true) {
                if (openedFile == false) {
                    ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
                    outfileStream_a << "    USER      UTA   GROUND      RFA    TIME LINK" << NEW_LINE;
                    openedFile = true;
                }

                PrintLinks(ptrTrailVehicle->GetDirectDownlinkAntenna());
            }
        }
    }

    if (openedFile == true) {
        outfile.close();
    }

    return;
}

/* *********************************************************************************** */

void
LinkReport::CreateDownlinkReport(const shared_ptr<MURALTypes::RelayVector> &ptrRelayList)
{
    bool                                openedFile = false;
    shared_ptr<RelaySatellite>          ptrRelay = NULL;
    shared_ptr<GroundPointingAntenna>   ptrGPA = NULL;
    shared_ptr<MURALTypes::GPAVector>   ptrGPAList = NULL;
    MURALTypes::GPAVector::iterator     gpaIter;
    MURALTypes::RelayVector::iterator relayIter;

    for( relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
        ptrRelay = (*relayIter);
        ptrGPAList = ptrRelay->GetGroundPointingAntennaList();

        for(gpaIter = ptrGPAList->begin(); gpaIter != ptrGPAList->end(); ++gpaIter) {
            ptrGPA = (*gpaIter);

            if (openedFile == false) {
                ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
                outfileStream_a << "   RELAY      GPA   GROUND      RFA    TIME LINK" << NEW_LINE;
                openedFile = true;
            }

            PrintLinks(ptrGPA);
        }
    }

    if (openedFile == true) {
        outfileStream_a.close();
    }
}

/* *********************************************************************************** */

void
LinkReport::CreateRelayMultiHopReport(const shared_ptr<MURALTypes::RelayVector> &ptrRelayList)
{
    bool                              openedFile = false;
    ofstream                          outfile;
    shared_ptr<RelaySatellite>        ptrRelay = NULL;
    shared_ptr<RelayPointingAntenna>  ptrRPA = NULL;
    shared_ptr<MURALTypes::RPAVector> ptrRPAList = NULL;
    MURALTypes::RPAVector::iterator   rpaIter;
    MURALTypes::RelayVector::iterator relayIter;

    for( relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
        ptrRelay = (*relayIter);
        ptrRPAList = ptrRelay->GetRelayPointingAntennaList();

        for(rpaIter = ptrRPAList->begin(); rpaIter != ptrRPAList->end(); ++rpaIter) {
            ptrRPA = (*rpaIter);

            if (ptrRPA->IsTransmittingAntenna() == true) {
                if (openedFile == false) {
                    ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
                    outfileStream_a << "  TRAN_RELAY  RPA   RECV_RELAY  RPA    TIME LINK" << NEW_LINE;
                    openedFile = true;
                }

                PrintLinks(ptrRPA);
            }
        }
    }

    if (openedFile == true) {
        outfile.close();
    }
}

/* *********************************************************************************** */

void
LinkReport::CreateUserSelfRelayReport(const shared_ptr<MURALTypes::UserVector> &ptrUserList)
{
    bool                              openedFile = false;
    ofstream                          outfile;
    shared_ptr<SelfRelayAntenna>      ptrSRA = NULL;
    shared_ptr<UserVehicle>           ptrUser = NULL;
    shared_ptr<UserVehicle>           ptrTrailVehicle = NULL;
    shared_ptr<MURALTypes::SRAVector> ptrSRAList = NULL;
    MURALTypes::SRAVector::iterator   sraIter;
    MURALTypes::UserVector::iterator userIter;

    for( userIter = ptrUserList->begin(); userIter != ptrUserList->end(); ++userIter) {
        ptrUser = (*userIter);
        ptrSRAList = ptrUser->GetSelfRelayAntennaList();

        for( sraIter = ptrSRAList->begin(); sraIter != ptrSRAList->end(); ++sraIter) {
            ptrSRA = (*sraIter);

            if (ptrSRA->IsTransmittingAntenna() == true) {
                if (openedFile == false) {
                    ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
                    outfileStream_a << "   TRAN_USER  SRA    RECV_USER  SRA    TIME LINK" << NEW_LINE;
                    openedFile = true;
                }

                PrintLinks(ptrSRA);
            }
        }

        ptrTrailVehicle = ptrUser->GetTrailVehicle();

        if (ptrTrailVehicle != NULL) {
            ptrSRAList = ptrTrailVehicle->GetSelfRelayAntennaList();

            for( sraIter = ptrSRAList->begin(); sraIter != ptrSRAList->end(); ++sraIter) {
                ptrSRA = (*sraIter);

                if (ptrSRA->IsTransmittingAntenna() == true) {
                    if (openedFile == false) {
                        ParseUtility::OpenOutputFile(outfileStream_a, outputFileName_a);
                        outfile << "   TRAN_USER  SRA    RECV_USER  SRA    TIME LINK" << NEW_LINE;
                        openedFile = true;
                    }

                    PrintLinks(ptrSRA);
                }
            }
        }
    }

    if (openedFile == true) {
        outfile.close();
    }

    return;
}

/* ************************************************************************************* */

void
LinkReport::PrintLinks(const shared_ptr<Antenna> &antenna)
{
    int                   timeIndex = 0;
    const int             numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    string                fromDesignator = "";
    string                fromAntennaDesignator = "";
    string                toDesignator = "";
    string                toAntennaDesignator = "";
    const string          providerDesignator = antenna->GetDataProviderDesignator();
    const LinkList<Link> *ptrLinksList = antenna->GetLinksList();
    ListIterator<Link>    linkIter(ptrLinksList);
    Link                  *ptrLink = linkIter.First();

    while (ptrLink != NULL) {
        fromDesignator = ptrLink->GetTransmitDesignator();

        if (fromDesignator == providerDesignator) {
            fromAntennaDesignator = ptrLink->GetTransmitAntennaDesignator();
            toDesignator = ptrLink->GetReceiveDesignator();
            toAntennaDesignator = ptrLink->GetReceiveAntennaDesignator();
            timeIndex = 0;

            while (timeIndex < numberOfTimeSteps) {
                outfileStream_a << setw(8) << fromDesignator << " " << setw(8) << fromAntennaDesignator << " ";
                outfileStream_a << setw(8) << toDesignator << " " << setw(8) << toAntennaDesignator << " ";
                TimePiece::PrintIndex(outfileStream_a, timeIndex);
                outfileStream_a << " " << setw(4) << ptrLink->GetLinkStatusNumber(timeIndex) << NEW_LINE;
                ++timeIndex;
            }
        }

        ptrLink = linkIter.Next();
    }

    return;
}

/* ********************************************************************************************** */

LinkReport::~LinkReport()
{
    // Do nothing
}
