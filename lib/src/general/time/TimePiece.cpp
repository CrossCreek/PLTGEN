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
 * TimePiece.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/



#include <fstream>
#include <ctime>
#include "TimePiece.h"
#include "general/data/InputData.h"
#include "general/data/InputDataElement.h"
//#include "general/array/ListIterator.h"

int        TimePiece::secondsPerTimeStep_s = 0;
int         TimePiece::numberOfTimeSteps_s = 0;
int         TimePiece::dayBoundaryStartTimeSteps_s = -1;
int         TimePiece::dayBoundaryEndTimeSteps_s = -1;
int         TimePiece::systemStartSeconds_s = 0;
const double TimePiece::OF_START_IN_J2000_TIME_s = -883656000.00;
shared_ptr<Array<int>> TimePiece::ptrLeapSecondsArray_s = NULL;
shared_ptr<string>      TimePiece::ptrLeapSecondFileName_s = NULL;
shared_ptr<Calendar>    TimePiece::ptrSimulationStart_s = NULL;
shared_ptr<Calendar>    TimePiece::ptrSimulationEnd_s = NULL;

//------------------------------------------------------------------------------
//
//  Method Name: TimePiece::TimePiece
//
//  Purpose:
//    Constructor for the TimePiece class.
//
//  Calling Method:
//     TimePiece();
//
//  Calling Parameters
//  Parameters      i/o  description
//  ----------      ---  -----------
//
//  Returns:
//     None
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

TimePiece::TimePiece()
{
    return;
}

TimePiece::TimePiece(const TimePiece &TimePiece)
{
    return;
}

bool
TimePiece::operator == (const TimePiece &compareMe) const
{
    return(true);
}

bool
TimePiece::operator != (const TimePiece &compareMe) const
{
    return(false);
}

void
TimePiece::PrintAttributes(ostream &outfile)
{
    const TIME_UNIT inputTimeUnits = UnitData::GetInputTimeUnits();
    outfile << "<TIME_PIECE_START>" << NEW_LINE;

    if (ptrSimulationStart_s != NULL) {
        outfile << "   Start Date                : ";
        outfile << ptrSimulationStart_s->GetDateDisplay() << NEW_LINE;
        outfile << "   Start Time                : ";
        outfile << ptrSimulationStart_s->GetTimeDisplay() << NEW_LINE;
    }

    if (ptrSimulationEnd_s != NULL) {
        outfile << "   End Date                  : ";
        outfile << ptrSimulationEnd_s->GetDateDisplay() << NEW_LINE;
        outfile << "   End Time                  : ";
        outfile << ptrSimulationEnd_s->GetTimeDisplay() << NEW_LINE;
    }

    outfile << "   Seconds Per Time Step     : " << secondsPerTimeStep_s << NEW_LINE;
    outfile.unsetf(ios::right);
    outfile.setf(ios::left);
    outfile << "   Day Boundary Offset Start : ";
    PrintDuration(outfile, dayBoundaryStartTimeSteps_s, inputTimeUnits);
    outfile << NEW_LINE;
    outfile << "   Day Boundary Offset End   : ";
    PrintDuration(outfile, dayBoundaryEndTimeSteps_s, inputTimeUnits);
    outfile << NEW_LINE;

    if (ptrLeapSecondFileName_s != NULL) {
        outfile << "   Leap Second File Name     : ";
        outfile << *ptrLeapSecondFileName_s << NEW_LINE;
    }

    outfile.unsetf(ios::left);
    outfile.setf(ios::right);
    outfile << "<TIME_PIECE_END>" << NEW_LINE;
    return;
}

void
TimePiece::SetSystemStartTime()
{
    try {
        systemStartSeconds_s = GetCurrentSystemSeconds();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "SetSystemStartTime()");
        throw;
    }

    return;
}



const int
TimePiece::GetCurrentSystemSeconds()
{
    int seconds = 0;
    // Sun logic removed
    time_t s;
    time(&s);
    seconds = static_cast<int>(s);
    return(seconds);
}

