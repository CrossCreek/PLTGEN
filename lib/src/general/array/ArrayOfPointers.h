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
 * ArrayOfPointers.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ARRAY_OF_POINTERS_H
#define ARRAY_OF_POINTERS_H "ArrayOfPointers"

#include <iostream>
#include <string>
#include <sstream>
#include <typeinfo>

#include "general/exception/IndexOutOfBoundsException.h"
#include "general/exception/InputException.h"

using namespace std;

//------------------------------------------------------------------------------
// Class Attribute Descriptions
//
// Name                   Description (units)
// ------------------     ------------------------------------------------------
// dimension_a            number of elements in the array
// ptrArrayOfPointers_a** 'C-style' array of pointers
//
//------------------------------------------------------------------------------

template<class Type>
class ArrayOfPointers
{
public:

    inline           ArrayOfPointers();
    inline           ArrayOfPointers(int initialSize,
                                     Type *ptrInitialPointer = NULL);
    inline           ArrayOfPointers(const ArrayOfPointers<Type> &copyMe);
    inline virtual  ~ArrayOfPointers();

    inline ArrayOfPointers<Type>& operator = (const ArrayOfPointers<Type> &copyMe);
    inline bool           operator == (const ArrayOfPointers<Type> &compareMe);
    inline Type*          operator [] (int index);

    inline void        AddElement(Type *ptrAddObject);
    inline void        AddElement(int index,
                                  Type *ptrNewObject);
    inline void        Delete();
    inline void        RemoveAll();
    inline void        SwitchElements(int index1, int index2);

    inline const int  Size() const;

    inline string      GetClassName() const;

    inline const Type* ElementAt(int index) const;

protected:

private:

    int   dimension_a;
    Type **ptrArrayOfPointers_a;
};

template<class Type>
inline
ArrayOfPointers<Type>::ArrayOfPointers()
    : dimension_a(0), ptrArrayOfPointers_a(NULL)
{
    return;
}

template<class Type>
inline
ArrayOfPointers<Type>::ArrayOfPointers(int initialSize,
                                       Type *ptrInitialPointer)
    : dimension_a(initialSize), ptrArrayOfPointers_a(NULL)
{
    int index = 0;

    if (initialSize <= 0) {
        throw new InputException(GetClassName(), "Array(int)",
                                 "Entered zero or negative number for initial size");
    }

    ptrArrayOfPointers_a = new Type * [initialSize];

    while (index < initialSize) {
        ptrArrayOfPointers_a[index] = ptrInitialPointer;
        ++index;
    }

    return;
}

template<class Type>
inline
ArrayOfPointers<Type>::ArrayOfPointers(const ArrayOfPointers<Type> &copyMe)
    : dimension_a(copyMe.dimension_a), ptrArrayOfPointers_a(NULL)
{
    int index = 0;

    if (copyMe.ptrArrayOfPointers_a != NULL) {
        ptrArrayOfPointers_a = new Type * [dimension_a];

        while (index < dimension_a) {
            if (copyMe.ElementAt(index) != NULL) {
                ptrArrayOfPointers_a[index] = new Type(*(copyMe.ElementAt(index)));
            } else {
                ptrArrayOfPointers_a[index] = NULL;
            }

            ++index;
        }
    }

    return;
}

template<class Type>
inline
ArrayOfPointers<Type>&
ArrayOfPointers<Type>::operator = (const ArrayOfPointers<Type> &copyMe)
{
    int index = 0;
    dimension_a          = copyMe.dimension_a;
    ptrArrayOfPointers_a = NULL;

    if (copyMe.ptrArrayOfPointers_a != NULL) {
        ptrArrayOfPointers_a = new Type * [dimension_a];

        while (index < dimension_a) {
            if (copyMe.ElementAt(index) != NULL) {
                ptrArrayOfPointers_a[index] = new Type(*(copyMe.ElementAt(index)));
            } else {
                ptrArrayOfPointers_a[index] = NULL;
            }

            ++index;
        }
    }

    return(*this);
}

