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
 * ArrayOfLinkLists.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ARRAY_OF_LINK_LISTS_H
#define ARRAY_OF_LINK_LISTS_H "ArrayOfLinkLists"

#include "ListIterator.h"

#include "general/exception/InputException.h"

using namespace std;

//------------------------------------------------------------------------------
// Class Attribute Descriptions
//
// Name                     Description (units)
// ------------------       ----------------------------------------------------
// dimension_a              number of elements in the array
// ptrArrayOfLinkLists_a**  'C-style' array of pointers to a LinkList template
//                            class
//
//------------------------------------------------------------------------------

template<class Type>
class ArrayOfLinkLists
{
public:

    inline           ArrayOfLinkLists();
    inline           ArrayOfLinkLists(int initialSize);
    inline           ArrayOfLinkLists(const LinkList<Type> **ptrCStyleArray,
                                      int arraySize);
    inline           ArrayOfLinkLists(const ArrayOfLinkLists<Type> &copyMe);
    inline virtual  ~ArrayOfLinkLists();

    inline ArrayOfLinkLists<Type>& operator =  (const ArrayOfLinkLists<Type> &copyMe);
    inline bool            operator == (const ArrayOfLinkLists<Type> &compareMe);
    inline LinkList<Type>* operator [] (int index);

    inline void         AddLinkList(LinkList<Type> *&ptrLinkList);
    inline void         AddLinkList(LinkList<Type> *&ptrLinkList,
                                    int index);
    inline void         AddObject(Type *ptrNewValue,
                                  int index,
                                  int sortValue = -1,
                                  ORDERING ordering = ASCENDING,
                                  string listDesignator = "Undefined");
    inline void         AddUniqueObject(Type *ptrNewValue,
                                        int index,
                                        int sortValue = -1,
                                        ORDERING ordering = ASCENDING,
                                        string listDesignator = "Undefined");
    inline void         Delete();
    inline void         RemoveAll();
    inline void         RemoveAll(int arrayIndex);
    inline void         ReplaceObjects(int arrayIndex,
                                       const LinkList<Type> *ptrLinkList);

    inline const int   Size() const;
    inline const int   GetTotalSize() const;
    inline const int   GetListSize(int arrayIndex) const;

    inline string       GetClassName() const;
    inline const string GetListDesignator(int arrayIndex) const;

    inline Type*        GetObjectAt(int arrayIndex,
                                    int listIndex) const;
    inline Type*        Retrieve(int arrayIndex);

    inline const Type* GetFirstObject(int arrayIndex) const;

    inline const LinkList<Type>* LinkListAt(int arrayIndex) const;

protected:

private:

    int             dimension_a;
    LinkList<Type> **ptrArrayOfLinkLists_a;
};

template<class Type>
inline
ArrayOfLinkLists<Type>::ArrayOfLinkLists()
    : dimension_a(0), ptrArrayOfLinkLists_a(NULL)
{
    return;
}

template<class Type>
inline
ArrayOfLinkLists<Type>::ArrayOfLinkLists(int initialSize)
    : dimension_a(initialSize), ptrArrayOfLinkLists_a(NULL)
{
    int index = 0;

    if (initialSize <= 0) {
        throw new InputException(GetClassName(), "ArrayOfLinkLists(int)",
                                 "Entered zero or negative number for initial size");
    }

    ptrArrayOfLinkLists_a = new LinkList<Type> * [initialSize];

    while (index < initialSize) {
        ptrArrayOfLinkLists_a[index] = NULL;
        ++index;
    }

    return;
}

template<class Type>
inline
ArrayOfLinkLists<Type>::ArrayOfLinkLists(const LinkList<Type> **ptrCStyleArray,
        int arraySize)
    : dimension_a(arraySize), ptrArrayOfLinkLists_a(NULL)
{
    int index = 0;

    if (arraySize <= 0) {
        throw new InputException(GetClassName(),
                                 "ArrayOfLinkLists(const LinkList<Type>**, int)",
                                 "Entered zero or negative number for initial size");
    }

    ptrArrayOfLinkLists_a = new LinkList<Type> * [arraySize];

    while (index < arraySize) {
        if (ptrCStyleArray[index] != NULL) {
            ptrArrayOfLinkLists_a[index]
                = new LinkList<Type>(*(ptrCStyleArray[index]));
        }

        ++index;
    }

    return;
}