const string
TimePiece::GetSystemTime(int seconds)
{
    time_t s = static_cast<time_t>(seconds);
    string timeDateString(ctime(&s));
    return(timeDateString);
}

const double
TimePiece::ConvertTime(double inputTime,
                       const TIME_UNIT &fromUnits,
                       const TIME_UNIT &toUnits)
{
    double returnTime = (double)inputTime;

    if (fromUnits != toUnits) {
        if (fromUnits == TIME_STEPS) {
            if (toUnits == MINUTES) {
                returnTime *= ((double)secondsPerTimeStep_s * MINUTES_PER_SECOND);
            } else if (toUnits == HOURS) {
                returnTime *= ((double)secondsPerTimeStep_s * HOURS_PER_SECOND);
            } else {
                returnTime *= (double)secondsPerTimeStep_s;

                if (toUnits == J2000) {
                    returnTime += (ptrSimulationStart_s->GetJ2000Time() - (double)secondsPerTimeStep_s);
                }
            }
        } else if (fromUnits == MINUTES) {
            if (toUnits == TIME_STEPS) {
                returnTime *= (SECONDS_PER_MINUTE / (double)secondsPerTimeStep_s);
            } else if (toUnits == HOURS) {
                returnTime *= HOURS_PER_MINUTE;
            } else {
                returnTime *= SECONDS_PER_MINUTE;

                if (toUnits == J2000) {
                    returnTime += (ptrSimulationStart_s->GetJ2000Time() - (double)secondsPerTimeStep_s);
                }
            }
        } else if (fromUnits == HOURS) {
            if (toUnits == TIME_STEPS) {
                returnTime *= (SECONDS_PER_HOUR / (double)secondsPerTimeStep_s);
            } else if (toUnits == MINUTES) {
                returnTime *= MINUTES_PER_HOUR;
            } else {
                returnTime *= SECONDS_PER_HOUR;

                if (toUnits == J2000) {
                    returnTime += (ptrSimulationStart_s->GetJ2000Time() - (double)secondsPerTimeStep_s);
                }
            }
        } else if (fromUnits == SECONDS) {
            if (toUnits == TIME_STEPS) {
                returnTime /= (double)secondsPerTimeStep_s;
            } else if (toUnits == MINUTES) {
                returnTime *= MINUTES_PER_SECOND;
            } else if (toUnits == HOURS) {
                returnTime *= HOURS_PER_SECOND;
            } else {
                returnTime += (ptrSimulationStart_s->GetJ2000Time() - (double)secondsPerTimeStep_s);
            }
        } else if (fromUnits == J2000) {
            // first convert to seconds
            returnTime -= (ptrSimulationStart_s->GetJ2000Time() - (double)secondsPerTimeStep_s);

            if (toUnits == TIME_STEPS) {
                returnTime /= (double)secondsPerTimeStep_s;
            } else if (toUnits == MINUTES) {
                returnTime *= MINUTES_PER_SECOND;
            } else if (toUnits == HOURS) {
                returnTime *= HOURS_PER_SECOND;
            }
        }
    }

    return(returnTime);
}

const bool
TimePiece::ConvertToStoreTimeIndex(double inputTimeInstant,
                                   int &storeTimeIndex)
{
    bool            withinSimulation = false;
    const TIME_UNIT inputUnits = UnitData::GetInputTimeUnits();

    if ( (inputTimeInstant >= GetStartTime(inputUnits)) &&
            (inputTimeInstant <= GetEndTime(inputUnits)) ) {
        withinSimulation = true;
        storeTimeIndex = UnitData::GetInternalTimeIndex(inputTimeInstant);
    } else {
        storeTimeIndex = -1;
    }

    return(withinSimulation);
}

const double
TimePiece::GetStartTime(TIME_UNIT toUnits)
{
    return(ConvertTime(ptrSimulationStart_s->GetJ2000Time(), J2000, toUnits));
}

