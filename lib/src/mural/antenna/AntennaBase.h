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
 * AntennaBase.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ANTENNA_BASE_H
#define ANTENNA_BASE_H "AntennaBase"

#include <memory>
#include <string>

using namespace std;

class CoordinateFrame;
class SpaceVector;
class AntennaCoordinateFrame;
class Element;

/**
 * Base class of Antenna.  Contains the geometric aspects of the antenna.  Holds the antenna coordinate
 * frame and a method to Determine the Azimuth and Elevation angles (based off the antenna coordiante frame)
 *
 * Name                               Description (units)
 * -------------                      --------------------------------------------
 * *ptrAntennaDesignator_a            The Name of the Antenna (shared_ptr)
 * *ptrElement_a                      Pointer to the element (user/relay/ground)(shared_ptr)
 * *ptrAntennaCoordinateFrame_a       The coordinate frame of the antenna (shared_ptr)
 *
 * @author Brian Fowler
 * @date 10/04/11
 */
class AntennaBase
{
public:

    enum ANTENNA_CAPABILITY {
        UNKNOWN_CAPABILITY = -1,
        TRANSMIT = 0,
        RECEIVE = 1,
        DUPLEX = 2
    };

    AntennaBase();



    explicit AntennaBase(shared_ptr<Element> ptrElement);

    virtual ~AntennaBase();

    /**
     * Method to get the antenna coordinate frame object.
     *
     * @return Antenna Coordinate frame object
     */
    shared_ptr<AntennaCoordinateFrame> GetAntennaCoordinateFrame() const;

    /**
     * Method to get the coordinate frame of the antenna.  The platformOffsetAngle_a is a user
     * defined input on the antenna
     *
     * @param timeIndex the time step to get the coordinate frame
     * @param additionalSeconds time between timesteps
     * @param regionNumber the region we are pointing at
     * @param providerAzimuthOffsetAngle the azimuth offset being looked at (from LinkWhenPointingAtRegion)
     */
    shared_ptr<CoordinateFrame> GetAntennaCoordinateFrame(int timeIndex,
            double additionalSeconds,
            double regionNumber = 0.0,
            double providerAzimuthOffsetAngle = 0.0) const;


    /**
     * Method to call the respective method on the antenna coordinate frame
     *
     * @param timeIndex the time index to determine az/el angles for
     * @param lineOfSightVector the line of site vector from the body center to other element
     * @param additionalSeconds seconds past the timeindex to compute the angles for
     * @param azimuthAngle (return) calculated azimuth angle
     * @param elevationAngle (return) calculated elevation angle
     * @param regionNumber region number for the Z-Axis of the coordiante frame to point to (for users)
     * @param azimuthOffsetAngle the angle to offset the angle calculation (for Field of Regard)
     */
    void   DetermineAzElAngles(int timeIndex,
                               const SpaceVector &lineOfSightVector,
                               double additionalSeconds,
                               double &azimuthAngle,
                               double &elevationAngle,
                               double regionNumber = 0.0,
                               double azimuthOffsetAngle = 0.0) const;

    /**
     * Gets the designator of the antenna
     *
     * @return designator
     */
    string                  GetAntennaDesignator() const;

    /**
     * Gets the designator of the DataProvider/Element name
     *
     * @return designator
     */
    string                  GetDataProviderDesignator() const;

    /**
     * Returns the pointer to the element
     *
     * @return shared_ptr<Element>
     */
    shared_ptr<Element>     GetElement() const;

    /**
     * Returns the antenna capability
     *
     * @return ANTENNA_CAPABILITY
     */
    ANTENNA_CAPABILITY      GetAntennaCapability() const;

    /**
     * Returns the antenna capability as a string
     *
     * @return string
     */
    string                  GetAntennaCapabilityString(const ANTENNA_CAPABILITY &capability) const;

    /**
     * Sets the designator of the antenna
     *
     * @param ptrAntennaDesignator antenna designator
     */
    void         SetAntennaDesignator(const shared_ptr<string> &ptrAntennaDesignator);

    /**
     * Sets the antenna coordinate frame of the antenna
     *
     * @param ptrAntennaDesignator antenna designator
     */
    void         SetAntennaCoordinateFrame(const shared_ptr<AntennaCoordinateFrame> &ptrAntennaCoordinateFrame);

    /**
     * Sets the antenna capability of the antenna
     *
     * @param ANTENNA_CAPABILITY antenna capability
     */
    void         SetAntennaCapability(ANTENNA_CAPABILITY antennaCapability);

protected:

    shared_ptr<string>                 ptrAntennaDesignator_a;
    shared_ptr<Element>                ptrElement_a;
    shared_ptr<AntennaCoordinateFrame> ptrAntennaCoordinateFrame_a;
    ANTENNA_CAPABILITY                 antennaCapability_a;

private:

    static inline string GetClassName();
};

inline
string
AntennaBase::GetClassName()
{
    return(ANTENNA_BASE_H);
}

#endif
