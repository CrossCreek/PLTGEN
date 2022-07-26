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
 * Orbit.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ORBIT_H
#define ORBIT_H "Orbit"

#include <iostream>
#include "mural/modules/pg/Journey.h"

using namespace std;

/**
 * Child class of Journey which holds data used to calculate an element which
 * has an orbit
 *
 * Name                      Description (units)
 * -------------             ---------------------------------------------------
 * revolutionNumberOffset_a  offset used when determining revolution numbers
 * epochTime_a               time of epoch in J2000 seconds
 * semiMajorAxis_a           semi-major axis
 * eccentricity_a            eccentricity
 * inclination_a             inclination
 * rightAscension_a          right ascension of ascending node (RAAN)
 * argumentOfPerigee_a       argument of perigee (AOP)
 * trueAnomaly_a             true anomaly
 * meanAnomaly_a             mean anomaly
 * nodalPrecessionRate_a     motion of the RAAN due to a satellite experiencing
 *                             1st order J2 secular perturbation
 * apsidalRotationRate_a     motion of the AOP due to a satellite experiencing
 *                             1st order J2 secular perturbation
 * anomalisticMeanMotion_a   average motion of a satellite in its orbit plane
 *                             experiencing 1st order J2 secular perturbation
 * revolutionBoundary_a      point in the orbit used for determining revolutions
 * ptrRevolutionList_a*      list of revolutions based on revolutionBoundary_a
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class Orbit : public Journey
{
public:

    enum ANOMALY_CONVERSION_TYPE {
        ANY_TO_ANY = 0,
        TRUE_TO_ECCENTRIC = 1,
        ECCENTRIC_TO_MEAN = 2,
        TRUE_TO_MEAN = 3,
        MEAN_TO_ECCENTRIC = 4,
        ECCENTRIC_TO_TRUE = 5,
        MEAN_TO_TRUE = 6
    };

    enum REVOLUTION_BOUNDARY {
        UNKNOWN_BOUNDARY = -1,
        SOUTH_POINT      = 0,
        ASCENDING_NODE   = 1,
        NORTH_POINT      = 2,
        DESCENDING_NODE  = 3
    };

    Orbit();
    Orbit(double epochTime,
          double semiMajorAxis,
          double eccentricity,
          double inclination,
          double rightAscension,
          double argumentOfPerigee,
          double trueAnomaly,
          bool jplRequired);
    Orbit(const Orbit &copyMe);
    virtual ~Orbit();

    Orbit&  operator =  (const Orbit &copyMe);
    bool    operator == (const Orbit &compareMe);

    static double AnomalyConverter(double anomaly,
                                   const ANOMALY_CONVERSION_TYPE &type,
                                   double eccentricity);

    /**
     * Calcuates J2J3 Perturbations of the orbit :
     * anomalisticMeanMotion_a, nodalPrecessionRate_a, and apsidalRotationRate_a
     *
     * Based on General Engineering's SpatialMJ2J4 Class.
     */
    void CalculatePerturbations();

    /**
     * Calculates the classical elements using the first time step postion and velocity.
     * Calls the method CalculateClassicalElements with the stateVectorFormat_a attribute.
     *
     * @param positionVector the position vector to convert
     * @param velocityVector the velocity vector to convert
     */
    void CalculateClassicalElements();

    /**
     * This is the main entry method to determine which method to use to convert the initial state vector.
     * Depending if the vector is a 'mean' or 'osculating' vector.

     * @param positionVector the position vector to convert
     * @param velocityVector the velocity vector to convert
     * @param vectorFormat the state vector format (osculating or mean)
     */
    void CalculateClassicalElements(const SpaceVector &positionVector,
                                    const SpaceVector &velocityVector,
                                    STATE_VECTOR_FORMAT vectorFormat);

    void   DetermineRevolutionTimes();
    void   PrintAttributes(ostream &outfile) const;
    void   PrintUpdatedOrbitFile(ofstream &updatedOrbitFile,
                                 const string &userDesignator) const;

    int  GetRevolutionNumber(int timeIndex) const;

    int   GetOrbitPeriod() const;
    int   GetRevolutionIndex(int timeIndex) const;
    int   GetRevStartTime(int revIndex) const;
    int   GetRevEndTime(int revIndex) const;

    double GetTrueAnomaly(int timeIndex,
                          double additionalSeconds) const;

    LinkList<Revolution>* GenerateSouthPointRevolutionList() const;

    inline void   SetEpochTime(double j2000Time);
    inline void   SetSemiMajorAxis(double newSemiMajorAxis);
    inline void   SetEccentricity(double newEccentricity);
    inline void   SetInclination(double newInclination);
    inline void   SetRightAscension(double newRightAscension);
    inline void   SetArgumentOfPerigee(double argumentOfPerigee);
    inline void   SetTrueAnomaly(double trueAnomaly);
    inline void   SetMeanAnomaly(double meanAnomaly);
    inline void   SetNodalPrecessionRate(double rate);
    inline void   SetApsidalRotationRate(double rate);
    inline void   SetAnomalisticMeanMotion(double meanMotion);
    inline void   SetRevolutionBoundary(const REVOLUTION_BOUNDARY &boundary);
    inline void   SetRevolutionNumberOffset(int offset);

    inline void   SetStateVectorFormat(const STATE_VECTOR_FORMAT &stateVectorFormat);

    inline int  GetRevolutionNumberOffset() const;

    inline int   GetNumberOfRevs() const;

    inline double GetEpochTime() const;
    inline double GetSemiMajorAxis() const;
    inline double GetEccentricity() const;
    inline double GetInclination() const;
    inline double GetRightAscension() const;
    inline double GetArgumentOfPerigee() const;
    inline double GetTrueAnomaly() const;
    inline double GetMeanAnomaly() const;
    inline double GetNodalPrecessionRate() const;
    inline double GetApsidalRotationRate() const;
    inline double GetAnomalisticMeanMotion() const;
    inline double GetSemiMajorAxisCubed() const;
    inline double GetSemiLatusRectum() const;

    inline bool   UsesSouthPointBoundary() const;

    inline string GetRevolutionBoundaryString() const;

    inline const REVOLUTION_BOUNDARY GetRevolutionBoundary() const;

    inline const LinkList<Revolution>* GetRevolutionList() const;

    inline STATE_VECTOR_FORMAT GetStateVectorFormat() const;


    static REVOLUTION_BOUNDARY ConvertRevolutionBoundary(const string &inputValueString);
    static STATE_VECTOR_FORMAT ConvertStateVectorFormat(const string &valueString);

    /**
     * Returns a State vector format from the string from the run control
     *
     * @param valueString the Mean/Osculating string
     * @return STATE_VECTOR_FORMAT
     */
    static string GetStateVectorFormatString(const STATE_VECTOR_FORMAT &stateVectorFormat);

