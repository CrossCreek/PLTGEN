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
 * Journey.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "Journey.h"
#include "Revolution.h"
#include "PositionUtility.h"


#include "general/array/ListIterator.h"
#include "general/math/SpaceVector.h"
#include "general/utility/StringEditor.h"
#include "general/time/TimePiece.h"
#include "general/data/PositionState.h"
#include "general/data/InputData.h"
#include "general/data/InputDataElement.h"
#include "general\utility\Utility.h"

Journey::Journey()
    : positionGenerationMethod_a(UNKNOWN_POSITION_METHOD),
      ptrPositionStateArray_a(NULL)
{
    return;
}

Journey::Journey(const Journey &copyMe)
    : positionGenerationMethod_a(copyMe.positionGenerationMethod_a),
      ptrPositionStateArray_a(NULL)
{
    if (copyMe.ptrPositionStateArray_a != NULL) {
        ptrPositionStateArray_a = shared_ptr<MURALTypes::PositionStateVector>(copyMe.ptrPositionStateArray_a);
    }

    return;
}

Journey&
Journey::operator = (const Journey &copyMe)
{
    positionGenerationMethod_a = copyMe.positionGenerationMethod_a;
    ptrPositionStateArray_a  = NULL;

    if (copyMe.ptrPositionStateArray_a != NULL) {
        ptrPositionStateArray_a = shared_ptr<MURALTypes::PositionStateVector>(copyMe.ptrPositionStateArray_a);
    }

    return(*this);
}

bool
Journey::operator == (const Journey &compareMe)
{
    return((positionGenerationMethod_a == compareMe.positionGenerationMethod_a) &&
           (*ptrPositionStateArray_a == *(compareMe.ptrPositionStateArray_a)));
}

void
Journey::PrintAttributes(ostream &outfile) const
{
    return;
}

void
Journey::PrintUpdatedOrbitFile(ofstream &updatedOrbitFile,
                               const string &userDesignator) const
{
    return;
}

void
Journey::SetPositionState(int timeIndex,
                          const shared_ptr<PositionState> &ptrPositionState)
{
    if (ptrPositionStateArray_a == NULL) {
        int listSize = 1;

        if (positionGenerationMethod_a != STATIONARY) {
            listSize = TimePiece::GetNumberOfTimeSteps();
        }

        ptrPositionStateArray_a = shared_ptr<MURALTypes::PositionStateVector>(new MURALTypes::PositionStateVector(listSize));
    }

    try {
        // should i replace this with ->insert?
        (*ptrPositionStateArray_a)[timeIndex] = ptrPositionState;
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "SetPositionState(int, PositionState*)");
        throw;
    }

    return;
}

void
Journey::CheckPositionStates(const string &designator)
{
    int       timeIndex = 0;
    int       numberOfStates = 0;
    string     errorString = "";

    if (ptrPositionStateArray_a == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "CheckPositionState(const string&)",
                                       ("No positions found/generated for " + designator));
    }

    //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
    numberOfStates = Utility::CastSizetToInt(ptrPositionStateArray_a->size());

    while (timeIndex < numberOfStates) {
        if (ptrPositionStateArray_a->at(timeIndex) == NULL) {
            errorString = "No PositionState found for " + designator;
            errorString += " for time step ";
            errorString += StringEditor::ConvertToString(TimePiece::GetTimeStep(timeIndex));
            throw new InputException(GetClassName(), "CheckPositionState(const string&)",
                                     errorString);
        }

        ++timeIndex;
    }

    return;
}

double
Journey::GetLatitude(int timeIndex) const
{
    double      latitude = 0.0;
    SpaceVector positionVector;

    try {
        latitude = GetPosition(timeIndex).GetLatitude();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetLatitude(int)");
        throw;
    }

    return(latitude);
}

