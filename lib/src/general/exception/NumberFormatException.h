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
 * NumberFormatException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef NUMBER_FORMAT_EXCEPTION_H
#define NUMBER_FORMAT_EXCEPTION_H "NumberFormatException"

#include "Exception.h"
#include "general/utility/Constant.h"

using namespace std;

/**
 * NumberFormatException is the exception that occurs when a string
 *  is unsuccessfully converted to a number.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * ptrNumberString_a*           The string that was attempted to be converted
 *                                into a number (string pointer)
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class NumberFormatException : public Exception
{
public:

    NumberFormatException();
    NumberFormatException(const string &currentClassName,
                          const string &currentMethodName,
                          const string &numberString);
    NumberFormatException(const NumberFormatException &copyMe);
    virtual ~NumberFormatException();

    NumberFormatException& operator =  (const NumberFormatException &copyMe);
    inline bool      operator == (const NumberFormatException &compareMe) const;
    inline bool      operator != (const NumberFormatException &compareMe) const;

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;

    inline const string* GetNumberInQuestion() const;

protected:

private:




    string             *ptrNumberString_a;
};

inline
bool
NumberFormatException::operator == (const NumberFormatException &compareMe) const
{
    return( (Exception::operator == (compareMe)) &&
            (*ptrNumberString_a == *(compareMe.ptrNumberString_a)) );
}

inline
bool
NumberFormatException::operator != (const NumberFormatException &compareMe) const
{
    return(!(operator == (compareMe)));
}

inline
const string*
NumberFormatException::GetNumberInQuestion() const
{
    return(ptrNumberString_a);
}

inline
int
NumberFormatException::ReportDetailedError() const
{
    if (ptrNumberString_a != NULL) {
        cout << "NUMBER : " << *ptrNumberString_a << NEW_LINE;
    }

    return(NUMBER_FORMAT_EXCEPTION);
}

inline
const string
NumberFormatException::GetExceptionString() const
{
    return(NUMBER_FORMAT_EXCEPTION_H);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
NumberFormatException::GetClassName()
{
    return(NUMBER_FORMAT_EXCEPTION_H);
}
*/

#endif
