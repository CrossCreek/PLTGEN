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
 * Constant.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef CONSTANT_H
#define CONSTANT_H "Constant"

#include <cfloat>
#include <climits>
#include <string>

using namespace std;

/**
 * Global class to hold various constants
 *
 * @author Rob Lowry
 * @author Brian Fowler
 * @date 08/16/11
 */

enum STATE_VECTOR_FORMAT {
    UNKNOWN_FORMAT = -1,
    MEAN = 0,
    OSCULATING = 1
};

enum REFERENCE_FRAME {
    UNKNOWN_FRAME = -1,
    EARTH_CENTER_FIXED = 0,
    EARTH_CENTER_INERTIAL = 1,
    LAT_LON_ALTITUDE = 2,
    PQW = 3,
    CARTESIAN = 4
};

enum ANGLE_UNIT {
    UNKNOWN_ANGLE_UNIT = -1,
    DEGREES = 0,
    RADIANS = 1,
    GRADIENTS = 2
};

enum DISTANCE_UNIT {
    UNKNOWN_DISTANCE_UNIT = -1,
    METERS = 0,
    KILOMETERS = 1,
    INCHES = 2,
    FEET = 3,
    MILES = 4,
    NAUTICAL_MILES = 5
};

enum TIME_UNIT {
    UNKNOWN_TIME_UNIT = -1,
    TIME_STEPS = 0,
    MINUTES = 1,
    HOURS = 2,
    SECONDS = 3,
    J2000 = 4
};

enum MAP_DIRECTION {
    UNKNOWN_DIRECTION = -1,
    NORTH = 0,
    NORTHEAST = 1,
    EAST = 2,
    SOUTHEAST = 3,
    SOUTH = 4,
    SOUTHWEST = 5,
    WEST = 6,
    NORTHWEST = 7
};

static const int MONTH_MAXIMUM = 12;
static const int LEAP_MONTH = 2;
static const int DAYS_IN_MONTH[MONTH_MAXIMUM] = {31, 28, 31, 30, 31, 30,
                                                 31, 31, 30, 31, 30, 31
                                                };

static const double EARTH_RADIUS_NMI_s  = 3443.918467;
static const double EARTH_MU_NMI_SEC_s  = 62750.168;   // nmi^3 / sec^2
static const double EARTH_PERIOD_SEC_s = 86164.091;

static const double EARTH_ROTATION_RATE_RAD_PER_S = 7.2921158553e-5;
static const double ECCENTRICITY_TOLERANCE = 0.15;

static const double ANGLE_TOLERANCE = 0.00001;
static const double PI = 3.14159265358979324;
static const double HALF_PI = PI / 2.0;
static const double TWO_PI = 2.0 * PI;
static const double FOUR_PI = 4.0 * PI;
static const double SIX_PI = 6.0 * PI;
static const double DEGREES_TO_RADIANS   = PI / 180.0;
static const double DEGREES_TO_GRADIENTS = 200.0 / 180.0;
static const double RADIANS_TO_DEGREES   = 1.0 / DEGREES_TO_RADIANS;
static const double RADIANS_TO_GRADIENTS = 200.0 / PI;
static const double GRADIENTS_TO_DEGREES = 1.0 / DEGREES_TO_GRADIENTS;
static const double GRADIENTS_TO_RADIANS = 1.0 / RADIANS_TO_GRADIENTS;

static const double UNDEFINED_COEFFICIENT = -99.99;
static const double UNDEFINED_ANGLE = 999.99;
static const double UNDEFINED_DISTANCE = -9999999.99;

static const int  CIRCLE = 360;
static const int  SEMI_CIRCLE = CIRCLE / 2;
static const int  NORTH_POLE = 90;
static const int  SOUTH_POLE = -90;

static const double  NORTH_POLE_DEGREES = 90.0;
static const double  SOUTH_POLE_DEGREES = -90.0;
static const double  DATE_LINE_DEGREES = 180.0;

static const double ZENITH_DEGREES = 90.0;
static const double NADIR_DEGREES = -90.0;

static const double DAYS_PER_YEAR = 365.0;
static const double DAYS_PER_LEAP_YEAR = 366.0;
static const double HOURS_PER_DAY = 24.0;
static const double DAYS_PER_HOUR = 1.0 / HOURS_PER_DAY;
static const double MINUTES_PER_DAY = 1440.0;
static const double DAYS_PER_MINUTE = 1.0 / MINUTES_PER_DAY;
static const double SECONDS_PER_DAY = 86400.0;
static const double DAYS_PER_SECOND = 1.0 / SECONDS_PER_DAY;
static const double MINUTES_PER_HOUR = 60.0;
static const double HOURS_PER_MINUTE = 1.0 / MINUTES_PER_HOUR;
static const double SECONDS_PER_HOUR = 3600.0;
static const double HOURS_PER_SECOND = 1.0 / SECONDS_PER_HOUR;
static const double SECONDS_PER_MINUTE = 60.0;
static const double MINUTES_PER_SECOND = 1.0 / SECONDS_PER_MINUTE;
static const double SECONDS_PER_YEAR = SECONDS_PER_DAY * DAYS_PER_YEAR;
static const double SECONDS_PER_LEAP_YEAR = SECONDS_PER_DAY * DAYS_PER_LEAP_YEAR;

