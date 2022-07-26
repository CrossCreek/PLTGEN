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
 * Array2D.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ARRAY_2D_H
#define ARRAY_2D_H "Array2D"

#include "Array.h"
#include "general/exception/NullPointerException.h"

using namespace std;

//------------------------------------------------------------------------------
// Class Attribute Descriptions
//
// Name                  Description (units)
// ------------------    -------------------------------------------------------
// dimension_a           number of pointers to Array classes
// ptrMatrix_a*          pointer to a 'C-style' array of Array classes
//
//------------------------------------------------------------------------------

template<class Type>
class Array2D
{
public:

    inline           Array2D();
    inline           Array2D(int firstDimension);
    inline           Array2D(int firstDimension,
                             int secondDimension);
    inline           Array2D(int firstDimension,
                             int secondDimension,
                             const Type &initialValue);
    inline           Array2D(const Array2D<Type> &copyMe);
    inline virtual  ~Array2D();

    inline Array2D<Type>& operator =  (const Array2D<Type> &copyMe);
    inline bool           operator == (const Array2D<Type> &compareMe);
    inline Array<Type>&   operator [] (int firstIndex);

    inline void               AddArray(Array<Type> *&ptrNewArray);
    inline void               AddArray(int firstIndex,
                                       Array<Type> *&ptrNewArray);
    inline void               AddArray(int firstIndex,
                                       int secondDimension,
                                       const Type &initialValue);
    inline void               AddElement(int firstIndex,
                                         const Type &newValue);
    inline void               AddUniqueElement(int firstIndex,
            const Type &newValue);
    inline void               Delete();
    inline void               InsertElement(int firstIndex,
                                            const Type &newValue);
    inline void               InsertUniqueElement(int firstIndex,
            const Type &newValue);
    inline void               Print(ostream &outputStream) const;
    inline void               Remove(int firstIndex);
    inline void               RemoveAll();
    inline void               ResetValues(const Type &resetValue);
    inline void               ResetValues(int firstIndex,
                                          const Type &resetValue);
    inline void               ResetValue(int firstIndex,
                                         int secondIndex,
                                         const Type &resetValue);
    inline void               SetOrderToDecreasing();

    inline int               FindIndex(int firstIndex,
                                       const Type &value) const;
    inline const int         Size() const;
    inline const int         GetSize(int firstIndex) const;

    inline bool               AllValuesEqual(int firstIndex,
            const Type &value) const;
    inline bool               AllValuesGreaterThan(int firstIndex,
            const Type &value) const;
    inline bool               AllValuesGreaterThanOrEqual(int firstIndex,
            const Type &value) const;
    inline bool               AllValuesLessThan(int firstIndex,
            const Type &value) const;
    inline bool               AllValuesLessThanOrEqual(int firstIndex,
            const Type &value) const;
    inline bool               ValueExists(const Type &value) const;
    inline bool               ValueExists(int firstIndex,
                                          const Type &value) const;

    inline string             GetClassName() const;

    inline Type               GetSum() const;
    inline Type               GetColumnSum(int columnIndex) const;
    inline Type               GetRowSum(int rowIndex) const;
    inline Type               GetMaximum() const;
    inline Type               GetMaximum(int firstIndex) const;
    inline Type               GetMinimum() const;
    inline Type               GetMinimum(int firstIndex) const;

    inline const Type&        LastElement(int firstIndex) const;
    inline const Type&        ElementAt(int firstIndex,
                                        int secondIndex) const;

    inline const Array<Type>* ElementAt(int firstIndex) const;

protected:

private:

    int          dimension_a;
    Array<Type> **ptrMatrix_a;
};

template<class Type>
inline
Array2D<Type>::Array2D()
    : dimension_a(0), ptrMatrix_a(NULL)
{
    return;
}

template<class Type>
inline
Array2D<Type>::Array2D(int firstDimension)
    : dimension_a(firstDimension), ptrMatrix_a(NULL)
{
    int index = 0;
    ptrMatrix_a = new Array<Type> * [dimension_a];

    while (index < dimension_a) {
        ptrMatrix_a[index] = NULL;
        ++index;
    }

    return;
}

