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
 * Trek.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "Trek.h"

#include "general/parse/ParseUtility.h"
#include "general/array/ListIterator.h"
#include "general/time/TimePiece.h"

Trek::Trek()
    : Journey(), ptrTrekFilename_a(NULL), ptrTrekNodeList_a(NULL), initialLayover_a(-1),
      latitude_a(UNDEFINED_ANGLE), longitude_a(UNDEFINED_ANGLE), altitude_a(UNDEFINED_ANGLE)
{
    return;
}

Trek::Trek(const Trek &copyMe)
    : Journey(copyMe), ptrTrekFilename_a(NULL), ptrTrekNodeList_a(NULL)
{
    if (copyMe.ptrTrekNodeList_a != NULL) {
        ptrTrekNodeList_a = new LinkList<TrekNode>(*(copyMe.ptrTrekNodeList_a));
    }

    ptrTrekFilename_a = copyMe.ptrTrekFilename_a;
    initialLayover_a = copyMe.initialLayover_a;
    latitude_a = copyMe.latitude_a;
    longitude_a = copyMe.longitude_a;
    altitude_a = copyMe.altitude_a;
    return;
}

Trek&
Trek::operator = (const Trek &copyMe)
{
    Journey::operator = (copyMe);
    ptrTrekFilename_a = copyMe.ptrTrekFilename_a;

    if (copyMe.ptrTrekNodeList_a != NULL) {
        ptrTrekNodeList_a = new LinkList<TrekNode>(*(copyMe.ptrTrekNodeList_a));
    }

    return(*this);
}

bool
Trek::operator == (const Trek &compareMe)
{
    return( (Journey::operator == (compareMe)) &&
            (*ptrTrekFilename_a == *(compareMe.ptrTrekFilename_a)) &&
            (*ptrTrekNodeList_a == *(compareMe.ptrTrekNodeList_a)) );
}

void
Trek::PrintAttributes(ostream &outfile) const
{
    TrekNode *ptrFirstTrekNode = GetFirstTrekNode();
    outfile << "   <POSITION_GENERATION_BEGIN>" << NEW_LINE;
    outfile << "      Orbiting Data Provider                                  : false";
    outfile << NEW_LINE;
    outfile << "      Position Generation Method (READ/STATIONARY/MOVING)     : ";
    outfile << GetGenerationMethodString(positionGenerationMethod_a) << NEW_LINE;
    outfile << "      Latitude                                                : ";
    outfile << UnitData::GetInputAngle(ptrFirstTrekNode->GetLatitude()) << NEW_LINE;
    outfile << "      Longitude                                               : ";
    outfile << UnitData::GetInputAngle(ptrFirstTrekNode->GetLongitude()) << NEW_LINE;
    outfile << "      Altitude                                                : ";
    outfile << UnitData::GetInputDistance(ptrFirstTrekNode->GetAltitude()) << NEW_LINE;

    if (positionGenerationMethod_a == MOVING) {
        outfile.unsetf(ios::right);
        outfile.setf(ios::left);
        outfile << "      Initial Layover Time Steps                              : ";
        TimePiece::PrintDuration(outfile, ptrFirstTrekNode->GetNumberOfLayoverTimeSteps(),
                                 UnitData::GetInputTimeUnits());
        outfile << NEW_LINE;
        outfile.unsetf(ios::left);
        outfile.setf(ios::right);

        if (ptrTrekFilename_a != NULL) {
            outfile << "      Trek Filename                                           : ";
            outfile << *ptrTrekFilename_a << NEW_LINE;
        }
    }

    outfile << "   <POSITION_GENERATION_STOP>" << NEW_LINE;
    return;
}

TrekNode*
Trek::GetFirstTrekNode() const
{
    const int firstIndex = 0;

    if (ptrTrekNodeList_a == NULL) {
        throw new NullPointerException(GetClassName(), "GetFirstTrekNode()",
                                       "ptrTrekNodeList_a");
    }

    return(ptrTrekNodeList_a->GetObjectAt(firstIndex));
}

TrekNode*
Trek::GetLastTrekNode() const
{
    int lastIndex = 0;

    if (ptrTrekNodeList_a == NULL) {
        throw new NullPointerException(GetClassName(), "GetLastTrekNode()",
                                       "ptrTrekNodeList_a");
    }

    lastIndex = ptrTrekNodeList_a->Size() - 1;
    return(ptrTrekNodeList_a->GetObjectAt(lastIndex));
}

