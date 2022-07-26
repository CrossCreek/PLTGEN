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
 * ParseException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef PARSE_EXCEPTION_H
#define PARSE_EXCEPTION_H "ParseException"

#include "Exception.h"
#include "general/utility/Constant.h"

using namespace std;

/**
 * ParseException is the exception that occurs when a string is
 *  unable to be parsed correctly to obtain the necessary information.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * ptrParseString_a*            String that was attempted to be parsed
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class ParseException : public Exception
{
public:

    ParseException();
    ParseException(const string &currentClassName,
                   const string &currentMethodName,
                   const string &parseString);
    ParseException(const ParseException &copyMe);
    virtual ~ParseException();

    ParseException& operator =  (const ParseException &copyMe);
    inline bool     operator == (const ParseException &compareMe) const;
    inline bool     operator != (const ParseException &compareMe) const;

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;
    inline const string* GetParseString() const;

protected:

private:




    string             *ptrParseString_a;
};

inline
bool
ParseException::operator == (const ParseException &compareMe) const
{
    return( (Exception::operator == (compareMe)) &&
            (*ptrParseString_a == *(compareMe.ptrParseString_a)) );
}

inline
bool
ParseException::operator != (const ParseException &compareMe) const
{
    return(!(operator == (compareMe)));
}

inline
const string*
ParseException::GetParseString() const
{
    return(ptrParseString_a);
}

inline
int
ParseException::ReportDetailedError() const
{
    if (ptrParseString_a != NULL) {
        cout << "PARSE  : " << *ptrParseString_a << NEW_LINE;
    }

    return(PARSE_EXCEPTION);
}

inline
const string
ParseException::GetExceptionString() const
{
    return(PARSE_EXCEPTION_H);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
ParseException::GetClassName()
{
    return(PARSE_EXCEPTION_H);
}
*/

#endif
