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
 * Element.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ELEMENT_H
#define ELEMENT_H "Element"

#include <memory>
#include <string>

using namespace std;

class CoordinateFrame;
class ElementCoordinateFrame;
class SpaceVector;

#include "mural/modules/pg/Journey.h"
#include "general/utility/Constant.h"

/**
 * Base class of Data Provider which hold geometric information about an element including
 * it's Journey and CoordinateFrame.
 *
 * @author Brian Fowler
 * @date 10/04/11
 */
class Element
{
public:

    enum ELEMENT_TYPE {
        UNKNOWN_ELEMENT_TYPE = -1,
        USER_VEHICLE = 0,
        RELAY_SATELLITE = 1,
        GROUND_STATION = 2
    };



    explicit Element(const ELEMENT_TYPE &elementType);

    Element(const Element &copyMe);
    virtual ~Element();

    /**
     * Gets the designator of the element
     *
     * @return designator
     */
    string               GetDesignator() const;

    /**
     * Gets the Journey (positions at each time) of the element
     *
     * @return shared_ptr<Journey>
     */
    shared_ptr<Journey>  GetJourney() const;

    /**
     * Convienience Getters for Journey
     */
    double GetLatitude(int timeIndex) const;
    double GetLongitude(int timeIndex) const;
    double GetAltitude(int timeIndex) const;
    double GetSemiMajorAxis() const;
    double GetEccentricity() const;
    double GetInclination() const;
    double GetRightAscension() const;
    double GetArgumentOfPerigee() const;
    double GetTrueAnomaly() const;
    double GetTrueAnomaly(int timeIndex,  double additionalSeconds = 0.0) const;
    double GetNodalPrecessionRate() const;
    double GetApsidalRotationRate() const;
    double GetAnomalisticMeanMotion() const;
    double GetSemiLatusRectum() const;
    int   GetRevNumber(int timeIndex) const;
    int   GetRevIndex(int timeIndex) const;
    int   GetRevStartTime(int revIndex) const;
    int   GetRevEndTime(int revIndex) const;
    int   GetNumberOfRevs() const;
    int   GetOrbitPeriod() const;
    bool   UsesSouthPointBoundary() const;

    /**
     * Position/Velocity Getters
     */
    SpaceVector   GetPosition(int timeIndex) const;
    SpaceVector   GetPosition(int timeIndex,
                              const REFERENCE_FRAME &returnFrame) const;
    SpaceVector   GetInterimPosition(int timeIndex,
                                     double additionalSeconds) const;
    SpaceVector   GetVelocity(int timeIndex) const;
    SpaceVector   GetVelocity(int timeIndex,
                              const REFERENCE_FRAME &returnFrame) const;
    SpaceVector   GetInterimVelocity(int timeIndex,
                                     double additionalSeconds) const;

    /**
     * Gets the Element Type. This was added for convienience to avoid having to do
     * multiple dynamic casts to figure out the element type.
     *
     * @return ELEMENT_TYPE
     */
    ELEMENT_TYPE  GetElementType() const;

    /**
     * Gets the Element coordinate frame of the body at timeIndex
     *
     * @param timeIndex time to get coordinate frame for
     * @param additionalSeconds time to interpolate past the timeIndex (in seconds)
     * @param regionNumber region number to point Z-axis to (for user CF)
     * @return shared_ptr<CoordinateFrame>
     */
    shared_ptr<CoordinateFrame> GetCoordinateFrame(int timeIndex,
            double additionalSeconds = 0.0,
            double regionNumber = 0.0) const;


    /**
     * Sets the Element Coordinate frame of the body.  This is the DEFINITION of the coordinate
     * frame.  This is not setting the coordinate frame at a timestep.
     *
     * @param ptrCoordinateFrame the definition of the coordinate frame
     */
    void SetCoordinateFrame(shared_ptr<ElementCoordinateFrame> ptrCoordinateFrame);

    /**
     * Sets the Element Journey (positions)
     *
     * @param ptrCoordinateFrame the definition of the coordinate frame
     */
    void SetJourney(shared_ptr<Journey> ptrJourney);

    /**
     * Sets the Designator (Name)
     *
     * @param ptrDesignator the new Designator
     */
    void SetDesignator(shared_ptr<string> ptrDesignator);


protected:

    ELEMENT_TYPE                       elementType_a;

    shared_ptr<string>                 ptrDesignator_a;
    shared_ptr<Journey>                ptrJourney_a;
    shared_ptr<ElementCoordinateFrame> ptrCoordinateFrame_a;

private:



};

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
Element::GetClassName()
{
    return(ELEMENT_H);
}
*/

#endif
