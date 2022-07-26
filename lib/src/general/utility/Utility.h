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
 * Utility.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef UTILITY_H
#define UTILITY_H "Utility"

#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
//RAGUSA 5/2/2018 - Added for CastSizetToInt Method
#include "general/exception/ArithmeticException.h"
#include "general/utility/StringEditor.h"
//=================================================
#include "Constant.h"

/**
 * Utility is a class full of static helper methods for convenience.
 *
 * @author Brian Fowler
 * @date 08/31/11
 */
namespace Utility
{

/**
 * Determines if the vector contains the value or not
 *
 * @param inputVector the vector to check
 * @param T& value the value to check
 * @return bool if the vector contains the value
 */
template<class T>
inline bool VectorContains(const vector<T> &inputVector, const T& value)
{
    bool found = false;

    if ( std::find(inputVector.begin(), inputVector.end(), value) != inputVector.end() ) {
        found = true;
    }

    return found;
}

/**
 * Method to print a vector of strings, formatting them with commas
 *
 * @param inputVector the vector of strings to print
 */
inline void PrintStringVector(vector<string> &inputVector, ofstream &outfile)
{
    vector<string>::iterator groupIter;

    for( groupIter = inputVector.begin(); groupIter != inputVector.end(); groupIter++) {
        if ( groupIter != inputVector.begin() ) {
            outfile << ", ";
        }

        outfile << (*groupIter);
    }
}

/**
 * Determines if the eccentricity is greater than the tolerance to signify a
 * highly elliptical orbit
 *
 * @param eccentricity the eccentricity to check
 * @return bool return value
 */
inline bool IsHighlyElliptical(double eccentricity)
{
    return(eccentricity > ECCENTRICITY_TOLERANCE);
}

/*
 * RAGUSA 5/1/2018 - Typecasts size_t data types into int's for the purpose of avoiding
 * 'possible data loss' warnings (for 64-bit processing).
 * includes logic check if the size_t is larger than the max int
 * size, then the run must end
 *
 * @param size - size_t being typecasted
 * @return int return value
 */
inline int CastSizetToInt(size_t size)
{
    int castedSize;

    if (size > INT32_MAX) {
        throw new ArithmeticException("Utility", "CastSizetToInt(size_t size)");
    } else {
        castedSize = (int)size;
    }

    return (castedSize);
}

/*
* RAGUSA 5/2/2018 - Typecasts double data types into int's for the purpose of avoiding
* 'possible data loss' warnings (for 64-bit processing).
* includes logic check if the whole value of the double is larger than the max int
* size, then the run must end
*
* @param value - double being typecasted
* @return int return value
*/
inline int CastDoubleToInt(double value)
{
    int castedValue;

    if (value > INT32_MAX) {
        throw new ArithmeticException("Utility", "CastDoubleToInt(double value)");
    } else {
        castedValue = (int)value;
    }

    return (castedValue);
}

// RAGUSA 5/2/2018 - Added for the exception, but wouldn't work I'm assuming because this is a header class
//inline string GetClassName() {
//	return(UTILITY_H);
//}
//=================================================================

/**
 * Adds a unique value to the input vector.  Will not add it if the vector contains it already
 *
 * @param inputVector the vector to check
 * @param T& value the value to check
 */
template<class T>
inline void AddUnique(vector<T> &inputVector, const T& value)
{
    if ( Utility::VectorContains(inputVector, value) == false ) {
        inputVector.push_back(value);
    }
}
};

#endif
