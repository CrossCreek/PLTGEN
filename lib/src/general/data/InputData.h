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
 * InputData.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef INPUT_DATA_H
#define INPUT_DATA_H "InputData"

#include <string>
#include <iostream>
#include <iomanip>
//RAGUSA 7/24/2017 - Needed to resolve shared_ptr removal from std library.
#include <memory>
#include "general/array/Array.h"
#include "general\utility\Utility.h"

using namespace std;

/**
 * Heritage MURAL class which represents the given input found in an input line.
 * It has methods to get the label, value, list of values, and specific values
 * found in bracket delimeters
 *
 * Example:
 * <Label of Input> [ <optional values> ]    : <Values>
 *
 *
 *
 * Class Attribute Descriptions
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * ptrLabel_a*                  Array containing an Input label
 * ptrValueArray_a*             Array containing Input Values
 *
 * @author Rob Lowry
 * @author Brian Fowler
 * @date 08/16/11
 */

class InputData
{
public:

    /**
     * Constructor for the InputData class.
     */
    InputData();
    InputData(const string &newLabel,
              const string &newValue);
    InputData(const InputData &copyMe);
    virtual ~InputData();

    InputData&      operator =  (const InputData &copyMe);
    bool            operator == (const InputData &compareMe) const;
    bool            operator != (const InputData &compareMe) const;

    static string GetSpecificNumberString(double specificNumber1,
                                          double specificNumber2);

    int        GetIntValue(int index = 0) const;
    double      GetDoubleValue(int index = 0) const;
    bool        GetBoolValue(int index = 0) const;
    char        GetCharValue(int index = 0) const;
    const char* GetCharStringValue(int index = 0) const;
    string      GetStringValue(int index = 0) const;

    bool   GetSpecificIntNumber(int &specificNumber) const;
    bool   GetSpecificDoubleNumber(double &specificNumber) const;
    bool   GetSpecificIntNumbers(int &specificNumber1,
                                 int &specificNumber2) const;
    bool   GetSpecificNumbers(double &specificNumber1,
                              double &specificNumber2) const;
    int  GetSpecificNumbers(Array<double> *specificNumberList) const;
    bool   GetSpecificType(string &specificType) const;
    bool   GetSpecificType(string &specificType1,
                           string &specificType2) const;

    string GetInputDataLine() const;

    static inline string GetSpecificTypeString(const string &specificType);
    static inline string GetSpecificTypeString(const string &specificType1,
            const string &specificType2);


    inline int  GetLabelSize() const;

    inline int   GetNumberOfValues() const;

    inline string GetLabel() const;
    inline string GetValueString() const;

    inline bool   LabelContainsString(const string &checkString) const;

    inline Array<string>* GetValueArray() const;

protected:

private:

    string FindValue(int index) const;

    static inline string GetClassName();

    static const char   INPUT_SEPARATOR_s;
    static const char   SPECIFIC_INPUT_START_s;
    static const char   SPECIFIC_INPUT_END_s;

    shared_ptr<string>         ptrLabel_a;
    shared_ptr<string>         ptrValueString_a;
    shared_ptr<Array<string>>  ptrValueArray_a;
};

inline
string
InputData::GetLabel() const
{
    return(*ptrLabel_a);
}

inline
string
InputData::GetValueString() const
{
    return(*ptrValueString_a);
}


inline
int
InputData::GetLabelSize() const
{
    //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
    return(Utility::CastSizetToInt(ptrLabel_a->size()));
}

inline
int
InputData::GetNumberOfValues() const
{
    return(ptrValueArray_a->Size());
}

inline
string
InputData::GetSpecificTypeString(const string &specificType)
{
    string specificTypeString = "[";
    specificTypeString += specificType;
    specificTypeString += "]";
    return(specificTypeString);
}

inline
string
InputData::GetSpecificTypeString(const string &specificType1,
                                 const string &specificType2)
{
    string specificTypeString = "[";
    specificTypeString += specificType1;
    specificTypeString += ", ";
    specificTypeString += specificType2;
    specificTypeString += "]";
    return(specificTypeString);
}

inline
Array<string>*
InputData::GetValueArray() const
{
    return(ptrValueArray_a.get());
}

inline
bool
InputData::LabelContainsString(const string &checkString) const
{
    return(ptrLabel_a->find(checkString) != string::npos);
}

inline
string
InputData::GetClassName()
{
    return(INPUT_DATA_H);
}

#endif
