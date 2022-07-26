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
 * Orbit.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "Orbit.h"
#include "Revolution.h"
#include "PositionUtility.h"
#include "mural/Earth.h"


#include "general/array/ListIterator.h"
#include "general/data/UnitData.h"
#include "general/math/SimpleMath.h"
#include "general/math/SpaceVector.h"
#include "general/time/TimePiece.h"
#include "general/data/PositionState.h"
#include "general/data/InputData.h"
#include "general/data/InputDataElement.h"

// From General Engineering
#include "constants.h"
#include "MeanOsc.h"
#include "OrbitElKeplerian.h"
#include "TimeJ2000.h"
#include "VectorCartesian.h"
#include "CoorTransDcm.h"
#include "IERS96.h"
#include "GenUtility.h"

const double Orbit::J2_s = 1082.629989e-6;
const double Orbit::J4_s = -1.610988e-6;

Orbit::Orbit()
    : Journey(), revolutionNumberOffset_a(-1), epochTime_a(0.0),
      semiMajorAxis_a(0.0), eccentricity_a(0.0),
      inclination_a(0.0), rightAscension_a(0.0), argumentOfPerigee_a(0.0),
      trueAnomaly_a(0.0), meanAnomaly_a(0.0), nodalPrecessionRate_a(0.0),
      apsidalRotationRate_a(0.0), anomalisticMeanMotion_a(0.0), stateVectorFormat_a(MEAN),
      revolutionBoundary_a(UNKNOWN_BOUNDARY),
      ptrRevolutionList_a(NULL), jplRequired_a(false)
{
    return;
}

Orbit::Orbit(double epochTime,
             double semiMajorAxis,
             double eccentricity,
             double inclination,
             double rightAscension,
             double argumentOfPerigee,
             double trueAnomaly,
             bool jplRequired)
    : Journey(), revolutionNumberOffset_a(-1), epochTime_a(epochTime),
      semiMajorAxis_a(semiMajorAxis), eccentricity_a(eccentricity),
      inclination_a(inclination), rightAscension_a(rightAscension),
      argumentOfPerigee_a(argumentOfPerigee),
      trueAnomaly_a(trueAnomaly), meanAnomaly_a(0.0), nodalPrecessionRate_a(0.0),
      apsidalRotationRate_a(0.0), anomalisticMeanMotion_a(0.0), stateVectorFormat_a(MEAN),
      revolutionBoundary_a(UNKNOWN_BOUNDARY),
      ptrRevolutionList_a(NULL), jplRequired_a(jplRequired)
{
    meanAnomaly_a = AnomalyConverter(trueAnomaly, TRUE_TO_MEAN, eccentricity);
    return;
}

Orbit::Orbit(const Orbit &copyMe)
    : Journey(copyMe), revolutionNumberOffset_a(copyMe.revolutionNumberOffset_a),
      epochTime_a(copyMe.epochTime_a),
      semiMajorAxis_a(copyMe.semiMajorAxis_a),
      eccentricity_a(copyMe.eccentricity_a),
      inclination_a(copyMe.inclination_a),
      rightAscension_a(copyMe.rightAscension_a),
      argumentOfPerigee_a(copyMe.argumentOfPerigee_a),
      trueAnomaly_a(copyMe.trueAnomaly_a),
      meanAnomaly_a(copyMe.meanAnomaly_a),
      jplRequired_a(copyMe.jplRequired_a),
      nodalPrecessionRate_a(copyMe.nodalPrecessionRate_a),
      apsidalRotationRate_a(copyMe.apsidalRotationRate_a),
      anomalisticMeanMotion_a(copyMe.anomalisticMeanMotion_a),
      stateVectorFormat_a(copyMe.stateVectorFormat_a),
      revolutionBoundary_a(copyMe.revolutionBoundary_a),
      ptrRevolutionList_a(NULL)
{
    if (copyMe.ptrRevolutionList_a != NULL) {
        ptrRevolutionList_a = new
        LinkList<Revolution>(*(copyMe.ptrRevolutionList_a));
    }

    return;
}

Orbit&
Orbit::operator = (const Orbit &copyMe)
{
    Journey::operator = (copyMe);
    revolutionNumberOffset_a = copyMe.revolutionNumberOffset_a;
    epochTime_a              = copyMe.epochTime_a;
    semiMajorAxis_a          = copyMe.semiMajorAxis_a;
    eccentricity_a           = copyMe.eccentricity_a;
    inclination_a            = copyMe.inclination_a;
    rightAscension_a         = copyMe.rightAscension_a;
    argumentOfPerigee_a      = copyMe.argumentOfPerigee_a;
    trueAnomaly_a            = copyMe.trueAnomaly_a;
    meanAnomaly_a            = copyMe.meanAnomaly_a;
    nodalPrecessionRate_a    = copyMe.nodalPrecessionRate_a;
    apsidalRotationRate_a    = copyMe.apsidalRotationRate_a;
    anomalisticMeanMotion_a  = copyMe.anomalisticMeanMotion_a;
    revolutionBoundary_a     = copyMe.revolutionBoundary_a;
    stateVectorFormat_a      = copyMe.stateVectorFormat_a;
    jplRequired_a            = copyMe.jplRequired_a;

    if (copyMe.ptrRevolutionList_a != NULL) {
        ptrRevolutionList_a = new
        LinkList<Revolution>(*(copyMe.ptrRevolutionList_a));
    }

    return(*this);
}

