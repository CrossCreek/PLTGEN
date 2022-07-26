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
 * PositionUtility.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "PositionUtility.h"
#include "general/array/ListIterator.h"
#include "mural/modules/pg/Orbit.h"
#include "mural/modules/pg/Trek.h"


#include "general/parse/ParseUtility.h"
#include "general/utility/Constant.h"
#include "general/time/TimePiece.h"
#include "general/data/PositionState.h"
#include "general/math/SimpleMath.h"
#include "general/math/SpaceVector.h"

PositionUtility::PositionUtility()
{
    return;
}

/* ******************************************************************************** */

PositionUtility::PositionUtility(const PositionUtility &copyMe)
{
    return;
}

/* ******************************************************************************** */

shared_ptr<PositionState>
PositionUtility::GeneratePositionState(shared_ptr<Orbit> ptrOrbit,
                                       double j2000Time)
{
    double componentX;
    double componentY;
    double componentZ;
    const double   deltaTime = j2000Time - ptrOrbit->GetEpochTime();
    const double   inclination = ptrOrbit->GetInclination();
    const double   eccentricity = ptrOrbit->GetEccentricity();
    const double   rightAscension = ptrOrbit->GetRightAscension();
    const double   semiLatusRectum = ptrOrbit->GetSemiLatusRectum();
    const double   apsidalRotation = ptrOrbit->GetApsidalRotationRate();
    const double   nodalPrecession = ptrOrbit->GetNodalPrecessionRate();
    const double   anomalisticMean = ptrOrbit->GetAnomalisticMeanMotion();
    const double   currentRA = MakeAngleBetweenZeroAndTwoPI(rightAscension + (deltaTime * nodalPrecession));
    const double   currentAOP = MakeAngleBetweenZeroAndTwoPI(ptrOrbit->GetArgumentOfPerigee() + (deltaTime * apsidalRotation));
    const double   currentMA = MakeAngleBetweenZeroAndTwoPI(ptrOrbit->GetMeanAnomaly() + (deltaTime * anomalisticMean));
    const double   trueAnomaly = Orbit::AnomalyConverter(currentMA, Orbit::MEAN_TO_TRUE, eccentricity);
    const double   cosineTrueAnomaly = cos(trueAnomaly);
    const double   sineTrueAnomaly = sin(trueAnomaly);
    const double   radius = semiLatusRectum / (1.0 + eccentricity * cosineTrueAnomaly);
    const double   vfactor = anomalisticMean * SimpleMath::SquareRoot(ptrOrbit->GetSemiMajorAxisCubed() / semiLatusRectum);
    const REFERENCE_FRAME storeFrame = UnitData::GetInternalReferenceFrame();
    SpaceVector    position;
    SpaceVector    velocity;
    shared_ptr<PositionState> ptrPositionState = NULL;
    // position and velocity in the PQW system
    position = SpaceVector((radius * cosineTrueAnomaly),
                           (radius * sineTrueAnomaly), 0.0, PQW);
    velocity = SpaceVector((-vfactor * sineTrueAnomaly),
                           (vfactor * (eccentricity + cosineTrueAnomaly)), 0.0, PQW);
    // convert to ECI first to add other components
    position = ConvertPQWToECI(position, inclination, currentRA, currentAOP);
    velocity = ConvertPQWToECI(velocity, inclination, currentRA, currentAOP);
    // add apsidal rotation component to velocity
    componentX = (-apsidalRotation * (position.GetZ() * cos(currentRA)
                                      * sin(inclination) - position.GetY() * cos(inclination)));
    componentY = (apsidalRotation * (position.GetX() * cos(inclination)
                                     - position.GetZ() * sin(currentRA) * sin(inclination)));
    componentZ = (apsidalRotation * (position.GetY() * sin(currentRA)
                                     + position.GetX() * cos(currentRA)) * sin(inclination));
    velocity += SpaceVector(componentX, componentY, componentZ, EARTH_CENTER_INERTIAL);
    // add nodal rotation component to velocity
    componentX = (-nodalPrecession * position.GetY());
    componentY = (nodalPrecession * position.GetX());
    componentZ = 0.0;
    velocity += SpaceVector(componentX, componentY, componentZ, EARTH_CENTER_INERTIAL);
    ptrPositionState = shared_ptr<PositionState>(new PositionState(position.ConvertTo(storeFrame, j2000Time),
                       velocity.ConvertTo(storeFrame, j2000Time)));
    return(ptrPositionState);
}

