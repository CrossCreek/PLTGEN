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
 * NullPointerException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef NULL_POINTER_EXCEPTION_H
#define NULL_POINTER_EXCEPTION_H "NullPointerException"

#include "Exception.h"
#include "general/utility/Constant.h"

using namespace std;

/**
 * NullPointerException is the exception that occurs whenever a
 *  null pointer is encountered in a situation where a null pointer
 *  is unacceptable.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * ptrPointerNameString_a*      Name of the pointer that was null when accessed
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class NullPointerException : public Exception
{
public:

    NullPointerException();
    NullPointerException(const string &currentClassName,
                         const string &currentMethodName,
                         string pointerName = "");
    NullPointerException(const NullPointerException &copyMe);
    virtual ~NullPointerException();

    NullPointerException& operator = (const NullPointerException &copyMe);

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;

protected:

private:




    string             *ptrPointerNameString_a;
};

inline
int
NullPointerException::ReportDetailedError() const
{
    if (ptrPointerNameString_a != NULL) {
        cout << "NAME   : " << *ptrPointerNameString_a << NEW_LINE;
    }

    return(NULL_POINTER_EXCEPTION);
}

inline
const string
NullPointerException::GetExceptionString() const
{
    return(NULL_POINTER_EXCEPTION_H);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
NullPointerException::GetClassName()
{
    return(NULL_POINTER_EXCEPTION_H);
}
*/

#endif