bool
Orbit::operator == (const Orbit &compareMe)
{
    return( (Journey::operator == (compareMe)) &&
            (revolutionNumberOffset_a == compareMe.revolutionNumberOffset_a) &&
            (epochTime_a == compareMe.epochTime_a) &&
            (semiMajorAxis_a == compareMe.semiMajorAxis_a) &&
            (eccentricity_a == compareMe.eccentricity_a) &&
            (inclination_a == compareMe.inclination_a) &&
            (rightAscension_a == compareMe.rightAscension_a) &&
            (argumentOfPerigee_a == compareMe.argumentOfPerigee_a) &&
            (trueAnomaly_a == compareMe.trueAnomaly_a) &&
            (meanAnomaly_a == compareMe.meanAnomaly_a) &&
            (nodalPrecessionRate_a == compareMe.nodalPrecessionRate_a) &&
            (apsidalRotationRate_a == compareMe.apsidalRotationRate_a) &&
            (anomalisticMeanMotion_a == compareMe.anomalisticMeanMotion_a) &&
            (revolutionBoundary_a == compareMe.revolutionBoundary_a) &&
            (*ptrRevolutionList_a == *(compareMe.ptrRevolutionList_a) &&
             (stateVectorFormat_a == compareMe.stateVectorFormat_a)) &&
            (jplRequired_a == compareMe.jplRequired_a) );
}

void
Orbit::PrintAttributes(ostream &outfile) const
{
    Calendar epochCalendar(epochTime_a);
    outfile << "   <POSITION_GENERATION_BEGIN>" << NEW_LINE;
    outfile << "      Orbiting Data Provider                                  : true";
    outfile << NEW_LINE;
    outfile << "      Position Generation Method (READ/READ_P470/J2J4)        : ";
    outfile << GetGenerationMethodString(positionGenerationMethod_a) << NEW_LINE;
    outfile << "      Epoch Date                                              : ";
    outfile << epochCalendar.GetDateDisplay() << NEW_LINE;
    outfile << "      Epoch Time                                              : ";
    outfile << epochCalendar.GetTimeDisplay() << NEW_LINE;
    outfile << "      Mean Osculating Definition                              : ";
    outfile << GetStateVectorFormatString(stateVectorFormat_a) << NEW_LINE;
    outfile << "      Semi-Major Axis                                         : ";
    outfile << UnitData::GetInputDistance(semiMajorAxis_a) << NEW_LINE;
    outfile << "      Eccentricity                                            : ";
    outfile << eccentricity_a << NEW_LINE;
    outfile << "      Inclination                                             : ";
    outfile << UnitData::GetInputAngle(inclination_a) << NEW_LINE;
    outfile << "      Argument of Perigee                                     : ";
    outfile << UnitData::GetInputAngle(argumentOfPerigee_a) << NEW_LINE;
    outfile << "      Right Ascension                                         : ";
    outfile << UnitData::GetInputAngle(rightAscension_a) << NEW_LINE;
    outfile << "      True Anomaly                                            : ";
    //Add Mean Anomaly to Output - RAGUSA 7/20/2017
    outfile << UnitData::GetInputAngle(trueAnomaly_a) << NEW_LINE;
    outfile << "      Mean Anomaly                                            : ";
    outfile << UnitData::GetInputAngle(meanAnomaly_a) << NEW_LINE;

    if (revolutionBoundary_a != UNKNOWN_BOUNDARY) {
        outfile << "      Revolution Boundary                                     : ";
        outfile << GetRevolutionBoundaryString() << NEW_LINE;
    }

    if (revolutionNumberOffset_a >= 0) {
        outfile << "      Revolution Number Offset                                : ";
        outfile << revolutionNumberOffset_a << NEW_LINE;
    }

    outfile << "   <POSITION_GENERATION_STOP>" << NEW_LINE;
    return;
}

/* ************************************************************************************************ */

void
Orbit::CalculatePerturbations()
{
    double b0;
    double b1;
    double c0;
    double c1;
    double c2;
    const double e2 = eccentricity_a * eccentricity_a;
    // need to get gravitational constant (mu) in storeUnits^3 / sec^2
    const double mu = Earth::GetMU(UnitData::GetInternalDistanceUnits(), SECONDS);
    const double n0 = SimpleMath::SquareRoot(mu / GetSemiMajorAxisCubed());
    const double slr = GetSemiLatusRectum();
    // convert radius from NMI to internal units (meters for MURAL)
    const double earthRadius = SimpleMath::ConvertDistance(RADIUS_NMI, NAUTICAL_MILES, UnitData::GetInternalDistanceUnits());
    const double rop2 = (earthRadius * earthRadius) / slr / slr;
    const double rop4 = rop2 * rop2;
    const double si = sin(inclination_a);
    const double ci = cos(inclination_a);
    b0   = 1.0 - e2;
    b1   = SimpleMath::SquareRoot(b0);
    c0   = 1.5 * J2_s * rop2 * (1.0 - 1.5 * si * si);
    c1   = 3.0 * J2_s * J2_s * (16.0 * b1 + 25.0 * b0 - 15.0 + (30.0
                                - 96 * b1 - 90.0 * b0) * ci * ci + (105.0 +144.0 * b1 + 25.0
                                        * b0) * pow(ci, 4.0));
    c2   = -45.0 * J4_s * e2 * (3.0 - 30.0
                                * ci * ci + 35.0 * pow(ci, 4.0));
    // calculate anomalistic mean motion
    anomalisticMeanMotion_a = (n0 * (1 + b1 * (c0 + (c1 + c2) * rop4/128.0)) );
    // calculate mean right ascension rate
    nodalPrecessionRate_a = (-1.5 * J2_s * anomalisticMeanMotion_a * rop2
                             * ci * (1 + 1.5 * J2_s
                                     * (1.5 + e2 / 6.0 - 2.0 * b1 - (2.5 - 5.0 * e2 / 24.0
                                             - 3.0 * b1) * si * si) * rop2) - 0.3125 * J4_s * n0 * (1.0
                                                     + 1.5 * e2) * (12.0 - 21.0 * si * si) * ci * rop4 );
    // calculate mean argument of perigee rate
    apsidalRotationRate_a = (1.5 * J2_s * anomalisticMeanMotion_a
                             * (2.0 - 2.5 * si * si) * (1.0
                                     + 1.5 * J2_s * (2.0 + e2/2.0 - 2.0 * b1 - (43.0/24.0
                                             - e2 / 48.0 - 3.0 * b1) * si * si) * rop2) * rop2 - 1.25
                             * J2_s * J2_s * e2 * n0 * pow(ci, 4.0) * rop4 - 4.375
                             * J4_s * n0 * (12.0/7.0 - 93.0/14.0 * si * si + 5.25
                                            * pow(si, 4.0) + e2 * (27.0/14.0 - 6.75 * si * si
                                                    + 5.0625 * pow(si, 4.0))) * rop4 );
}

