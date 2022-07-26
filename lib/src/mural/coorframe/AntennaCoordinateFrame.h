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
 * AntennaCoordinateFrame.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ANTENNA_COORDINATE_FRAME_H
#define ANTENNA_COORDINATE_FRAME_H "AntennaCoordinateFrame"

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <memory>

#include "general/math/CoordinateFrame.h"

class Element;

/**
 * Class which serves as the base class for computing the geometry of the antenna coordinate
 * frame.  It computes the coordinate frame of the antenna by appling roations about the X,Y, and Z
 * axies with the platform, gimbal, and roll offset angles
 *
 * There is a method to compute azimuth and elvation angles as well.  The angles are defined seperatly
 * for each element type.
 *
 * @author Brian Fowler
 * @date 09/15/11
 */
class AntennaCoordinateFrame
{
public:

    AntennaCoordinateFrame(
        const shared_ptr<Element> &ptrElement,
        double platformOffsetAngle,
        double gimbalFrameOffsetAngle,
        double rollOffsetAngle);



    explicit AntennaCoordinateFrame(const shared_ptr<Element> &ptrElement);

    ~AntennaCoordinateFrame();

    /**
     * Method to get the coordinate frame of the antenna.  The platformOffsetAngle_a is a user
     * defined input on the antenna
     *
     * @param additionalSeconds time between timesteps
     * @param regionNumber the region we are pointing at
     * @param providerAzimuthOffsetAngle the azimuth offset being looked at (from LinkWhenPointingAtRegion)
     */
    shared_ptr<CoordinateFrame> GetAntennaCoordinateFrame(int timeIndex,
            double additionalSeconds,
            double regionNumber = 0.0,
            double providerAzimuthOffsetAngle = 0.0) const;


    /**
     * Method to figure out the azimuth/elvation angles for the coordinate frame.  How the angles are measured
     * are based on the element type (user, relay, ground).  This method will call one of the private DetermineAzElAngles
     * methods
     *
     * @param timeIndex the time index to determine az/el angles for
     * @param lineOfSightVector the line of site vector from the body center to other element
     * @param additionalSeconds seconds past the timeindex to compute the angles for
     * @param azimuthAngle (return) calculated azimuth angle
     * @param elevationAngle (return) calculated elevation angle
     * @param regionNumber region number for the Z-Axis of the coordiante frame to point to (for users)
     * @param azimuthOffsetAngle the angle to offset the angle calculation (for Field of Regard)
     */
    void DetermineAzElAngles(int timeIndex,
                             const SpaceVector &lineOfSightVector,
                             double additionalSeconds,
                             double &azimuthAngle,
                             double &elevationAngle,
                             double regionNumber,
                             double azimuthOffsetAngle) const;

    /**
     * Getters
     */
    const Element& GetElement() const;
    double GetPlatformOffsetAngle() const;
    double GetGimbalFrameOffsetAngle() const;
    double GetRollOffsetAngle() const;

    /**
     * Setters
     */
    void SetPlatformOffsetAngle(double platformOffsetAngle);
    void SetGimbalFrameOffsetAngle(double gimbalFrameOffsetAngle);
    void SetRollOffsetAngle(double rollOffsetAngle);

protected:

private:
    /**
     * Determines the Azimuth and Elevation angles for a user element
     *
     * AZIMUTH   => measured from the X-axis in the X-Y plane
     * ELEVATION => measured from the X-Y plane, above the vehicle (where comm antenna is
     *              located) is positive and below is negative
     *
     * @param &frame the calculated antenna coordinate frame (represents the time instnace)
     * @param unitLOS the line of site vector from body center
     * @param transferredLOS the line of site vector transferred to the antenna
     * @param azimuthAngle (return) calculated azimuth angle
     * @param elevationAngle (return) calculated elevation angle
     */
    void DetermineAzElAnglesForUser(const CoordinateFrame &frame,
                                    const SpaceVector &unitLOS,
                                    const SpaceVector &transferredLOS,
                                    double &azimuthAngle,
                                    double &elevationAngle) const;

    /**
     * Determines the Azimuth and Elevation angles for a relay element
     *
     * AZIMUTH   => measured from the Y-axis in the Y-Z plane
     * ELEVATION => meausred from the Y-Z plane
     *
     * @param &frame the calculated antenna coordinate frame (represents the time instnace)
     * @param unitLOS the line of site vector from body center
     * @param transferredLOS the line of site vector transferred to the antenna
     * @param azimuthAngle (return) calculated azimuth angle
     * @param elevationAngle (return) calculated elevation angle
     */
    void DetermineAzElAnglesForRelay(const CoordinateFrame &frame,
                                     const SpaceVector &unitLOS,
                                     const SpaceVector &transferredLOS,
                                     double &azimuthAngle,
                                     double &elevationAngle) const;

    /**
     * Determines the Azimuth and Elevation angles for a ground element
     *
     * AZIMUTH   => measured from the X-axis (pointing north) in the X-Y plane
     * ELEVATION => measured from the X-Y plane, directly overhead is 90 degrees and the
     *              horizon is 0 degrees
     *
     * @param &frame the calculated antenna coordinate frame (represents the time instnace)
     * @param unitLOS the line of site vector from body center
     * @param transferredLOS the line of site vector transferred to the antenna
     * @param azimuthAngle (return) calculated azimuth angle
     * @param elevationAngle (return) calculated elevation angle
     */
    void DetermineAzElAnglesForGround(const CoordinateFrame &frame,
                                      const SpaceVector &unitLOS,
                                      const SpaceVector &transferredLOS,
                                      double &azimuthAngle,
                                      double &elevationAngle) const;

    static inline std::string GetClassName();

    shared_ptr<Element>                ptrElement_a;
    double                             platformOffsetAngle_a;
    double                             gimbalFrameOffsetAngle_a;
    double                             rollOffsetAngle_a;
};

inline
std::string
AntennaCoordinateFrame::GetClassName()
{
    return(ANTENNA_COORDINATE_FRAME_H);
}

#endif