template<class Type>
inline
ArrayOfLinkLists<Type>::ArrayOfLinkLists(const ArrayOfLinkLists<Type> &copyMe)
    : dimension_a(copyMe.dimension_a), ptrArrayOfLinkLists_a(NULL)
{
    int index = 0;

    if (copyMe.ptrArrayOfLinkLists_a != NULL) {
        ptrArrayOfLinkLists_a = new LinkList<Type> * [dimension_a];

        while (index < dimension_a) {
            if (copyMe.ptrArrayOfLinkLists_a[index] != NULL) {
                ptrArrayOfLinkLists_a[index]
                    = new LinkList<Type>(*(copyMe.LinkListAt(index)));
            }

            ++index;
        }
    }

    return;
}

template<class Type>
inline
ArrayOfLinkLists<Type>&
ArrayOfLinkLists<Type>::operator = (const ArrayOfLinkLists<Type> &copyMe)
{
    dimension_a           = copyMe.dimension_a;
    ptrArrayOfLinkLists_a = copyMe.ptrArrayOfLinkLists_a;
    return(*this);
}

template<class Type>
inline
bool
ArrayOfLinkLists<Type>::operator == (const ArrayOfLinkLists<Type> &compareMe)
{
    int arrayIndex = 0;
    int listSize = 0;
    int listIndex = 0;
    bool equal = false;

    if (dimension_a == compareMe.dimension_a) {
        equal = true;

        while ((arrayIndex < dimension_a) && (equal == true)) {
            listSize = GetListSize(arrayIndex);

            if (listSize != compareMe.GetListSize(arrayIndex)) {
                equal = false;
            } else {
                listIndex = 0;

                while ((listIndex < listSize) && (equal == true)) {
                    if (*(GetObjectAt(arrayIndex, listIndex))
                            != *(compareMe.GetObjectAt(arrayIndex, listIndex))) {
                        equal = false;
                    } else {
                        ++listIndex;
                    }
                }

                ++arrayIndex;
            }
        }
    }

    return(equal);
}

template<class Type>
inline
LinkList<Type>*
ArrayOfLinkLists<Type>::operator [] (int index)
{
    if ((index < 0) || (index >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "operator [] (int)",
                                            dimension_a, index);
    }

    return(ptrArrayOfLinkLists_a[index]);
}

template<class Type>
inline
void
ArrayOfLinkLists<Type>::AddLinkList(LinkList<Type> *&ptrLinkList)
{
    int             index = 0;
    LinkList<Type> **ptrNewArrayOfLinkLists = NULL;

    if (ptrArrayOfLinkLists_a == NULL) {
        dimension_a = 1;
        ptrArrayOfLinkLists_a = new LinkList<Type> * [dimension_a];
    } else {
        ptrNewArrayOfLinkLists = new LinkList<Type> * [dimension_a + 1];

        while (index < dimension_a) {
            ptrNewArrayOfLinkLists[index] = ptrArrayOfLinkLists_a[index];
            ++index;
        }

        ++dimension_a;
        delete ptrArrayOfLinkLists_a;
        ptrArrayOfLinkLists_a = ptrNewArrayOfLinkLists;
    }

    ptrArrayOfLinkLists_a[index] = ptrLinkList;
    return;
}


template<class Type>
inline
void
ArrayOfLinkLists<Type>::AddLinkList(LinkList<Type> *&ptrLinkList,
                                    int arrayIndex)
{
    int             index = 0;
    int             newDimension = 0;
    LinkList<Type> **ptrNewArrayOfLinkLists = NULL;

    if (arrayIndex < 0) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddLinkList(LinkList<Type>*&, int)",
                                            dimension_a, arrayIndex);
    }

    if (ptrArrayOfLinkLists_a == NULL) {
        dimension_a = arrayIndex + 1;
        ptrArrayOfLinkLists_a = new LinkList<Type> * [dimension_a];

        while (index < arrayIndex) {
            ptrArrayOfLinkLists_a[index] = NULL;
            ++index;
        }
    } else if (arrayIndex < dimension_a) {
        if (ptrArrayOfLinkLists_a[arrayIndex] != NULL) {
            ptrArrayOfLinkLists_a[arrayIndex]->Delete();
            delete ptrArrayOfLinkLists_a[arrayIndex];
        }
    } else {
        newDimension = arrayIndex + 1;
        ptrNewArrayOfLinkLists = new LinkList<Type> * [newDimension];

        while (index < dimension_a) {
            ptrNewArrayOfLinkLists[index] = ptrArrayOfLinkLists_a[index];
            ++index;
        }

        while (index < newDimension) {
            ptrNewArrayOfLinkLists[index] = NULL;
            ++index;
        }

        dimension_a = newDimension;
        delete ptrArrayOfLinkLists_a;
        ptrArrayOfLinkLists_a = ptrNewArrayOfLinkLists;
    }

    ptrArrayOfLinkLists_a[arrayIndex] = ptrLinkList;
    return;
}