/* ************************************************************************************************ */
void
Orbit::CalculateClassicalElements()
{
    int firstTimeIndex = 0;
    SpaceVector positionVector = GetPosition(firstTimeIndex);
    SpaceVector velocityVector = GetVelocity(firstTimeIndex);
    CalculateClassicalElements(positionVector, velocityVector, stateVectorFormat_a);
    return;
}

/* ************************************************************************************************ */

void
Orbit::CalculateClassicalElements(const SpaceVector &positionVector,
                                  const SpaceVector &velocityVector,
                                  STATE_VECTOR_FORMAT vectorFormat)
{
    try {
        if ( vectorFormat == OSCULATING ) {
            ConvertOsculatingVectors(positionVector, velocityVector);
        } else if ( vectorFormat == MEAN ) {
            ConvertMeanVectors(positionVector, velocityVector);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "CalculateClassicalElements(const SpaceVector&, const SpaceVector&)");
        throw;
    }

    return;
}

/**
 * This method performs the conversion from mean vectors to mean keplarian.  It is used
 * when reading in an 'mean' initial state vector.  MURAL performs the conversion
 *
 * @param positionVector a 'mean' position vector
 * @param velocityVector a 'mean' velocity vector
 */
void
Orbit::ConvertMeanVectors(const SpaceVector &positionVector,
                          const SpaceVector &velocityVector)
{
    const double FULL_CIRCLE = UnitData::GetStoreFullCircle();
    const double MU = Earth::GetMU(UnitData::GetInternalDistanceUnits(), SECONDS);
    SpaceVector  positionVectorECI;
    SpaceVector  velocityVectorECI;
    SpaceVector  crossVector;

    try {
        double ECSTA;
        double ESNTA;
        double crossMagnitude;
        double positionMagnitude;
        double velocityMagnitude;
        cout << "MURALInputProcessor(Orbit Conversion): Interpret State Vector as Mean" << NEW_LINE;
        PositionState positionState(positionVector, velocityVector);
        // make sure the vectors are in ECI
        positionVectorECI = positionState.GetPositionECI(epochTime_a);
        velocityVectorECI = positionState.GetVelocityECI(epochTime_a);
        crossVector = positionVectorECI.CrossProduct(velocityVectorECI);
        positionMagnitude = positionVectorECI.GetMagnitude();
        velocityMagnitude = velocityVectorECI.GetMagnitude();
        crossMagnitude = crossVector.GetMagnitude();
        inclination_a = SimpleMath::ArcCosine(crossVector.GetZ() / crossMagnitude);
        rightAscension_a = SimpleMath::Remainder((SimpleMath::ArcTangent(crossVector.GetX(),
                           -crossVector.GetY()) + FULL_CIRCLE), FULL_CIRCLE);
        semiMajorAxis_a = 1.0 / (2.0 / positionMagnitude - velocityMagnitude * velocityMagnitude / MU);
        ECSTA = crossMagnitude * crossMagnitude / (MU * positionMagnitude) - 1.0;
        ESNTA = positionVectorECI.DotProduct(velocityVectorECI) * crossMagnitude / (MU * positionMagnitude);
        eccentricity_a = SimpleMath::SquareRoot(ECSTA * ECSTA + ESNTA * ESNTA);
        trueAnomaly_a = SimpleMath::ArcTangent(ESNTA, ECSTA);
        argumentOfPerigee_a = SimpleMath::Remainder((SimpleMath::ArcTangent(positionVectorECI.GetZ(),
                              (crossVector.GetX() * positionVectorECI.GetY() - crossVector.GetY() * positionVectorECI.GetX()) / crossMagnitude)
                              - trueAnomaly_a + FULL_CIRCLE), FULL_CIRCLE);
        trueAnomaly_a = SimpleMath::Remainder((trueAnomaly_a + FULL_CIRCLE), FULL_CIRCLE);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "CalculateClassicalElements(const SpaceVector&, const SpaceVector&)");
        throw;
    }

    return;
}

/**
 * This method performs the conversion from osculating vectors to mean keplarian.  It is used
 * when reading in an 'osculating' initial state vector.  it uses a General Engineering routine
 * to perform the conversion.
 *
 * @param positionVector an 'osculating' position vector
 * @param velocityVector an 'osculating' velocity vector
 */