const double
TimePiece::GetEndTime(TIME_UNIT toUnits)
{
    return(ConvertTime(ptrSimulationEnd_s->GetJ2000Time(), J2000, toUnits));
}

void
TimePiece::PrintIndex(ostream &outfile,
                      int storeTimeIndex,
                      TIME_UNIT printUnits)
{
    const double printTime = ConvertTime((double)(storeTimeIndex + 1),
                                         UnitData::GetInternalTimeUnits(), printUnits);
    PrintTime(outfile, printTime, printUnits);
    return;
}

void
TimePiece::PrintDuration(ostream &outfile,
                         int storeTimeDuration,
                         TIME_UNIT printUnits)
{
    const double printTime = ConvertTime((double)storeTimeDuration,
                                         UnitData::GetInternalTimeUnits(), printUnits);
    PrintTime(outfile, printTime, printUnits);
    return;
}

//RAGUSA 3/18/2019 - Overloaded method to allow double type storeTimeDuration
void
TimePiece::PrintDuration(ostream &outfile,
	double storeTimeDuration,
	TIME_UNIT printUnits)
{
	const double printTime = ConvertTime(storeTimeDuration,
		UnitData::GetInternalTimeUnits(), printUnits);
		PrintPreciseTime(outfile, printTime, printUnits);
	return;
}
//RAGUSA 3/22/2019 - New method to print double precision duration output
void
TimePiece::PrintPreciseTime(ostream &outfile,
	double printTime,
	const TIME_UNIT &printUnits)
{
	int width = 7;
	int precision = 2;

	if (printUnits == MINUTES) {
		if ((secondsPerTimeStep_s % (int)SECONDS_PER_MINUTE) != 0) {
			precision = 2;
		}
	}
	else if (printUnits == HOURS) {
		if ((secondsPerTimeStep_s % (int)SECONDS_PER_HOUR) != 0) {
			precision = 4;
		}
	}
	else if (printUnits == SECONDS) {
		width = 11;
	}

	outfile << setw(width) << setprecision(precision) << printTime;

	return;
}

void
TimePiece::PrintTime(ostream &outfile,
                     double printTime,
                     const TIME_UNIT &printUnits)
{
    int width = 7;
    int precision = 0;

    if (printUnits == MINUTES) {
        if ((secondsPerTimeStep_s % (int)SECONDS_PER_MINUTE) != 0) {
            precision = 2;
        }
    } else if (printUnits == HOURS) {
        if ((secondsPerTimeStep_s % (int)SECONDS_PER_HOUR) != 0) {
            precision = 4;
        }
    } else if (printUnits == SECONDS) {
        width = 11;
    }

    if (precision == 0) {
        outfile << setw(width) << (int)printTime;
    } else {
        outfile << setw(width) << setprecision(precision) << printTime;
    }

    return;
}

const double
TimePiece::ConvertOFToJ2000(double OFTime)
{
    int   leapSecondIndex = 0;
    double returnJ2000Time = OFTime + OF_START_IN_J2000_TIME_s;
    bool   foundAllLeapSeconds = false;

    if (ptrLeapSecondsArray_s == NULL) {
        throw new InputException(GetClassName(), "ConvertOFToJ2000(const double&)",
                                 "Must input Leap Second File for OF time conversions");
    }

    while ( (leapSecondIndex < ptrLeapSecondsArray_s->Size()) &&
            (foundAllLeapSeconds == false) ) {
        if ((int)OFTime < ptrLeapSecondsArray_s->ElementAt(leapSecondIndex)) {
            foundAllLeapSeconds = true;
        } else {
            ++leapSecondIndex;
            returnJ2000Time -= 1.0;
        }
    }

    return(returnJ2000Time);
}

