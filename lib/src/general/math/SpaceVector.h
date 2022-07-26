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
 * SpaceVector.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef SPACE_VECTOR_H
#define SPACE_VECTOR_H "SpaceVector"

#include <iostream>
#include "general/data/UnitData.h"

using namespace std;

/**
 * Basic class to represent a 3 projection vector of doubles. Represents a point
 *  within a coordinate system.
 *
 * @author Rob Lowry
 * @author Brian Fowler
 * @date 08/16/11
 *
 * Class Attribute Descriptions
 *
 *  Name                   Description (units)
 * -------------          ------------------------------------------
 * firstProjection_a      first projection (x-axis)
 * secondProjection_a     second projection (y-axis)
 * thirdProjection_a      third projection (z-axis)
 * referenceFrame_a       reference frame of this vector (ECF, ECI, LLA, PWQ)
 */

class SpaceVector
{
public:

    /**
     * Init Constructor
     *
     * @param frame reference frame to use
     */


    explicit SpaceVector(REFERENCE_FRAME frame = UnitData::GetInternalReferenceFrame());

    /**
     * Init Constructor
     *
     * @param firstProjection x-value
     * @param secondProjection y-value
     * @param thirdProjection z-value
     * @param frame reference frame to use (defaults to UnitData::InternalUnits)
     * @param angle angle unit to use (defaults to UnitData::InternalUnits)
     * @param distance distance unit to use (defaults to UnitData::InternalUnits)
     */
    SpaceVector(double firstProjection,
                double secondProjection,
                double thirdProjection,
                REFERENCE_FRAME frame = UnitData::GetInternalReferenceFrame(),
                ANGLE_UNIT angle = UnitData::GetInternalAngleUnits(),
                DISTANCE_UNIT distance = UnitData::GetInternalDistanceUnits());
    SpaceVector(const SpaceVector &copyMe);
    virtual ~SpaceVector();

    SpaceVector&    operator =  (const SpaceVector &copyMe);
    SpaceVector&    operator += (const SpaceVector &addMe);
    SpaceVector&    operator -= (const SpaceVector &subractMe);
    SpaceVector&    operator *= (double multiplyMe);
    SpaceVector&    operator /= (double divideByMe);
    SpaceVector     operator +  (const SpaceVector &addMe) const;
    SpaceVector     operator -  (const SpaceVector &subractMe) const;
    SpaceVector     operator *  (double multiplyMe) const;
    SpaceVector     operator /  (double divideByMe) const;
    double          operator *  (const SpaceVector &dotMe) const;
    bool            operator == (const SpaceVector &compareMe) const;
    bool            operator != (const SpaceVector &compareMe) const;
    bool            operator <  (const SpaceVector &compareMe) const;

    /**
     * Method to compare only the x,y,z values
     *
     * @return bool result
     */
    bool   CompareXYZ(const SpaceVector &compareMe) const;

    /**
     * Method to convert to the unit vector by divding by the magnitude
     */
    void   ConvertToUnitVector();

    /**
     * Method which zeros out all the x,y,z projections
     */
    void   EraseVector();

    /**
     * Computes the dot product with (this) and the argument
     *
     * @param &dotMe the vector to dot with
     */
    double DotProduct(const SpaceVector &dotMe) const;

    /**
     * Gets the separation angle between (this) and the argument
     *
     * @param &compareMe the vector to get the sep angle for
     * @param angle the units to convert to (defaults to the internal angle units)
     * @return double the separation angle
     */
    double GetSeparationAngle(const SpaceVector &compareMe,
                              ANGLE_UNIT angle = UnitData::GetInternalAngleUnits()) const;

    /**
     * Gets the separation distance between (this) and the argument
     *
     * @param &compareMe the vector to get the distance for
     * @param distance the units to convert to (defaults to the internal distance units)
     * @return double the separation distance
     */
    double GetSeparationDistance(const SpaceVector &compareMe,
                                 DISTANCE_UNIT distance = UnitData::GetInternalDistanceUnits()) const;

    /**
     * Gets the magnitude squared
     *
     * @return double magnitude squared
     */
    double GetMagnitudeSquared() const;

    /**
     * Gets the magnitude
     *
     * @return double magnitude
     */
    double GetMagnitude() const;