/* ******************************************************************************** */

void
PositionUtility::RetrievePositionData(const string &positionFile,
                                      const string &designator,
                                      shared_ptr<Journey> ptrJourney)
{
    string       vehicleDesignator = "";
    string       inputString = "";
    ifstream     ephemerisFile;
    const ANGLE_UNIT      inputAngle = UnitData::GetInputAngleUnits();
    const DISTANCE_UNIT   inputDistance = UnitData::GetInputDistanceUnits();
    const REFERENCE_FRAME inputFrame = UnitData::GetInputReferenceFrame();
    SpaceVector           positionVector;
    SpaceVector           velocityVector;
    shared_ptr<PositionState>  ptrPositionState = NULL;

    try {
        int timeIndex = 0;
        double inputTime = 0;
        double xPosition = 0.0;
        double yPosition = 0.0;
        double zPosition = 0.0;
        double xVelocity = 0.0;
        double yVelocity = 0.0;
        double zVelocity = 0.0;
        ParseUtility::OpenInputFile(ephemerisFile, positionFile);
        // get the header line first
        getline(ephemerisFile, inputString, NEW_LINE);

        while (getline(ephemerisFile, inputString, NEW_LINE)) {
            istringstream inputStream(inputString);
            inputStream >> vehicleDesignator >> inputTime;

            if ( (vehicleDesignator == designator) &&
                    (TimePiece::ConvertToStoreTimeIndex(inputTime, timeIndex) == true) ) {
                inputStream >> xPosition >> yPosition >> zPosition;
                inputStream >> xVelocity >> yVelocity >> zVelocity;
                positionVector = SpaceVector(xPosition, yPosition, zPosition, inputFrame,
                                             inputAngle, inputDistance);
                velocityVector = SpaceVector(xVelocity, yVelocity, zVelocity, inputFrame,
                                             inputAngle, inputDistance);
                ptrPositionState = shared_ptr<PositionState>(new PositionState(positionVector.GetStoreSpaceVector(timeIndex),
                                   velocityVector.GetStoreSpaceVector(timeIndex)));
                ptrJourney->SetPositionState(timeIndex, ptrPositionState);
                ptrPositionState = NULL;
            }
        }

        ephemerisFile.close();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "RetrievePositionData(Journey*, const string&)");
        throw;
    }

    return;
}

/* ******************************************************************************** */