double
Journey::GetLatitude(int timeIndex,
                     const ANGLE_UNIT &returnAngleUnit) const
{
    double latitude = 0.0;

    try {
        latitude = GetPosition(timeIndex).GetLatitude(returnAngleUnit);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetLatitude(int, const ANGLE_UNIT&)");
        throw;
    }

    return(latitude);
}

double
Journey::GetLongitude(int timeIndex) const
{
    double longitude = 0.0;

    try {
        longitude = GetPosition(timeIndex).GetLongitude();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetLongitude(int)");
        throw;
    }

    return(longitude);
}

double
Journey::GetLongitude(int timeIndex,
                      const ANGLE_UNIT &returnAngleUnit) const
{
    double longitude = 0.0;

    try {
        longitude = GetPosition(timeIndex).GetLongitude(returnAngleUnit);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetLongitude(int, const ANGLE_UNIT&)");
        throw;
    }

    return(longitude);
}

double
Journey::GetAltitude(int timeIndex) const
{
    double altitude = 0.0;

    try {
        altitude = GetPosition(timeIndex).GetAltitude();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetAltitude(int)");
        throw;
    }

    return(altitude);
}

double
Journey::GetAltitude(int timeIndex,
                     const DISTANCE_UNIT &returnDistanceUnit) const
{
    double altitude = 0.0;

    try {
        altitude = GetPosition(timeIndex).GetAltitude(returnDistanceUnit);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetAltitude(int, const DISTANCE_UNIT&)");
        throw;
    }

    return(altitude);
}

SpaceVector
Journey::GetPosition(int timeIndex) const
{
    SpaceVector positionVector;

    try {
        positionVector = GetPosition(timeIndex,
                                     UnitData::GetInternalReferenceFrame());
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetPosition(int)");
        throw;
    }

    return(positionVector);
}

SpaceVector
Journey::GetPosition(int timeIndex,
                     const REFERENCE_FRAME &returnFrame) const
{
    int        listIndex = 0;
    SpaceVector positionVector;

    if (positionGenerationMethod_a != STATIONARY) {
        listIndex = timeIndex;
    }

    try {
        positionVector = (ptrPositionStateArray_a->at(listIndex))
                         ->GetPosition(returnFrame, timeIndex);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetPosition(int, const REFERENCE_FRAME&)");
        throw;
    }

    return(positionVector);
}

SpaceVector
Journey::GetInterimPosition(int timeIndex,
                            double additionalSeconds) const
{
    const double totalSeconds = (double)TimePiece::GetSecondsPerTimeStep();
    SpaceVector  interimPosition;

    if ((additionalSeconds == 0.0) || (positionGenerationMethod_a == STATIONARY)) {
        interimPosition = GetPosition(timeIndex);
    } else if (timeIndex < TimePiece::GetEndIndexTime()) {
        try {
            interimPosition = PositionUtility::DetermineInterimPosition(GetPosition(timeIndex),
                              GetPosition(timeIndex + 1),
                              totalSeconds, additionalSeconds);
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(),
                                "GetInterimPosition(int, const double&)");
            throw;
        }
    }

    return(interimPosition);
}

SpaceVector
Journey::GetVelocity(int timeIndex) const
{
    SpaceVector velocityVector;

    try {
        velocityVector = GetVelocity(timeIndex,
                                     UnitData::GetInternalReferenceFrame());
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetVelocity(int)");
        throw;
    }

    return(velocityVector);
}

SpaceVector
Journey::GetVelocity(int timeIndex,
                     const REFERENCE_FRAME &returnFrame) const
{
    int        listIndex = 0;
    SpaceVector velocityVector;

    if (positionGenerationMethod_a != STATIONARY) {
        listIndex = timeIndex;
    }

    try {
        velocityVector = (ptrPositionStateArray_a->at(listIndex))
                         ->GetVelocity(returnFrame, timeIndex);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetVelocity(int, const REFERENCE_FRAME&)");
        throw;
    }

    return(velocityVector);
}