void
Trek::CreateTrek()
{
    string errorString = "";
    TrekNode *ptrFirstNode = NULL;

    if (positionGenerationMethod_a == STATIONARY) {
        initialLayover_a = TimePiece::GetNumberOfTimeSteps();
    }

    ptrFirstNode = new TrekNode(0, latitude_a, longitude_a, altitude_a, initialLayover_a);
    ptrTrekNodeList_a = new LinkList<TrekNode>();
    ptrTrekNodeList_a->Append(ptrFirstNode);
    ptrFirstNode = NULL;

    if (positionGenerationMethod_a == MOVING) {
        if (initialLayover_a < 0) {
            if (errorString.size() > 0) {
                errorString += "\n       ";
            }

            errorString += "Missing/invalid 'Initial Layover Time Steps' input.  Mandatory for MOVING Trek";
        }

        if (ptrTrekFilename_a == NULL) {
            if (errorString.size() > 0) {
            }
        } else {
            try {
                ParseUtility::VerifyInputFile(*ptrTrekFilename_a);
            } catch (Exception *ptrError) {
                ptrError->AddMethod(GetClassName(), "ExtractTrekInfo(InputDataElement*)");
                throw;
            }
        }

        try {
            RetrieveTrekNodes();
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "ExtractTrekInfo(InputDataElement&)");
            throw;
        }

        // if only one valid trek node is found, change method to stationary
        if (ptrTrekNodeList_a->Size() == 1) {
            positionGenerationMethod_a = STATIONARY;
        }
    }
}

void
Trek::RetrieveTrekNodes()
{
    int         layoverTimeSteps = 0;
    int         previousDeparture = GetFirstTrekNode()->GetDepartureTimeStep();
    const int   endIndexTime = TimePiece::GetEndIndexTime();
    string       inputString = "";
    string       errorString = "";
    ifstream     inputFile;
    TrekNode    *ptrTrekNode = NULL;

    try {
        int arrivalTimeIndex = 0;
        double arrivalTime = 0.0;
        double latitude = 0.00;
        double longitude = 0.00;
        double altitude = 0.00;
        double layoverTime = 0.00;
        bool foundLastNode = false;
        ParseUtility::OpenInputFile(inputFile, *ptrTrekFilename_a);
        // get the header line
        getline(inputFile, inputString, NEW_LINE);

        while (getline(inputFile, inputString, NEW_LINE) && (foundLastNode == false) &&
                (errorString.size() == 0)) {
            if (inputString.find("#") == string::npos) {
                istringstream inputStream(inputString);
                inputStream >> arrivalTime >> latitude >> longitude >> altitude >> layoverTime;

                if (TimePiece::ConvertToStoreTimeIndex(arrivalTime, arrivalTimeIndex) == false) {
                    foundLastNode = true;
                } else if (arrivalTimeIndex <= previousDeparture) {
                    errorString = "Node arrives before previous departs";
                } else {
                    latitude = UnitData::GetInternalAngle(latitude);
                    longitude = UnitData::GetInternalAngle(longitude);
                    altitude = UnitData::GetInternalDistance(altitude);
                    layoverTimeSteps = UnitData::GetInternalTimeDuration(layoverTime);

                    if ((arrivalTimeIndex + layoverTimeSteps) > endIndexTime) {
                        layoverTimeSteps = endIndexTime - arrivalTimeIndex;
                    }

                    ptrTrekNode = new TrekNode(arrivalTimeIndex, latitude, longitude,
                                               altitude, layoverTimeSteps);
                    previousDeparture = ptrTrekNode->GetDepartureTimeStep();
                    ptrTrekNodeList_a->Append(ptrTrekNode);
                    ptrTrekNode = NULL;
                }
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "RetrieveTrekNodes()");
        throw;
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "RetrieveTrekNodes()", errorString);
    }

    ptrTrekNode = GetLastTrekNode();

    if (ptrTrekNode->GetDepartureTimeStep() < endIndexTime) {
        layoverTimeSteps = endIndexTime - ptrTrekNode->GetArrivalTimeStep();
        ptrTrekNode->SetNumberOfLayoverTimeSteps(layoverTimeSteps);
    }

    return;
}

/* ************************************************************************************** */
void
Trek::SetInitialLayover(int initialLayover)
{
    initialLayover_a = initialLayover;
}

/* ************************************************************************************** */

void
Trek::SetLatitude(double latitude)
{
    latitude_a = latitude;
}

/* ************************************************************************************** */

void
Trek::SetLongitude(double longitude)
{
    longitude_a = longitude;
}

/* ************************************************************************************** */

void
Trek::SetAltitude(double altitude)
{
    altitude_a = altitude;
}

/* ************************************************************************************** */

void
Trek::SetTrekFileName(shared_ptr<string> fileName)
{
    ptrTrekFilename_a = fileName;
}

/* ************************************************************************************* */

Trek::~Trek()
{
    if (ptrTrekNodeList_a != NULL) {
        ptrTrekNodeList_a->Delete();
        delete ptrTrekNodeList_a;
        ptrTrekNodeList_a = NULL;
    }

    return;
}