const double
TimePiece::ConvertJ2000ToOF(double j2000Time)
{
    int   leapSecondIndex = 0;
    double returnOFTime = j2000Time - OF_START_IN_J2000_TIME_s;
    bool   foundAllLeapSeconds = false;

    if (ptrLeapSecondsArray_s == NULL) {
        throw new InputException(GetClassName(), "ConvertOFToJ2000(const double&)",
                                 "Must input Leap Second File for OF time conversions");
    }

    while ( (leapSecondIndex < ptrLeapSecondsArray_s->Size()) &&
            (foundAllLeapSeconds == false) ) {
        if ((int)returnOFTime < ptrLeapSecondsArray_s->ElementAt(leapSecondIndex)) {
            foundAllLeapSeconds = true;
        } else {
            ++leapSecondIndex;
            returnOFTime += 1.0;
        }
    }

    return(returnOFTime);
}

/* ***************************************************************************** */

void
TimePiece::SetSimStartCalendar(Calendar *startCalendar)
{
    ptrSimulationStart_s = shared_ptr<Calendar>(startCalendar);
}

/* ***************************************************************************** */

void
TimePiece::SetSimEndCalendar(Calendar *endCalendar)
{
    ptrSimulationEnd_s =  shared_ptr<Calendar>(endCalendar);
}

/* ***************************************************************************** */

void
TimePiece::SetSecondsPerTimeStep(int secondsPerTimestep)
{
    secondsPerTimeStep_s = secondsPerTimestep;
}

/* ***************************************************************************** */

void
TimePiece::SetNumberOfTimesteps(int numberOfTimesteps)
{
    numberOfTimeSteps_s = numberOfTimesteps;
}

/* ***************************************************************************** */

void
TimePiece::SetDayBoundaryStart(int dayBoundaryStart)
{
    dayBoundaryStartTimeSteps_s = dayBoundaryStart;
}

/* ***************************************************************************** */

void
TimePiece::SetDayBoundaryEnd(int dayBoundaryEnd)
{
    dayBoundaryEndTimeSteps_s = dayBoundaryEnd;
}

/* ***************************************************************************** */

void
TimePiece::SetLeapSecondArray(Array<int>* leapSecondArray)
{
    ptrLeapSecondsArray_s =  shared_ptr<Array<int>>(leapSecondArray);
}

/* ***************************************************************************** */

void
TimePiece::SetLeapSecondFileName(string* leapSecondFileName)
{
    ptrLeapSecondFileName_s = shared_ptr<string>( leapSecondFileName);
}

/* ***************************************************************************** */

const int
TimePiece::GetSecondsPerTimeStep()
{
    return(secondsPerTimeStep_s);
}

/* ***************************************************************************** */

const int
TimePiece::GetStartYear()
{
    return(ptrSimulationStart_s->GetYear());
}

/* ***************************************************************************** */

const int
TimePiece::GetStartMonth()
{
    return(ptrSimulationStart_s->GetMonth());
}

/* ***************************************************************************** */

const int
TimePiece::GetStartDay()
{
    return(ptrSimulationStart_s->GetDay());
}

/* ***************************************************************************** */

const int
TimePiece::GetEndYear()
{
    return(ptrSimulationEnd_s->GetYear());
}

/* ***************************************************************************** */

const int
TimePiece::GetEndMonth()
{
    return(ptrSimulationEnd_s->GetMonth());
}

/* ***************************************************************************** */

const int
TimePiece::GetEndDay()
{
    return(ptrSimulationEnd_s->GetDay());
}

/* ***************************************************************************** */

const int
TimePiece::GetNumberOfTimeSteps()
{
    return(numberOfTimeSteps_s);
}

/* ***************************************************************************** */

const int
TimePiece::GetReportStartIndexTime()
{
    return(dayBoundaryStartTimeSteps_s);
}

/* ***************************************************************************** */

const int
TimePiece::GetReportEndIndexTime()
{
    return(GetEndIndexTime() - dayBoundaryEndTimeSteps_s);
}

/* ***************************************************************************** */

const int
TimePiece::GetReportableTimeSteps()
{
    return(GetReportEndIndexTime() - GetReportStartIndexTime() + 1);
}

