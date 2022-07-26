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
 * Sensor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef SENSOR_H
#define SENSOR_H "Sensor"

#include <fstream>
#include <iostream>
#include <string>

#include "mural/access/AccessPattern.h"
#include "mural/sensor/iqe/IQE.h"

#include "general/array/Array.h"
#include "general/utility/Constant.h"
#include "general/data/InputDataElement.h"

using namespace std;

/**
 * Sensor is the class that models any sensor asset that can be attached
 *  to a data provider.
 *
 * Name                            Description (units)
 * -------------                   ---------------------------------------------
 * minimumPowerDownTime_a          the smallest duration allowed between active
 *                                   time steps to be able to power down the
 *                                   sensor
 * dailyDutyCycle_a                the amount of time a sensor may be active
 *                                   over the course of 24 hours
 * ptrRollingRevDutyCycle_a*       array of duty cycle(s) for rolling revs
 * sensorWeightFactor_a            score multiplier to targets for this sensor
 * dataRate_a                      collection Data Rate
 * maximumSunElevationAngle_a      maximum sun elevation angle
 * minimumSunElevationAngle_a      minimum sun elevation angle
 * offsetAngle_a                   offset angle for this sensor
 * ptrRequirementDeckFileName_a*   file name of the requiremenst deck (if any)
 * sensorType_a                    type of sensor (SENSOR_TYPE)
 * ptrAccessPattern_a*             pointer to the access pattern data
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class Sensor
{
public:

    /**
     * Enum to hold various sensor statistics. Simple way to orgainize sensor statistics
     * in the Senosr class.
     */
    /*RAGUSA 11/2/17 Updated values to include SSAR sensor*/
    /*RAGUSA 2/27/2018 - REVISION - SSAR is a type of SAR, therefore it is
    								not to be included into the # target sensors.
    								This was causing errors in target deck parsing.
    								Later on, MURAL loops based on these indicies.
    								It is advised to NOT change them unless necessary.*/
    enum SENSOR_STATS {
        NUMBER_OF_TARGET_SENSORS = 9,
        NUMBER_OF_NON_SPECTRAL_SENSORS = 3,
        NUMBER_OF_SPECTRAL_SENSORS = 6,
        NUMBER_OF_DECK_SENSORS = 12,
        NUMBER_OF_SENSOR_TYPES = 12
    };

    /**
     * Enum of sensor types.  The first 8 sensors (VIS - HS_EMISSIVE) map directly to the current target decks.  The
     * indices are ordered to aid in the parsing of the TargetDeck (TargetDeck.cpp).  WIDE_FOV does not appear
     * in the target deck and SPECIAL_VISIBLE is a type of vis sensor. ADDITION: SSAR(11) is now included in target deck 11/2/17.
     *
     * The Get Sensor Number method adds 1 to this index for display purposes in the MURALLP section of the mrl file.
     */
    enum SENSOR_TYPE {
        UNKNOWN       = -1,
        VISIBLE       = 0,
        LONG_WAVE     = 1,
        SAR           = 2,
        MS_REFLECTIVE = 3,
        MS_EMISSIVE   = 4,
        US_REFLECTIVE = 5,
        US_EMISSIVE   = 6,
        HS_REFLECTIVE = 7,
        HS_EMISSIVE   = 8,
        WIDE_FOV      = 9,
        SPECIAL_VISIBLE = 10,
        /*RAGUSA 11/2/17 Updated to include SSAR sensor*/
        SSAR   = 11,
    };

    Sensor();
    Sensor(shared_ptr<InputDataElement> ptrInputDataElement,
           const SENSOR_TYPE &type);
    Sensor(const Sensor &copyMe);
    virtual ~Sensor();

    Sensor&         operator =  (const Sensor &copyMe);
    bool            operator == (const Sensor &compareMe) const;
    bool            operator != (const Sensor &compareMe) const;
    bool            operator <  (const Sensor &compareMe) const;

    static SENSOR_TYPE GetSensorType(shared_ptr<InputDataElement> ptrSensorElement);

    virtual int   GetSensorIndex() const;
    virtual void   CheckInputData(bool runTimelineGenerator = false) const;
    virtual void   PrintAttributes(ostream &outfile) const;

    virtual double  GetCollectionRate(int missionIndex,
                                      int qualityIndex) const;
    virtual double  GetResourceWeight(int missionIndex,
                                      int qualityIndex) const;

    void   UpdateRollingRevs(int numberOfRevs);

    int  CalculateQualityLevel(double elevationAngle,
                               const string &missionType,
                               const SpaceVector &targetPos,
                               const PositionState &ecfVehilceState,
                               bool enhancedAreaMode = false) const;

    int   GetDutyCycle(int revIndex) const;

    double GetOuterConeElevationAngle(const AccessPattern::PATTERN &pattern,
                                      int missionIndex = -1) const;
    double GetOuterConeElevationAngle(const string &missionString) const;
    double GetInnerConeElevationAngle(const AccessPattern::PATTERN &pattern,
                                      int missionIndex = -1) const;
    double GetInnerConeElevationAngle(const string &missionString) const;

    bool   InsideAccessPattern(int missionIndex,
                               const SpaceVector &targetPosition,
                               const SpaceVector &vehiclePosition,
                               const SpaceVector &vehicleVelocity) const;
    bool   MissionSpecificEqualsLargest(int missionIndex) const;

    static inline int   GetNumberOfSensors();
    static inline int   GetNumberOfTargetSensors();
    static inline int   GetNumberOfDeckSensors();
    static inline int   GetNumberOfNonSpectralSensors();
    static inline int   GetNumberOfSpectralSensors();

    static inline bool   IsSpectralSensor(int sensorIndex);

    static inline string GetSensorTypeString(int sensorIndex);

    virtual inline bool  IsReflectiveEnabled() const;
    virtual inline bool  IsEmissiveEnabled() const;

    inline int  GetBandwidthCompression() const;
    inline int  GetMinimumPowerDownTime() const;
    inline int  GetSensorNumber() const;
    inline int  GetNumberOfRollingRevs() const;

    inline double  GetSensorWeightFactor() const;

    inline double GetDataRate() const;
    inline double GetMinimumSunElevationAngle() const;
    inline double GetMaximumSunElevationAngle() const;
    inline double GetOffsetAngle() const;

    inline bool   HasSunAngleRequirement() const;
    inline bool   MeetsSunAngleRequirement(double sunAngle) const;
    inline bool   UseDutyCycle() const;
    inline bool   UseRollingRevs() const;
    inline bool   UseRequirementDeck() const;
    inline bool   UsingRequirementDeck(const string &requirementDeckName) const;

    inline string GetSensorName() const;

    inline const shared_ptr<string>  GetRequirementDeckFileName() const;

    inline AccessPattern* GetAccessPattern();

    inline shared_ptr<IQE> GetIQE();