SpaceVector
Journey::GetInterimVelocity(int timeIndex,
                            double additionalSeconds) const
{
    const double totalSeconds = (double)TimePiece::GetSecondsPerTimeStep();
    SpaceVector  interimVelocity;

    if ((additionalSeconds == 0.0) || (positionGenerationMethod_a == STATIONARY)) {
        interimVelocity = GetVelocity(timeIndex);
    } else if (timeIndex < TimePiece::GetEndIndexTime()) {
        try {
            interimVelocity = PositionUtility::DetermineInterimPosition(GetVelocity(timeIndex),
                              GetVelocity(timeIndex + 1),
                              totalSeconds, additionalSeconds);
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(),
                                "GetInterimVelocity(int, const double&)");
            throw;
        }
    }

    return(interimVelocity);
}

/* ************************************************************************************************** */

int
Journey::GetNumberOfPositionStates() const
{
    int numberOfPositionStates = 0;

    if ( ptrPositionStateArray_a != NULL ) {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        numberOfPositionStates = Utility::CastSizetToInt(ptrPositionStateArray_a->size());
    }

    return numberOfPositionStates;
}

/* ************************************************************************************************** */

shared_ptr<PositionState>
Journey::GetPositionStateAt(int timeIndex) const
{
    return ptrPositionStateArray_a->at(timeIndex);
}

/* ************************************************************************************************** */

int
Journey::GetRevolutionNumber(int timeIndex) const
{
    return(0);
}

int
Journey::GetOrbitPeriod() const
{
    return(-1);
}

int
Journey::GetRevolutionIndex(int timeIndex) const
{
    return(-1);
}

int
Journey::GetRevStartTime(int revIndex) const
{
    return(-1);
}

int
Journey::GetRevEndTime(int revIndex) const
{
    return(-1);
}

double
Journey::GetTrueAnomaly(int timeIndex,
                        double additionalSeconds) const
{
    return(UNDEFINED_ANGLE);
}

LinkList<Revolution>*
Journey::GenerateSouthPointRevolutionList() const
{
    Revolution *ptrRevolution = new Revolution();
    LinkList<Revolution> *ptrRevolutionList = new LinkList<Revolution>();
    ptrRevolutionList->Append(ptrRevolution);
    return(ptrRevolutionList);
}

/* **************************************************************************** */

void
Journey::SetPositionGenerationMethod(POSITION_METHOD positionMethod)
{
    positionGenerationMethod_a = positionMethod;
}

/* **************************************************************************** */

Journey::POSITION_METHOD
Journey::ConvertPositionMethod(const string &positionMethodString)
{
    POSITION_METHOD positionMethod = UNKNOWN_POSITION_METHOD;

    if (positionMethodString == "J2J4") {
        positionMethod = J2J4;
    } else if (positionMethodString == "STATIONARY") {
        positionMethod = STATIONARY;
    } else if (positionMethodString == "MOVING") {
        positionMethod = MOVING;
    } else if (positionMethodString == "READ_P470") {
        positionMethod = READ_P470;
    } else if (positionMethodString == "READ" || positionMethodString == "READ_MURAL") {
        positionMethod = READ_MURAL;
    } else if (positionMethodString == "READ_OPS") {
        positionMethod = READ_OPS;
    }

    return(positionMethod);
}

/* ******************************************************************************** */

string
Journey::GetGenerationMethodString(const POSITION_METHOD &positionMethod)
{
    string methodString = "UNKNOWN";

    if (positionMethod == J2J4) {
        methodString = "J2J4";
    } else if (positionMethod == STATIONARY) {
        methodString = "STATIONARY";
    } else if (positionMethod == MOVING) {
        methodString = "MOVING";
    } else if (positionMethod == READ_MURAL) {
        methodString = "READ_MURAL";
    } else if (positionMethod == READ_OPS) {
        methodString = "READ_OPS";
    } else if (positionMethod == READ_P470) {
        methodString = "READ_P470";
    }

    return(methodString);
}

/* ************************************************************************************* */
Journey::~Journey()
{
    return;
}