void
Orbit::ConvertOsculatingVectors(const SpaceVector &positionVector,
                                const SpaceVector &velocityVector)
{
    cout << "MURALInputProcessor(Orbit Conversion): Converting Osculating State Vector to Mean" << NEW_LINE;
    A3::VectorCartesian posVector(positionVector.GetX(), positionVector.GetY(), positionVector.GetZ());
    A3::VectorCartesian velVector(velocityVector.GetX(), velocityVector.GetY(), velocityVector.GetZ());
    // Convert from MURAL units (meters) to GenEng units (nmi)
    // MURAL angle units are the same as GenEng (radians)
    posVector *= METERS_TO_NMI;
    velVector *= METERS_TO_NMI;

    if ( jplRequired_a == true ) {
        cout << "MURALInputProcessor(Orbit Conversion): Found JPL Files.  Converting from J2000 to True of Date" << NEW_LINE;
        // Translation from J2000 to TOD
        A3::TimeJ2000 timeJ2000(epochTime_a);
        A3::CoorTransDcm J2000ToModDCM;
        A3::IERS96::dcmJ2000ToMod(timeJ2000, J2000ToModDCM);
        J2000ToModDCM.Multiply(posVector, posVector);
        J2000ToModDCM.Multiply(velVector, velVector);
        A3::CoorTransDcm ModToTodDCM;
        A3::IERS96::dcmModToTod(timeJ2000, ModToTodDCM);
        ModToTodDCM.Multiply(posVector, posVector);
        ModToTodDCM.Multiply(velVector, velVector);
    } else {
        cout << "MURALInputProcessor(Orbit Conversion): Did not find JPL File.  Assuming vector is already in True of Date" << NEW_LINE;
    }

    // Create data object for GenEng with the osculating elements
    A3::OrbitElKeplerian oscElements(A3::TimeJ2000(epochTime_a), posVector, velVector);
    A3::OrbitElKeplerian meanElements;

    // Call the GenEng routine to convert from osculating to mean
    try {
        A3::osculatingToMean(oscElements, meanElements);
    } catch(Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "A3::osculatingToMean(const SpaceVector&, const SpaceVector&)");
        throw;
    }

    // Extract the Mean Elements back to the MURAL attributes
    inclination_a = meanElements.GetInclination();
    rightAscension_a = meanElements.GetRAAscendingNode();
    semiMajorAxis_a = meanElements.GetSemiMajorAxis();
    eccentricity_a = meanElements.GetEccentricity();
    argumentOfPerigee_a = meanElements.GetArgumentOfPeriapsis();
    trueAnomaly_a = meanElements.GetTrueAnomaly();
    // Convert from SATSIM (nmi) units to MURAL units (meters)
    // MURAL angle units are the same as GenEng (radians)
    semiMajorAxis_a *= NMI_TO_METERS;
}

//------------------------------------------------------------------------------
//      This function returns a specified orbital anomaly given any other
//      orbital anomaly.  Input and output anomalies may be in either radians or
//      degrees.  All returned output anomalies will be in the range 0 to 2*pi
//      radians or 0 to 360 degrees.
//      This function may also be used to adjust any angle in radians to the
//      range 0 to 2*pi (icod = 0).
//
//      Based on a FORTRAN function by A. Kelch.
//
//      Calling Sequence:  double Anomaly = anomaly(icod, eccen, animp);
//
//      SYMBOL   I/O   TYPE    UNITS   DESCRIPTION
//      icod      I    int             Anomaly conversion type code:
//                   Code    Input   Output    Units
//                   0       any     any       radians
//                   1       true    eccen     radians
//                   2       eccen   mean      radians
//                   3       true    mean      radians
//                   4       mean    eccen     radians
//                   5       eccen   true      radians
//                   6       mean    true      radians
//                   -1      true    eccen     degrees
//                   -2      eccen   mean      degrees
//                   -3      true    mean      degrees
//                   -4      mean    eccen     degrees
//                   -5      eccen   true      degrees
//                   -6      mean    true      degrees
//      eccen     I    int             Orbital eccentricity (0 <= eccen < 1)
//      anminp    I   double   deg/rad Input anomaly angle
//
//------------------------------------------------------------------------------

