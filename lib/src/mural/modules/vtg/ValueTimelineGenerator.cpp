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
 * ValueTimelineGenerator.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include "ValueTimelineGenerator.h"
#include "mural/element/UserVehicle.h"
#include "mural/modules/lg/Link.h"
#include "mural/Region.h"
#include "mural/io/FileNames.h"

#include "general/math/SimpleMath.h"

bool ValueTimelineGenerator::resetCrosslinkStatus_s = false;

ValueTimelineGenerator::ValueTimelineGenerator()
    : TimelineGenerator()
{
    return;
}

ValueTimelineGenerator::ValueTimelineGenerator(const ValueTimelineGenerator &copyMe)
    : TimelineGenerator(copyMe)
{
    return;
}

ValueTimelineGenerator&
ValueTimelineGenerator::operator = (const ValueTimelineGenerator &copyMe)
{
    TimelineGenerator::operator = (copyMe);
    return(*this);
}

bool
ValueTimelineGenerator::operator == (const ValueTimelineGenerator &compareMe) const
{
    return(TimelineGenerator::operator == (compareMe));
}

bool
ValueTimelineGenerator::operator != (const ValueTimelineGenerator &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
ValueTimelineGenerator::GenerateValueTimelines(const shared_ptr<DataProviderCollection> &ptrDataProviderCollection)
{
    try {
        GenerateTimelines(ptrDataProviderCollection);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GenerateValueTimelines(LinkList<DataProvider>*)");
        throw;
    }

    return;
}

double
ValueTimelineGenerator::FactorWhenPointingAtRegion(Link *ptrTransmitLink,
        const shared_ptr<UserVehicle> &ptrUserVehicle,
        Region *ptrRegion,
        int timeIndex,
        double regionScore,
        ofstream &valtLogFile,
        double azimuthOffsetAngle,
        double cableWrapOffsetAngle)
{
    double regionFactor = 0.0;
    string userDesignator = "";
    string resourceDesignator = "";

    if ( (ptrTransmitLink->IsInview(timeIndex) == true) ||
            (ptrTransmitLink->TransmitElevationLinkProblem(timeIndex) == true) ) {
        if (TimelineGenerator::LinkWhenPointingAtRegion(ptrTransmitLink, ptrUserVehicle, timeIndex,
                ptrRegion->GetRegionNumber(),
                azimuthOffsetAngle, cableWrapOffsetAngle) == true) {
            regionFactor = 1.0;

            if (ptrTransmitLink->TransmitElevationLinkProblem(timeIndex) == true) {
                ptrTransmitLink->SetLinkStatus(timeIndex, Link::LINK);
                resetCrosslinkStatus_s = true;
            }
        }

        if (FileNames::HasFileNameDefined(FileNames::VALT_DEBUGFILE) == true) {
            userDesignator = ptrUserVehicle->GetDesignator();
            resourceDesignator = ptrTransmitLink->GetReceiveDesignator();
            valtLogFile << setw(8) << userDesignator << " ";
            TimePiece::PrintIndex(valtLogFile, timeIndex);
            valtLogFile << "      ";
            ptrRegion->PrintFixedWidthRegionNumber(valtLogFile);
            valtLogFile << "  ";
            valtLogFile << setw(8) << resourceDesignator << " ";

            if (regionFactor == 0.0) {
                valtLogFile << "                                                 ";
                valtLogFile << "               ZERO_FACTOR" << NEW_LINE;
            } else if (regionFactor == 1.0) {
                valtLogFile << "                                                 ";
                valtLogFile << " " << setw(12) << setprecision(3) << regionScore;
                valtLogFile << "  NO_FACTOR" << NEW_LINE;
            } else {
                valtLogFile << "   Region Factor: " << setw(6) << setprecision(4) << regionFactor;
                valtLogFile << "                         ";
                valtLogFile << " " << setw(12) << setprecision(3) << (regionScore * regionFactor);
                valtLogFile << "  PARTIAL_FACTOR" << NEW_LINE;
            }
        }
    }

    return(regionFactor);
}

ValueTimelineGenerator::~ValueTimelineGenerator()
{
    return;
}