protected:

    inline void SetMinimumSunElevationAngle(double minAngle);
    inline void SetMaximumSunElevationAngle(double maxAngle);

    int               minimumPowerDownTime_a;
    int                dailyDutyCycle_a;
    Array<int>        *ptrRollingRevDutyCycle_a;
    double               sensorWeightFactor_a;
    double              dataRate_a;
    double              minimumSunElevationAngle_a;
    double              maximumSunElevationAngle_a;
    shared_ptr<string>  ptrRequirementDeckFileName_a;
    SENSOR_TYPE         sensorType_a;
    shared_ptr<AccessPattern> ptrAccessPattern_a;
    shared_ptr<IQE>           ptrIQE_a;

private:

    void ExtractSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement);

    static inline string GetClassName();
};

inline
bool
Sensor::IsReflectiveEnabled() const
{
    return(true);
}

inline
bool
Sensor::IsEmissiveEnabled() const
{
    return(true);
}

/**
 * Method to return number of sensors
 *
 * @return int
 */
inline
int
Sensor::GetNumberOfSensors()
{
    return(NUMBER_OF_SENSOR_TYPES);
}

/**
 * Method to return number of sensors that appear in the target deck. All SENSOR_TYPE indices that are less than
 * this number are part of the target deck
 *
 * @return int
 */