double
Orbit::AnomalyConverter(double inputAnomaly,
                        const ANOMALY_CONVERSION_TYPE &type,
                        double eccentricity)
{
    const int  maxConversionTypes = 7;
    const int  maxConversionOptions = 3;
    const int  algorithmFlag[maxConversionOptions][maxConversionTypes]
    = {{0, 0, 0, 0,-1, 0,-1},{0, 1, 0, 1, 0,-1,-1},{0, 0, 1, 1, 0, 0, 0} };
    const int  meanToEccentric = 0;
    const int  meanToFromTrue = 1;
    const int  eccentricToMean = 2;
    const int  MAX_ITERATIONS = 10;
    double       outputAnomaly = -1.0;
    const double DELMIN = 1.0e-7;
    const double fullCircle = UnitData::GetStoreFullCircle();

    // initialize internal code

    // check validity of input code and eccentricity
    if ((type >= ANY_TO_ANY) && (type <= MEAN_TO_TRUE)
            && (eccentricity >= 0.0) && (eccentricity < 1.0)) {
        double term = 0.00;
        double temp = 0.00;
        outputAnomaly = inputAnomaly;
        // ensure the correct range for input anomaly
        outputAnomaly = SimpleMath::Remainder(outputAnomaly + (3.0 * fullCircle), fullCircle);

        // if mean to eccentric anomaly conversion is required
        if (algorithmFlag[meanToEccentric][type] != 0) {
            int iteration = 0;
            iteration = 0;
            term = outputAnomaly;
            outputAnomaly = term + eccentricity*sin(term);

            do {
                ++iteration;
                temp = outputAnomaly;
                outputAnomaly = outputAnomaly - (outputAnomaly
                                                 - eccentricity * sin(outputAnomaly) - term)
                                / (1.0 - eccentricity * cos(outputAnomaly));
            } while ( (iteration < MAX_ITERATIONS) &&

                      (fabs(outputAnomaly - temp) > DELMIN) );
        }

        // if true to eccentric or eccentric to true conversion is required
        if (algorithmFlag[meanToFromTrue][type] != 0) {
            if (algorithmFlag[meanToFromTrue][type] > 0) {
                term = (1.0 - eccentricity)/(1.0 + eccentricity);
            } else {
                term = (1.0 + eccentricity)/(1.0 - eccentricity);
            }

            term = SimpleMath::SquareRoot(term);
            temp = outputAnomaly - PI;

            if (fabs(temp) < 1.0e-5) {
                outputAnomaly = temp/term - (temp >= 0.0 ? PI : -PI);
            } else {
                outputAnomaly = 2.0 * SimpleMath::ArcTangent(term * tan(0.5 * outputAnomaly));
            }
        }

        // if eccentric to mean conversion is required
        if (algorithmFlag[eccentricToMean][type] != 0) {
            outputAnomaly = outputAnomaly - eccentricity * sin(outputAnomaly);
        }

        // ensure the correct range for output anomaly
        if (outputAnomaly < 0.0) {
            outputAnomaly = outputAnomaly + fullCircle;
        }
    }

    return(outputAnomaly);
}

void
Orbit::DetermineRevolutionTimes()
{
    if (ptrRevolutionList_a != NULL) {
        ptrRevolutionList_a->Delete();
        delete ptrRevolutionList_a;
    }

    ptrRevolutionList_a = GenerateRevolutionList(revolutionBoundary_a);
    SanityCheckRevolutions();
    return;
}

LinkList<Revolution>*
Orbit::GenerateSouthPointRevolutionList() const
{
    return(GenerateRevolutionList(SOUTH_POINT));
}

LinkList<Revolution>*
Orbit::GenerateRevolutionList(const REVOLUTION_BOUNDARY &boundary) const
{
    int                 number = 1 + revolutionNumberOffset_a;
    int                  revolutionStart = 0;
    const int            numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    double                ascendingZeroCrossingLongitude = UNDEFINED_ANGLE;
    //double averageLongitudeDelta;
    bool                  undefinedZeroCrossingLongitude = false;
    Revolution           *ptrRevolution = NULL;
    LinkList<Revolution> *ptrRevolutionList = new LinkList<Revolution>();

    if (numberOfTimeSteps > 2) {
        int timeIndex = 0;
        int revolutionEnd = 0;
        double previousLatitude = 0.0;
        double currentLatitude = 0.0;
        bool previouslyAscending = false;
        previousLatitude = GetLatitude(timeIndex);

        if (previousLatitude == 0.00) {
            ascendingZeroCrossingLongitude = GetLongitude(timeIndex);
        }

        currentLatitude = GetLatitude(++timeIndex);

        if (currentLatitude > previousLatitude) {
            previouslyAscending = true;
        } else {
            ascendingZeroCrossingLongitude = UNDEFINED_ANGLE;
        }

        ++timeIndex;
        previousLatitude = currentLatitude;

        while (timeIndex < numberOfTimeSteps) {
            currentLatitude = GetLatitude(timeIndex);

            if (AtRevolutionBoundary(boundary, previouslyAscending, previousLatitude,
                                     currentLatitude) == true) {
                if ((boundary == ASCENDING_NODE) || (boundary == DESCENDING_NODE)) {
                    revolutionEnd = timeIndex - 1;
                } else {
                    revolutionEnd = timeIndex - 2;
                }

                ptrRevolution = new Revolution(number, revolutionStart, revolutionEnd,
                                               ascendingZeroCrossingLongitude);
                ptrRevolutionList->Append(ptrRevolution, revolutionStart);
                ptrRevolution = NULL;
                ++number;

                if (ascendingZeroCrossingLongitude == UNDEFINED_ANGLE) {
                    undefinedZeroCrossingLongitude = true;
                } else {
                    ascendingZeroCrossingLongitude = UNDEFINED_ANGLE;
                }

                revolutionStart = revolutionEnd + 1;
            }

            if (currentLatitude >= previousLatitude) {
                previouslyAscending = true;

                if ((previousLatitude < 0.0) && (currentLatitude >= 0.0)) {
                    ascendingZeroCrossingLongitude = GetLongitude(timeIndex);
                }
            } else {
                previouslyAscending = false;
            }

            previousLatitude = currentLatitude;
            ++timeIndex;
        }
    }

    ptrRevolution = new Revolution(number, revolutionStart, (numberOfTimeSteps - 1),
                                   ascendingZeroCrossingLongitude);
    ptrRevolutionList->Append(ptrRevolution, revolutionStart);

    if ( (ascendingZeroCrossingLongitude == UNDEFINED_ANGLE) ||
            (undefinedZeroCrossingLongitude == true) ) {
        CalculateZeroCrossingLongitudes(ptrRevolutionList, boundary);
    }

    return(ptrRevolutionList);
}