/* ***************************************************************************** */

const double
TimePiece::GetJ2000Time(int indexTime)
{
    return(ptrSimulationStart_s->GetJ2000Time()
           + (double)(indexTime * secondsPerTimeStep_s));
}

/* ***************************************************************************** */

const double
TimePiece::GetOFTime(int indexTime)
{
    const double j2000Time = GetJ2000Time(indexTime);
    return(ConvertJ2000ToOF(j2000Time));
}

/* ***************************************************************************** */

const Calendar
TimePiece::GetStartCalendar()
{
    return(*ptrSimulationStart_s);
}

/* ***************************************************************************** */

const Calendar
TimePiece::GetEndCalendar()
{
    return(*ptrSimulationEnd_s);
}

/* ***************************************************************************** */

const Calendar
TimePiece::GetCalendar(int indexTime)
{
    return(ptrSimulationStart_s->GetJ2000Time()
           + ((double)indexTime * secondsPerTimeStep_s));
}

/* ***************************************************************************** */

const int
TimePiece::GetTimeStep(int indexTime)
{
    return(indexTime + 1);
}

/* ***************************************************************************** */

const int
TimePiece::GetStartIndexTime()
{
    return(0);
}

/* ***************************************************************************** */

const int
TimePiece::GetEndIndexTime()
{
    return(numberOfTimeSteps_s - 1);
}

/* ***************************************************************************** */

const int
TimePiece::GetIndexTime(double j2000Seconds)
{
    int returnTimeStep = 0;

    if ( (j2000Seconds < ptrSimulationStart_s->GetJ2000Time())
            || (j2000Seconds > ptrSimulationEnd_s->GetJ2000Time()) ) {
        throw new TimeException(GetClassName(), "GetIndexTime(const double&)");
    }

    returnTimeStep = (int)((j2000Seconds - ptrSimulationStart_s->GetJ2000Time())
                           / secondsPerTimeStep_s);
    return(returnTimeStep);
}

/* ***************************************************************************** */

const int
TimePiece::GetIndexTime(Calendar currentCalendar)
{
    return(TimePiece::GetIndexTime(currentCalendar.GetJ2000Time()));
}

/* ***************************************************************************** */

const string
TimePiece::GetStartClockTime()
{
    return(ptrSimulationStart_s->GetTimeDisplay());
}

/* ***************************************************************************** */

const string
TimePiece::GetClockTime(int indexTime,
                        double additionalSeconds)
{
    Calendar calendar(GetJ2000Time(indexTime) + additionalSeconds);
    return(calendar.GetTimeDisplay());
}

/* ***************************************************************************** */

const string
TimePiece::GetStartDate()
{
    return(ptrSimulationStart_s->GetDateDisplay());
}

/* ***************************************************************************** */

const string
TimePiece::GetDate(int indexTime,
                   double additionalSeconds)
{
    Calendar calendar = Calendar(GetJ2000Time(indexTime) + additionalSeconds);
    return(calendar.GetDateDisplay());
}

/* ***************************************************************************** */

const string
TimePiece::GetClockTimeAndDate(int indexTime,
                               double additionalSeconds)
{
    Calendar calendar = Calendar(GetJ2000Time(indexTime) + additionalSeconds);
    return(calendar.GetDateAndTimeDisplay());
}

/* ***************************************************************************** */

const string
TimePiece::GetSystemCurrentTime()
{
    return(GetSystemTime(GetCurrentSystemSeconds()));
}

/* ***************************************************************************** */

const string
TimePiece::GetSystemStartTime()
{
    return(GetSystemTime(systemStartSeconds_s));
}

/* ***************************************************************************** */

const int
TimePiece::GetSystemDuration()
{
    double duration = (double)(GetCurrentSystemSeconds() - systemStartSeconds_s + 30);
    return((int)(duration * MINUTES_PER_SECOND));
}

TimePiece::~TimePiece()
{
    return;
}