template<class Type>
inline
Array2D<Type>::Array2D(int firstDimension,
                       int secondDimension)
    : dimension_a(firstDimension), ptrMatrix_a(NULL)
{
    int index = 0;
    ptrMatrix_a = new Array<Type> * [dimension_a];

    while (index < dimension_a) {
        ptrMatrix_a[index] = new Array<Type>(secondDimension);
        ++index;
    }

    return;
}

template<class Type>
inline
Array2D<Type>::Array2D(int firstDimension,
                       int secondDimension,
                       const Type &initialValue)
    : dimension_a(firstDimension), ptrMatrix_a(NULL)
{
    int index = 0;
    ptrMatrix_a = new Array<Type> * [dimension_a];

    while (index < dimension_a) {
        ptrMatrix_a[index] = new Array<Type>(secondDimension, initialValue);
        ++index;
    }

    return;
}

template<class Type>
inline
Array2D<Type>::Array2D(const Array2D<Type> &copyMe)
    : dimension_a(copyMe.dimension_a), ptrMatrix_a(NULL)
{
    int index = 0;

    if (copyMe.ptrMatrix_a != NULL) {
        ptrMatrix_a = new Array<Type> * [dimension_a];

        while (index < dimension_a) {
            if (copyMe.ElementAt(index) != NULL) {
                ptrMatrix_a[index] = new Array<Type>(*(copyMe.ElementAt(index)));
            } else {
                ptrMatrix_a[index] = NULL;
            }

            ++index;
        }
    }

    return;
}

template<class Type>
inline
Array2D<Type>&
Array2D<Type>::operator = (const Array2D<Type> &copyMe)
{
    int index = 0;

    if (ptrMatrix_a != NULL) {
        while (index < dimension_a) {
            if (ptrMatrix_a[index] != NULL) {
                delete ptrMatrix_a[index];
                ptrMatrix_a[index] = NULL;
            }

            ++index;
        }

        delete [] ptrMatrix_a;
        ptrMatrix_a = NULL;
    }

    dimension_a = copyMe.dimension_a;
    ptrMatrix_a = copyMe.ptrMatrix_a;
    return(*this);
}

template<class Type>
inline
bool
Array2D<Type>::operator == (const Array2D<Type> &compareMe)
{
    int index = 0;
    bool equal = false;

    if (dimension_a == compareMe.dimension_a) {
        equal = true;

        while ((index < dimension_a) && (equal == true)) {
            if (*(ElementAt(index)) != *(compareMe.ElementAt(index))) {
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
Array<Type>&
Array2D<Type>::operator [] (int firstIndex)
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "operator [] (int)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        throw new NullPointerException(GetClassName(), "operator [] (int)",
                                       "ptrMatrix_a[firstIndex]");
    }

    return(*(ptrMatrix_a[firstIndex]));
}

template<class Type>
inline
const int
Array2D<Type>::Size() const
{
    return(dimension_a);
}

template<class Type>
inline
const int
Array2D<Type>::GetSize(int firstIndex) const
{
    int secondDimensionSize = 0;

    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "GetSize(int )",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] != NULL) {
        secondDimensionSize = ptrMatrix_a[firstIndex]->Size();
    }

    return(secondDimensionSize);
}


template<class Type>
inline
Type
Array2D<Type>::GetSum() const
{
    if (dimension_a == 0) {
        throw new IndexOutOfBoundsException(GetClassName(), "GetSum()",
                                            dimension_a, dimension_a);
    }

    int index = 0;
    Type sum = ptrMatrix_a[index]->GetSum();

    while (++index < dimension_a) {
        sum += ptrMatrix_a[index]->GetSum();
    }

    return(sum);
}

template<class Type>
inline
Type
Array2D<Type>::GetColumnSum(int columnIndex) const
{
    int rowIndex = 0;
    Type columnSum;

    if (dimension_a == 0) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "GetColumnSum(int)",
                                            dimension_a, dimension_a);
    }

    try {
        columnSum = ptrMatrix_a[rowIndex]->ElementAt(columnIndex);
        ++rowIndex;

        while (rowIndex < dimension_a) {
            columnSum += ptrMatrix_a[rowIndex]->ElementAt(columnIndex);
            ++rowIndex;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetColumnSum(int)");
        throw ptrError;
    }

    return(columnSum);
}

