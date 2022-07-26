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
 * UnitData.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <string>

#include "general/utility/Constant.h"

#ifndef UNIT_DATA_H
#define UNIT_DATA_H "UnitData"

/**
 * Data class to hold information about the internal units of the analysis.  It
 * defines what angle, distance, time, and reference frame should be used within.
 * the library/model as whole.
 *
 * It also has a set of input units to govern what units the input files are in and
 * a set of output units to govern what units the output files should be in.
 *
 * @author Brian Fowler
 * @date 08/16/11
 */
class UnitData
{
public:

    /**
     * Default Constructor
     */
    UnitData();
    virtual ~UnitData();

    /* ************************************************
     * Basic Set Routines
     * ************************************************ */

    /**
     * Sets the Reference Frame to be used internally by the analysis
     *
     * @param &frame the internal reference frame
     */
    static void SetInternalReferenceFrame(const REFERENCE_FRAME &frame);

    /**
     * Sets the Angle Unit to be used internally by the analysis
     *
     * @param &units the input angle units
     */
    static void SetInternalAngleUnits(const ANGLE_UNIT &units);

    /**
     * Sets the Distance Unit to be used internally by the analysis
     *
     * @param &units the input distance units
     */
    static void SetInternalDistanceUnits(const DISTANCE_UNIT &units);

    /**
     * Sets the Time Unit to be used internally by the analysis
     *
     * @param &units the input time units
     */
    static void SetInternalTimeUnits(const TIME_UNIT &units);

    /**
     * Sets the Reference Frame for output files
     *
     * @param &frame the output reference frame
     */
    static void SetOutputReferenceFrame(const REFERENCE_FRAME &frame);

    /**
     * Sets the Angle Unit for output files
     *
     * @param &units the output angle units
     */
    static void SetOutputAngleUnits(const ANGLE_UNIT &units);

    /**
     * Sets the Distance Unit for output files
     *
     * @param &units the output distance units
     */
    static void SetOutputDistanceUnits(const DISTANCE_UNIT &units);

    /**
     * Sets the Time Unit for output files
     *
     * @param &units the output time units
     */
    static void SetOutputTimeUnits(const TIME_UNIT &units);

    /**
     * Sets the Reference Frame for input files
     *
     * @param &frame the input reference frame
     */
    static void SetInputReferenceFrame(const REFERENCE_FRAME &frame);

    /**
     * Sets the Angle Unit for input files
     *
     * @param &units the input angle units
     */
    static void SetInputAngleUnits(const ANGLE_UNIT &units);

    /**
     * Sets the Distance Unit for input files
     *
     * @param &units the input distance units
     */
    static void SetInputDistanceUnits(const DISTANCE_UNIT &units);

    /**
     * Sets the Time Unit for input files
     *
     * @param &units the input time units
     */
    static void SetInputTimeUnits(const TIME_UNIT &units);


    /* ************************************************
     * Basic Get Routines
     * ************************************************ */

    /**
       * Gets the Reference Frame for output files
       *
       * @return REFERENCE_FRAME
       */
    static REFERENCE_FRAME GetOutputReferenceFrame();

    /**
     * Gets the Angle units for output files
     *
     * @return ANGLE_UNIT
     */
    static ANGLE_UNIT       GetOutputAngleUnits();

    /**
       * Gets the Distance units for output files
       *
       * @return DISTANCE_UNIT
       */
    static DISTANCE_UNIT       GetOutputDistanceUnits();

    /**
       * Gets the Time units for output files
       *
       * @return TIME_UNIT
       */
    static TIME_UNIT       GetOutputTimeUnits();

    /**
     * Gets the Reference Frame for input files
     *
     * @return REFERENCE_FRAME
     */
    static REFERENCE_FRAME GetInputReferenceFrame();

    /**
     * Gets the Angle units for input files
     *
     * @return ANGLE_UNIT
     */
    static ANGLE_UNIT GetInputAngleUnits();

    /**
       * Gets the Distant Units for input files
       *
       * @return DISTANCE_UNIT
       */
    static DISTANCE_UNIT GetInputDistanceUnits();

    /**
     * Gets the Time units for input files
     *
     * @return TIME_UNIT
     */
    static TIME_UNIT GetInputTimeUnits();

    /**
     * Converts the ANGLE_UNIT to a string representation
     *
     * @param &angle the ANGLE_UNIT to convert
     * @return string
     */
    static string GetAngleUnitString(const ANGLE_UNIT &angle);

    /**
     * Converts the DISTANCE_UNIT to a string representation
     *
     * @param &distance the DISTANCE_UNIT to convert
     * @return string
     */
    static string GetDistanceUnitString(const DISTANCE_UNIT &distance);

    /**
     * Converts the TIME_UNIT to a string representation
     *
     * @param &time the TIME_UNIT to convert
     * @return string
     */
    static string GetTimeUnitString(const TIME_UNIT &time);

    /**
     * Converts the REFERENCE_FRAME to a string representation
     *
     * @param &frame the REFERENCE_FRAME to convert
     * @return string
     */
    static string GetReferenceFrameString(const REFERENCE_FRAME &frame);

    /**
     * Converts the string representaion to ANGLE_UNIT
     *
     * @param &valueString the string to convert to ANGLE_UNIT
     * @return ANGLE_UNIT
     */
    static ANGLE_UNIT      GetAngleUnits(const string &valueString);

    /**
     * Converts the string representaion to DISTANCE_UNIT
     *
     * @param &valueString the string to convert to DISTANCE_UNIT
     * @return DISTANCE_UNIT
     */
    static DISTANCE_UNIT   GetDistanceUnits(const string &valueString);

