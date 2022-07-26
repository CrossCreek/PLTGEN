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
 * Array2DOfLinkLists.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ARRAY_2D_OF_LINK_LISTS_H
#define ARRAY_2D_OF_LINK_LISTS_H "Array2DOfLinkLists"

#include "ArrayOfLinkLists.h"

using namespace std;

//------------------------------------------------------------------------------
// Class Attribute Descriptions
//
// Name                       Description (units)
// ------------------         --------------------------------------------------
// dimension_a                number of elements in the array
// ptr2DArrayOfLinkLists_a**  'C-style' array of pointers to ArrayOfLinkLists
//                              template class(es)
//
//------------------------------------------------------------------------------

template<class Type>
class Array2DOfLinkLists
{
public:

    inline           Array2DOfLinkLists();
    inline           Array2DOfLinkLists(int firstDimension);
    inline           Array2DOfLinkLists(int firstDimension,
                                        int secondDimension);
    inline           Array2DOfLinkLists(const Array2DOfLinkLists<Type> &copyMe);
    inline virtual  ~Array2DOfLinkLists();

    inline Array2DOfLinkLists<Type>& operator =  (const Array2DOfLinkLists<Type> &copyMe);
    inline bool                      operator == (const Array2DOfLinkLists<Type> &compareMe);
    inline ArrayOfLinkLists<Type>*   operator [] (int firstDimension);

    inline void        AddObject(Type *ptrNewValue,
                                 int firstDimension,
                                 int secondDimension,
                                 int sortValue = -1,
                                 ORDERING ordering = ASCENDING,
                                 string listDesignator = "Undefined");
    inline void        AddUniqueObject(Type *ptrNewValue,
                                       int firstDimension,
                                       int secondDimension,
                                       int sortValue = -1,
                                       ORDERING ordering = ASCENDING,
                                       string listDesignator = "Undefined");
    inline void        Delete();
    inline void        ReplaceObjects(int firstDimension,
                                      int secondDimension,
                                      const LinkList<Type> *ptrLinkList);

    inline const int  Size() const;
    inline const int  GetSize(int firstDimension) const;
    inline const int  GetTotalSize() const;
    inline const int  GetListSize(int firstDimension,
                                  int secondDimension) const;

    inline string      GetClassName() const;

    inline Type*       GetObjectAt(int firstDimension,
                                   int secondDimension,
                                   int listIndex) const;

    inline const LinkList<Type>* LinkListAt(int firstDimension,
                                            int secondDimension) const;

protected:

private:

    int                     dimension_a;
    ArrayOfLinkLists<Type> **ptr2DArrayOfLinkLists_a;
};

template<class Type>
inline
Array2DOfLinkLists<Type>::Array2DOfLinkLists()
    : dimension_a(0), ptr2DArrayOfLinkLists_a(NULL)
{
    return;
}

template<class Type>
inline
Array2DOfLinkLists<Type>::Array2DOfLinkLists(int firstDimension)
    : dimension_a(firstDimension), ptr2DArrayOfLinkLists_a(NULL)
{
    int element = 0;

    if (firstDimension <= 0) {
        throw new InputException(GetClassName(), "Array2DOfLinkLists(int)",
                                 "Entered zero or negative number for initial size");
    }

    ptr2DArrayOfLinkLists_a = new ArrayOfLinkLists<Type> * [firstDimension];

    while (element < firstDimension) {
        ptr2DArrayOfLinkLists_a[element] = NULL;
        ++element;
    }

    return;
}

template<class Type>
inline
Array2DOfLinkLists<Type>::Array2DOfLinkLists(int firstDimension,
        int secondDimension)
    : dimension_a(firstDimension), ptr2DArrayOfLinkLists_a(NULL)
{
    int element = 0;

    if ((firstDimension <= 0) || (secondDimension <= 0)) {
        throw new InputException(GetClassName(),
                                 "Array2DOfLinkLists(int, int)",
                                 "Entered zero or negative number for initial size");
    }

    ptr2DArrayOfLinkLists_a = new ArrayOfLinkLists<Type> * [firstDimension];

    while (element < firstDimension) {
        ptr2DArrayOfLinkLists_a[element] = new ArrayOfLinkLists<Type>(secondDimension);
        ++element;
    }

    return;
}

