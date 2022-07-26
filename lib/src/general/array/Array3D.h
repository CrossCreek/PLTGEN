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
 * Array3D.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ARRAY_3D_H
#define ARRAY_3D_H "Array3D"

#include "Array2D.h"

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
class Array3D
{
public:

    inline           Array3D();
    inline           Array3D(int firstDimension);
    inline           Array3D(int firstDimension,
                             int secondDimension);
    inline           Array3D(int firstDimension,
                             int secondDimension,
                             int thirdDimension);
    inline           Array3D(int firstDimension,
                             int secondDimension,
                             int thirdDimension,
                             const Type &initialValue);
    inline           Array3D(const Array3D<Type> &copyMe);
    inline virtual  ~Array3D();

    inline Array3D<Type>& operator =  (const Array3D<Type> &copyMe);
    inline bool           operator == (const Array3D<Type> &compareMe);
    inline Array2D<Type>& operator [] (int index);

    inline void                 AddArray(Array2D<Type> *&ptrNewArray);
    inline void                 AddArray(int firstIndex,
                                         Array2D<Type> *&ptrNewArray);
    inline void                 AddArray(int firstIndex,
                                         int secondIndex,
                                         Array<Type> *&ptrNewArray);
    inline void                 AddArray(int firstIndex,
                                         int secondIndex,
                                         int thirdDimension,
                                         const Type &initialValue);
    inline void                 Delete();
    inline void                 Print(ostream &outputStream) const;
    inline void                 ResetValues(const Type &resetValue);

    inline const int           Size() const;
    inline const int           GetSize(int index) const;
    inline const int           GetSize(int firstIndex,
                                       int secondIndex) const;

    inline string               GetClassName() const;

    inline Type                 GetMaximum() const;
    inline Type                 GetMinimum() const;

    inline const Type&          ElementAt(int firstIndex,
                                          int secondIndex,
                                          int thirdIndex) const;
    inline const Array<Type>*   ElementAt(int firstIndex,
                                          int secondIndex) const;
    inline const Array2D<Type>* ElementAt(int index) const;

protected:

private:

    int            dimension_a;
    Array2D<Type> **ptrMatrix_a;
};

template<class Type>
inline
Array3D<Type>::Array3D()
    : dimension_a(0), ptrMatrix_a(NULL)
{
    return;
}

template<class Type>
inline
Array3D<Type>::Array3D(int firstDimension)
    : dimension_a(firstDimension), ptrMatrix_a(NULL)
{
    int index = 0;
    ptrMatrix_a = new Array2D<Type> * [dimension_a];

    while (index < dimension_a) {
        ptrMatrix_a[index] = NULL;
        ++index;
    }

    return;
}

template<class Type>
inline
Array3D<Type>::Array3D(int firstDimension,
                       int secondDimension)
    : dimension_a(firstDimension), ptrMatrix_a(NULL)
{
    int index = 0;
    ptrMatrix_a = new Array2D<Type> * [dimension_a];

    while (index < dimension_a) {
        ptrMatrix_a[index] = new Array2D<Type>(secondDimension);
        ++index;
    }

    return;
}

template<class Type>
inline
Array3D<Type>::Array3D(int firstDimension,
                       int secondDimension,
                       int thirdDimension)
    : dimension_a(firstDimension), ptrMatrix_a(NULL)
{
    int index = 0;
    ptrMatrix_a = new Array2D<Type> * [dimension_a];

    while (index < dimension_a) {
        ptrMatrix_a[index] = new Array2D<Type>(secondDimension, thirdDimension);
        ++index;
    }

    return;
}

template<class Type>
inline
Array3D<Type>::Array3D(int firstDimension,
                       int secondDimension,
                       int thirdDimension,
                       const Type &initialValue)
    : dimension_a(firstDimension), ptrMatrix_a(NULL)
{
    int index = 0;
    ptrMatrix_a = new Array2D<Type> * [dimension_a];

    while (index < dimension_a) {
        ptrMatrix_a[index] = new Array2D<Type>(secondDimension, thirdDimension,
                                               initialValue);
        ++index;
    }

    return;
}

template<class Type>
inline
Array3D<Type>::Array3D(const Array3D<Type> &copyMe)
    : dimension_a(copyMe.dimension_a), ptrMatrix_a(NULL)
{
    int index = 0;

    if (copyMe.ptrMatrix_a != NULL) {
        ptrMatrix_a = new Array2D<Type> * [dimension_a];

        while (index < dimension_a) {
            if (copyMe.ElementAt(index) != NULL) {
                ptrMatrix_a[index] = new Array2D<Type>(*(copyMe.ElementAt(index)));
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
Array3D<Type>&
Array3D<Type>::operator = (const Array3D<Type> &copyMe)
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
Array3D<Type>::operator == (const Array3D<Type> &compareMe)
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
Array2D<Type>&
Array3D<Type>::operator [] (int firstIndex)
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
Array3D<Type>::Size() const
{
    return(dimension_a);
}

template<class Type>
inline
const int
Array3D<Type>::GetSize(int firstIndex) const
{
    int secondDimension = 0;

    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "GetSize(int)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] != NULL) {
        secondDimension = ptrMatrix_a[firstIndex]->Size();
    }

    return(secondDimension);
}

template<class Type>
inline
const int
Array3D<Type>::GetSize(int firstIndex,
                       int secondIndex) const
{
    int thirdDimension = 0;

    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "GetSize(int, int)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "GetSize(int, int)",
                                       "ptrMatrix_a[firstIndex]");
    }

    if ((secondIndex < 0) || (secondIndex >= ptrMatrix_a[firstIndex]->Size())) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "GetSize(int, int)",
                                            dimension_a,
                                            ptrMatrix_a[firstIndex]->Size());
    }

    if (ptrMatrix_a[firstIndex]->ElementAt(secondIndex) != NULL) {
        thirdDimension = (ptrMatrix_a[firstIndex]->ElementAt(secondIndex))->Size();
    }

    return(thirdDimension);
}