template<class Type>
inline
Type
Array2D<Type>::GetRowSum(int rowIndex) const
{
    if ((rowIndex < 0) || (rowIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "GetRowSum(int)",
                                            dimension_a, rowIndex);
    }

    return(ptrMatrix_a[rowIndex]->GetSum());
}

template<class Type>
inline
Type
Array2D<Type>::GetMaximum() const
{
    if (dimension_a == 0) {
        throw new ArithmeticException(GetClassName(), "GetMaximum()");
    }

    int index = 0;
    Type maximum;
    Type matrixMaximum = ptrMatrix_a[index]->GetMaximum();

    while (++index < dimension_a) {
        maximum = ptrMatrix_a[index]->GetMaximum();

        if (maximum > matrixMaximum) {
            matrixMaximum = maximum;
        }
    }

    return(matrixMaximum);
}

template<class Type>
inline
Type
Array2D<Type>::GetMaximum(int firstIndex) const
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "GetMaximum(int)",
                                            dimension_a, firstIndex);
    }

    return(ptrMatrix_a[firstIndex]->GetMaximum());
}

template<class Type>
inline
Type
Array2D<Type>::GetMinimum() const
{
    if (dimension_a == 0) {
        throw new ArithmeticException(GetClassName(), "GetMinimum()");
    }

    int index = 0;
    Type minimum;
    Type matrixMinimum = ptrMatrix_a[index]->GetMinimum();

    while (++index < dimension_a) {
        minimum = ptrMatrix_a[index]->GetMinimum();

        if (minimum < matrixMinimum) {
            matrixMinimum = minimum;
        }
    }

    return(matrixMinimum);
}

template<class Type>
inline
Type
Array2D<Type>::GetMinimum(int firstIndex) const
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "GetMinimum(int)",
                                            dimension_a, firstIndex);
    }

    return(ptrMatrix_a[firstIndex]->GetMinimum());
}

template<class Type>
inline
const Type&
Array2D<Type>::LastElement(int firstIndex) const
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "LastElement(int)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        throw new NullPointerException(GetClassName(), "LastElement(int)",
                                       "ptrMatrix_a[firstIndex]");
    }

    return(ptrMatrix_a[firstIndex]->LastElement());
}

template<class Type>
inline
void
Array2D<Type>::AddArray(Array<Type> *&ptrNewArray)
{
    int          index = 0;
    Array<Type> **ptrNewMatrix = NULL;

    if (ptrMatrix_a == NULL) {
        dimension_a = 1;
        ptrMatrix_a = new Array<Type> * [dimension_a];
        ptrMatrix_a[0] = ptrNewArray;
    } else {
        ptrNewMatrix = new Array<Type> * [dimension_a + 1];

        while (index < dimension_a) {
            ptrNewMatrix[index] = ptrMatrix_a[index];
            ++index;
        }

        ptrNewMatrix[dimension_a] = ptrNewArray;
        delete [] ptrMatrix_a;
        ptrMatrix_a = ptrNewMatrix;
        ++dimension_a;
    }

    return;
}

template<class Type>
inline
void
Array2D<Type>::AddArray(int firstIndex,
                        Array<Type> *&ptrNewArray)
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddArray(int, const Array<Type>*)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] != NULL) {
        delete ptrMatrix_a[firstIndex];
    }

    ptrMatrix_a[firstIndex] = ptrNewArray;
    return;
}

template<class Type>
inline
void
Array2D<Type>::AddArray(int firstIndex,
                        int secondDimension,
                        const Type &initialValue)
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddArray(int, int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] != NULL) {
        delete ptrMatrix_a[firstIndex];
    }

    ptrMatrix_a[firstIndex] = new Array<Type>(secondDimension, initialValue);
    return;
}

template<class Type>
inline
void
Array2D<Type>::AddElement(int firstIndex,
                          const Type &newValue)
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddElement(int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        ptrMatrix_a[firstIndex] = new Array<Type>();
    }

    ptrMatrix_a[firstIndex]->AddElement(newValue);
    return;
}

