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
 * UpdateException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef UPDATE_EXCEPTION_H
#define UPDATE_EXCEPTION_H "UpdateException"

#include "Exception.h"
#include "general/utility/Constant.h"

/**
 *  Exception which occurs during an update.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * ptrUpdateErrorString_a*      Specific update error incurred
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class UpdateException : public Exception
{
public:

    UpdateException();
    UpdateException(const string &currentClassName,
                    const string &currentMethodName,
                    string updateErrorString = "");
    UpdateException(const UpdateException &copyMe);
    virtual ~UpdateException();

    UpdateException& operator = (const UpdateException &copyMe);

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;

protected:

private:




    string             *ptrUpdateErrorString_a;
};

inline
int
UpdateException::ReportDetailedError() const
{
    if (ptrUpdateErrorString_a != NULL) {
        cout << "UPDATE: " << *ptrUpdateErrorString_a << NEW_LINE;
    }

    return(UPDATE_EXCEPTION);
}

inline
const string
UpdateException::GetExceptionString() const
{
    return(UPDATE_EXCEPTION_H);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
UpdateException::GetClassName()
{
    return(UPDATE_EXCEPTION_H);
}
*/

#endif