template<class Type>
inline
void
ArrayOfLinkLists<Type>::AddObject(Type *ptrNewValue,
                                  int index,
                                  int sortValue,
                                  ORDERING ordering,
                                  string listDesignator)
{
    if ((index < 0) || (index >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddObject(Type*, int, int, string)",
                                            dimension_a, index);
    }

    if (ptrArrayOfLinkLists_a[index] == NULL) {
        ptrArrayOfLinkLists_a[index] = new LinkList<Type>(ordering, listDesignator);
    }

    ptrArrayOfLinkLists_a[index]->Append(ptrNewValue, sortValue);
    return;
}

template<class Type>
inline
void
ArrayOfLinkLists<Type>::AddUniqueObject(Type *ptrNewValue,
                                        int index,
                                        int sortValue,
                                        ORDERING ordering,
                                        string listDesignator)
{
    if ((index < 0) || (index >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "AddUniqueObject(Type*, int, int, string)",
                                            dimension_a, index);
    }

    if (ptrArrayOfLinkLists_a[index] == NULL) {
        ptrArrayOfLinkLists_a[index] = new LinkList<Type>(ordering, listDesignator);
    }

    ptrArrayOfLinkLists_a[index]->AppendUnique(ptrNewValue, sortValue);
    return;
}
template<class Type>
inline
void
ArrayOfLinkLists<Type>::ReplaceObjects(int arrayIndex,
                                       const LinkList<Type> *ptrLinkList)
{
    Type *ptrObject = NULL;

    if ((arrayIndex < 0) || (arrayIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "ReplaceObjects(int, const LinkList<Type>*)",
                                            dimension_a, arrayIndex);
    }

    if (ptrArrayOfLinkLists_a[arrayIndex] == NULL) {
        ptrArrayOfLinkLists_a[arrayIndex] = new LinkList<Type>();
    } else {
        ptrArrayOfLinkLists_a[arrayIndex]->RemoveAll();
    }

    ListIterator<Type> listIter(ptrLinkList);
    ptrObject = listIter.First();

    while (ptrObject != NULL) {
        ptrArrayOfLinkLists_a[arrayIndex]->Append(ptrObject);
        ptrObject = listIter.Next();
    }

    return;
}

template<class Type>
inline
const int
ArrayOfLinkLists<Type>::Size() const
{
    return(dimension_a);
}

template<class Type>
inline
const int
ArrayOfLinkLists<Type>::GetTotalSize() const
{
    int index = 0;
    int totalSize = 0;

    while (index < dimension_a) {
        if (ptrArrayOfLinkLists_a[index] != NULL) {
            totalSize += ptrArrayOfLinkLists_a[index]->Size();
        }

        ++index;
    }

    return(totalSize);
}

template<class Type>
inline
const int
ArrayOfLinkLists<Type>::GetListSize(int arrayIndex) const
{
    int listSize = 0;

    if ((arrayIndex < 0) || (arrayIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "operator [] (int)",
                                            dimension_a, arrayIndex);
    }

    if (ptrArrayOfLinkLists_a[arrayIndex] != NULL) {
        listSize = ptrArrayOfLinkLists_a[arrayIndex]->Size();
    }

    return(listSize);
}

template<class Type>
inline
string
ArrayOfLinkLists<Type>::GetClassName() const
{
    string className = ARRAY_OF_LINK_LISTS_H;
    className += "<";
    className += typeid(Type).name();
    className += ">";
    return(className);
}

template<class Type>
inline
Type*
ArrayOfLinkLists<Type>::GetObjectAt(int arrayIndex,
                                    int listIndex) const
{
    if ((arrayIndex < 0) || (arrayIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "GetObjectAt(int, int)",
                                            dimension_a, arrayIndex);
    }

    if ((listIndex < 0) || (listIndex >= GetListSize(arrayIndex))) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "GetObjectAt(int, int)",
                                            GetListSize(arrayIndex), listIndex);
    }

    return(ptrArrayOfLinkLists_a[arrayIndex]->GetObjectAt(listIndex));
}