void
Orbit::CalculateZeroCrossingLongitudes(LinkList<Revolution> *ptrRevolutionList,
                                       const REVOLUTION_BOUNDARY &boundary) const
{
    double deltaLongitude;
    double       crossingLongitude = UNDEFINED_ANGLE;
    double       otherCrossingLongitude = 0.0;
    double       fullRevDeltaLongitude = 0.0;
    const double quarterCircle = UnitData::GetStoreQuarterCircle();
    const double halfCircle = 2.0 * quarterCircle;
    const double fullCircle = 4.0 * quarterCircle;
    const double mu = Earth::GetMU(UnitData::GetInternalDistanceUnits(), SECONDS);
    const double orbitPeriod = fullCircle / SimpleMath::SquareRoot(mu / GetSemiMajorAxisCubed());
    Revolution  *ptrOtherRevolution = NULL;
    ListIterator<Revolution> revolutionListIter(ptrRevolutionList);
    Revolution              *ptrRevolution = revolutionListIter.First();
    deltaLongitude = -fullCircle * (orbitPeriod / Earth::GetPeriod());

    if (inclination_a < quarterCircle) {
        fullRevDeltaLongitude = fullCircle + deltaLongitude;
    } else {
        fullRevDeltaLongitude = fullCircle - deltaLongitude;
    }

    if (ptrRevolution != NULL) {
        crossingLongitude = ptrRevolution->GetAscendingZeroCrossingLongitude();

        if (crossingLongitude == UNDEFINED_ANGLE) {
            ptrOtherRevolution = revolutionListIter.Next();

            if (ptrOtherRevolution != NULL) {
                otherCrossingLongitude = ptrOtherRevolution->GetAscendingZeroCrossingLongitude();

                if (otherCrossingLongitude != UNDEFINED_ANGLE) {
                    crossingLongitude = otherCrossingLongitude - deltaLongitude;
                }
            }

            if (crossingLongitude == UNDEFINED_ANGLE) {
                otherCrossingLongitude = GetLongitude(ptrRevolution->GetEndTime());

                if (boundary == SOUTH_POINT) {
                    if (inclination_a < quarterCircle) {
                        crossingLongitude = otherCrossingLongitude - (0.75 * fullRevDeltaLongitude);
                    } else {
                        crossingLongitude = otherCrossingLongitude + (0.75 * fullRevDeltaLongitude);
                    }
                } else if (boundary == ASCENDING_NODE) {
                    if (inclination_a < quarterCircle) {
                        crossingLongitude = otherCrossingLongitude - fullRevDeltaLongitude;
                    } else {
                        crossingLongitude = otherCrossingLongitude + fullRevDeltaLongitude;
                    }
                } else if (boundary == NORTH_POINT) {
                    if (inclination_a < quarterCircle) {
                        crossingLongitude = otherCrossingLongitude - (0.25 * fullRevDeltaLongitude);
                    } else {
                        crossingLongitude = otherCrossingLongitude + (0.25 * fullRevDeltaLongitude);
                    }
                } else if (boundary == DESCENDING_NODE) {
                    if (inclination_a < quarterCircle) {
                        crossingLongitude = otherCrossingLongitude - (0.5 * fullRevDeltaLongitude);
                    } else {
                        crossingLongitude = otherCrossingLongitude + (0.5 * fullRevDeltaLongitude);
                    }
                }
            }

            if (crossingLongitude < -halfCircle) {
                crossingLongitude += fullCircle;
            } else if (crossingLongitude > halfCircle) {
                crossingLongitude -= fullCircle;
            }

            ptrRevolution->SetAscendingZeroCrossingLongitude(crossingLongitude);
        }
    }

    ptrRevolution = revolutionListIter.Last();

    if (ptrRevolution != NULL) {
        crossingLongitude = ptrRevolution->GetAscendingZeroCrossingLongitude();

        if (crossingLongitude == UNDEFINED_ANGLE) {
            ptrOtherRevolution = revolutionListIter.Previous();

            if (ptrOtherRevolution != NULL) {
                otherCrossingLongitude = ptrOtherRevolution->GetAscendingZeroCrossingLongitude();

                if (otherCrossingLongitude != UNDEFINED_ANGLE) {
                    crossingLongitude = otherCrossingLongitude + deltaLongitude;
                }
            }

            if (crossingLongitude == UNDEFINED_ANGLE) {
                otherCrossingLongitude = GetLongitude(ptrRevolution->GetStartTime());

                if (boundary == SOUTH_POINT) {
                    if (inclination_a < quarterCircle) {
                        crossingLongitude = otherCrossingLongitude + (0.25 * fullRevDeltaLongitude);
                    } else {
                        crossingLongitude = otherCrossingLongitude - (0.25 * fullRevDeltaLongitude);
                    }
                } else if (boundary == ASCENDING_NODE) {
                    crossingLongitude = otherCrossingLongitude;
                } else if (boundary == NORTH_POINT) {
                    if (inclination_a < quarterCircle) {
                        crossingLongitude = otherCrossingLongitude + (0.75 * fullRevDeltaLongitude);
                    } else {
                        crossingLongitude = otherCrossingLongitude - (0.75 * fullRevDeltaLongitude);
                    }
                } else if (boundary == DESCENDING_NODE) {
                    if (inclination_a < quarterCircle) {
                        crossingLongitude = otherCrossingLongitude + (0.5 * fullRevDeltaLongitude);
                    } else {
                        crossingLongitude = otherCrossingLongitude - (0.5 * fullRevDeltaLongitude);
                    }
                }
            }

            if (crossingLongitude > halfCircle) {
                crossingLongitude -= fullCircle;
            } else if (crossingLongitude < -halfCircle) {
                crossingLongitude += fullCircle;
            }

            ptrRevolution->SetAscendingZeroCrossingLongitude(crossingLongitude);
        }
    }

    return;
}

