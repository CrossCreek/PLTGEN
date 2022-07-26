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
 * Journey.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef JOURNEY_H
#define JOURNEY_H "Journey"

#include <fstream>
#include <vector>

#include "mural/MURALTypes.h"


#include "general/array/LinkList.h"
#include "general/utility/Constant.h"

using namespace std;

class InputDataElement;
class Revolution;
class SpaceVector;
class PositionState;

/**
 * Base Class which holds the states of an element
 *
 * Name                        Description (units)
 * -------------               -------------------------------------------------
 * positionGenerationMethod_a  the POSITION_METHOD used for generating positions
 * ptrPositionStateArray_a*    an array of pointer(s) to PositionState
 *                               instances for each time index (non-STATIONARY)
 *                               or single instance (STATIONARY)
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class Journey
{
public:

    enum POSITION_METHOD {
        UNKNOWN_POSITION_METHOD = -1,
        READ_MURAL = 0,
        READ_P470 = 1,
        READ_OPS = 2,
        J2J4 = 3,
        STATIONARY = 4,
        MOVING = 5
    };

    Journey();
    Journey(const Journey &copyMe);
    virtual ~Journey();

    Journey& operator =  (const Journey &copyMe);
    bool     operator == (const Journey &compareMe);

    void   CheckPositionStates(const string &designator);
    void   SetPositionState(int timeIndex,
                            const shared_ptr<PositionState> &ptrPositionState);
    void   SetPositionGenerationMethod(POSITION_METHOD positionMethod);


    int    GetNumberOfPositionStates() const;

    double GetLatitude(int timeIndex) const;
    double GetLatitude(int timeIndex,
                       const ANGLE_UNIT &returnAngleUnit) const;
    double GetLongitude(int timeIndex) const;
    double GetLongitude(int timeIndex,
                        const ANGLE_UNIT &returnAngleUnit) const;
    double GetAltitude(int timeIndex) const;
    double GetAltitude(int timeIndex,
                       const DISTANCE_UNIT &returnDistanceUnit) const;

    SpaceVector GetPosition(int timeIndex) const;
    SpaceVector GetPosition(int timeIndex,
                            const REFERENCE_FRAME &returnFrame) const;
    SpaceVector GetInterimPosition(int timeIndex,
                                   double additionalSeconds) const;
    SpaceVector GetVelocity(int timeIndex) const;
    SpaceVector GetVelocity(int timeIndex,
                            const REFERENCE_FRAME &returnFrame) const;
    SpaceVector GetInterimVelocity(int timeIndex,
                                   double additionalSeconds) const;

    shared_ptr<PositionState> GetPositionStateAt(int timeIndex) const;

    virtual inline int GetNumberOfRevs() const;
    virtual inline double GetSemiMajorAxis() const;
    virtual inline double GetEccentricity() const;
    virtual inline double GetInclination() const;
    virtual inline double GetRightAscension() const;
    virtual inline double GetArgumentOfPerigee() const;
    virtual inline double GetTrueAnomaly() const;
    virtual inline double GetNodalPrecessionRate() const;
    virtual inline double GetApsidalRotationRate() const;
    virtual inline double GetAnomalisticMeanMotion() const;
    virtual inline double GetSemiLatusRectum() const;
    virtual inline bool   UsesSouthPointBoundary() const;
    virtual inline const LinkList<Revolution>* GetRevolutionList() const;
    virtual void   PrintAttributes(ostream &outfile) const;
    virtual void   PrintUpdatedOrbitFile(ofstream &updatedOrbitFile, const string &userDesignator) const;
    virtual int  GetRevolutionNumber(int timeIndex) const;
    virtual int   GetOrbitPeriod() const;
    virtual int   GetRevolutionIndex(int timeIndex) const;
    virtual int   GetRevStartTime(int revIndex) const;
    virtual int   GetRevEndTime(int revIndex) const;
    virtual double GetTrueAnomaly(int timeIndex, double additionalSeconds) const;
    virtual LinkList<Revolution>* GenerateSouthPointRevolutionList() const;

    inline bool   IsStationaryPosition() const;

    inline const Journey::POSITION_METHOD GetGenerationMethod() const;

    /**
     * Method to conert from a string to a POSITION_METHOD enum
     *
     * @param &methodString string to convert
     */
    static POSITION_METHOD ConvertPositionMethod(const string &methodString);

    /**
     * Returns a Position Generation format from the string from the run control
     *
     * @param positionMethod the psotion generation enum to convert
     * @return POSITION_METHOD
     */
    static string GetGenerationMethodString(const POSITION_METHOD &positionMethod);
protected:

    POSITION_METHOD                             positionGenerationMethod_a;
    shared_ptr<MURALTypes::PositionStateVector> ptrPositionStateArray_a;

private:

    static inline string GetClassName();
};

inline
bool
Journey::IsStationaryPosition() const
{
    return(positionGenerationMethod_a == STATIONARY);
}

inline
int
Journey::GetNumberOfRevs() const
{
    return(0);
}

inline
double
Journey::GetSemiMajorAxis() const
{
    return(UNDEFINED_DISTANCE);
}

inline
double
Journey::GetEccentricity() const
{
    return(-1.0);
}

inline
double
Journey::GetInclination() const
{
    return(UNDEFINED_ANGLE);
}

inline
double
Journey::GetRightAscension() const
{
    return(UNDEFINED_ANGLE);
}

inline
double
Journey::GetArgumentOfPerigee() const
{
    return(UNDEFINED_ANGLE);
}

inline
double
Journey::GetTrueAnomaly() const
{
    return(UNDEFINED_ANGLE);
}

inline
double
Journey::GetNodalPrecessionRate() const
{
    return(0.0);
}

inline
double
Journey::GetApsidalRotationRate() const
{
    return(0.0);
}

inline
double
Journey::GetAnomalisticMeanMotion() const
{
    return(0.0);
}

inline
double
Journey::GetSemiLatusRectum() const
{
    return(0.0);
}

inline
bool
Journey::UsesSouthPointBoundary() const
{
    return(false);
}

inline
const Journey::POSITION_METHOD
Journey::GetGenerationMethod() const
{
    return(positionGenerationMethod_a);
}

inline
const LinkList<Revolution>*
Journey::GetRevolutionList() const
{
    return(NULL);
}

inline
string
Journey::GetClassName()
{
    return(JOURNEY_H);
}

#endif
