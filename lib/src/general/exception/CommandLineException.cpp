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
 * CommandLineException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "CommandLineException.h"

CommandLineException::CommandLineException()
    : Exception(), numberOfArgumentsInput_a(0), numberOfArgumentsRequired_a(0)
{
    return;
}

CommandLineException::CommandLineException(const int &argumentsInput,
        const int &argumentsRequired)
    : Exception(), numberOfArgumentsInput_a(argumentsInput),
      numberOfArgumentsRequired_a(argumentsRequired)
{
    return;
}

CommandLineException::CommandLineException(const CommandLineException &copyMe)
    : Exception(copyMe), numberOfArgumentsInput_a(copyMe.numberOfArgumentsInput_a),
      numberOfArgumentsRequired_a(copyMe.numberOfArgumentsRequired_a)
{
    return;
}

CommandLineException&
CommandLineException::operator = (const CommandLineException &copyMe)
{
    Exception::operator = (copyMe);
    numberOfArgumentsInput_a    = copyMe.numberOfArgumentsInput_a;
    numberOfArgumentsRequired_a = copyMe.numberOfArgumentsRequired_a;
    return(*this);
}

CommandLineException::~CommandLineException()
{
    return;
}