// Convert from meters
static const double METERS_TO_KILOMETERS = 0.001;
static const double METERS_TO_INCHES     = 39.3700787401575;
static const double METERS_TO_FEET       = 3.28083989501312;
static const double METERS_TO_MILES      = 0.00062137119224;
static const double METERS_TO_NMI        = 0.00053995680346;

// Convert from kilometers
static const double KILOMETERS_TO_METERS = 1000.0;
static const double KILOMETERS_TO_INCHES = 39370.0787401575;
static const double KILOMETERS_TO_FEET   = 3280.83989501312;
static const double KILOMETERS_TO_MILES  = 0.62137119223733;
static const double KILOMETERS_TO_NMI    = 0.53995680345572;

// Convert from inches
static const double INCHES_TO_METERS     = 0.0254;
static const double INCHES_TO_KILOMETERS = 0.0000254;
static const double INCHES_TO_FEET       = 0.08333333333333;
static const double INCHES_TO_MILES      = 0.00001578282828;
static const double INCHES_TO_NMI        = 0.00001371490281;

// Convert from feet
static const double FEET_TO_METERS       = 0.3048;
static const double FEET_TO_KILOMETERS   = 0.0003048;
static const double FEET_TO_INCHES       = 12.0;
static const double FEET_TO_MILES        = 0.00018939393939;
static const double FEET_TO_NMI          = 0.00016457883369;

// Convert from miles
static const double MILES_TO_METERS      = 1609.344;
static const double MILES_TO_KILOMETERS  = 1.609344;
static const double MILES_TO_INCHES      = 63360.0;
static const double MILES_TO_FEET        = 5280.0;
static const double MILES_TO_NMI         = 1.15077944802354;

// Convert from nmi
static const double NMI_TO_METERS        = 1852.0;
static const double NMI_TO_KILOMETERS    = 1.852;
static const double NMI_TO_INCHES        = 72913.38582672;
static const double NMI_TO_FEET          = 6076.11548556;
static const double NMI_TO_MILES         = 0.86897624190065;

// Earth Constants
static const double              RADIUS_NMI = 3443.918467;
static const double              MU_NMI_SEC = 62750.168;
static const double              PERIOD_SEC = 86164.091;

static const double DEGREES_PER_NMI = 1.0 / 60.0;

static const int  ERROR_ASSIGNMENT = -999;
static const int  STATE_OF_HEALTH_ASSIGNMENT = 9999;
static const int  INVALID_TIMESTEP = -2;
static const int  NAME_SIZE = 128;
static const int  SHORT_ZERO = (int)0;
static const int  STRING_SIZE = 1024;
static const int  VALID_TIMESTEP = -1;

static const int   INVALID_SOLUTION_INDEX = -999;
static const int   UNASSIGNED_SOLUTION_INDEX = -1;
static const int   LONG_ZERO = 0;

static const double EFFICIENCY_MULTIPLIER = 1000.00;

static const double DOUBLE_ZERO = 0.00;
static const double INVALID_OBJECTIVE_VALUE = -99999.00;
static const double INVALID_SECONDS = 999999.00;

static const char   BLANK_CHARACTER = ' ';
static const char   ERROR_CHARACTER = '?';
static const char   NEW_LINE = '\n';
static const char   NULL_CHAR = '\0';
static const char   CAPACITY_FULL_CHARACTER = 'U';
static const char   PRECLUDE_CHARACTER = 'P';
static const char   PREPARATION_CHARACTER = 'p';
static const char   STATE_OF_HEALTH_CHARACTER = 'S';
static const char   USER_PREP_CHARACTER = 'u';
static const char   RELAY_PREP_CHARACTER = 'r';
static const char   GROUND_PREP_CHARACTER = 'g';
static const char   EMPTY_CAPACITY_CHARACTER = '.';
static const char   UNALLOCATED_CHARACTER = '*';
static const char   REQUIRED_DOWNLINK_OVERHEAD_CHARACTER = '+';

static const string CAPACITY_STRING = "CAPACITY";
static const string DEFAULT_STRING = "ALL";
static const string REVISIT_STRING = "REVISIT";
static const string UNKNOWN_STRING = "UNKNOWN";
static const string MURAL_VERSION  = "PLTGEN 1.0";

static const int  CONSTRAINT_NAME_MAX_SIZE = 16;

class Constant
{
public:

    virtual ~Constant();

protected:

private:

    static inline string GetClassName();

    inline Constant();
};

inline
Constant::Constant()
{
    return;
}

inline
string
Constant::GetClassName()
{
    return(CONSTANT_H);
}

inline
Constant::~Constant()
{
    return;
}

#endif