template<class Type>
inline
Type*
ArrayOfLinkLists<Type>::Retrieve(int arrayIndex)
{
    Type *ptrRetrieveObject = NULL;

    if ((arrayIndex < 0) || (arrayIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "Retrieve(int)",
                                            dimension_a, arrayIndex);
    }

    if (ptrArrayOfLinkLists_a[arrayIndex] != NULL) {
        ptrRetrieveObject = ptrArrayOfLinkLists_a[arrayIndex]->Retrieve();
    }

    return(ptrRetrieveObject);
}

template<class Type>
inline
const Type*
ArrayOfLinkLists<Type>::GetFirstObject(int arrayIndex) const
{
    const Type *ptrFirstObject = NULL;

    if ((arrayIndex < 0) || (arrayIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "GetFirstObject(int)",
                                            dimension_a, arrayIndex);
    }

    return(ptrArrayOfLinkLists_a[arrayIndex]->GetFirstObject());
}

template<class Type>
inline
const string
ArrayOfLinkLists<Type>::GetListDesignator(int arrayIndex) const
{
    string listDesignator = "Undefined";

    if ((arrayIndex < 0) || (arrayIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "GetListDesignator(int)",
                                            dimension_a, arrayIndex);
    }

    if (ptrArrayOfLinkLists_a[arrayIndex] != NULL) {
        listDesignator = ptrArrayOfLinkLists_a[arrayIndex]->GetListDesignator();
    }

    return(listDesignator);
}

template<class Type>
inline
const LinkList<Type>*
ArrayOfLinkLists<Type>::LinkListAt(int arrayIndex) const
{
    if ((arrayIndex < 0) || (arrayIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "LinkListAt(int)",
                                            dimension_a, arrayIndex);
    }

    return(ptrArrayOfLinkLists_a[arrayIndex]);
}

template<class Type>
inline
void
ArrayOfLinkLists<Type>::RemoveAll()
{
    int index = 0;

    if (ptrArrayOfLinkLists_a != NULL) {
        while (index < dimension_a) {
            if (ptrArrayOfLinkLists_a[index] != NULL) {
                ptrArrayOfLinkLists_a[index]->RemoveAll();
            }

            ++index;
        }
    }

    return;
}

template<class Type>
inline
void
ArrayOfLinkLists<Type>::RemoveAll(int arrayIndex)
{
    if ((arrayIndex < 0) || (arrayIndex >= dimension_a)) {
        throw new IndexOutOfBoundsException(GetClassName(),
                                            "RemoveAll(int)",
                                            dimension_a, arrayIndex);
    }

    if (ptrArrayOfLinkLists_a[arrayIndex] != NULL) {
        ptrArrayOfLinkLists_a[arrayIndex]->RemoveAll();
    }

    return;
}

template<class Type>
inline
void
ArrayOfLinkLists<Type>::Delete()
{
    int index = 0;

    if (ptrArrayOfLinkLists_a != NULL) {
        while (index < dimension_a) {
            if (ptrArrayOfLinkLists_a[index] != NULL) {
                ptrArrayOfLinkLists_a[index]->Delete();
                delete ptrArrayOfLinkLists_a[index];
                ptrArrayOfLinkLists_a[index] = NULL;
            }

            ++index;
        }

        delete [] ptrArrayOfLinkLists_a;
        ptrArrayOfLinkLists_a = NULL;
    }

    dimension_a = 0;
    return;
}

template<class Type>
inline
ArrayOfLinkLists<Type>::~ArrayOfLinkLists()
{
    int index = 0;

    if (ptrArrayOfLinkLists_a != NULL) {
        while (index < dimension_a) {
            if (ptrArrayOfLinkLists_a[index] != NULL) {
                ptrArrayOfLinkLists_a[index]->RemoveAll();
                delete ptrArrayOfLinkLists_a[index];
                ptrArrayOfLinkLists_a[index] = NULL;
            }

            ++index;
        }

        delete [] ptrArrayOfLinkLists_a;
        ptrArrayOfLinkLists_a = NULL;
    }

    dimension_a = 0;
    return;
}

#endif
