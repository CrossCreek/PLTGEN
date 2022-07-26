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
 * ClassCastException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef CLASS_CAST_EXCEPTION_H
#define CLASS_CAST_EXCEPTION_H "ClassCastException"

#include "Exception.h"
#include "general/utility/Constant.h"

using namespace std;

/**
 * ClassCastException is the exception that occurs when one class
 *  cannot be successfully cast unto another desired class.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class ClassCastException : public Exception
{
public:

    ClassCastException();
    ClassCastException(const string &currentClassName,
                       const string &currentMethodName);
    ClassCastException(const ClassCastException &copyMe);
    virtual ~ClassCastException();

    ClassCastException& operator = (const ClassCastException &copyMe);

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;

protected:

private:



};

inline
int
ClassCastException::ReportDetailedError() const
{
    return(CLASS_CAST_EXCEPTION);
}

inline
const string
ClassCastException::GetExceptionString() const
{
    return(CLASS_CAST_EXCEPTION_H);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
ClassCastException::GetClassName()
{
    return(CLASS_CAST_EXCEPTION_H);
}
*/

#endif
