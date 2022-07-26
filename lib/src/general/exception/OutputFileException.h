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
 * OutputFileException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef OUTPUT_FILE_EXCEPTION_H
#define OUTPUT_FILE_EXCEPTION_H "OutputFileException"

#include "Exception.h"
#include "general/utility/Constant.h"

using namespace std;

/**
 * OutputFileException is the exception that occurs when an output file
 *  cannot be opened, thereby inhibiting the reporting of vital data.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * ptrFileNameString_a*         Name of the output file not opened
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class OutputFileException : public Exception
{
public:

    OutputFileException();
    OutputFileException(const string &currentClassName,
                        const string &currentMethodName,
                        const string &fileName);
    OutputFileException(const OutputFileException &copyMe);
    virtual ~OutputFileException();

    OutputFileException& operator =  (const OutputFileException &copyMe);
    inline bool        operator == (const OutputFileException &compareMe) const;
    inline bool        operator != (const OutputFileException &compareMe) const;

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;
    inline const string* GetFileName() const;

protected:

private:




    string             *ptrFileNameString_a;
};

inline
bool
OutputFileException::operator == (const OutputFileException &compareMe) const
{
    return( (Exception::operator == (compareMe)) &&
            (*ptrFileNameString_a == *(compareMe.ptrFileNameString_a)) );
}

inline
bool
OutputFileException::operator != (const OutputFileException &compareMe) const
{
    return(!(operator == (compareMe)));
}

inline
int
OutputFileException::ReportDetailedError() const
{
    if (ptrFileNameString_a != NULL) {
        cout << "FILE  : " << *ptrFileNameString_a << NEW_LINE;
    }

    return(OUTPUT_FILE_EXCEPTION);
}

inline
const string
OutputFileException::GetExceptionString() const
{
    return(OUTPUT_FILE_EXCEPTION_H);
}

inline
const string*
OutputFileException::GetFileName() const
{
    return(ptrFileNameString_a);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
OutputFileException::GetClassName()
{
    return(OUTPUT_FILE_EXCEPTION_H);
}
*/

#endif