template<class Type>
inline
void
Array2D<Type>::AddUniqueElement(int firstIndex,
                                const Type &newValue)
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddUniqueElement(int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        ptrMatrix_a[firstIndex] = new Array<Type>();
    }

    ptrMatrix_a[firstIndex]->AddUniqueElement(newValue);
    return;
}

template<class Type>
inline
void
Array2D<Type>::InsertElement(int firstIndex,
                             const Type &newValue)
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "InsertElement(int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        ptrMatrix_a[firstIndex] = new Array<Type>();
    }

    ptrMatrix_a[firstIndex]->InsertElement(newValue);
    return;
}

template<class Type>
inline
void
Array2D<Type>::InsertUniqueElement(int firstIndex,
                                   const Type &newValue)
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "InsertUniqueElement(int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        ptrMatrix_a[firstIndex] = new Array<Type>();
    }

    ptrMatrix_a[firstIndex]->InsertUniqueElement(newValue);
    return;
}

template<class Type>
inline
void
Array2D<Type>::Print(ostream &outputStream) const
{
    int index = 0;

    if (ptrMatrix_a != NULL) {
        while (index < dimension_a) {
            if (ptrMatrix_a[index] != NULL) {
                ptrMatrix_a[index]->Print(outputStream);
                outputStream << NEW_LINE;
            }

            ++index;
        }
    }

    return;
}

template<class Type>
inline
void
Array2D<Type>::ResetValues(const Type &resetValue)
{
    int index = 0;

    if (ptrMatrix_a != NULL) {
        while (index < dimension_a) {
            if (ptrMatrix_a[index] != NULL) {
                ptrMatrix_a[index]->ResetValues(resetValue);
            }

            ++index;
        }
    }

    return;
}

template<class Type>
inline
void
Array2D<Type>::ResetValues(int firstIndex,
                           const Type &resetValue)
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "ResetValues(int, const Type&)",
                                            dimension_a, firstIndex);
    }

    ptrMatrix_a[firstIndex]->ResetValues(resetValue);
    return;
}

template<class Type>
inline
void
Array2D<Type>::ResetValue(int firstIndex,
                          int secondIndex,
                          const Type &resetValue)
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "ResetValue(int, int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "ResetValue(int, int, const Type&)",
                                       "ptrMatrix_a[firstIndex]");
    }

    try {
        ptrMatrix_a[firstIndex]->ResetValue(secondIndex, resetValue);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "ResetValue(int, int, const Type&)");
        throw ptrError;
    }

    return;
}

template<class Type>
inline
void
Array2D<Type>::SetOrderToDecreasing()
{
    int index = 0;

    if (ptrMatrix_a != NULL) {
        while (index < dimension_a) {
            if (ptrMatrix_a[index] != NULL) {
                ptrMatrix_a[index]->SetOrderToDecreasing();
            }

            ++index;
        }
    }

    return;
}

template<class Type>
inline
int
Array2D<Type>::FindIndex(int firstIndex,
                         const Type &value) const
{
    int arrayIndex = -1;

    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "FindIndex(int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] != NULL) {
        arrayIndex = ptrMatrix_a[firstIndex]->FindIndex(value);
    }

    return(arrayIndex);
}

template<class Type>
inline
bool
Array2D<Type>::ValueExists(const Type &value) const
{
    int firstIndex = 0;
    bool valueExists = false;

    while ((firstIndex < dimension_a) && (valueExists == false)) {
        valueExists = ptrMatrix_a[firstIndex]->ValueExists(value);
        ++firstIndex;
    }

    return(valueExists);
}

template<class Type>
inline
bool
Array2D<Type>::ValueExists(int firstIndex,
                           const Type &value) const
{
    bool valueExists = false;

    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "ValueExists(int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if ( ptrMatrix_a[firstIndex] != NULL ) {
        valueExists = ptrMatrix_a[firstIndex]->ValueExists(value);
    }

    return(valueExists);
}

template<class Type>
inline
bool
Array2D<Type>::AllValuesEqual(int firstIndex,
                              const Type &value) const
{
    bool allValues = false;

    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AllValuesEqual(int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "AllValuesEqual(int, const Type&)",
                                       "ptrMatrix_a[firstIndex]");
    }

    return(ptrMatrix_a[firstIndex]->AllValuesEqual(value));
}

