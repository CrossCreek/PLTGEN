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
 * StringEditor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iostream>
#include "general/utility/StringEditor.h"
#include "general/utility/Constant.h"
#include "general/exception/InputException.h"
#include "general/math/SimpleMath.h"

void
StringEditor::RemoveCharacter(string &inputString,
                              const char &removeCharacter)
{
    int       index = 0;
    const int stringSize = (int)(inputString.size());

    while (index < stringSize) {
        if (inputString[index] == removeCharacter) {
            inputString[index] = ' ';
        }

        ++index;
    }

    return;
}

void
StringEditor::TrimBlankSpaces(string &inputString)
{
    const string::size_type startPosition = inputString.find_first_not_of(" ");
    const string::size_type endPosition = inputString.find_last_not_of(" ");

    if ( (startPosition != string::npos) &&
            (endPosition != string::npos) &&
            (startPosition <= endPosition) ) {
        inputString = inputString.substr(startPosition, (endPosition - startPosition + 1));
    } else {
        inputString = "";
    }

    return;
}

int
StringEditor::ConvertToInt(const string &valueString)
{
    return(atoi(valueString.c_str()));
}

double
StringEditor::ConvertToDouble(const string &valueString)
{
    return(atof(valueString.c_str()));
}

bool
StringEditor::ConvertToBoolean(const string &valueString)
{
    bool returnBoolean = false;

    if (valueString == "false") {
        returnBoolean = false;
    } else if (valueString == "true") {
        returnBoolean = true;
    } else {
        throw new InputException(GetClassName(), "ConvertToBoolean(const string&)",
                                 ("Unknown boolean input '" + valueString + "'"));
    }

    return(returnBoolean);
}

string
StringEditor::ConvertToString(int value,
                              int totalFillZeros)
{
    int          tempValue = value;
    ostringstream valueStream;

    if ((value >= 0) && (totalFillZeros > 1)) {
        int numberOfDigits = 1;
        int numberOfFillZeros = 0;

        while (tempValue >= 10) {
            ++numberOfDigits;
            tempValue /= 10;
        }

        numberOfFillZeros = (totalFillZeros - numberOfDigits);

        if (numberOfFillZeros > 0) {
            int fillZero = 0;

            while (fillZero < numberOfFillZeros) {
                valueStream << "0";
                ++fillZero;
            }
        }
    }

    valueStream << value;
    return(valueStream.str());
}

string
StringEditor::ConvertToString(double value)
{
    ostringstream valueStream;
    valueStream << value;

    if (SimpleMath::Remainder(value) == 0.0) {
        valueStream << ".0";
    }

    return(valueStream.str());
}

string
StringEditor::ConvertToString(const bool &value)
{
    string returnString;

    if (value == true) {
        returnString = "true";
    } else {
        returnString = "false";
    }

    return(returnString);
}

string
StringEditor::ConvertToString(const char &value,
                              int multiplicity)
{
    return(string(multiplicity, value));
}

string
StringEditor::ConvertToString(const char *value)
{
    return(string(value));
}

bool
StringEditor::IsEmpty(const string *inStr)
{
    bool isEmpty = true;

    if ( inStr != NULL ) {
        if ((*inStr).size() > 0 ) {
            isEmpty = false;
        }
    }

    return isEmpty;
}

StringEditor::~StringEditor()
{
    return;
}
