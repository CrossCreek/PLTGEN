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
 * TimeException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef TIME_EXCEPTION_H
#define TIME_EXCEPTION_H "TimeException"

#include "Exception.h"
#include "general/utility/Constant.h"

using namespace std;

/**
 * A exception which occurs when an error during a time calculation occurs.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * ptrTimeErrorString_a*        Specific time error incurred
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class TimeException : public Exception
{
public:

    TimeException();
    TimeException(const string &currentClassName,
                  const string &currentMethodName);
    TimeException(const string &currentClassName,
                  const string &currentMethodName,
                  const string &timeErrorString);
    TimeException(const TimeException &copyMe);
    virtual ~TimeException();

    TimeException& operator = (const TimeException &copyMe);

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;

protected:

private:




    string             *ptrTimeErrorString_a;
};

inline
int
TimeException::ReportDetailedError() const
{
    if (ptrTimeErrorString_a != NULL) {
        cout << "TIME  : " << *ptrTimeErrorString_a << NEW_LINE;
    }

    return(TIME_EXCEPTION);
}

inline
const string
TimeException::GetExceptionString() const
{
    return(TIME_EXCEPTION_H);
}


//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
TimeException::GetClassName()
{
    return(TIME_EXCEPTION_H);
}
*/

#endif