template<class Type>
inline
Array2DOfLinkLists<Type>::Array2DOfLinkLists(const Array2DOfLinkLists<Type> &copyMe)
    : dimension_a(copyMe.dimension_a), ptr2DArrayOfLinkLists_a(NULL)
{
    int element = 0;

    if (copyMe.ptr2DArrayOfLinkLists_a != NULL) {
        ptr2DArrayOfLinkLists_a = new ArrayOfLinkLists<Type> * [dimension_a];

        while (element < dimension_a) {
            if (copyMe.LinkListAt(element) != NULL) {
                ptr2DArrayOfLinkLists_a[element] = new ArrayOfLinkLists<Type>(*(copyMe.LinkListAt(element)));
            } else {
                ptr2DArrayOfLinkLists_a[element] = NULL;
            }

            ++element;
        }
    }

    return;
}

template<class Type>
inline
Array2DOfLinkLists<Type>&
Array2DOfLinkLists<Type>::operator = (const Array2DOfLinkLists<Type> &copyMe)
{
    dimension_a             = copyMe.dimension_a;
    ptr2DArrayOfLinkLists_a = copyMe.ptr2DArrayOfLinkLists_a;
    return(*this);
}

template<class Type>
inline
bool
Array2DOfLinkLists<Type>::operator == (const Array2DOfLinkLists<Type> &compareMe)
{
    int element = 0;
    bool equal = false;

    if (dimension_a == compareMe.dimension_a) {
        equal = true;

        while ((element < dimension_a) && (equal == true)) {
            if ( (ptr2DArrayOfLinkLists_a[element] != NULL) &&
                    (compareMe.LinkListAt(element) != NULL) ) {
                if (*(ptr2DArrayOfLinkLists_a[element]) != *(compareMe.LinkListAt(element))) {
                    equal = false;
                }
            } else {
                equal = false;
            }
        }
    }

    return(equal);
}

template<class Type>
inline
ArrayOfLinkLists<Type>*
Array2DOfLinkLists<Type>::operator [] (int firstDimension)
{
    if ((firstDimension < 0) || (firstDimension >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "operator [] (int)",
                                            dimension_a, firstDimension);
    }

    return(ptr2DArrayOfLinkLists_a[firstDimension]);
}

template<class Type>
inline
void
Array2DOfLinkLists<Type>::AddObject(Type *ptrNewValue,
                                    int firstDimension,
                                    int secondDimension,
                                    int sortValue,
                                    ORDERING ordering,
                                    string listDesignator)
{
    if ((firstDimension < 0) || (firstDimension >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddObject(Type*, int, int, int)",
                                            dimension_a, firstDimension);
    }

    if (ptr2DArrayOfLinkLists_a[firstDimension] == NULL) {
        ptr2DArrayOfLinkLists_a[firstDimension] = new ArrayOfLinkLists<Type>();
    }

    try {
        ptr2DArrayOfLinkLists_a[firstDimension]->AddObject(ptrNewValue, secondDimension,
                sortValue, ordering, listDesignator);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "AddObject(Type*, int, int, ...)");
        throw ptrError;
    }

    return;
}

template<class Type>
inline
void
Array2DOfLinkLists<Type>::AddUniqueObject(Type *ptrNewValue,
        int firstDimension,
        int secondDimension,
        int sortValue,
        ORDERING ordering,
        string listDesignator)
{
    if ((firstDimension < 0) || (firstDimension >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddUniqueObject(Type*, int, int)",
                                            dimension_a, firstDimension);
    }

    if (ptr2DArrayOfLinkLists_a[firstDimension] == NULL) {
        ptr2DArrayOfLinkLists_a[firstDimension] = new ArrayOfLinkLists<Type>();
    }

    ptr2DArrayOfLinkLists_a[firstDimension]->AddUniqueObject(ptrNewValue, secondDimension,
            sortValue, ordering, listDesignator);
    return;
}

template<class Type>
inline
void
Array2DOfLinkLists<Type>::ReplaceObjects(int firstDimension,
        int secondDimension,
        const LinkList<Type> *ptrLinkList)
{
    Type *ptrObject = NULL;

    if ((firstDimension < 0) || (firstDimension >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "ReplaceObjects(int, int, const LinkList<Type>*)",
                                            dimension_a, firstDimension);
    }

    if (ptr2DArrayOfLinkLists_a[firstDimension] == NULL) {
        ptr2DArrayOfLinkLists_a[firstDimension] = new ArrayOfLinkLists<Type>();
    }

    ptr2DArrayOfLinkLists_a[firstDimension]->ReplaceObjects(secondDimension, ptrLinkList);
    return;
}

template<class Type>
inline
const int
Array2DOfLinkLists<Type>::Size() const
{
    return(dimension_a);
}

