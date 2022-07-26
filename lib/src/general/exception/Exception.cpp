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
 * Exception.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include "Exception.h"
#include "general/array/Array.h"

Exception::Exception()
    : ptrMethodPathArray_a(NULL)
{
    return;
}

Exception::Exception(const string &currentClassString,
                     const string &currentMethodString)
    : ptrMethodPathArray_a(NULL)
{
    AddMethod(currentClassString, currentMethodString);
    return;
}

Exception::Exception(const Exception &copyMe)
    : ptrMethodPathArray_a(NULL)
{
    if (copyMe.ptrMethodPathArray_a != NULL) {
        ptrMethodPathArray_a = new Array<string>(*(copyMe.ptrMethodPathArray_a));
    }

    return;
}

Exception&
Exception::operator = (const Exception &copyMe)
{
    ptrMethodPathArray_a = NULL;

    if (copyMe.ptrMethodPathArray_a != NULL) {
        ptrMethodPathArray_a = new Array<string>(*(copyMe.ptrMethodPathArray_a));
    }

    return(*this);
}

bool
Exception::operator == (const Exception &compareMe) const
{
    return(*ptrMethodPathArray_a == *(compareMe.ptrMethodPathArray_a));
}

bool
Exception::operator != (const Exception &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
Exception::AddMethod(const string &className,
                     const string &methodName)
{
    string totalMethod = className;
    totalMethod += "::";
    totalMethod += methodName;

    if (ptrMethodPathArray_a == NULL) {
        ptrMethodPathArray_a = new Array<string>(1, totalMethod);
    } else {
        ptrMethodPathArray_a->AddElement(totalMethod);
    }

    return;
}

int
Exception::GetNumberOfMethodPaths() const
{
    int numberOfMethodPaths = 0;

    if (ptrMethodPathArray_a != NULL) {
        numberOfMethodPaths = ptrMethodPathArray_a->Size();
    }

    return(numberOfMethodPaths);
}

string
Exception::GetMethodPath(int pathIndex) const
{
    string returnString = "";

    if (ptrMethodPathArray_a != NULL) {
        if ((pathIndex >= 0) &&
                (pathIndex < ptrMethodPathArray_a->Size())) {
            returnString = ptrMethodPathArray_a->ElementAt(pathIndex);
        }
    }

    return(returnString);
}

int
Exception::ReportError() const
{
    int returnStatus = 0;
    cout << "ERROR  : " << GetExceptionString() << " encountered!" << NEW_LINE;
    returnStatus = ReportDetailedError();
    cout << "STATUS : " << returnStatus << NEW_LINE;

    if (ptrMethodPathArray_a != NULL) {
        int methodIndex = 0;
        cout << "PATH   : " << ptrMethodPathArray_a->ElementAt(methodIndex) << NEW_LINE;
        ++methodIndex;

        while (methodIndex < ptrMethodPathArray_a->Size()) {
            cout << "         " << ptrMethodPathArray_a->ElementAt(methodIndex) << NEW_LINE;
            ++methodIndex;
        }
    }

    return(returnStatus);
}

Exception::~Exception()
{
    if (ptrMethodPathArray_a != NULL) {
        delete ptrMethodPathArray_a;
        ptrMethodPathArray_a = NULL;
    }

    return;
}
