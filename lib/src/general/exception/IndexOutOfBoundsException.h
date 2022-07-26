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
 * IndexOutOfBoundsException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef INDEX_OUT_OF_BOUNDS_EXCEPTION_H
#define INDEX_OUT_OF_BOUNDS_EXCEPTION_H "IndexOutOfBoundsException"

#include "Exception.h"
#include "general/utility/Constant.h"

using namespace std;

/**
 * IndexOutOfBoundsException is the exception that occurs when
 *  one attempts to index into an array with a number that is either
 *  less than zero, or outside of the available index slots.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * arraySize_a                  Size of the array (int)
 * arrayIndex_a                 Attempted index into the array (int)
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class IndexOutOfBoundsException : public Exception
{
public:

    IndexOutOfBoundsException();
    IndexOutOfBoundsException(const string &currentClassName,
                              const string &currentMethodName,
                              const int &arraySize,
                              const int &arrayIndex);
    IndexOutOfBoundsException(const IndexOutOfBoundsException &copyMe);
    virtual ~IndexOutOfBoundsException();

    IndexOutOfBoundsException& operator =  (const IndexOutOfBoundsException &copyMe);
    bool operator == (const IndexOutOfBoundsException &compareMe) const;

    inline int        ReportDetailedError() const;

    inline const string GetExceptionString() const;

    inline int         GetArraySize() const;
    inline int         GetArrayIndex() const;

protected:

private:




    int                arraySize_a;
    int                arrayIndex_a;
};

inline
int
IndexOutOfBoundsException::ReportDetailedError() const
{
    cout << "INDEX : " << arrayIndex_a << " (Size is " << arraySize_a << ")";
    cout << NEW_LINE;
    return(INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

inline
int
IndexOutOfBoundsException::GetArraySize() const
{
    return(arraySize_a);
}

inline
int
IndexOutOfBoundsException::GetArrayIndex() const
{
    return(arrayIndex_a);
}

inline
const string
IndexOutOfBoundsException::GetExceptionString() const
{
    return(INDEX_OUT_OF_BOUNDS_EXCEPTION_H);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
IndexOutOfBoundsException::GetClassName()
{
    return(INDEX_OUT_OF_BOUNDS_EXCEPTION_H);
}
*/

#endif
