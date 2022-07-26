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
 * Array.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ARRAY_H
#define ARRAY_H "Array"

#include <iostream>
#include <string>
#include <sstream>
#include <typeinfo>

#include "general/exception/ArithmeticException.h"
#include "general/exception/IndexOutOfBoundsException.h"
#include "general/exception/InputException.h"

using namespace std;

//------------------------------------------------------------------------------
// Class Attribute Descriptions
//
// Name                  Description (units)
// ------------------    -------------------------------------------------------
// dimension_a           number of elements in the array
// increasingOrder_a     indicates if the values are in decreasing (false) or
//                         increasing (true) order
// ptrArray_a*           pointer to a 'C-style' array
//
//------------------------------------------------------------------------------

template<class Type>
class Array
{
public:

    inline           Array();
    inline           Array(int initialSize);
    inline           Array(int initialSize,
                           const Type &initialValue);
    inline           Array(const Type *ptrCStyleArray,
                           int arraySize);
    inline           Array(const Array<Type> &copyMe);
    inline virtual  ~Array();

    inline Array<Type>& operator =  (const Array<Type> &copyMe);
    inline bool         operator == (const Array<Type> &compareMe);
    inline bool         operator != (const Array<Type> &compareMe);
    inline Type&        operator [] (int index);

    inline void        AddElement(const Type &newValue);
    inline void        AddUniqueElement(const Type &newValue);
    inline void        AddElementToFront(const Type &newValue);
    inline void        Delete();
    inline void        Increment(int startIndex,
                                 int endIndex) const;
    inline void        InsertElement(const Type &newValue);
    inline void        InsertUniqueElement(const Type &newValue);
    inline void        Print(ostream &outputStream) const;
    inline void        RemoveElement(const Type &removeValue);
    inline void        ResetValues(const Type &resetValue);
    inline void        ResetValue(int index,
                                  const Type &resetValue);
    inline void        ResetValues(const Array<Type> *ptrResetArray);
    inline void        SetOrderToDecreasing();

    inline int        CountTrueValues() const;
    inline int        CountTrueValues(int startIndex,
                                      int endIndex) const;
    inline int        ValuesGreaterThan(const Type &value) const;
    inline int        ValuesGreaterThanOrEqual(const Type &value) const;
    inline int        ValuesLessThan(const Type &value) const;
    inline int        ValuesLessThanOrEqual(const Type &value) const;

    inline const int  Size() const;
    inline const int  FindIndex(const Type &value) const;

    inline bool        AllValuesEqual(const Type &value) const;
    inline bool        AllValuesGreaterThan(const Type &value) const;
    inline bool        AllValuesGreaterThanOrEqual(const Type &value) const;
    inline bool        AllValuesLessThan(const Type &value) const;
    inline bool        AllValuesLessThanOrEqual(const Type &value) const;
    inline bool        ValueExists(const Type &value) const;
    inline bool        ValueExists(const Type &startValue,
                                   const Type &endValue) const;

    inline string      GetClassName() const;

    inline Type        GetSum() const;
    inline Type        GetSum(int startIndex,int endIndex) const;
    inline Type        GetAverage() const;
    inline Type        GetMaximum() const;
    inline Type        GetMinimum() const;

    inline const Type& ElementAt(int index) const;
    inline const Type& FirstElement() const;
    inline const Type& LastElement() const;

protected:

private:

    int  dimension_a;
    bool  increasingOrder_a;
    Type *ptrArray_a;
};

template<class Type>
inline
Array<Type>::Array()
    : dimension_a(0), increasingOrder_a(true), ptrArray_a(NULL)
{
    return;
}

template<class Type>
inline
Array<Type>::Array(int initialSize)
    : dimension_a(initialSize), increasingOrder_a(true), ptrArray_a(NULL)
{
    if (initialSize <= 0) {
        throw new InputException(GetClassName(), "Array(int)",
                                 "Entered zero or negative number for initial size");
    }

    ptrArray_a = new Type [initialSize];
    return;
}

template<class Type>
inline
Array<Type>::Array(int initialSize,
                   const Type &initialValue)
    : dimension_a(initialSize), increasingOrder_a(true), ptrArray_a(NULL)
{
    int index = 0;

    if (initialSize <= 0) {
        throw new InputException(GetClassName(), "Array(int)",
                                 "Entered zero or negative number for initial size");
    }

    ptrArray_a = new Type [initialSize];

    while (index < initialSize) {
        ptrArray_a[index] = initialValue;
        ++index;
    }

    return;
}

