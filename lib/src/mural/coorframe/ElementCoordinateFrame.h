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
 * ElementCoordinateFrame.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ELEMENT_COORDINATE_FRAME_H
#define ELEMENT_COORDINATE_FRAME_H "ElementCoordinateFrame"

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <memory>

#include "general/math/CoordinateFrame.h"

class SpaceVector;
class Element;

/**
 * Class which provides the coordinate frame for an Element (User, Relay, Ground).  Currently it has
 * fixed coordinate frames based on the type of element.  An enum designates which coordinate frame
 * to use.

 * This may be enhanced in the future to allow for custom frames.  This new structure would make such
 * enhancements easier.
 *
 * @author Brian Fowler
 * @date 09/15/11
 */
class ElementCoordinateFrame
{

public:
    enum COORDINATE_FRAME_TYPE {
        UNKNOWN_FRAME = -1,
        USER_FRAME = 0,
        RELAY_FRAME = 1,
        GROUND_FRAME = 2
    };

    /**
     * Default Constructor
     */



    explicit ElementCoordinateFrame(const COORDINATE_FRAME_TYPE &type);

    /**
     * Constructor for User Coordiante Frame, which provides the bodyTiltsForCollection attribute
     */
    ElementCoordinateFrame(const COORDINATE_FRAME_TYPE &type, bool bodyTiltsForCollection);

    /**
     * Constructor for Relay Coordiante Frame, which provides the bodyTiltAngle attribute
     */
    ElementCoordinateFrame(const COORDINATE_FRAME_TYPE &type, double bodyTiltAngle);

    /**
     * Destructor
     */
    ~ElementCoordinateFrame();

    /**
     * Gets the current coordinate frame at the timeIndex
     *
     * @param element the reference to the element to get pos/vel data
     * @param timeIndex the time index to get the coordainte frame
     * @param additionalSeconds additional seconds past the timeindex (to interpolate)
     * @param regionNumber (for user frames) the region number to point Z to.
     */
    shared_ptr<CoordinateFrame> GetCoordinateFrame(const Element &element,
            int timeIndex,
            double additionalSeconds = 0.0,
            double regionNumber = 0.0);

protected:

private:

    /**
     * Returns the fixed UserCoordinateFrame at the given instance in time
     *
     * @param element the reference to the element to get pos/vel data
     * @param timeIndex the time index to get the coordainte frame
     * @param additionalSeconds additional seconds past the timeindex (to interpolate)
     * @param regionNumber (for user frames) the region number to point Z to.
     */
    shared_ptr<CoordinateFrame> GetUserCoordinateFrame(const Element &element,
            int timeIndex,
            double additionalSeconds = 0.0,
            double regionNumber = 0.0);

    /**
     * Returns the fixed RelayCoordinateFrame at the given instance in time
     *
     * @param element the reference to the element to get pos/vel data
     * @param timeIndex the time index to get the coordainte frame
     * @param additionalSeconds additional seconds past the timeindex (to interpolate)
     */
    shared_ptr<CoordinateFrame> GetRelayCoordinateFrame(const Element &element,
            int timeIndex,
            double additionalSeconds = 0.0);

    /**
     * Returns the fixed GroundCoordinateFrame at the given instance in time
     *
     * @param element the reference to the element to get pos/vel data
     * @param timeIndex the time index to get the coordainte frame
     * @param additionalSeconds additional seconds past the timeindex (to interpolate)
     */
    shared_ptr<CoordinateFrame> GetGroundCoordinateFrame(const Element &element,
            int timeIndex,
            double additionalSeconds = 0.0);

    static inline std::string GetClassName();

    COORDINATE_FRAME_TYPE type_a;

    // For User CF
    bool bodyTiltsForCollection_a;

    // For Relay CF
    double bodyTiltAngle_a;

};

inline
std::string
ElementCoordinateFrame::GetClassName()
{
    return(ELEMENT_COORDINATE_FRAME_H);
}

#endif