int
Orbit::GetOrbitPeriod() const
{
    const double mu = Earth::GetMU(UnitData::GetInternalDistanceUnits(),
                                   UnitData::GetInternalTimeUnits());
    const double orbitPeriod = UnitData::GetStoreFullCircle()
                               / SimpleMath::SquareRoot(mu / GetSemiMajorAxisCubed());
    return(SimpleMath::Round(orbitPeriod));
}

bool
Orbit::AtRevolutionBoundary(const REVOLUTION_BOUNDARY &boundary,
                            const bool   &previouslyAscending,
                            double previousLatitude,
                            double currentLatitude)
{
    bool atBoundary = false;

    if (boundary == SOUTH_POINT) {
        if ((previouslyAscending == false) && (currentLatitude > previousLatitude)) {
            atBoundary = true;
        }
    } else if (boundary == ASCENDING_NODE) {
        if ((previousLatitude < 0.0) && (currentLatitude >= 0.0)) {
            atBoundary = true;
        }
    } else if (boundary == NORTH_POINT) {
        if ((previouslyAscending == true) && (currentLatitude < previousLatitude)) {
            atBoundary = true;
        }
    } else if (boundary == DESCENDING_NODE) {
        if ((previousLatitude > 0.0) && (currentLatitude <= 0.0)) {
            atBoundary = true;
        }
    }

    return(atBoundary);
}

void
Orbit::PrintUpdatedOrbitFile(ofstream &updatedOrbitFile,
                             const string &userDesignator) const
{
    Orbit updatedOrbit = PositionUtility::UpdateOrbit(*this, TimePiece::GetStartTime(J2000));
    updatedOrbitFile << "New Orbit Elements for " << userDesignator << NEW_LINE;
    updatedOrbitFile << "   <POSITION_GENERATION_BEGIN>" << NEW_LINE;
    updatedOrbitFile << "      Position Generation Method (READ/READ_P470/J2J4)        : J2J4";
    updatedOrbitFile << NEW_LINE;
    updatedOrbitFile << "      Epoch Date                                              : ";
    updatedOrbitFile << TimePiece::GetStartDate() << NEW_LINE;
    updatedOrbitFile << "      Epoch Time                                              : ";
    updatedOrbitFile << TimePiece::GetStartClockTime() << NEW_LINE;
    updatedOrbitFile << "      Semi-Major Axis                                         : ";
    updatedOrbitFile << UnitData::GetOutputDistance(updatedOrbit.GetSemiMajorAxis()) << NEW_LINE;
    updatedOrbitFile << "      Eccentricity                                            : ";
    updatedOrbitFile << setprecision(6) << updatedOrbit.GetEccentricity() << NEW_LINE;
    updatedOrbitFile << "      Inclination                                             : ";
    updatedOrbitFile << UnitData::GetOutputAngle(updatedOrbit.GetInclination()) << NEW_LINE;
    updatedOrbitFile << "      Argument of Perigee                                     : ";
    updatedOrbitFile << UnitData::GetOutputAngle(updatedOrbit.GetArgumentOfPerigee()) << NEW_LINE;
    updatedOrbitFile << "      Right Ascension                                         : ";
    updatedOrbitFile << UnitData::GetOutputAngle(updatedOrbit.GetRightAscension()) << NEW_LINE;
    updatedOrbitFile << "      True Anomaly                                            : ";
    updatedOrbitFile << UnitData::GetOutputAngle(updatedOrbit.GetTrueAnomaly()) << NEW_LINE;
    //Added Mean Anomaly to Outputs - RAGUSA 7/20/2017
    //Line commented out because MURAL can  take in either True or Mean anomaly, since this file is used
    //for input, it is helpful to do this.
    updatedOrbitFile << "      #Mean Anomaly                                           : ";
    updatedOrbitFile << UnitData::GetOutputAngle(updatedOrbit.GetMeanAnomaly()) << NEW_LINE;
    updatedOrbitFile << "   <POSITION_GENERATION_STOP>" << NEW_LINE << NEW_LINE;
    return;
}

void
Orbit::SanityCheckRevolutions() const
{
    bool                     validRevolutions = false;
    ListIterator<Revolution> revolutionListIter(ptrRevolutionList_a);
    Revolution              *ptrRevolution = revolutionListIter.First();

    if (ptrRevolution != NULL) {
        if (ptrRevolution->GetStartTime() == 0) {
            int previousEnd = 0;
            validRevolutions = true;
            previousEnd = ptrRevolution->GetEndTime();
            //JOE:
            ptrRevolution = revolutionListIter.Next();

            while ( (ptrRevolution != NULL) &&
                    (validRevolutions == true) ) {
                if (ptrRevolution->GetStartTime() == (previousEnd + 1)) {
                    previousEnd = ptrRevolution->GetEndTime();
                } else {
                    validRevolutions = false;
                }

                ptrRevolution = revolutionListIter.Next();
            }

            ptrRevolution = revolutionListIter.Last();

            if ( (ptrRevolution != NULL) && (validRevolutions == true) ) {
                if (ptrRevolution->GetEndTime() != TimePiece::GetEndIndexTime()) {
                    validRevolutions = false;
                }
            }
        }
    }

    if (validRevolutions == false) {
        throw new InputException(GetClassName(), "SanityCheckRevolutions()");
    }

    return;
}