void
PositionUtility::RetrieveP470PositionData(const string &positionFile,
        const string &designator,
        shared_ptr<Journey> ptrJourney)
{
    const double startTime = TimePiece::GetStartTime(J2000);
    const double endTime = TimePiece::GetEndTime(J2000);
    string       vehicleDesignator = "";
    string       inputString = "";
    ifstream     ephemerisFile;
    const DISTANCE_UNIT storeDistanceUnit = UnitData::GetInternalDistanceUnits();
    SpaceVector         positionVector;
    SpaceVector         velocityVector;
    shared_ptr<PositionState> ptrPositionState = NULL;

    try {
        int timeIndex = 0;
        double OFTime = 0;
        double xPosition = 0.00;
        double yPosition = 0.00;
        double zPosition = 0.00;
        double xVelocity = 0.00;
        double yVelocity = 0.00;
        double zVelocity = 0.00;
        ParseUtility::OpenInputFile(ephemerisFile, positionFile);

        while (getline(ephemerisFile, inputString, NEW_LINE)) {
            double j2000Time;
            istringstream inputStream(inputString);
            inputStream >> vehicleDesignator >> OFTime;
            j2000Time = TimePiece::ConvertOFToJ2000(OFTime);

            if ( (vehicleDesignator == designator) &&
                    (j2000Time >= startTime) && (j2000Time <= endTime) ) {
                inputStream >> xPosition >> yPosition >> zPosition;
                inputStream >> xVelocity >> yVelocity >> zVelocity;
                // Assumption is that OF/P470 data is in Feet and ECI
                xPosition = SimpleMath::ConvertDistance(xPosition, FEET, storeDistanceUnit);
                yPosition = SimpleMath::ConvertDistance(yPosition, FEET, storeDistanceUnit);
                zPosition = SimpleMath::ConvertDistance(zPosition, FEET, storeDistanceUnit);
                xVelocity = SimpleMath::ConvertDistance(xVelocity, FEET, storeDistanceUnit);
                yVelocity = SimpleMath::ConvertDistance(yVelocity, FEET, storeDistanceUnit);
                zVelocity = SimpleMath::ConvertDistance(zVelocity, FEET, storeDistanceUnit);
                positionVector = SpaceVector(xPosition, yPosition, zPosition,
                                             EARTH_CENTER_INERTIAL);
                velocityVector = SpaceVector(xVelocity, yVelocity, zVelocity,
                                             EARTH_CENTER_INERTIAL);
                timeIndex = TimePiece::GetIndexTime(j2000Time);
                ptrPositionState = shared_ptr<PositionState>(new PositionState(positionVector.GetStoreSpaceVector(timeIndex),
                                   velocityVector.GetStoreSpaceVector(timeIndex)));
                ptrJourney->SetPositionState(timeIndex, ptrPositionState);
                ptrPositionState = NULL;
            }
        }

        ephemerisFile.close();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "RetrieveP470PositionData(Journey*, const string&)");
        throw;
    }

    return;
}

/* ******************************************************************************** */

void
PositionUtility::GenerateStationaryTrek(shared_ptr<Trek> ptrTrek)
{
    const int     timeIndex = 0;
    TrekNode      *ptrNode = ptrTrek->GetFirstTrekNode();
    shared_ptr<PositionState> ptrPositionState = NULL;

    try {
        ptrPositionState = shared_ptr<PositionState>(new PositionState(ptrNode->GetPositionState(timeIndex)));
        ptrTrek->SetPositionState(timeIndex, ptrPositionState);
        ptrPositionState = NULL;
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GenerateStationaryTrek(Trek*)");
        throw;
    }

    return;
}

/* ******************************************************************************** */