template<class Type>
inline
Array<Type>::Array(const Type *ptrCStyleArray,
                   int arraySize)
    : dimension_a(arraySize), increasingOrder_a(true), ptrArray_a(NULL)
{
    int index = 0;

    if (arraySize <= 0) {
        throw new InputException(GetClassName(), "Array(const Type*, int)",
                                 "Entered zero or negative number for initial size");
    }

    ptrArray_a = new Type [arraySize];

    while (index < arraySize) {
        ptrArray_a[index] = ptrCStyleArray[index];
        ++index;
    }

    return;
}


template<class Type>
inline
Array<Type>::Array(const Array<Type> &copyMe)
    : dimension_a(copyMe.dimension_a), increasingOrder_a(copyMe.increasingOrder_a),
      ptrArray_a(NULL)
{
    int index = 0;

    if (copyMe.ptrArray_a != NULL) {
        ptrArray_a = new Type [dimension_a];

        while (index < dimension_a) {
            ptrArray_a[index] = copyMe.ElementAt(index);
            ++index;
        }
    }

    return;
}

template<class Type>
inline
Array<Type>&
Array<Type>::operator = (const Array<Type> &copyMe)
{
    dimension_a = copyMe.dimension_a;
    ptrArray_a  = NULL;

    if (copyMe.ptrArray_a != NULL) {
        ptrArray_a = new Type(*(copyMe.ptrArray_a));
    }

    return(*this);
}

template<class Type>
inline
bool
Array<Type>::operator == (const Array<Type> &compareMe)
{
    int index = 0;
    bool equal = false;

    if ( (dimension_a == compareMe.dimension_a) &&
            (increasingOrder_a == compareMe.increasingOrder_a) ) {
        equal = true;

        while ((index < dimension_a) && (equal == true)) {
            if (ElementAt(index) != compareMe.ElementAt(index)) {
                equal = false;
            } else {
                ++index;
            }
        }
    }

    return(equal);
}

template<class Type>
inline
bool
Array<Type>::operator != (const Array<Type> &compareMe)
{
    return(!(operator == (compareMe)));
}

template<class Type>
inline
Type&
Array<Type>::operator [] (int index)
{
    if ((index < 0) || (index >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "operator [] (int)",
                                            dimension_a, index);
    }

    return(ptrArray_a[index]);
}

template<class Type>
inline
const int
Array<Type>::Size() const
{
    return(dimension_a);
}

template<class Type>
inline
void
Array<Type>::Increment(int startIndex,
                       int endIndex) const
{
    int index = startIndex;

    if ((startIndex < 0) || (startIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "Increment(int, int)",
                                            dimension_a, startIndex);
    }

    if ((endIndex < 0) || (endIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "Increment(int, int)",
                                            dimension_a, endIndex);
    }

    while (index <= endIndex) {
        ++ptrArray_a[index];
        ++index;
    }
}

template<class Type>
inline
int
Array<Type>::CountTrueValues() const
{
    int index = 0;
    int trueValues = 0;

    while (index < dimension_a) {
        if (ptrArray_a[index] == true) {
            ++trueValues;
        }

        ++index;
    }

    return(trueValues);
}

template<class Type>
inline
int
Array<Type>::CountTrueValues(int startIndex,
                             int endIndex) const
{
    int index = startIndex;
    int trueValues = 0;

    if ((startIndex < 0) || (startIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "CountTrueValues(int, int)",
                                            dimension_a, startIndex);
    }

    if ((endIndex < 0) || (endIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "CountTrueValues(int, int)",
                                            dimension_a, endIndex);
    }

    while (index <= endIndex) {
        if (ptrArray_a[index] == true) {
            ++trueValues;
        }

        ++index;
    }

    return(trueValues);
}

template<class Type>
inline
int
Array<Type>::ValuesGreaterThan(const Type &value) const
{
    int index = 0;
    int numberGreaterThan = 0;

    while (index < dimension_a) {
        if (ptrArray_a[index] > value) {
            ++numberGreaterThan;
        }

        ++index;
    }

    return(numberGreaterThan);
}

template<class Type>
inline
int
Array<Type>::ValuesGreaterThanOrEqual(const Type &value) const
{
    int index = 0;
    int numberGreaterThanOrEqual = 0;

    while (index < dimension_a) {
        if (ptrArray_a[index] >= value) {
            ++numberGreaterThanOrEqual;
        }

        ++index;
    }

    return(numberGreaterThanOrEqual);
}

template<class Type>
inline
int
Array<Type>::ValuesLessThan(const Type &value) const
{
    int index = 0;
    int numberLessThan = 0;

    while (index < dimension_a) {
        if (ptrArray_a[index] < value) {
            ++numberLessThan;
        }

        ++index;
    }

    return(numberLessThan);
}

