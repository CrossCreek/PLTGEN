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
 * Exception.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef EXCEPTION_H
#define EXCEPTION_H "Exception"

#include <iostream>
#include <string>

using namespace std;

template<class Type> class Array;

/**
 * Exception is the class that contains information about what
 *  error has occured in code processing. This class may be thrown
 *  whenever there is an error.
 *
 * Name                    Description (units)
 * -------------           -----------------------------------------------------
 * ptrMethodPathArray_a*   the method in which the error was thrown and (if
 *                           more than one string) the trail of methods
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class Exception
{
public:

    enum EXCEPTION_TYPE {
        UNKNOWN_EXCEPTION = -1,
        ARITHMETIC_EXCEPTION = 1,
        CLASS_CAST_EXCEPTION = 2,
        COMMAND_LINE_EXCEPTION = 3,
        DATE_FORMAT_EXCEPTION = 4,
        DIVIDE_BY_ZERO_EXCEPTION = 5,
        FILE_NOT_FOUND_EXCEPTION = 6,
        ILLEGAL_ANGLE_EXCEPTION = 7,
        INDEX_OUT_OF_BOUNDS_EXCEPTION = 8,
        INPUT_EXCEPTION = 9,
        LOAD_PROBLEM_EXCEPTION = 10,
        NULL_POINTER_EXCEPTION = 11,
        NUMBER_FORMAT_EXCEPTION = 12,
        OUTPUT_FILE_EXCEPTION = 13,
        PARSE_EXCEPTION = 14,
        REQUEST_GENERATION_EXCEPTION = 15,
        TIME_EXCEPTION = 16,
        TIME_FORMAT_EXCEPTION = 17,
        UPDATE_EXCEPTION = 18
    };

    Exception();
    Exception(const string &currentClassString,
              const string &currentMethodString);
    Exception(const Exception &copyMe);
    virtual ~Exception();

    Exception& operator =  (const Exception &copyMe);
    bool       operator == (const Exception &compareMe) const;
    bool       operator != (const Exception &compareMe) const;

    void    AddMethod(const string &className,
                      const string &methodName);
    int   ReportError() const;

    int    GetNumberOfMethodPaths() const;

    string  GetMethodPath(int pathIndex = 0) const;

    virtual inline int        ReportDetailedError() const;

    virtual inline const string GetExceptionString() const;

protected:

    Array<string> *ptrMethodPathArray_a;

private:



};

inline
int
Exception::ReportDetailedError() const
{
    return(UNKNOWN_EXCEPTION);
}

inline
const string
Exception::GetExceptionString() const
{
    return(EXCEPTION_H);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
Exception::GetClassName()
{
    return(EXCEPTION_H);
}
*/

#endif
