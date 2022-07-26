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
 * InputData.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "InputData.h"
#include "general/utility/Constant.h"
#include "general/utility/StringEditor.h"
#include "general/exception/InputException.h"

const char   InputData::INPUT_SEPARATOR_s = ',';
const char   InputData::SPECIFIC_INPUT_START_s = '[';
const char   InputData::SPECIFIC_INPUT_END_s = ']';

InputData::InputData()
    : ptrLabel_a(NULL), ptrValueString_a(NULL), ptrValueArray_a(NULL)
{
    ptrLabel_a = shared_ptr<string>(new string("Undefined Label"));
    ptrValueString_a = shared_ptr<string>(new string("Undefined Value String"));
    ptrValueArray_a = shared_ptr<Array<string>>(new Array<string>());
    return;
}

InputData::InputData(const string &newLabel,
                     const string &newValue)
    : ptrLabel_a(NULL), ptrValueString_a(NULL), ptrValueArray_a(NULL)
{
    string       value = "";
    ptrValueString_a = shared_ptr<string>(new string(newValue));
    ptrLabel_a = shared_ptr<string>(new string(newLabel));
    ptrValueArray_a = shared_ptr<Array<string>>(new Array<string>());
    StringEditor::RemoveCharacter(*ptrValueString_a, INPUT_SEPARATOR_s);
    istringstream newValueStream(*ptrValueString_a);

    while (newValueStream >> value) {
        ptrValueArray_a->AddElement(value);
    }

    if (ptrValueArray_a->Size() == 0) {
        throw new InputException(GetClassName(),
                                 "InputData(const string&, const string&)",
                                 newValue);
    }

    return;
}

InputData::InputData(const InputData &copyMe)
    : ptrLabel_a(NULL), ptrValueString_a(NULL), ptrValueArray_a(NULL)
{
    ptrLabel_a = shared_ptr<string>(new string(*(copyMe.ptrLabel_a)));
    ptrValueString_a = shared_ptr<string>(new string(*(copyMe.ptrValueString_a)));
    ptrValueArray_a = shared_ptr<Array<string>>(new Array<string>(*(copyMe.ptrValueArray_a)));
    return;
}

InputData&
InputData::operator = (const InputData &copyMe)
{
    ptrLabel_a = shared_ptr<string>(new string(*(copyMe.ptrLabel_a)));
    ptrValueString_a = shared_ptr<string>(new string(*(copyMe.ptrValueString_a)));
    ptrValueArray_a = shared_ptr<Array<string>>(new Array<string>(*(copyMe.ptrValueArray_a)));
    return(*this);
}

bool
InputData::operator == (const InputData &compareMe) const
{
    return( (*ptrLabel_a == *(compareMe.ptrLabel_a)) &&
            (*ptrValueString_a == *(compareMe.ptrValueString_a)) &&
            (*ptrValueArray_a == *(compareMe.ptrValueArray_a)) );
}

bool
InputData::operator != (const InputData &compareMe) const
{
    return(!(operator == (compareMe)));
}

string
InputData::FindValue(int index) const
{
    string returnValue = "";

    try {
        returnValue = (*ptrValueArray_a)[index];
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "FindValue(int)");
        throw;
    }

    return(returnValue);
}

int
InputData::GetIntValue(int index) const
{
    string valueString = "";

    try {
        valueString = FindValue(index);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetIntValue(int)");
        throw;
    }

    return(StringEditor::ConvertToInt(valueString));
}

double
InputData::GetDoubleValue(int index) const
{
    string valueString = "";

    try {
        valueString = FindValue(index);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetDoubleValue(int)");
        throw;
    }

    return(StringEditor::ConvertToDouble(valueString));
}

bool
InputData::GetBoolValue(int index) const
{
    bool   returnValue = false;
    string valueString = "";

    try {
        valueString = FindValue(index);
        returnValue = StringEditor::ConvertToBoolean(valueString);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetBoolValue(int)");
        throw;
    }

    return(returnValue);
}

