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
 * TimeFormatException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef TIME_FORMAT_EXCEPTION_H
#define TIME_FORMAT_EXCEPTION_H "TimeFormatException"

#include "Exception.h"
#include "general/utility/Constant.h"

using namespace std;

/**
 * TimeFormatException is the exception that occurs when a supposed
 *  time input has an invalid format.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * ptrTimeString_a*             The time in question
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class TimeFormatException : public Exception
{
public:

    TimeFormatException();
    TimeFormatException(const string &currentClassName,
                        const string &currentMethodName,
                        const string &timeString);
    TimeFormatException(const TimeFormatException &previous,
                        const string &currentClassName,
                        const string &currentMethodName);
    TimeFormatException(const TimeFormatException &copyMe);
    virtual ~TimeFormatException();

    TimeFormatException& operator =  (const TimeFormatException &copyMe);
    inline bool        operator == (const TimeFormatException &compareMe) const;
    inline bool        operator != (const TimeFormatException &compareMe) const;

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;
    inline const string* GetTimeInQuestion() const;

protected:

private:

    static inline string GetClassName();

    string             *ptrTimeString_a;
};

inline
bool
TimeFormatException::operator == (const TimeFormatException &compareMe) const
{
    return( (Exception::operator == (compareMe)) &&
            (*ptrTimeString_a == *(compareMe.ptrTimeString_a)) );
}

inline
bool
TimeFormatException::operator != (const TimeFormatException &compareMe) const
{
    return(!(operator == (compareMe)));
}

inline
const string*
TimeFormatException::GetTimeInQuestion() const
{
    return(ptrTimeString_a);
}

inline
int
TimeFormatException::ReportDetailedError() const
{
    if (ptrTimeString_a != NULL) {
        cout << "TIME   : " << *ptrTimeString_a << NEW_LINE;
    }

    return(TIME_FORMAT_EXCEPTION);
}

inline
const string
TimeFormatException::GetExceptionString() const
{
    return(TIME_FORMAT_EXCEPTION_H);
}

inline
string
TimeFormatException::GetClassName()
{
    return(TIME_FORMAT_EXCEPTION_H);
}

#endif