template<class Type>
inline
const int
Array2DOfLinkLists<Type>::GetSize(int firstDimension) const
{
    int linkListSize = 0;

    if (ptr2DArrayOfLinkLists_a[firstDimension] != NULL) {
        linkListSize = ptr2DArrayOfLinkLists_a[firstDimension]->Size();
    }

    return(linkListSize);
}

template<class Type>
inline
const int
Array2DOfLinkLists<Type>::GetTotalSize() const
{
    int firstDimension = 0;
    int totalSize = 0;

    while (firstDimension < dimension_a) {
        if (ptr2DArrayOfLinkLists_a[firstDimension] != NULL) {
            totalSize += ptr2DArrayOfLinkLists_a[firstDimension]->Size();
        }

        ++firstDimension;
    }

    return(totalSize);
}

template<class Type>
inline
const int
Array2DOfLinkLists<Type>::GetListSize(int firstDimension,
                                      int secondDimension) const
{
    int listSize = 0;

    if ((firstDimension < 0) || (firstDimension >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "GetListSize(int, int)",
                                            dimension_a, firstDimension);
    }

    if (ptr2DArrayOfLinkLists_a[firstDimension] != NULL) {
        listSize = ptr2DArrayOfLinkLists_a[firstDimension]->GetListSize(secondDimension);
    }

    return(listSize);
}

template<class Type>
inline
string
Array2DOfLinkLists<Type>::GetClassName() const
{
    string className = ARRAY_2D_OF_LINK_LISTS_H;
    className += "<";
    className += typeid(Type).name();
    className += ">";
    return(className);
}

template<class Type>
inline
Type*
Array2DOfLinkLists<Type>::GetObjectAt(int firstDimension,
                                      int secondDimension,
                                      int listIndex) const
{
    Type *ptrObjectAt = NULL;

    if ((firstDimension < 0) || (firstDimension >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "GetObjectAt(int, int, int)",
                                            dimension_a, firstDimension);
    }

    if (ptr2DArrayOfLinkLists_a[firstDimension] != NULL) {
        try {
            ptrObjectAt = ptr2DArrayOfLinkLists_a[firstDimension]->GetObjectAt(secondDimension, listIndex);
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(),
                                "GetObjectAt(int, int, int)");
            throw ptrError;
        }
    }

    return(ptrObjectAt);
}

template<class Type>
inline
const LinkList<Type>*
Array2DOfLinkLists<Type>::LinkListAt(int firstDimension,
                                     int secondDimension) const
{
    const LinkList<Type> *ptrLinkListAt = NULL;

    if ((firstDimension < 0) || (firstDimension >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "LinkListAt(int, int)",
                                            dimension_a, firstDimension);
    }

    if (ptr2DArrayOfLinkLists_a[firstDimension] != NULL) {
        try {
            ptrLinkListAt = ptr2DArrayOfLinkLists_a[firstDimension]->LinkListAt(secondDimension);
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(),
                                "LinkListAt(int, int)");
            throw ptrError;
        }
    }

    return(ptrLinkListAt);
}

template<class Type>
inline
void
Array2DOfLinkLists<Type>::Delete()
{
    int firstDimension = 0;

    if (ptr2DArrayOfLinkLists_a != NULL) {
        while (firstDimension < dimension_a) {
            if (ptr2DArrayOfLinkLists_a[firstDimension] != NULL) {
                ptr2DArrayOfLinkLists_a[firstDimension]->Delete();
                delete ptr2DArrayOfLinkLists_a[firstDimension];
                ptr2DArrayOfLinkLists_a[firstDimension] = NULL;
            }

            ++firstDimension;
        }

        delete [] ptr2DArrayOfLinkLists_a;
        ptr2DArrayOfLinkLists_a = NULL;
    }

    dimension_a = 0;
    return;
}

template<class Type>
inline
Array2DOfLinkLists<Type>::~Array2DOfLinkLists()
{
    int firstDimension = 0;

    if (ptr2DArrayOfLinkLists_a != NULL) {
        while (firstDimension < dimension_a) {
            if (ptr2DArrayOfLinkLists_a[firstDimension] != NULL) {
                delete ptr2DArrayOfLinkLists_a[firstDimension];
                ptr2DArrayOfLinkLists_a[firstDimension] = NULL;
            }

            ++firstDimension;
        }

        delete [] ptr2DArrayOfLinkLists_a;
        ptr2DArrayOfLinkLists_a = NULL;
    }

    dimension_a = 0;
    return;
}

#endif