protected:

private:

    static bool AtRevolutionBoundary(const REVOLUTION_BOUNDARY &boundary,
                                     const bool   &previouslyAscending,
                                     double previousLatitude,
                                     double currentLatitude);


    void CalculateZeroCrossingLongitudes(LinkList<Revolution> *ptrRevolutionList,
                                         const REVOLUTION_BOUNDARY &boundary) const;

    void ConvertMeanVectors(const SpaceVector &positionVector,
                            const SpaceVector &velocityVector);
    void ConvertOsculatingVectors(const SpaceVector &positionVector,
                                  const SpaceVector &velocityVector);

    void SanityCheckRevolutions() const;

    LinkList<Revolution>* GenerateRevolutionList(const REVOLUTION_BOUNDARY
            &boundary) const;

    inline static string GetClassName();

    static const double   J2_s;
    static const double   J4_s;

    string                opsDesignator_a;
    bool                  jplRequired_a;
    int                   revolutionNumberOffset_a;
    double                epochTime_a;
    double                semiMajorAxis_a;
    double                eccentricity_a;
    double                inclination_a;
    double                rightAscension_a;
    double                argumentOfPerigee_a;
    double                trueAnomaly_a;
    double                meanAnomaly_a;
    double                nodalPrecessionRate_a;
    double                apsidalRotationRate_a;
    double                anomalisticMeanMotion_a;
    STATE_VECTOR_FORMAT   stateVectorFormat_a;
    REVOLUTION_BOUNDARY   revolutionBoundary_a;
    LinkList<Revolution> *ptrRevolutionList_a;
};

inline
int
Orbit::GetRevolutionNumberOffset() const
{
    return(revolutionNumberOffset_a);
}

inline
int
Orbit::GetNumberOfRevs() const
{
    int numberOfRevs = 0;

    if (ptrRevolutionList_a != NULL) {
        numberOfRevs = ptrRevolutionList_a->Size();
    }

    return(numberOfRevs);
}

inline
double
Orbit::GetEpochTime() const
{
    return(epochTime_a);
}

inline
double
Orbit::GetSemiMajorAxis() const
{
    return(semiMajorAxis_a);
}

