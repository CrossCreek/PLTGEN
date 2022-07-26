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
 * DateFormatException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef DATE_FORMAT_EXCEPTION_H
#define DATE_FORMAT_EXCEPTION_H "DateFormatException"

#include "general/utility/Constant.h"
#include "Exception.h"

using namespace std;

/**
 * DateFormatException is the exception that occurs when a string
 *  is unsuccessfully attempted to be converted to a date.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------------
 * ptrDateString_a              The string that was attempted to be formulated
 *                                into a date (string pointer)
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class DateFormatException : public Exception
{
public:

    DateFormatException();
    DateFormatException(const string &currentClassName,
                        const string &currentMethodName,
                        const string &dateString);
    DateFormatException(const DateFormatException &copyMe);
    virtual ~DateFormatException();

    DateFormatException& operator =  (const DateFormatException &copyMe);
    inline bool        operator == (const DateFormatException &compareMe) const;
    inline bool        operator != (const DateFormatException &compareMe) const;

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;

    inline const string* GetDateInQuestion() const;

protected:

private:




    string             *ptrDateString_a;

};

inline
bool
DateFormatException::operator == (const DateFormatException &compareMe) const
{
    return( (Exception::operator == (compareMe)) &&
            (*ptrDateString_a == *(compareMe.ptrDateString_a)) );
}

inline
bool
DateFormatException::operator != (const DateFormatException &compareMe) const
{
    return (!(operator == (compareMe)));
}

inline
int
DateFormatException::ReportDetailedError() const
{
    if (ptrDateString_a != NULL) {
        cout << "DATE   : " << *ptrDateString_a << NEW_LINE;
    }

    return(DATE_FORMAT_EXCEPTION);
}

inline
const string
DateFormatException::GetExceptionString() const
{
    return(DATE_FORMAT_EXCEPTION_H);
}

inline
const string*
DateFormatException::GetDateInQuestion() const
{
    return(ptrDateString_a);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
DateFormatException::GetClassName()
{
    return(DATE_FORMAT_EXCEPTION_H);
}
*/

#endif