inline
int
Sensor::GetNumberOfTargetSensors()
{
    return(NUMBER_OF_TARGET_SENSORS);
}

/**
 * Method to return number of non spectral sensors
 *
 * @return int
 */
inline
int
Sensor::GetNumberOfNonSpectralSensors()
{
    return(NUMBER_OF_NON_SPECTRAL_SENSORS);
}

/**
 * Method to return number of spectral sensors
 *
 * @return int
 */
inline
int
Sensor::GetNumberOfSpectralSensors()
{
    return(NUMBER_OF_SPECTRAL_SENSORS);
}

/**
 * Method to return number of sensors that appear in the requirement deck. All SENSOR_TYPE indices that are less than
 * this number are part of the requirement deck
 *
 * @return int
 */
inline
int
Sensor::GetNumberOfDeckSensors()
{
    return(NUMBER_OF_DECK_SENSORS);
}

/**
 * Method to determine if the sensor is spectral or not.  Uses the SENSOR_TYPE indices to make this determination
 *
 * @param sensorIndex the index of the sensor
 * @return bool
 */
inline
bool
Sensor::IsSpectralSensor(int sensorIndex)
{
    return( (sensorIndex >= MS_REFLECTIVE) &&
            (sensorIndex <= HS_EMISSIVE) );
}

/**
 * Method to return a string representing the sensor type for display purposes.
 *
 * @param sensorIndex the index of the sensor
 * @return string
 */
inline
string
Sensor::GetSensorTypeString(int sensorIndex)
{
    string returnString = "INACTIVE";

    if (sensorIndex == VISIBLE) {
        returnString = "VIS";
    } else if (sensorIndex == SPECIAL_VISIBLE) {
        returnString = "SPC";
    } else if (sensorIndex == LONG_WAVE) {
        returnString = "LW";
    } else if (sensorIndex == SAR) {
        returnString = "SAR";
        /*RAGUSA 11/2/17 Updated to include SSAR sensor*/
    } else if (sensorIndex == SSAR) {
        returnString = "SSAR";
    } else if (sensorIndex == MS_REFLECTIVE) {
        returnString = "MSR";
    } else if (sensorIndex == MS_EMISSIVE) {
        returnString = "MSE";
    } else if (sensorIndex == HS_REFLECTIVE) {
        returnString = "HSR";
    } else if (sensorIndex == HS_EMISSIVE) {
        returnString = "HSE";
    } else if (sensorIndex == US_REFLECTIVE) {
        returnString = "USR";
    } else if (sensorIndex == US_EMISSIVE) {
        returnString = "USE";
    } else if (sensorIndex == WIDE_FOV) {
        returnString = "WFOV";
    }

    return(returnString);
}

/**
 * Gets the sensor name.  Simply calls GetSensorTypeString with the senosr type attribute
 *
 * @return string
 */
inline
string
Sensor::GetSensorName() const
{
    return(GetSensorTypeString(sensorType_a));
}

/**
 * Gets the Minimum Sun Elevation Angle for the sensor
 *
 * @return double min sun elevation angle
 */
inline
double
Sensor::GetMinimumSunElevationAngle() const
{
    return(minimumSunElevationAngle_a);
}

/**
 * Sets the Minimum Sun Elevation Angle for the sensor
 *
 * @param minAngle min sun elevation angle
 */
inline
void
Sensor::SetMinimumSunElevationAngle(double minAngle)
{
    minimumSunElevationAngle_a = minAngle;
}

/**
 * Gets the Maximum Sun Elevation Angle for the sensor
 *
 * @return double max sun elevation angle
 */
inline
double
Sensor::GetMaximumSunElevationAngle() const
{
    return(maximumSunElevationAngle_a);
}

/**
 * Sets the Maximum Sun Elevation Angle for the sensor
 *
 * @param maxAngle max sun elevation angle
 */
inline
void
Sensor::SetMaximumSunElevationAngle(double maxAngle)
{
    maximumSunElevationAngle_a = maxAngle;
}

/**
 * Determines if the sensor has sun elevation angles defined to constrain the sensor
 *
 * @return bool
 */