char
InputData::GetCharValue(int index) const
{
    string valueString = "";

    try {
        valueString = FindValue(index);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetCharValue(int)");
        throw;
    }

    return(valueString[0]);
}

const char*
InputData::GetCharStringValue(int index) const
{
    string valueString = "";

    try {
        valueString = FindValue(index);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetCharStringValue(int)");
        throw;
    }

    //CPPCHECK_FIX Error - dangerous usage of c_str().
    const char* valueString_c_str = valueString.c_str();
    return(valueString_c_str);
}

string
InputData::GetStringValue(int index) const
{
    string valueString = "";

    try {
        valueString = FindValue(index);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetStringValue(int)");
        throw;
    }

    return(valueString);
}

string
InputData::GetInputDataLine() const
{
    string inputDataLine = "";

    try {
        int index = 0;
        inputDataLine = GetLabel();
        inputDataLine += " : ";
        inputDataLine += GetStringValue(index);
        ++index;

        while (index < GetNumberOfValues()) {
            inputDataLine += (", " + GetStringValue(index));
            ++index;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetInputDataLine()");
        throw;
    }

    return(inputDataLine);
}

bool
InputData::GetSpecificIntNumber(int &specificNumber) const
{
    string                  specificTypeString = "";
    const string::size_type startPosition = ptrLabel_a->find_first_of(SPECIFIC_INPUT_START_s);
    const string::size_type endPosition = ptrLabel_a->find_last_of(SPECIFIC_INPUT_END_s);
    specificNumber = 0;

    if ( (startPosition != string::npos) && (endPosition != string::npos) &&
            ((startPosition + 1) < endPosition) ) {
        specificTypeString = ptrLabel_a->substr((startPosition + 1),
                                                (endPosition - startPosition - 1));
        istringstream specificTypeStream(specificTypeString);
        specificTypeStream >> specificNumber;
    }

    return(specificNumber > 0);
}

bool
InputData::GetSpecificDoubleNumber(double &specificNumber) const
{
    bool                    foundSpecificDoubleNumber = false;
    string                  specificTypeString = "";
    const string::size_type startPosition = ptrLabel_a->find_first_of(SPECIFIC_INPUT_START_s);
    const string::size_type endPosition = ptrLabel_a->find_last_of(SPECIFIC_INPUT_END_s);

    if ( (startPosition != string::npos) && (endPosition != string::npos) &&
            ((startPosition + 1) < endPosition) ) {
        specificTypeString = ptrLabel_a->substr((startPosition + 1),
                                                (endPosition - startPosition - 1));
        istringstream specificTypeStream(specificTypeString);

        if (specificTypeStream >> specificNumber) {
            foundSpecificDoubleNumber = true;
        }
    }

    return(foundSpecificDoubleNumber);
}

bool
InputData::GetSpecificIntNumbers(int &specificNumber1,
                                 int &specificNumber2) const
{
    bool                    foundSpecificNumbers = false;
    string                  specificTypeString = "";
    const string::size_type startPosition = ptrLabel_a->find_first_of(SPECIFIC_INPUT_START_s);
    const string::size_type endPosition = ptrLabel_a->find_last_of(SPECIFIC_INPUT_END_s);

    if ( (startPosition != string::npos) && (endPosition != string::npos) &&
            ((startPosition + 1) < endPosition) ) {
        specificTypeString = ptrLabel_a->substr((startPosition + 1),
                                                (endPosition - startPosition - 1));
        StringEditor::RemoveCharacter(specificTypeString, INPUT_SEPARATOR_s);
        istringstream specificTypeStream(specificTypeString);

        if (specificTypeStream >> specificNumber1 >> specificNumber2) {
            foundSpecificNumbers = true;
        }
    }

    return(foundSpecificNumbers);
}

bool
InputData::GetSpecificNumbers(double &specificNumber1,
                              double &specificNumber2) const
{
    bool                    foundSpecificNumbers = false;
    string                  specificTypeString = "";
    const string::size_type startPosition = ptrLabel_a->find_first_of(SPECIFIC_INPUT_START_s);
    const string::size_type endPosition = ptrLabel_a->find_last_of(SPECIFIC_INPUT_END_s);

    if ( (startPosition != string::npos) && (endPosition != string::npos) &&
            ((startPosition + 1) < endPosition) && (ptrLabel_a->find(INPUT_SEPARATOR_s) != string::npos) ) {
        specificTypeString = ptrLabel_a->substr((startPosition + 1),
                                                (endPosition - startPosition - 1));
        StringEditor::RemoveCharacter(specificTypeString, INPUT_SEPARATOR_s);
        istringstream specificTypeStream(specificTypeString);

        if (specificTypeStream >> specificNumber1 >> specificNumber2) {
            foundSpecificNumbers = true;
        }
    }

    return(foundSpecificNumbers);
}

int
InputData::GetSpecificNumbers(Array<double> *specificNumberList) const
{
    int numberCount                     = 0;
    string specificTypeString             = "";
    const string::size_type startPosition = ptrLabel_a->find_first_of(SPECIFIC_INPUT_START_s);
    const string::size_type endPosition   = ptrLabel_a->find_last_of(SPECIFIC_INPUT_END_s);

    if ( (startPosition != string::npos) && (endPosition != string::npos) &&
            ((startPosition + 1) < endPosition) ) {
        double nextNumber = 0.0;
        specificTypeString = ptrLabel_a->substr((startPosition + 1),
                                                (endPosition - startPosition - 1));
        StringEditor::RemoveCharacter(specificTypeString, INPUT_SEPARATOR_s);
        istringstream specificTypeStream(specificTypeString);

        while (specificTypeStream >> nextNumber ) {
            specificNumberList->AddElement(nextNumber);
            numberCount++;
        }
    }

    return(numberCount);
}

bool
InputData::GetSpecificType(string &specificType) const
{
    string                  specificTypeString = "";
    const string::size_type startPosition = ptrLabel_a->find_first_of(SPECIFIC_INPUT_START_s);
    const string::size_type endPosition = ptrLabel_a->find_last_of(SPECIFIC_INPUT_END_s);
    specificType = "";

    if ( (startPosition != string::npos) && (endPosition != string::npos) &&
            (startPosition < endPosition) ) {
        specificTypeString = ptrLabel_a->substr((startPosition + 1),
                                                (endPosition - startPosition - 1));
        istringstream specificTypeStream(specificTypeString);
        specificTypeStream >> specificType;
    }

    return(specificType.size() > 0);
}

bool
InputData::GetSpecificType(string &specificType1,
                           string &specificType2) const
{
    string                  specificTypeString = "";
    const string::size_type startPosition = ptrLabel_a->find_first_of(SPECIFIC_INPUT_START_s);
    const string::size_type endPosition = ptrLabel_a->find_last_of(SPECIFIC_INPUT_END_s);
    specificType1 = "";
    specificType2 = "";

    if ( (startPosition != string::npos) && (endPosition != string::npos) &&
            (startPosition < endPosition) && (ptrLabel_a->find(INPUT_SEPARATOR_s) != string::npos) ) {
        specificTypeString = ptrLabel_a->substr((startPosition + 1),
                                                (endPosition - startPosition - 1));
        StringEditor::RemoveCharacter(specificTypeString, INPUT_SEPARATOR_s);
        istringstream specificTypeStream(specificTypeString);
        specificTypeStream >> specificType1 >> specificType2;
    }

    return((specificType1.size() > 0) && (specificType2.size() > 0));
}

string
InputData::GetSpecificNumberString(double specificNumber1,
                                   double specificNumber2)
{
    string specificNumberString = StringEditor::ConvertToString(SPECIFIC_INPUT_START_s);
    specificNumberString += StringEditor::ConvertToString(specificNumber1);
    specificNumberString += ", ";
    specificNumberString += StringEditor::ConvertToString(specificNumber2);
    specificNumberString += StringEditor::ConvertToString(SPECIFIC_INPUT_END_s);
    return(specificNumberString);
}

InputData::~InputData()
{
    return;
}