template<class Type>
inline
void
Array3D<Type>::AddArray(Array2D<Type> *&ptrNewArray)
{
    int            index = 0;
    Array2D<Type> **ptrNewMatrix = NULL;

    if (ptrMatrix_a == NULL) {
        dimension_a = 1;
        ptrMatrix_a = new Array2D<Type> * [dimension_a];
        ptrMatrix_a[index] = ptrNewArray;
    } else {
        ptrNewMatrix = new Array2D<Type> * [dimension_a + 1];

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
Array3D<Type>::AddArray(int firstIndex,
                        Array2D<Type> *&ptrNewArray)
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddArray(int, const Array2D<Type>*&)",
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
Array3D<Type>::AddArray(int firstIndex,
                        int secondIndex,
                        Array<Type> *&ptrNewArray)
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddArray(int, int, const Array<Type>*&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "AddArray(int, int, const Array<Type>*&)",
                                       "ptrMatrix_a[firstIndex]");
    }

    if ((secondIndex < 0) || (secondIndex >= ptrMatrix_a[firstIndex]->Size())) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddArray(int, int, const Array<Type>*&)",
                                            dimension_a, ptrMatrix_a[firstIndex]->Size());
    }

    ptrMatrix_a[firstIndex]->AddArray(secondIndex, ptrNewArray);
    return;
}

template<class Type>
inline
void
Array3D<Type>::AddArray(int firstIndex,
                        int secondIndex,
                        int thirdDimension,
                        const Type &initialValue)
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddArray(int, int, int, const Type&)",
                                            dimension_a, firstIndex);
    }

    if (ptrMatrix_a[firstIndex] == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "AddArray(int, int, int, const Type&)",
                                       "ptrMatrix_a[firstIndex]");
    }

    if ((secondIndex < 0) || (secondIndex >= ptrMatrix_a[firstIndex]->Size())) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddArray(int, int, int, const Type&)",
                                            dimension_a, ptrMatrix_a[firstIndex]->Size());
    }

    ptrMatrix_a[firstIndex]->AddArray(secondIndex, thirdDimension, initialValue);
    return;
}

template<class Type>
inline
void
Array3D<Type>::Print(ostream &outputStream) const
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
Array3D<Type>::ResetValues(const Type &resetValue)
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
string
Array3D<Type>::GetClassName() const
{
    int          startSubString = 0;
    int          durationSubString = 0;
    bool          pointer = false;
    char          character = ' ';
    string        typeName = typeid(Type).name();
    string        className = ARRAY_3D_H;
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

        durationSubString = (int)typeName.size() - startSubString + 1;
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
Array3D<Type>::GetMaximum() const
{
    int index = 0;
    Type maximum;
    Type matrixMaximum = ptrMatrix_a[index]->GetMaximum();

    if (dimension_a == 0) {
        throw new ArithmeticException(GetClassName(), "GetMaximum()");
    }

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
Array3D<Type>::GetMinimum() const
{
    int index = 0;
    Type minimum;
    Type matrixMinimum = ptrMatrix_a[index]->GetMinimum();

    if (dimension_a == 0) {
        throw new ArithmeticException(GetClassName(), "GetMinimum()");
    }

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
const Array2D<Type>*
Array3D<Type>::ElementAt(int firstIndex) const
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "ElementAt(int)",
                                            dimension_a, firstIndex);
    }

    return(ptrMatrix_a[firstIndex]);
}

template<class Type>
inline
const Array<Type>*
Array3D<Type>::ElementAt(int firstIndex,
                         int secondIndex) const
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "ElementAt(int, int)",
                                            dimension_a, firstIndex);
    }

    if ((secondIndex < 0) || (secondIndex >= GetSize(firstIndex))) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "ElementAt(int, int)",
                                            GetSize(firstIndex), secondIndex);
    }

    return(ptrMatrix_a[firstIndex]->ElementAt(secondIndex));
}

template<class Type>
inline
const Type&
Array3D<Type>::ElementAt(int firstIndex,
                         int secondIndex,
                         int thirdIndex) const
{
    if ((firstIndex < 0) || (firstIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "ElementAt(int, int, int)",
                                            dimension_a, firstIndex);
    }

    if ((secondIndex < 0) || (secondIndex >= GetSize(firstIndex))) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "ElementAt(int, int, int)",
                                            GetSize(firstIndex), secondIndex);
    }

    if ((thirdIndex < 0) || (thirdIndex >= GetSize(firstIndex, secondIndex))) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "ElementAt(int, int, int)",
                                            GetSize(firstIndex, secondIndex), thirdIndex);
    }

    return((ptrMatrix_a[firstIndex]->ElementAt(secondIndex))->ElementAt(thirdIndex));
}

template<class Type>
inline
void
Array3D<Type>::Delete()
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
Array3D<Type>::~Array3D()
{
    Delete();
    return;
}

#endif