inline
bool
Sensor::HasSunAngleRequirement() const
{
    return( (minimumSunElevationAngle_a != UNDEFINED_ANGLE) ||
            (maximumSunElevationAngle_a != UNDEFINED_ANGLE) );
}

/**
 * Determines if the angle argument meets the sun angle requirement
 *
 * @param sunAngle the paramter angle to test
 * @return bool
 */
inline
bool
Sensor::MeetsSunAngleRequirement(double sunAngle) const
{
    bool meetsSunAngleRequirement = true;

    if ( ((minimumSunElevationAngle_a != UNDEFINED_ANGLE) && (sunAngle < minimumSunElevationAngle_a)) ||
            ((maximumSunElevationAngle_a != UNDEFINED_ANGLE) && (sunAngle > maximumSunElevationAngle_a)) ) {
        meetsSunAngleRequirement = false;
    }

    return(meetsSunAngleRequirement);
}

/**
 * Determines if the sensor duty cycle should be used for the sensor (dailyDutyCycle_a > 0)
 *
 * @return bool
 */
inline
bool
Sensor::UseDutyCycle() const
{
    return(dailyDutyCycle_a > 0);
}

/**
 * Determines if the sensor has rolling rev duty cycle constraints.  Rolling rev duty cycles are for
 * constraining requested time based on the cumualtive rev pass.
 *
 * @return bool
 */
inline
bool
Sensor::UseRollingRevs() const
{
    return(ptrRollingRevDutyCycle_a != NULL);
}

/**
 * Determines if the sensor has a requirement deck defined
 *
 * @return bool
 */
inline
bool
Sensor::UseRequirementDeck() const
{
    return(ptrRequirementDeckFileName_a != NULL);
}

/**
 * Tests if the the Requirement Deck File name matches the argument
 *
 * @param requirementDeckName the requirement deck to check
 * @return bool if the requirement deck matches
 */
inline
bool
Sensor::UsingRequirementDeck(const string &requirementDeckName) const
{
    bool usingRequirementDeck = false;

    if (ptrRequirementDeckFileName_a != NULL) {
        if (*ptrRequirementDeckFileName_a == requirementDeckName) {
            usingRequirementDeck = true;
        }
    }

    return(usingRequirementDeck);
}

/**
 * Returns the access pattern object
 *
 * @return AccessPattern
 */
inline
AccessPattern*
Sensor::GetAccessPattern()
{
    return(ptrAccessPattern_a.get());
}

/**
 * Returns the IQE
 *
 * @return IQE
 */
inline
shared_ptr<IQE>
Sensor::GetIQE()
{
    return(ptrIQE_a);
}

/**
 * Gets the sensor weight factor
 *
 * @return double
 */
inline
double
Sensor::GetSensorWeightFactor() const
{
    return(sensorWeightFactor_a);
}

/**
 * Gets the sensor data rate
 *
 * @return double
 */
inline
double
Sensor::GetDataRate() const
{
    return(dataRate_a);
}

/**
 * Gets the requirement deck file name
 *
 * @return string
 */
inline
const shared_ptr<string>
Sensor::GetRequirementDeckFileName() const
{
    return(ptrRequirementDeckFileName_a);
}

/**
 * Gets the minimum power down time value
 *
 * @return int
 */
inline
int
Sensor::GetMinimumPowerDownTime() const
{
    return(minimumPowerDownTime_a);
}

/**
 * Gets the sensor number for display purposes and as a unique identifier in the timeline.  Equals the SENSOR_TYPE
 * plus one
 *
 * @return int
 */
inline
int
Sensor::GetSensorNumber() const
{
    return(sensorType_a + 1);
}

/**
 * Gets the number of rolling revs
 *
 * @return int
 */
inline
int
Sensor::GetNumberOfRollingRevs() const
{
    int numberOfRollingRevs = 0;

    if (ptrRollingRevDutyCycle_a != NULL) {
        numberOfRollingRevs = ptrRollingRevDutyCycle_a->Size();
    }

    return(numberOfRollingRevs);
}

/**
 * Gets the class name
 *
 * @return string
 */
inline
string
Sensor::GetClassName()
{
    return(SENSOR_H);
}

#endif