void
PositionUtility::GenerateMovingTrek(shared_ptr<Trek> ptrTrek)
{
    shared_ptr<PositionState> ptrPositionState = NULL;
    ListIterator<TrekNode>    trekNodeListIter(ptrTrek->GetTrekNodeList());
    TrekNode                  *ptrCurrentTrekNode = trekNodeListIter.First();
    TrekNode                  *ptrNextTrekNode = NULL;

    try {
        int timeIndex = 0;
        int departureTimeIndex = 0;
        //JOE:
        ptrNextTrekNode = trekNodeListIter.Next();

        while (ptrNextTrekNode != NULL) {
            timeIndex = ptrCurrentTrekNode->GetArrivalTimeStep();
            departureTimeIndex = ptrCurrentTrekNode->GetDepartureTimeStep();

            while (timeIndex <= departureTimeIndex) {
                ptrPositionState = shared_ptr<PositionState>(
                                       new PositionState(ptrCurrentTrekNode->GetPositionState(timeIndex)));
                ptrTrek->SetPositionState(timeIndex, ptrPositionState);
                ptrPositionState = NULL;
                ++timeIndex;
            }

            DetermineInterimTrekNodes(ptrTrek, ptrCurrentTrekNode, ptrNextTrekNode);
            ptrCurrentTrekNode = ptrNextTrekNode;
            ptrNextTrekNode = trekNodeListIter.Next();
        }

        timeIndex = ptrCurrentTrekNode->GetArrivalTimeStep();
        departureTimeIndex = ptrCurrentTrekNode->GetDepartureTimeStep();

        while (timeIndex <= departureTimeIndex) {
            ptrPositionState = shared_ptr<PositionState>(
                                   new PositionState(ptrCurrentTrekNode->GetPositionState(timeIndex)));
            ptrTrek->SetPositionState(timeIndex, ptrPositionState);
            ptrPositionState = NULL;
            ++timeIndex;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GenerateMovingTrek(Trek*)");
        throw;
    }

    return;
}

/* ******************************************************************************** */

void
PositionUtility::DetermineInterimTrekNodes(shared_ptr<Trek> ptrTrek,
        TrekNode *ptrStartNode,
        TrekNode *ptrEndNode)
{
    /**************************
      const int   departureTime = ptrStartNode->GetDepartureTimeStep();
      const int   arrivalTime = ptrEndNode->GetArrivalTimeStep();
      const int   totalTimeSteps = arrivalTime - departureTime + 1;
      int         timeIndex = departureTime + 1;
      double       currentSeconds = 0.0;
      double       currentAngle = 0.0;
      double       cosCurrentAngle = 0.0;
      double       sinCurrentAngle = 0.0;
      double       currentMagnitude = 0.0;
      double       j2000Time = 0.0;
      const double secondsPerTimeStep = (double)TimePiece::GetSecondsPerTimeStep();
      const double totalSeconds = totalTimeSteps * secondsPerTimeStep;
      SpaceVector  startECF = ptrStartNode->GetPositionVector(departureTime, EARTH_CENTER_FIXED);
      SpaceVector  endECF = ptrEndNode->GetPositionVector(arrivalTime, EARTH_CENTER_FIXED);
      double       startMagnitude = startECF.GetMagnitude();
      double       endMagnitude = endECF.GetMagnitude();
      SpaceVector  startECFUnit = SpaceVector((startECF.GetX() / startMagnitude),
                                              (startECF.GetY() / startMagnitude),
                                              (startECF.GetZ() / startMagnitude), EARTH_CENTER_FIXED);
      double       dotProduct = endECF.DotProduct(startECF) / (startMagnitude * startMagnitude);
      SpaceVector  endOntoStartECF = SpaceVector((startECF.GetX() * dotProduct),
                                                 (startECF.GetY() * dotProduct),
                                                 (startECF.GetZ() * dotProduct), EARTH_CENTER_FIXED);
      SpaceVector  endOrtho = SpaceVector(endECF - endOntoStartECF);
      double       orthoMagnitude = endOrtho.GetMagnitude();
      SpaceVector  endOrthoUnit = SpaceVector((endOrtho.GetX() / orthoMagnitude),
                                              (endOrtho.GetY() / orthoMagnitude),
                                              (endOrtho.GetZ() / orthoMagnitude), EARTH_CENTER_FIXED);
      double       separationAngle = SimpleMath::ArcCosine((startECF.DotProduct(endECF))
                                                           / (startMagnitude * endMagnitude));
      double       separationAngleRate = separationAngle / totalSeconds;
      double       magnitudeDifference = endMagnitude - startMagnitude;
      double       magnitudeRate = magnitudeDifference / totalSeconds;
      SpaceVector       startUnitAdjust;
      SpaceVector       endOrthoUnitAdjust;
      SpaceVector       interimPosition;
      const SpaceVector velocity = SpaceVector();
      PositionState    *ptrPositionState = NULL;

      try
      {
        while (timeIndex < arrivalTime)
        {
          currentSeconds   = (double)(timeIndex - departureTime) * secondsPerTimeStep;
          currentAngle     = separationAngleRate * currentSeconds;
          cosCurrentAngle  = cos(currentAngle);
          sinCurrentAngle  = sin(currentAngle);
          currentMagnitude = magnitudeRate * currentSeconds;

          startUnitAdjust = SpaceVector((startECFUnit.GetX() * cosCurrentAngle),
                                        (startECFUnit.GetY() * cosCurrentAngle),
                                        (startECFUnit.GetZ() * cosCurrentAngle), EARTH_CENTER_FIXED);
          endOrthoUnitAdjust = SpaceVector((endOrthoUnit.GetX() * sinCurrentAngle),
                                           (endOrthoUnit.GetY() * sinCurrentAngle),
                                           (endOrthoUnit.GetZ() * sinCurrentAngle), EARTH_CENTER_FIXED);
          interimPosition = SpaceVector((startUnitAdjust + endOrthoUnitAdjust)
                                        * (startMagnitude + currentMagnitude));

          j2000Time = TimePiece::GetJ2000Time(timeIndex);

          ptrPositionState = new PositionState(interimPosition.GetStoreSpaceVector(timeIndex),
                                               velocity);

          ptrTrek->SetPositionState(timeIndex, ptrPositionState);
          ptrPositionState = NULL;

          ++timeIndex;
        }
      }

      catch (Exception *ptrError)
      {
        ptrError->AddMethod(GetClassName(),
                            "DetermineInterimTrekNodes(Trek*, TrekNode*, TrekNode*)");
        throw ptrError;
      }
    ***************************/
    const int     departureTime = ptrStartNode->GetDepartureTimeStep();
    const int     arrivalTime = ptrEndNode->GetArrivalTimeStep();
    const double   secondsPerTimeStep = (double)TimePiece::GetSecondsPerTimeStep();
    const double   totalSeconds = (double)(arrivalTime - departureTime + 1) * secondsPerTimeStep;
    SpaceVector    startPosition = ptrStartNode->GetPositionVector(departureTime);
    SpaceVector    endPosition = ptrEndNode->GetPositionVector(arrivalTime);
    SpaceVector    interimPosition;
    SpaceVector    interimVelocity = (endPosition - startPosition) / totalSeconds;
    shared_ptr<PositionState> ptrPositionState = NULL;

    try {
        int timeIndex = 0;
        timeIndex = departureTime + 1;

        while (timeIndex < arrivalTime) {
            double currentSeconds;
            currentSeconds = (double)(timeIndex - departureTime) * secondsPerTimeStep;
            interimPosition = DetermineInterimPosition(startPosition, endPosition,
                              totalSeconds, currentSeconds);
            ptrPositionState = shared_ptr<PositionState>(new PositionState(interimPosition, interimVelocity));
            ptrTrek->SetPositionState(timeIndex, ptrPositionState);
            ptrPositionState = NULL;
            ++timeIndex;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "DetermineInterimTrekNodes(Trek*, TrekNode*, TrekNode*)");
        throw;
    }

    return;
}

/* ******************************************************************************** */

SpaceVector
PositionUtility::DetermineInterimPosition(const SpaceVector &startPosition,
        const SpaceVector &endPosition,
        double totalSeconds,
        double currentSeconds)
{
    const double startMagnitude = startPosition.GetMagnitude();
    const double endMagnitude = endPosition.GetMagnitude();
    SpaceVector  startUnitAdjust;
    SpaceVector  endOntoStartPosition;
    SpaceVector  endOrthoUnitAdjust;
    SpaceVector  interimPosition;

    try {
        double currentMagnitude;
        double currentAngle;
        currentMagnitude = ((endMagnitude - startMagnitude) / totalSeconds) * currentSeconds;
        currentAngle = (startPosition.GetSeparationAngle(endPosition) / totalSeconds) * currentSeconds;
        startUnitAdjust = startPosition.GetUnitVector() * cos(currentAngle);
        endOntoStartPosition = startPosition * endPosition.ProjectionOnto(startPosition);
        endOrthoUnitAdjust = (endPosition - endOntoStartPosition).GetUnitVector() * sin(currentAngle);
        interimPosition = (startUnitAdjust + endOrthoUnitAdjust) * (startMagnitude + currentMagnitude);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "DetermineInterimPosition(const SpaceVector&, const SpaceVector&, const double&, const double&)");
        throw;
    }

    return(interimPosition);
}