    /**
     * Gets the latitude
     *
     * @param angle the units to convert latitude to (defaults to internal angle units)
     * @return double latitude
     */
    double GetLatitude(ANGLE_UNIT angle = UnitData::GetInternalAngleUnits()) const;

    /**
     * Gets the longitude
     *
     * @param angle the units to convert longitude to (defaults to internal angle units)
     * @return double longitude
     */
    double GetLongitude(ANGLE_UNIT angle = UnitData::GetInternalAngleUnits()) const;

    /**
     * Gets the altitude
     *
     * @param distance the units to convert distance to (defaults to internal distance units)
     * @return double longitude
     */
    double GetAltitude(DISTANCE_UNIT distance = UnitData::GetInternalDistanceUnits()) const;

    double ProjectionOnto(const SpaceVector &vector2) const;

    SpaceVector ConvertTo(const REFERENCE_FRAME &toFrame,
                          int timeIndex = -1) const;
    SpaceVector ConvertTo(const REFERENCE_FRAME &toFrame,
                          double j2000Time) const;
    SpaceVector GetInputSpaceVector(int timeIndex = -1) const;
    SpaceVector GetOutputSpaceVector(int timeIndex = -1) const;
    SpaceVector GetStoreSpaceVector(int timeIndex = -1) const;
    SpaceVector CrossProduct(const SpaceVector &crossMe) const;
    SpaceVector GetUnitVector() const;
    SpaceVector Negate() const;
    SpaceVector ProjectIntoPlane(const SpaceVector &plane) const;
    SpaceVector ToEarthsSurface(int timeIndex = -1) const;
    SpaceVector TransformCoordinates(const SpaceVector &xFrame,
                                     const SpaceVector &yFrame,
                                     const SpaceVector &zFrame) const;

    void SetX(double newX);
    void SetY(double newY);
    void SetZ(double newZ);
    void Set(double firstProjection,
             double secondProjection,
             double thirdProjection);

    double GetX() const;
    double GetY() const;
    double GetZ() const;

    void SetReferenceFrame(const REFERENCE_FRAME &newFrame);

    REFERENCE_FRAME GetReferenceFrame() const;

protected:

private:

    static double AngleBetweenVectors(const SpaceVector &vector1,
                                      const SpaceVector &vector2,
                                      const ANGLE_UNIT &returnAngleUnits);
    static double DistanceBetweenVectors(const SpaceVector &vector1,
                                         const SpaceVector &vector2,
                                         const DISTANCE_UNIT &distance);
    static double DotProduct(const SpaceVector &vector1,
                             const SpaceVector &vector2);

    /**
     * General method to convert from a reference frame (ECF, ECI, LLA) to another.  Takes the inputVector
     * as an argument and returns the converted vector.
     *
     * This method only converts from ECF to ECI properly for position.  It does not include the angular
     * velocity component of the earth when converting a velocity vector from ECF to ECI.  The methods
     * on the PositionState Object account for this and should be used from ECF to ECI.
     *
     * The Base DCM for the postion conversion is:
     * theta = Greenwich Sidereal angle
     * a[0][0] = cos(theta)
     * a[0][1] = -sin(theta)
     * a[1][0] = sin(theta)
     * a[1][1] = cos(theta)
     *
     * @param inputVector the source space vector
     * @param fromFrame the 'from' REFERENCE_FRAME
     * @param toFrame the 'to' REFERENCE_FRAME
     * @param j2000Time the J2000 Time used to compute the Greenwich Sidereal Angle
     *
     * @return the converted vector
     */
    static SpaceVector ConvertVector(const SpaceVector &inputVector,
                                     const REFERENCE_FRAME &fromFrame,
                                     const REFERENCE_FRAME &toFrame,
                                     double j2000Time);

    static SpaceVector ProjectIntoPlane(const SpaceVector &projectVector,
                                        const SpaceVector &planeVector);
    static SpaceVector TransformCoordinates(const SpaceVector &transformVector,
                                            const SpaceVector &xFrame,
                                            const SpaceVector &yFrame,
                                            const SpaceVector &zFrame);

    static inline string GetClassName();

    double              firstProjection_a;
    double              secondProjection_a;
    double              thirdProjection_a;
    REFERENCE_FRAME     referenceFrame_a;
};



inline
string
SpaceVector::GetClassName()
{
    return(SPACE_VECTOR_H);
}

#endif
