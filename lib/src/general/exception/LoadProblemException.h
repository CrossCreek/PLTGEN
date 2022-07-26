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
 * LoadProblemException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef LOAD_PROBLEM_EXCEPTION_H
#define LOAD_PROBLEM_EXCEPTION_H "LoadProblemException"

#include "Exception.h"
#include "general/utility/Constant.h"

/**
 * LoadProblemException is the exception that occurs when data
 *     cannot be loaded properly.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * ptrLoadProblemErrorString_a* Specific loading problem error incurred
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class LoadProblemException : public Exception
{
public:

    LoadProblemException();
    LoadProblemException(const string &currentClassName,
                         const string &currentMethodName,
                         const string &loadErrorString);
    LoadProblemException(const LoadProblemException &exception);
    virtual ~LoadProblemException();

    LoadProblemException& operator = (const LoadProblemException &copyMe);

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;

protected:

private:




    string             *ptrLoadProblemErrorString_a;
};

inline
int
LoadProblemException::ReportDetailedError() const
{
    if (ptrLoadProblemErrorString_a != NULL) {
        cout << "PROBLEM: " << *ptrLoadProblemErrorString_a << NEW_LINE;
    }

    return(LOAD_PROBLEM_EXCEPTION);
}

inline
const string
LoadProblemException::GetExceptionString() const
{
    return(LOAD_PROBLEM_EXCEPTION_H);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
LoadProblemException::GetClassName()
{
    return(LOAD_PROBLEM_EXCEPTION_H);
}
*/

#endif