template<class Type>
inline
int
Array<Type>::ValuesLessThanOrEqual(const Type &value) const
{
    int index = 0;
    int numberLessThanOrEqual = 0;

    while (index < dimension_a) {
        if (ptrArray_a[index] <= value) {
            ++numberLessThanOrEqual;
        }

        ++index;
    }

    return(numberLessThanOrEqual);
}

template<class Type>
inline
bool
Array<Type>::AllValuesEqual(const Type &value) const
{
    int index = 0;
    bool allValuesEqual = true;

    while ((index < dimension_a) && (allValuesEqual == true)) {
        if (ptrArray_a[index] == value) {
            ++index;
        } else {
            allValuesEqual = false;
        }
    }

    return(allValuesEqual);
}

template<class Type>
inline
bool
Array<Type>::AllValuesLessThan(const Type &value) const
{
    int index = 0;
    bool allValuesLessThan = true;

    while ((index < dimension_a) && (allValuesLessThan == true)) {
        if (ptrArray_a[index] < value) {
            ++index;
        } else {
            allValuesLessThan = false;
        }
    }

    return(allValuesLessThan);
}

template<class Type>
inline
bool
Array<Type>::AllValuesLessThanOrEqual(const Type &value) const
{
    int index = 0;
    bool allValuesLessThanOrEqual = true;

    while ((index < dimension_a) && (allValuesLessThanOrEqual == true)) {
        if (ptrArray_a[index] <= value) {
            ++index;
        } else {
            allValuesLessThanOrEqual = false;
        }
    }

    return(allValuesLessThanOrEqual);
}

template<class Type>
inline
bool
Array<Type>::AllValuesGreaterThan(const Type &value) const
{
    int index = 0;
    bool allValuesGreaterThan = true;

    while ((index < dimension_a) && (allValuesGreaterThan == true)) {
        if (ptrArray_a[index] > value) {
            ++index;
        } else {
            allValuesGreaterThan = false;
        }
    }

    return(allValuesGreaterThan);
}

template<class Type>
inline
bool
Array<Type>::AllValuesGreaterThanOrEqual(const Type &value) const
{
    int index = 0;
    bool allValuesGreaterThanOrEqual = true;

    while ((index < dimension_a) && (allValuesGreaterThanOrEqual == true)) {
        if (ptrArray_a[index] >= value) {
            ++index;
        } else {
            allValuesGreaterThanOrEqual = false;
        }
    }

    return(allValuesGreaterThanOrEqual);
}

template<class Type>
inline
bool
Array<Type>::ValueExists(const Type &value) const
{
    int index = 0;
    bool valueExists = false;

    while ((index < dimension_a) && (valueExists == false)) {
        if (ptrArray_a[index] == value) {
            valueExists = true;
        } else {
            ++index;
        }
    }

    return(valueExists);
}

template<class Type>
inline
bool
Array<Type>::ValueExists(const Type &startValue,
                         const Type &endValue) const
{
    int index = 0;
    bool valueExists = false;

    while ((index < dimension_a) && (valueExists == false)) {
        if ((ptrArray_a[index] >= startValue) && (ptrArray_a[index] <= endValue)) {
            valueExists = true;
        } else {
            ++index;
        }
    }

    return(valueExists);
}

template<class Type>
inline
const int
Array<Type>::FindIndex(const Type &value) const
{
    int index = 0;
    bool foundIndex = false;

    while ((index < dimension_a) && (foundIndex == false)) {
        if (ptrArray_a[index] == value) {
            foundIndex = true;
        } else {
            ++index;
        }
    }

    if (foundIndex == false) {
        index = -1;
    }

    return(index);
}

template<class Type>
inline
void
Array<Type>::SetOrderToDecreasing()
{
    increasingOrder_a = false;
    return;
}

template<class Type>
inline
void
Array<Type>::AddElement(const Type &newValue)
{
    int  index = 0;
    Type *ptrNewArray = NULL;

    if (ptrArray_a == NULL) {
        dimension_a = 1;
        ptrArray_a = new Type [dimension_a];
        ptrArray_a[index] = newValue;
    } else {
        ptrNewArray = new Type [dimension_a + 1];

        while (index < dimension_a) {
            ptrNewArray[index] = ptrArray_a[index];
            ++index;
        }

        ptrNewArray[dimension_a] = newValue;
        delete [] ptrArray_a;
        ptrArray_a = ptrNewArray;
        ++dimension_a;
    }

    return;
}

