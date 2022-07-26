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
 * ArithmeticException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ARITHMETIC_EXCEPTION_H
#define ARITHMETIC_EXCEPTION_H "ArithmeticException"

#include "Exception.h"

using namespace std;

/**
 * ArithmeticException is the exception that occurs when a
 *  desired arithmetic operation cannot be performed.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class ArithmeticException : public Exception
{
public:

    ArithmeticException();
    ArithmeticException(const string &currentClassName,
                        const string &currentMethodName);
    ArithmeticException(const ArithmeticException &copyMe);
    virtual ~ArithmeticException();

    ArithmeticException& operator = (const ArithmeticException &copyMe);

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;

protected:

private:



};

inline
int
ArithmeticException::ReportDetailedError() const
{
    return(ARITHMETIC_EXCEPTION);
}

inline
const string
ArithmeticException::GetExceptionString() const
{
    return(ARITHMETIC_EXCEPTION_H);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
ArithmeticException::GetClassName()
{
    return(ARITHMETIC_EXCEPTION_H);
}
*/

#endif
