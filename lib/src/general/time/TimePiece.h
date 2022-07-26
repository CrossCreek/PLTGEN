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
 * TimePiece.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef TIME_PIECE_H
#define TIME_PIECE_H "TimePiece"

#include <iostream>
#include <string>

#include "Calendar.h"
#include "general/exception/TimeException.h"
#include "general/data/UnitData.h"

class InputDataElement;

/**
 * MURAL's time class
 *
 * @author Rob Lowry
 * @author Brian Fowler
 * @date 08/16/11
 */
class TimePiece
{
public:

    /**
     *  Constructor for the TimePiece class.  Called from Method: TimePiece();
     */
    TimePiece();
    TimePiece(const TimePiece &copyMe);
    virtual ~TimePiece();

    bool            operator == (const TimePiece &compareMe) const;
    bool            operator != (const TimePiece &compareMe) const;

    static void         PrintAttributes(ostream &outfile);
    static void         PrintIndex(ostream &outfile,
                                   int storeTimeIndex,
                                   TIME_UNIT printUnits = UnitData::GetOutputTimeUnits());
    static void         PrintDuration(ostream &outfile,
                                      int storeTimeDuration,
                                      TIME_UNIT printUnits = UnitData::GetOutputTimeUnits());
	//RAGUSA 3/18/2019 //RAGUSA 3/18/2019 - Overloaded method to allow double type storeTimeDuration
	static void         PrintDuration(ostream &outfile,
									double storeTimeDuration,
									TIME_UNIT printUnits = UnitData::GetOutputTimeUnits());
    static void         SetSystemStartTime();

    static const double ConvertTime(double inputTime,
                                    const TIME_UNIT &fromUnits,
                                    const TIME_UNIT &toUnits);
    static const double ConvertOFToJ2000(double OFTime);
    static const double ConvertJ2000ToOF(double j2000Time);
    static const double GetStartTime(TIME_UNIT toUnits = UnitData::GetInternalTimeUnits());
    static const double GetEndTime(TIME_UNIT toUnits = UnitData::GetInternalTimeUnits());

    static const bool   ConvertToStoreTimeIndex(double inputTimeInstant,
            int &storeTimeIndex);

    static const int  GetSecondsPerTimeStep();
    static const int  GetStartYear();
    static const int  GetStartMonth();
    static const int  GetStartDay();
    static const int  GetEndYear();
    static const int  GetEndMonth();
    static const int  GetEndDay();

    static const int   GetNumberOfTimeSteps();
    static const int   GetReportStartIndexTime();
    static const int   GetReportEndIndexTime();
    static const int   GetReportableTimeSteps();
    static const int   GetTimeStep(int indexTime);
    static const int   GetIndexTime(double j2000Seconds);
    static const int   GetIndexTime(Calendar currentCalendar);
    static const int   GetStartIndexTime();
    static const int   GetEndIndexTime();
    static const int   GetSystemDuration();

    static const double GetJ2000Time(int indexTime);
    static const double GetOFTime(int indexTime);

    static const string GetStartClockTime();
    static const string GetClockTime(int indexTime,
                                     double additionalSeconds = 0.0);
    static const string GetStartDate();
    static const string GetDate(int indexTime,
                                double additionalSeconds = 0.0);
    static const string GetClockTimeAndDate(int indexTime,
                                            double additionalSeconds = 0.0);
    static const string GetSystemCurrentTime();
    static const string GetSystemStartTime();

    static const Calendar GetStartCalendar();
    static const Calendar GetEndCalendar();
    static const Calendar GetCalendar(int indexTime);

    /**
     * Sets the start Calendar
     *
     * @param *startCalendar Start calendar to set
     */
    static void SetSimStartCalendar(Calendar *startCalendar);

    /**
     * Sets the end Calendar
     *
     * @param *endCalendar End calendar to set
     */
    static void SetSimEndCalendar(Calendar *endCalendar);

    /**
     * Sets the Seconds Per Timestep
     *
     * @param secondsPerTimestep seconds per timestep
     */
    static void SetSecondsPerTimeStep(int secondsPerTimestep);

    /**
     * Sets the Number Of Timesteps Timestep
     *
     * @param numberOfTimesteps number of timesteps
     */
    static void SetNumberOfTimesteps(int numberOfTimesteps);

    /**
     * Sets the Day Boundary Start
     *
     * @param dayBoundaryStart day boundary start
     */
    static void SetDayBoundaryStart(int dayBoundaryStart);

    /**
     * Sets the Day Boundary End
     *
     * @param dayBoundaryEnd day boundary end
     */
    static void SetDayBoundaryEnd(int dayBoundaryEnd);

    /**
     * Sets the Leap Second Array
     *
     * @param dayBoundaryEnd day boundary end
     */
    static void SetLeapSecondArray(Array<int>* leapSecondArray);

    /**
     * Sets the Leap Second File Name
     *
     * @param leapSecondFileName leap second file name
     */
    static void SetLeapSecondFileName(string* leapSecondFileName);

protected:

private:

    static void         PrintTime(ostream &outfile,
                                  double printTime,
                                  const TIME_UNIT &printUnits);
	//RAGUSA 3/22/2019 - New method to print double precision duration output
	static void         PrintPreciseTime(ostream &outfile,
								  double printTime,
								  const TIME_UNIT &printUnits);

    static const int   GetCurrentSystemSeconds();

    static const string GetSystemTime(int seconds);

    /**
     * Gets the class name
     *
     * @return string
     */
    static inline string GetClassName();

    static int          secondsPerTimeStep_s;
    static int          numberOfTimeSteps_s;
    static int          dayBoundaryStartTimeSteps_s;
    static int          dayBoundaryEndTimeSteps_s;
    static int          systemStartSeconds_s;
    static const double  OF_START_IN_J2000_TIME_s;
    static shared_ptr<string> ptrLeapSecondFileName_s;
    static shared_ptr<Array<int>> ptrLeapSecondsArray_s;
    static shared_ptr<Calendar> ptrSimulationStart_s;
    static shared_ptr<Calendar> ptrSimulationEnd_s;
};

inline
string
TimePiece::GetClassName()
{
    return(TIME_PIECE_H);
}

#endif