/* ******************************************************************************** */

Orbit
PositionUtility::UpdateOrbit(const Orbit &currentOrbit,
                             double newEpochTime)
{
    double newRightAscension;
    double newArgumentOfPerigee;
    double newMeanAnomaly;
    double newTrueAnomaly;
    const double deltaTime = newEpochTime - currentOrbit.GetEpochTime();
    newRightAscension = MakeAngleBetweenZeroAndTwoPI(currentOrbit.GetRightAscension()
                        + (deltaTime * currentOrbit.GetNodalPrecessionRate()));
    newArgumentOfPerigee = MakeAngleBetweenZeroAndTwoPI(currentOrbit.GetArgumentOfPerigee()
                           + (deltaTime * currentOrbit.GetApsidalRotationRate()));
    newMeanAnomaly = MakeAngleBetweenZeroAndTwoPI(currentOrbit.GetMeanAnomaly()
                     + (deltaTime * currentOrbit.GetAnomalisticMeanMotion()));
    newTrueAnomaly = Orbit::AnomalyConverter(newMeanAnomaly, Orbit::MEAN_TO_TRUE,
                     currentOrbit.GetEccentricity());
    return(Orbit(newEpochTime, currentOrbit.GetSemiMajorAxis(),
                 currentOrbit.GetEccentricity(), currentOrbit.GetInclination(),
                 newRightAscension, newArgumentOfPerigee, newTrueAnomaly, false));
}