template<class Type>
inline
bool
Array2D<Type>::AllValuesGreaterThan(int firstIndex,
                                    const Type &value) const
{
    bool allValues = false;

    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AllValuesGreaterThan(int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "AllValuesGreaterThan(int, const Type&)",
                                       "ptrMatrix_a[firstIndex]");
    }

    return(ptrMatrix_a[firstIndex]->AllValuesGreaterThan(value));
}

template<class Type>
inline
bool
Array2D<Type>::AllValuesGreaterThanOrEqual(int firstIndex,
        const Type &value) const
{
    bool allValues = false;

    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AllValuesGreaterThanOrEqual(int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "AllValuesGreaterThanOrEqual(int, const Type&)",
                                       "ptrMatrix_a[firstIndex]");
    }

    return(ptrMatrix_a[firstIndex]->AllValuesGreaterThanOrEqual(value));
}

template<class Type>
inline
bool
Array2D<Type>::AllValuesLessThan(int firstIndex,
                                 const Type &value) const
{
    bool allValues = false;

    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AllValuesLessThan(int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "AllValuesLessThan(int, const Type&)",
                                       "ptrMatrix_a[firstIndex]");
    }

    return(ptrMatrix_a[firstIndex]->AllValuesLessThan(value));
}

template<class Type>
inline
bool
Array2D<Type>::AllValuesLessThanOrEqual(int firstIndex,
                                        const Type &value) const
{
    bool allValues = false;

    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AllValuesLessThanOrEqual(int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "AllValuesLessThanOrEqual(int, const Type&)",
                                       "ptrMatrix_a[firstIndex]");
    }

    return(ptrMatrix_a[firstIndex]->AllValuesLessThanOrEqual(value));
}

template<class Type>
inline
string
Array2D<Type>::GetClassName() const
{
    int          startSubString = 0;
    int          durationSubString = 0;
    bool          pointer = false;
    char          character = ' ';
    string        typeName = typeid(Type).name();
    string        className = ARRAY_2D_H;
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
const Array<Type>*
Array2D<Type>::ElementAt(int firstIndex) const
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "ElementAt(int)",
                                            dimension_a, firstIndex);
    }

    return(ptrMatrix_a[firstIndex]);
}

template<class Type>
inline
const Type&
Array2D<Type>::ElementAt(int firstIndex,
                         int secondIndex) const
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "ElementAt(int, int)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "ElementAt(int, int)",
                                       "ptrMatrix_a[firstIndex]");
    }

    if ((secondIndex < 0) || (secondIndex >= ptrMatrix_a[firstIndex]->Size())) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "ElementAt(int, int)",
                                            ptrMatrix_a[firstIndex]->Size(), secondIndex);
    }

    return(ptrMatrix_a[firstIndex]->ElementAt(secondIndex));
}

template<class Type>
inline
void
Array2D<Type>::Remove(int firstIndex)
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "Remove(int)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] != NULL) {
        delete ptrMatrix_a[firstIndex];
        ptrMatrix_a[firstIndex] = NULL;
    }

    return;
}

template<class Type>
inline
void
Array2D<Type>::RemoveAll()
{
    int index = 0;

    if (ptrMatrix_a != NULL) {
        while (index < dimension_a) {
            if (ptrMatrix_a[index] != NULL) {
                delete ptrMatrix_a[index];
                ptrMatrix_a[index] = NULL;
            }

            ++index;
        }
    }

    return;
}

template<class Type>
inline
void
Array2D<Type>::Delete()
{
    int index = 0;

    if (ptrMatrix_a != NULL) {
        while (index < dimension_a) {
            if (ptrMatrix_a[index] != NULL) {
                delete ptrMatrix_a[index];
                ptrMatrix_a[index] = NULL;
            }

            ++index;
        }

        delete [] ptrMatrix_a;
        ptrMatrix_a = NULL;
    }

    dimension_a = 0;
    return;
}

template<class Type>
inline
Array2D<Type>::~Array2D()
{
    Delete();
    return;
}

#endif
