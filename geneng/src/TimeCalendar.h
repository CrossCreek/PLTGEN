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
 * TimeCalendar.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : TimeCalendar.h

//  NAMESPACE : A3

//  FILE      : TimeCalendar.h

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-001  09/26/1997  JAM  Initial Release
//              V1.3-006  12/20/1999  JAM  reorganized manipulator decl & defs
//                                         to reduce compile time overhead;
//                                         made IoManipulator more
//                                         flexible; changed ioSeparator from
//                                         global to stream dependent (hence
//                                         more flexible) like other manips
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.5      2009-07-01  JAM  added cast from size_t to int
//                                         at 1 line to eliminate conversion
//                                         warning, move i/o to TimeJ2000,
//                                         add extraction stream state check
//                                         before calling TimeJ2000 extraction
//                                         operator
//              V5.0      2010-07-15  JAM  move attributes from protected to
//                                         private, moved class static defs

//  NOTES : base header file for the time class family
//          smallest value is millisecond

// ============================================================================

#ifndef TIMECALENDAR_H
#define TIMECALENDAR_H   "TimeCalendar.h V5.0"

#include "TimeBase.h"
#include "TestValue.h"

#include <string>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class TimeJ2000;

class TimeCalendar
{
public:

    // -------------------------------------------------------------------
    // constructors and destructors
    TimeCalendar();

    TimeCalendar( int year, int month,  int day,
                  int hour, int minute, double second );

    TimeCalendar( int year, int month,  int day,
                  double secondsSinceMidnight );


    TimeCalendar( TimeJ2000 time );
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit TimeCalendar( TimeJ2000 time );


    TimeCalendar( const std::string& sYMDHMS );
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit TimeCalendar( const std::string& sYMDHMS );

    // -------------------------------------------------------------------
    // access methods
    void Get( int& year, int& month,  int& day,
              int& hour, int& minute, double& second ) const;

    void Get( int& year, int& month,  int& day,
              double& secondsSinceMidnight ) const;

    void Set( int year, int month,  int day,
              int hour, int minute, double second );

    void Set( int year, int month,  int day,
              double secondsSinceMidnight );

    int    GetYear() const;
    int    GetMonth() const;
    int    GetDay() const;
    int    GetHour() const;
    int    GetMinute() const;
    double GetSecond() const;
    double GetSecondsSinceMidnight() const;

    void GetMonthShort( std::string& strMonth ) const;
    void GetMonthLong( std::string& strMonth ) const;

    void SetYear( int year );
    void SetMonth( int month );
    void SetDay( int day );
    void SetHour( int hour );
    void SetMinute( int minute );
    void SetSecond( double second );
    void SetSecondsSinceMidnight( double secondsSinceMidnight );

    void SetMonth( const std::string& strMonth );

    // -------------------------------------------------------------------
    // operators
    operator std::string() const;

    // -------------------------------------------------------------------
    // I/O functions

    // ---------------------------------------------------------------------
protected:
    // ---------------------------------------------------------------------
private:
    // attributes
    int m_year, m_month, m_day, m_hour, m_minute;
    double m_second;

    // class static members
    static const char *const s_MONTHSHORT[];
    static const char *const s_MONTHLONG[];
    static const std::string s_MONTHCMP;
    static const int s_YEAR_INC;

    static const TestBounds< double > s_SECOND_FRACTION_LIMITS;
    static const TestBounds< int > s_SECOND_LIMITS;
    static const TestBounds< int > s_MINUTE_LIMITS;
    static const TestBounds< int > s_HOUR_LIMITS;
    static const TestBounds< int > s_DAY_LIMITS;
    static const TestBounds< int > s_MONTH_LIMITS;
    static const TestBounds< int > s_YEAR_LIMITS;
};

// ----------------------------------------------------------------------------
// global prototypes
// i/o operators
std::ostream& operator<<(std::ostream& os, const TimeCalendar &t);
std::istream& operator>>(std::istream& is, TimeCalendar &t);

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif // TIMECALENDAR_H