template<class Type>
inline
void
Array<Type>::AddUniqueElement(const Type &newValue)
{
    int  index = 0;
    Type *ptrNewArray = NULL;

    if (ptrArray_a == NULL) {
        dimension_a = 1;
        ptrArray_a = new Type [dimension_a];
        ptrArray_a[index] = newValue;
    } else if (ValueExists(newValue) == false) {
        AddElement(newValue);
    }

    return;
}

template<class Type>
inline
void
Array<Type>::AddElementToFront(const Type &newValue)
{
    int  index = 0;
    Type *ptrNewArray = NULL;

    if (ptrArray_a == NULL) {
        dimension_a = 1;
        ptrArray_a = new Type [dimension_a];
        ptrArray_a[index] = newValue;
    } else {
        ptrNewArray = new Type [dimension_a + 1];

        while (index < dimension_a) {
            ptrNewArray[index + 1] = ptrArray_a[index];
            ++index;
        }

        ptrNewArray[0] = newValue;
        delete [] ptrArray_a;
        ptrArray_a = ptrNewArray;
        ++dimension_a;
    }

    return;
}

template<class Type>
inline
void
Array<Type>::InsertElement(const Type &newValue)
{
    int  index = 0;
    int  newIndex = 0;
    bool  inserted = false;
    Type  currentValue;
    Type *ptrNewArray = NULL;

    if (ptrArray_a == NULL) {
        dimension_a = 1;
        ptrArray_a = new Type [dimension_a];
        ptrArray_a[index] = newValue;
    } else if  ( ((increasingOrder_a == true) && (newValue > LastElement())) ||
                 ((increasingOrder_a == false) && (newValue < LastElement())) ) {
        AddElement(newValue);
    } else if ( ((increasingOrder_a == true) && (newValue < FirstElement())) ||
                ((increasingOrder_a == false) && (newValue > FirstElement())) ) {
        AddElementToFront(newValue);
    } else {
        ptrNewArray = new Type [dimension_a + 1];

        while (index < dimension_a) {
            if (inserted == false) {
                currentValue = ptrArray_a[index];

                if ((increasingOrder_a == true) && (newValue < currentValue)) {
                    ptrNewArray[index] = newValue;
                    inserted = true;
                    ++newIndex;
                } else if ((increasingOrder_a == false) && (newValue > currentValue)) {
                    ptrNewArray[index] = newValue;
                    inserted = true;
                    ++newIndex;
                }
            }

            ptrNewArray[newIndex] = ptrArray_a[index];
            ++newIndex;
            ++index;
        }

        if (ptrNewArray != NULL) {
            delete [] ptrArray_a;
            ptrArray_a = ptrNewArray;
            ++dimension_a;
        }
    }

    return;
}

template<class Type>
inline
void
Array<Type>::InsertUniqueElement(const Type &newValue)
{
    int  index = 0;
    Type *ptrNewArray = NULL;

    if (ptrArray_a == NULL) {
        dimension_a = 1;
        ptrArray_a = new Type [dimension_a];
        ptrArray_a[index] = newValue;
    } else if (ValueExists(newValue) == false) {
        InsertElement(newValue);
    }

    return;
}

template<class Type>
inline
void
Array<Type>::RemoveElement(const Type &removeValue)
{
    int  index = 0;
    int  newIndex = 0;
    int  numberOfElementsToRemove = 0;
    Type *ptrNewArray = NULL;

    if (ptrArray_a != NULL) {
        while (index < dimension_a) {
            if (ElementAt(index) == removeValue) {
                ++numberOfElementsToRemove;
            }

            ++index;
        }

        if (numberOfElementsToRemove > 0) {
            if (numberOfElementsToRemove == dimension_a) {
                delete [] ptrArray_a;
                ptrArray_a = NULL;
            } else {
                ptrNewArray = new Type [dimension_a - numberOfElementsToRemove];
                index = 0;

                while (index < dimension_a) {
                    if (ElementAt(index) != removeValue) {
                        ptrNewArray[newIndex] = ptrArray_a[index];
                        ++newIndex;
                    }

                    ++index;
                }

                delete [] ptrArray_a;
                ptrArray_a = ptrNewArray;
            }

            dimension_a -= numberOfElementsToRemove;
        }
    }

    return;
}

template<class Type>
inline
void
Array<Type>::ResetValues(const Type &resetValue)
{
    int index = 0;

    if (ptrArray_a != NULL) {
        while (index < dimension_a) {
            ptrArray_a[index] = resetValue;
            ++index;
        }
    }

    return;
}

template<class Type>
inline
void
Array<Type>::ResetValue(int index,
                        const Type &resetValue)
{
    if ((index < 0) || (index >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "ResetValue(int, const Type&)",
                                            dimension_a, index);
    }

    ptrArray_a[index] = resetValue;
    return;
}

