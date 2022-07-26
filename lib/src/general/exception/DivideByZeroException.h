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
 * DivideByZeroException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef DIVIDE_BY_ZERO_EXCEPTION_H
#define DIVIDE_BY_ZERO_EXCEPTION_H "DivideByZeroException"

#include "Exception.h"

using namespace std;

/**
 *  DivideByZeroException is the exception that occurs when one
 *  attempts to divide a number by zero.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class DivideByZeroException : public Exception
{
public:

    DivideByZeroException();
    DivideByZeroException(const string &currentClassName,
                          const string &currentMethodName);
    DivideByZeroException(const DivideByZeroException &copyMe);
    virtual ~DivideByZeroException();

    DivideByZeroException& operator = (const DivideByZeroException &copyMe);

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;

protected:

private:



};

inline
int
DivideByZeroException::ReportDetailedError() const
{
    return(DIVIDE_BY_ZERO_EXCEPTION);
}

inline
const string
DivideByZeroException::GetExceptionString() const
{
    return(DIVIDE_BY_ZERO_EXCEPTION_H);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
DivideByZeroException::GetClassName()
{
    return(DIVIDE_BY_ZERO_EXCEPTION_H);
}
*/

#endif
