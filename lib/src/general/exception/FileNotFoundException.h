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
 * FileNotFoundException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef FILE_NOT_FOUND_EXCEPTION_H
#define FILE_NOT_FOUND_EXCEPTION_H "FileNotFoundException"

#include "Exception.h"
#include "general/utility/Constant.h"

using namespace std;

/**
 * FileNotFoundException is the exception that occurs when a file
 *  cannot be found, thereby inhibiting the obtaining of vital data.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * ptrFileNameString_a*         Name of the file not found
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class FileNotFoundException : public Exception
{
public:

    FileNotFoundException();
    FileNotFoundException(const string &currentClassName,
                          const string &currentMethodName,
                          const string &fileName);
    FileNotFoundException(const FileNotFoundException &copyMe);
    virtual ~FileNotFoundException();

    FileNotFoundException& operator =  (const FileNotFoundException &copyMe);
    bool            operator == (const FileNotFoundException &compareMe) const;
    bool            operator != (const FileNotFoundException &compareMe) const;

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;
    inline const string* GetFileName() const;

protected:

private:




    string             *ptrFileNameString_a;
};

inline
int
FileNotFoundException::ReportDetailedError() const
{
    if (ptrFileNameString_a != NULL) {
        cout << "FILE   : " << *ptrFileNameString_a << NEW_LINE;
    }

    return(FILE_NOT_FOUND_EXCEPTION);
}

inline
const string
FileNotFoundException::GetExceptionString() const
{
    return(FILE_NOT_FOUND_EXCEPTION_H);
}

inline
const string*
FileNotFoundException::GetFileName() const
{
    return(ptrFileNameString_a);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
FileNotFoundException::GetClassName()
{
    return(FILE_NOT_FOUND_EXCEPTION_H);
}
*/

#endif