int
Orbit::GetRevolutionNumber(int timeIndex) const
{
    int                    revNumber = -1;
    ListIterator<Revolution> revListIter(ptrRevolutionList_a);
    Revolution              *ptrRevolution = revListIter.First();

    while ((ptrRevolution != NULL) && (revNumber < 0)) {
        if (ptrRevolution->IsDuringTimePeriod(timeIndex) == true) {
            revNumber = ptrRevolution->GetRevNumber();
        }

        ptrRevolution = revListIter.Next();
    }

    if (revNumber < 0) {
        throw new InputException(GetClassName(), "GetRevolutionNumber(int)",
                                 "Could not find revolution number");
    }

    return(revNumber);
}

int
Orbit::GetRevolutionIndex(int timeIndex) const
{
    int                     revIndex = 0;
    bool                     foundRev = false;
    ListIterator<Revolution> revListIter(ptrRevolutionList_a);
    Revolution              *ptrRevolution = revListIter.First();

    while ((ptrRevolution != NULL) && (foundRev == false)) {
        if (ptrRevolution->IsDuringTimePeriod(timeIndex) == true) {
            foundRev = true;
        } else {
            ++revIndex;
            ptrRevolution = revListIter.Next();
        }
    }

    if (foundRev == false) {
        throw new InputException(GetClassName(), "GetRevolutionIndex(int)",
                                 "Could not find revolution index");
    }

    return(revIndex);
}

int
Orbit::GetRevStartTime(int revIndex) const
{
    int revStartTime = -1;

    if (ptrRevolutionList_a != NULL) {
        if ((revIndex >= 0) && (revIndex < ptrRevolutionList_a->Size())) {
            revStartTime = (ptrRevolutionList_a->GetObjectAt(revIndex))->GetStartTime();
        }
    }

    return(revStartTime);
}

int
Orbit::GetRevEndTime(int revIndex) const
{
    int revEndTime = -1;

    if (ptrRevolutionList_a != NULL) {
        if ((revIndex >= 0) && (revIndex < ptrRevolutionList_a->Size())) {
            revEndTime = (ptrRevolutionList_a->GetObjectAt(revIndex))->GetEndTime();
        }
    }

    return(revEndTime);
}

double
Orbit::GetTrueAnomaly(int timeIndex,
                      double additionalSeconds) const
{
    double       trueAnomaly = 0.0;
    const double FULL_CIRCLE = UnitData::GetStoreFullCircle();
    SpaceVector  currentPosition = GetInterimPosition(timeIndex, additionalSeconds);

    try {
        if (eccentricity_a > ECCENTRICITY_TOLERANCE) {
            // if non-circular, base TA on the radial distance at this time
            double radialDistance;
            bool afterApogee = false;
            radialDistance = currentPosition.GetMagnitude();
            trueAnomaly = SimpleMath::ArcCosine(((GetSemiLatusRectum() / radialDistance) - 1.0)
                                                / eccentricity_a);

            if (timeIndex == 0) {
                if (GetPosition(timeIndex + 1).GetMagnitude() < radialDistance) {
                    afterApogee = true;
                }
            } else if (GetPosition(timeIndex - 1).GetMagnitude() > radialDistance) {
                afterApogee = true;
            }

            // if on the 'descending' side of the orbit, transfer TA to that side
            if (afterApogee == true) {
                trueAnomaly = FULL_CIRCLE - trueAnomaly;
            }
        } else {
            // if circular, base TA on the RAAN
            trueAnomaly = currentPosition.GetLongitude() - rightAscension_a;

            // assure that TA is positive (between 0 and 360)
            while (trueAnomaly < 0.0) {
                trueAnomaly += FULL_CIRCLE;
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetTrueAnomaly(int, const double&)");
        throw;
    }

    return(trueAnomaly);
}

/* ********************************************************************************** */

STATE_VECTOR_FORMAT
Orbit::ConvertStateVectorFormat(const string &valueString)
{
    STATE_VECTOR_FORMAT stateVectorFormat = UNKNOWN_FORMAT;

    if (valueString == "MEAN") {
        stateVectorFormat = MEAN;
    } else if (valueString == "OSC") {
        stateVectorFormat = OSCULATING;
    } else if (valueString == "OSCULATING") {
        stateVectorFormat = OSCULATING;
    }

    return(stateVectorFormat);
}

/* ***************************************************************************** */

string
Orbit::GetStateVectorFormatString(const STATE_VECTOR_FORMAT &stateVectorFormat)
{
    string stateVectorFormatStr = "UNKNOWN";

    if (stateVectorFormat == MEAN) {
        stateVectorFormatStr = "MEAN";
    } else if (stateVectorFormat == OSCULATING) {
        stateVectorFormatStr = "OSCULATING";
    }

    return(stateVectorFormatStr);
}

/* ***************************************************************************** */

Orbit::REVOLUTION_BOUNDARY
Orbit::ConvertRevolutionBoundary(const string &inputValueString)
{
    REVOLUTION_BOUNDARY boundary = UNKNOWN_BOUNDARY;

    if (inputValueString == "SOUTH_POINT") {
        boundary = SOUTH_POINT;
    } else if (inputValueString == "ASCENDING_NODE") {
        boundary = ASCENDING_NODE;
    } else if (inputValueString == "NORTH_POINT") {
        boundary = NORTH_POINT;
    } else if (inputValueString == "DESCENDING_NODE") {
        boundary = DESCENDING_NODE;
    }

    return(boundary);
}

/* ******************************************************************************************* */

Orbit::~Orbit()
{
    if (ptrRevolutionList_a != NULL) {
        ptrRevolutionList_a->Delete();
        delete ptrRevolutionList_a;
        ptrRevolutionList_a = NULL;
    }

    return;
}

