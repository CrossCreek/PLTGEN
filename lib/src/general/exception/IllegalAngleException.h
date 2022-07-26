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
 * IllegalAngleException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ILLEGAL_ANGLE_EXCEPTION_H
#define ILLEGAL_ANGLE_EXCEPTION_H "IllegalAngleException"

#include "Exception.h"
#include "general/utility/Constant.h"

using namespace std;

/**
 * IllegalAngleException is the exception that occurs when an
 *  impossible angle is attempted to be created or used. Note that
 *  the exception should not be thrown in a case where the angle may
 *  validly be normalized within the valid range of acceptable angles.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * minimumAngle_a               Minimum allowable angle (double)
 * maximumAngle_a               Maximum allowable angle (double)
 * illegalAngle_a               Offending angle (double)
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class IllegalAngleException : public Exception
{
public:

    IllegalAngleException();
    IllegalAngleException(const string &currentClassName,
                          const string &currentMethodName,
                          double minimumAngle,
                          double maximumAngle,
                          double illegalAngle );
    IllegalAngleException(const IllegalAngleException &copyMe);
    virtual ~IllegalAngleException();

    IllegalAngleException& operator = (const IllegalAngleException &copyMe);
    inline bool      operator == (const IllegalAngleException &compareMe) const;
    inline bool      operator != (const IllegalAngleException &compareMe) const;

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;

    inline double        GetMinimumAllowableAngle() const;
    inline double        GetMaximumAllowableAngle() const;
    inline double        GetIllegalAngle() const;

protected:

private:




    double              minimumAngle_a;
    double              maximumAngle_a;
    double              illegalAngle_a;
};

inline
bool
IllegalAngleException::operator == (const IllegalAngleException &compareMe) const
{
    return( (Exception::operator == (compareMe)) &&
            (minimumAngle_a == compareMe.minimumAngle_a) &&
            (maximumAngle_a == compareMe.maximumAngle_a) &&
            (illegalAngle_a == compareMe.illegalAngle_a) );
}

inline
bool
IllegalAngleException::operator != (const IllegalAngleException &compareMe) const
{
    return(!(operator == (compareMe)));
}

inline
int
IllegalAngleException::ReportDetailedError() const
{
    cout << "ANGLE : " << illegalAngle_a << " (Minimum " << minimumAngle_a;
    cout << ", Maximum " << maximumAngle_a << ")" << NEW_LINE;
    return(ILLEGAL_ANGLE_EXCEPTION);
}

inline
const string
IllegalAngleException::GetExceptionString() const
{
    return(ILLEGAL_ANGLE_EXCEPTION_H);
}

inline
double
IllegalAngleException::GetMinimumAllowableAngle() const
{
    return(minimumAngle_a);
}

inline
double
IllegalAngleException::GetMaximumAllowableAngle() const
{
    return(maximumAngle_a);
}

inline
double
IllegalAngleException::GetIllegalAngle() const
{
    return(illegalAngle_a);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
IllegalAngleException::GetClassName()
{
    return(ILLEGAL_ANGLE_EXCEPTION_H);
}
*/

#endif