inline
double
Orbit::GetEccentricity() const
{
    return(eccentricity_a);
}

inline
double
Orbit::GetInclination() const
{
    return(inclination_a);
}

inline
double
Orbit::GetRightAscension() const
{
    return(rightAscension_a);
}

inline
double
Orbit::GetArgumentOfPerigee() const
{
    return(argumentOfPerigee_a);
}

inline
double
Orbit::GetTrueAnomaly() const
{
    return(trueAnomaly_a);
}

inline
double
Orbit::GetMeanAnomaly() const
{
    return(meanAnomaly_a);
}

inline
double
Orbit::GetNodalPrecessionRate() const
{
    return(nodalPrecessionRate_a);
}

inline
double
Orbit::GetApsidalRotationRate() const
{
    return(apsidalRotationRate_a);
}

inline
double
Orbit::GetAnomalisticMeanMotion() const
{
    return(anomalisticMeanMotion_a);
}

inline
double
Orbit::GetSemiLatusRectum() const
{
    return(semiMajorAxis_a * (1.0 - (eccentricity_a * eccentricity_a)));
}

inline
double
Orbit::GetSemiMajorAxisCubed() const
{
    return(semiMajorAxis_a * semiMajorAxis_a * semiMajorAxis_a);
}

inline
bool
Orbit::UsesSouthPointBoundary() const
{
    return(revolutionBoundary_a == SOUTH_POINT);
}

inline
const Orbit::REVOLUTION_BOUNDARY
Orbit::GetRevolutionBoundary() const
{
    return(revolutionBoundary_a);
}

inline
STATE_VECTOR_FORMAT
Orbit::GetStateVectorFormat() const
{
    return(stateVectorFormat_a);
}

inline
const LinkList<Revolution>*
Orbit::GetRevolutionList() const
{
    return(ptrRevolutionList_a);
}

inline
string
Orbit::GetRevolutionBoundaryString() const
{
    string boundaryString = "UNKNOWN";

    if (revolutionBoundary_a == SOUTH_POINT) {
        boundaryString = "SOUTH_POINT";
    } else if (revolutionBoundary_a == ASCENDING_NODE) {
        boundaryString = "ASCENDING_NODE";
    } else if (revolutionBoundary_a == NORTH_POINT) {
        boundaryString = "NORTH_POINT";
    } else if (revolutionBoundary_a == DESCENDING_NODE) {
        boundaryString = "DESCENDING_NODE";
    }

    return(boundaryString);
}

inline
void
Orbit::SetEpochTime(double j2000Time)
{
    epochTime_a = j2000Time;
    return;
}

inline
void
Orbit::SetSemiMajorAxis(double newSemiMajorAxis)
{
    semiMajorAxis_a = newSemiMajorAxis;
    return;
}

inline
void
Orbit::SetEccentricity(double newEccentricity)
{
    eccentricity_a = newEccentricity;
    return;
}

inline
void
Orbit::SetInclination(double newInclination)
{
    inclination_a = newInclination;
    return;
}

inline
void
Orbit::SetRightAscension(double newRightAscension)
{
    rightAscension_a = newRightAscension;
    return;
}

inline
void
Orbit::SetArgumentOfPerigee(double argumentOfPerigee)
{
    argumentOfPerigee_a = argumentOfPerigee;
    return;
}

inline
void
Orbit::SetTrueAnomaly(double trueAnomaly)
{
    trueAnomaly_a = trueAnomaly;
}

inline
void
Orbit::SetMeanAnomaly(double meanAnomaly)
{
    meanAnomaly_a = meanAnomaly;
}

inline
void
Orbit::SetNodalPrecessionRate(double rate)
{
    nodalPrecessionRate_a = rate;
    return;
}

inline
void
Orbit::SetApsidalRotationRate(double rate)
{
    apsidalRotationRate_a = rate;
    return;
}

inline
void
Orbit::SetAnomalisticMeanMotion(double meanMotion)
{
    anomalisticMeanMotion_a = meanMotion;
    return;
}

inline
void
Orbit::SetRevolutionBoundary(const REVOLUTION_BOUNDARY &boundary)
{
    revolutionBoundary_a = boundary;
    return;
}

inline
void
Orbit::SetRevolutionNumberOffset(int offset)
{
    revolutionNumberOffset_a = offset;
}

inline
void
Orbit::SetStateVectorFormat(const STATE_VECTOR_FORMAT &stateVectorFormat)
{
    stateVectorFormat_a = stateVectorFormat;
}

inline
string
Orbit::GetClassName()
{
    return(ORBIT_H);
}

#endif