template<class Type>
inline
bool
ArrayOfPointers<Type>::operator == (const ArrayOfPointers<Type> &compareMe)
{
    int index = 0;
    bool equal = false;

    if (dimension_a == compareMe.dimension_a) {
        equal = true;

        while ((index < dimension_a) && (equal == true)) {
            if (*(ElementAt(index)) != *(compareMe.ElementAt(index))) {
                equal = false;
            }

            ++index;
        }
    }

    return(equal);
}

template<class Type>
inline
Type*
ArrayOfPointers<Type>::operator [] (int index)
{
    if ((index < 0) || (index >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "operator [] (int)",
                                            dimension_a, index);
    }

    return(ptrArrayOfPointers_a[index]);
}

template<class Type>
inline
const Type*
ArrayOfPointers<Type>::ElementAt(int index) const
{
    if ((index < 0) || (index >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "ElementAt(int)",
                                            dimension_a, index);
    }

    return(ptrArrayOfPointers_a[index]);
}

template<class Type>
inline
const int
ArrayOfPointers<Type>::Size() const
{
    return(dimension_a);
}

template<class Type>
inline
void
ArrayOfPointers<Type>::AddElement(Type *ptrAddObject)
{
    int   index = 0;
    Type **ptrNewArrayOfPointers = new Type * [dimension_a + 1];

    if (ptrArrayOfPointers_a != NULL) {
        while (index < dimension_a) {
            ptrNewArrayOfPointers[index] = ptrArrayOfPointers_a[index];
            ++index;
        }

        delete [] ptrArrayOfPointers_a;
        ptrArrayOfPointers_a = NULL;
    }

    ptrNewArrayOfPointers[index] = ptrAddObject;
    ptrArrayOfPointers_a = ptrNewArrayOfPointers;
    ++dimension_a;
    return;
}

template<class Type>
inline
void
ArrayOfPointers<Type>::AddElement(int index,
                                  Type *ptrNewObject)
{
    if ((index < 0) || (index >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddElement(int, Type*&)",
                                            dimension_a, index);
    }

    ptrArrayOfPointers_a[index] = ptrNewObject;
    return;
}

template<class Type>
inline
void
ArrayOfPointers<Type>::SwitchElements(int index1, int index2)
{
    if ((index1 < 0) || (index1 >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "SwitchElements(int, int)",
                                            dimension_a, index1);
    }

    if ((index2 < 0) || (index2 >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "SwitchElements(int, int)",
                                            dimension_a, index2);
    }

    Type *ptrTemp = ptrArrayOfPointers_a[index1];
    ptrArrayOfPointers_a[index1] = ptrArrayOfPointers_a[index2];
    ptrArrayOfPointers_a[index2] = ptrTemp;
}

template<class Type>
inline
string
ArrayOfPointers<Type>::GetClassName() const
{
    int          startSubString = 0;
    int          durationSubString = 0;
    bool          pointer = false;
    char          character = ' ';
    string        typeName = typeid(Type).name();
    string        className = ARRAY_OF_POINTERS_H;
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
void
ArrayOfPointers<Type>::RemoveAll()
{
    int index = 0;

    if (ptrArrayOfPointers_a != NULL) {
        while (index < dimension_a) {
            ptrArrayOfPointers_a[index] = NULL;
            ++index;
        }
    }

    return;
}

template<class Type>
inline
void
ArrayOfPointers<Type>::Delete()
{
    int index = 0;

    if (ptrArrayOfPointers_a != NULL) {
        while (index < dimension_a) {
            delete ptrArrayOfPointers_a[index];
            ptrArrayOfPointers_a[index] = NULL;
            ++index;
        }

        delete [] ptrArrayOfPointers_a;
        ptrArrayOfPointers_a = NULL;
    }

    dimension_a = 0;
    return;
}

template<class Type>
inline
ArrayOfPointers<Type>::~ArrayOfPointers()
{
    if (ptrArrayOfPointers_a != NULL) {
        delete [] ptrArrayOfPointers_a;
        ptrArrayOfPointers_a = NULL;
    }

    dimension_a = 0;
    return;
}

#endif
