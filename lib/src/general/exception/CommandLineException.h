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
 * CommandLineException.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef COMMAND_LINE_EXCEPTION_H
#define COMMAND_LINE_EXCEPTION_H "CommandLineException"

#include "Exception.h"
#include "general/utility/Constant.h"

/**
 * CommandLineException is the exception that occurs when the
 *  command line arguments cannot be properly dealt with.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------------
 * numberOfArgumentsInput_a     Number of arguments entered on the command line
 * numberOfArgumentsRequired_a  Number of arguments required to be entered
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class CommandLineException : public Exception
{
public:

    CommandLineException();
    CommandLineException(const int &argumentsInput,
                         const int &argumentsRequired);
    CommandLineException(const CommandLineException &copyMe);
    virtual ~CommandLineException();

    CommandLineException& operator = (const CommandLineException &copyMe);

    inline int         ReportDetailedError() const;

    inline const string  GetExceptionString() const;

protected:

private:




    int               numberOfArgumentsInput_a;
    int               numberOfArgumentsRequired_a;
};

inline
int
CommandLineException::ReportDetailedError() const
{
    cout << "ARGS   : " << numberOfArgumentsInput_a << " input, ";
    cout << numberOfArgumentsRequired_a << " required!" << NEW_LINE;
    return(COMMAND_LINE_EXCEPTION);
}

inline
const string
CommandLineException::GetExceptionString() const
{
    return(COMMAND_LINE_EXCEPTION_H);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
CommandLineException::GetClassName()
{
    return(COMMAND_LINE_EXCEPTION_H);
}
*/

#endif