    /**
     * Converts the string representaion to TIME_UNIT
     *
     * @param &valueString the string to convert to TIME_UNIT
     * @return TIME_UNIT
     */
    static TIME_UNIT       GetTimeUnits(const string &valueString);

    /**
     * Converts the string representaion to REFERENCE_FRAME
     *
     * @param &valueString the string to convert to REFERENCE_FRAME
     * @return REFERENCE_FRAME
     */
    static REFERENCE_FRAME GetReferenceFrame(const string &valueString);

    /**
     * Gets the Reference Frame used internally
     *
     * @return REFERENCE_FRAME
     */
    static REFERENCE_FRAME GetInternalReferenceFrame();

    /**
     * Gets the Time units used for internally
     *
     * @return TIME_UNIT
     */
    static TIME_UNIT GetInternalTimeUnits();

    /**
     * Gets the Angle units used internally
     *
     * @return ANGLE_UNIT
     */
    static ANGLE_UNIT GetInternalAngleUnits();

    /**
     * Gets the Distance units used internally
     *
     * @return DISTANCE_UNIT
     */
    static DISTANCE_UNIT GetInternalDistanceUnits();

    /* ************************************************
     * Convenience Get Routines
     * ************************************************ */

    /**
       * Takes the paramter and converts it to the input time units and
       * returns the double value
       *
       * @param internalAngle the value (in interal units)
       * @return double the value (in input units)
       */
    static double GetInputAngle(double internalAngle);

    /**
       * Takes the paramter and converts it to the input distance units and
       * returns the double value
       *
       * @param internalDistance the value (in interal units)
       * @return double the value (in input units)
       */
    static double GetInputDistance(double internalDistance);

    /**
       * Takes the paramter and converts it to the input time units and
       * returns the double value
       *
       * @param internalDistance the value (in interal time steps)
       * @return double the value (in input units)
       */
    static double GetInputTimeInstant(int internalIndex);

    /**
       * Takes the paramter and converts it to the input time duration units and
       * returns the double value
       *
       * @param internalDistance the value (in interal time steps)
       * @return double the value (in input units)
       */
    static double GetInputTimeDuration(int internalDuration);


    /**
     * Takes the paramter and converts it to the output time units and
     * returns the double value
     *
     * @param internalAngle the value (in interal units)
     * @return double the value (in output units)
     */
    static double GetOutputAngle(double internalAngle);

    /**
     * Takes the paramter and converts it to the output distance units and
     * returns the double value
     *
     * @param internalDistance the value (in interal units)
     * @return double the value (in output units)
     */
    static double GetOutputDistance(double internalDistance);

    /**
     * Takes the paramter and converts it to the output time units and
     * returns the double value
     *
     * @param internalTimeIndex the value (in interal time steps)
     * @return double the value (in output units)
     */
    static double GetOutputTimeStep(int internalTimeIndex);

    /**
     * Takes the paramter and converts it to the output time units and
     * returns the double value
     *
     * @param internalDuration the value (in interal time steps)
     * @return double the value (in output units)
     */
    static double GetOutputTimeDuration(int internalDuration);

    /**
       * Takes the angle value paramter and converts it to the internal angle units
       *
       * @param inputAngle the value (in input angle units)
       * @return double the value (in internal angle units)
       */
    static double GetInternalAngle(double inputAngle);

    /**
       * Takes the distance value paramter and converts it to the internal distance units
       *
       * @param inputDistance the value (in input distance units)
       * @return double the value (in internal distance units)
       */
    static double GetInternalDistance(double inputDistance);

    /**
       * Takes the time paramter and converts it to the internal time index
       *
       * @param inputTimeInstant the value (in input time steps)
       * @return int the value (in internal timesteps)
       */
    static int   GetInternalTimeIndex(double inputTimeInstant);

    /**
     * Takes the time duration paramter and converts it to the internal
     * time step durations
     *
     * @param inputDuration the value (in input time steps)
     * @return int the value (in internal timesteps)
     */
    static int   GetInternalTimeDuration(double inputDuration);

    /**
     * Convenience method to get the quarter circle in internal units
     *
     * @return double quarter circle (in internal angle units)
     */
    static double GetStoreQuarterCircle();

    /**
     * Convenience method to get the half circle in internal units
     *
     * @return double half circle (in internal angle units)
     */
    static double GetStoreHalfCircle();

    /**
     * Convenience method to get the full circle in internal units
     *
     * @return double full circle (in internal angle units)
     */
    static double GetStoreFullCircle();

protected:

private:
    /**
     * Gets the class name
     *
     * @return string class name
     */




    static REFERENCE_FRAME inputReferenceFrame_s;
    static ANGLE_UNIT      inputAngleUnits_s;
    static DISTANCE_UNIT   inputDistanceUnits_s;
    static TIME_UNIT       inputTimeUnits_s;

    static REFERENCE_FRAME outputReferenceFrame_s;
    static ANGLE_UNIT      outputAngleUnits_s;
    static DISTANCE_UNIT   outputDistanceUnits_s;
    static TIME_UNIT       outputTimeUnits_s;

    static ANGLE_UNIT      internalAngleUnits_s;
    static DISTANCE_UNIT   internalDistanceUnits_s;
    static REFERENCE_FRAME internalReferenceFrame_s;
    static TIME_UNIT       internalTimeUnits_s;
};

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
std::string
UnitData::GetClassName()
{
    return(UNIT_DATA_H);
}
*/

#endif