template<class Type>
inline
void
Array<Type>::ResetValues(const Array<Type> *ptrResetArray)
{
    int index = 0;
    int numberOfElements = dimension_a;

    if (ptrArray_a != NULL) {
        if (ptrResetArray->Size() < numberOfElements) {
            numberOfElements = ptrResetArray->Size();
        }

        while (index < numberOfElements) {
            ptrArray_a[index] = ptrResetArray->ElementAt(index);
            ++index;
        }
    }

    return;
}

template<class Type>
inline
void
Array<Type>::Print(ostream &outputStream) const
{
    int index = 0;

    if (ptrArray_a != NULL) {
        outputStream << ElementAt(index);
        ++index;

        while (index < dimension_a) {
            outputStream << ", " << ElementAt(index);
            ++index;
        }
    }

    return;
}

template<class Type>
inline
string
Array<Type>::GetClassName() const
{
    int          startSubString = 0;
    int          durationSubString = 0;
    bool          pointer = false;
    char          character = ' ';
    string        typeName = typeid(Type).name();
    string        className = ARRAY_H;
    istringstream typeStream(typeName);
    typeStream >> character;

    if (character == 'P') {
        pointer = true;
        typeStream >> character;
        ++startSubString;
    }

    if (typeName.find("basic_string") != string::npos) {
        typeName = "string";
    } else if (islower(character)) {
        if (character == 'i') {
            typeName = "int";
        } else if (character == 'd') {
            typeName = "double";
        }
    } else {
        while (isupper(character) == 0) {
            typeStream >> character;
            ++startSubString;
        }

        durationSubString = (int)(typeName.size()) - startSubString + 1;
        typeName = typeName.substr(startSubString, durationSubString);
    }

    className += "<";
    className += typeName;

    if (pointer == true) {
        className += "*";
    }

    className += ">";
    return(className);
}

template<class Type>
inline
Type
Array<Type>::GetSum() const
{
    if (dimension_a == 0) {
        throw new ArithmeticException(GetClassName(), "GetSum()");
    }

    int index = 0;
    Type sum = ptrArray_a[index];

    while (++index < dimension_a) {
        sum += ptrArray_a[index];
    }

    return(sum);
}

template<class Type>
inline
Type
Array<Type>::GetSum(int startTime,int endTime) const
{
    if (dimension_a == 0) {
        throw new ArithmeticException(GetClassName(), "GetSum()");
    }

    int index = startTime;
    Type sum = ptrArray_a[index];

    while (++index <= endTime) {
        sum += ptrArray_a[index];
    }

    return(sum);
}

template<class Type>
inline
Type
Array<Type>::GetAverage() const
{
    if (dimension_a == 0) {
        throw new ArithmeticException(GetClassName(), "GetAverage()");
    }

    return(GetSum() / (Type)dimension_a);
}

template<class Type>
inline
Type
Array<Type>::GetMaximum() const
{
    if (dimension_a == 0) {
        throw new ArithmeticException(GetClassName(), "GetMaximum()");
    }

    int index = 0;
    Type maximum = ptrArray_a[index];

    while (++index < dimension_a) {
        if (ptrArray_a[index] > maximum) {
            maximum = ptrArray_a[index];
        }
    }

    return(maximum);
}

template<class Type>
inline
Type
Array<Type>::GetMinimum() const
{
    if (dimension_a == 0) {
        throw new ArithmeticException(GetClassName(), "GetMinimum()");
    }

    int index = 0;
    Type minimum = ptrArray_a[index];

    while (++index < dimension_a) {
        if (ptrArray_a[index] < minimum) {
            minimum = ptrArray_a[index];
        }
    }

    return(minimum);
}

template<class Type>
inline
const Type&
Array<Type>::ElementAt(int index) const
{
    if ((index < 0) || (index >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "ElementAt(int)",
                                            dimension_a, index);
    }

    return(ptrArray_a[index]);
}

template<class Type>
inline
const Type&
Array<Type>::FirstElement() const
{
    const int index = 0;
    return(ElementAt(index));
}

template<class Type>
inline
const Type&
Array<Type>::LastElement() const
{
    const int index = dimension_a - 1;
    return(ElementAt(index));
}

template<class Type>
inline
void
Array<Type>::Delete()
{
    if (ptrArray_a != NULL) {
        delete [] ptrArray_a;
        ptrArray_a = NULL;
    }

    dimension_a = 0;
    return;
}

template<class Type>
inline
Array<Type>::~Array()
{
    Delete();
    return;
}

#endif
