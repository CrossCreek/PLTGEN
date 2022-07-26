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
 * InputException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef INPUT_EXCEPTION_H
#define INPUT_EXCEPTION_H "InputException"

#include "Exception.h"
#include "general/utility/Constant.h"

/**
 * InputException is the exception that occurs when any input /
 *  output function cannot be performed. This mainly concerns reading
 *  from and writing to files..
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class InputException : public Exception
{
public:

    InputException();
    InputException(const string &currentClassName,
                   const string &currentMethodName,
                   string inputString = "");
    InputException(const InputException &copyMe);
    virtual ~InputException();

    InputException& operator = (const InputException &copyMe);

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;

protected:

private:




    string             *ptrInputString_a;
};

inline
int
InputException::ReportDetailedError() const
{
    if (ptrInputString_a != NULL) {
        cout << "INPUT  : " << *ptrInputString_a << NEW_LINE;
    }

    return(INPUT_EXCEPTION);
}

inline
const string
InputException::GetExceptionString() const
{
    return(INPUT_EXCEPTION_H);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
InputException::GetClassName()
{
    return(INPUT_EXCEPTION_H);
}
*/

#endif