/* ******************************************************************************** */

SpaceVector
PositionUtility::ConvertPQWToECI(const SpaceVector &pqwVector,
                                 double inclination,
                                 double rightAscension,
                                 double argumentOfPerigee)
{
    const double cosRightAscension  = cos(rightAscension);
    const double cosArgumentOfPerigee  = cos(argumentOfPerigee);
    const double cosInclination = cos(inclination);
    const double sinRightAscension  = sin(rightAscension);
    const double sinArgumentOfPerigee  = sin(argumentOfPerigee);
    const double sinInclination = sin(inclination);
    SpaceVector e0( cosRightAscension*cosArgumentOfPerigee
                    - sinRightAscension*sinArgumentOfPerigee*cosInclination,
                    - cosRightAscension*sinArgumentOfPerigee
                    - sinRightAscension*cosArgumentOfPerigee*cosInclination
                    , sinRightAscension*sinInclination, PQW);
    SpaceVector e1( sinRightAscension*cosArgumentOfPerigee
                    + cosRightAscension*sinArgumentOfPerigee*cosInclination,
                    - sinRightAscension*sinArgumentOfPerigee
                    + cosRightAscension*cosArgumentOfPerigee*cosInclination,
                    - cosRightAscension*sinInclination, PQW);
    SpaceVector e2((sinArgumentOfPerigee * sinInclination),
                   (cosArgumentOfPerigee * sinInclination),
                   cosInclination, PQW);
    return(SpaceVector((e0 * pqwVector), (e1 * pqwVector), (e2 * pqwVector), EARTH_CENTER_INERTIAL));
}

/* ******************************************************************************** */

double
PositionUtility::MakeAngleBetweenZeroAndTwoPI(double currentAngle)
{
    double returnAngle = SimpleMath::Remainder(currentAngle, TWO_PI);
    return(returnAngle < 0 ? (returnAngle + TWO_PI) : returnAngle);
}

/* ******************************************************************************** */

PositionUtility::~PositionUtility()
{
    return;
}
