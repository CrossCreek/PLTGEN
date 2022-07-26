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
 * LinkList.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef LINK_LIST_H
#define LINK_LIST_H "LinkList"

#include <iostream>
#include <string>
#include <typeinfo>

#include "general/exception/NullPointerException.h"
#include "general/exception/IndexOutOfBoundsException.h"

using namespace std;

//------------------------------------------------------------------------------
// Class Attribute Descriptions
//
// Name                  Description (units)
// ------------------    -------------------------------------------------------
// ordering_a            the order in which new elements are to be added
// size_a                number of elements on the current list
// maximumSize_a         the most elements that were ever on the list
// listDesignator_a          string containing the identification tag for this list
// ptrFirstElement_a*    pointer to first element in the list
// ptrLastElement_a*     pointer to last element in the list
//
//------------------------------------------------------------------------------

enum ORDERING {
    ASCENDING,
    DESCENDING,
    FIRST_IN_FIRST_OUT,
    LAST_IN_FIRST_OUT
};

template<class Type> struct ELEMENT {
    int     sortValue;
    Type    *ptrObject;
    ELEMENT *ptrNext;
};

template<class Type>
class LinkList
{
public:

    inline                 LinkList(ORDERING ordering = ASCENDING,
                                    string listName = "Undefined");
    inline                 LinkList(const LinkList<Type> &copyMe);
    inline virtual         ~LinkList();

    inline LinkList<Type>& operator =  (const LinkList<Type> &copyMe);
    inline bool            operator == (const LinkList<Type> &compareMe);
    inline Type&           operator [] (int index);

    inline void           Append(Type *ptrNewObject,
                                 int  value = -1);
    inline void           AppendUnique(Type *ptrNewObject,
                                       int  value = -1);
    inline void           Delete();
    inline void           Print(ostream &outputStream) const;
    inline void           RemoveAll();
    inline void           SetOrdering(const ORDERING &ordering);

    inline int           Size() const;
    inline int           GetMaximumSize() const;

    inline bool           ContainsObject(const Type *ptrObject) const;

    inline const string   GetListDesignator() const;

    inline Type*          GetFirstObject() const;
    inline Type*          GetLastObject() const;
    inline Type*          GetObjectAt(int index) const;
    inline Type*          Retrieve();
    inline Type*          RetrieveBack();

    inline ELEMENT<Type>* GetFirstElement() const;
    inline ELEMENT<Type>* GetLastElement() const;

protected:

private:

    inline string  GetClassName() const;

    int          ordering_a;
    int           size_a;
    int           maximumSize_a;
    string         listDesignator_a;
    ELEMENT<Type> *ptrFirstElement_a;
    ELEMENT<Type> *ptrLastElement_a;
};


template<class Type>
inline
LinkList<Type>::LinkList(ORDERING ordering,
                         string listName)
    : ordering_a(ordering),  size_a(0), maximumSize_a(0),
      listDesignator_a(listName), ptrFirstElement_a(NULL), ptrLastElement_a(NULL)
{
    return;
}

template<class Type>
inline
LinkList<Type>::LinkList(const LinkList<Type> &copyMe)
    : ordering_a(copyMe.ordering_a),  size_a(copyMe.size_a),
      maximumSize_a(copyMe.maximumSize_a), listDesignator_a(copyMe.listDesignator_a),
      ptrFirstElement_a(NULL), ptrLastElement_a(NULL)
{
    ELEMENT<Type> *ptrNewElement = NULL;
    ELEMENT<Type> *ptrPreviousElement = NULL;
    ELEMENT<Type> *ptrCopyElement = copyMe.GetFirstElement();

    while (ptrCopyElement != NULL) {
        ptrNewElement = new ELEMENT<Type>;
        ptrNewElement->sortValue = ptrCopyElement->sortValue;
        ptrNewElement->ptrObject = new Type(*(ptrCopyElement->ptrObject));
        ptrNewElement->ptrNext   = NULL;

        if (ptrPreviousElement == NULL) {
            ptrFirstElement_a  = ptrNewElement;
        } else {
            ptrPreviousElement->ptrNext = ptrNewElement;
        }

        ptrPreviousElement = ptrNewElement;
        ptrCopyElement = ptrCopyElement->ptrNext;

        if (ptrCopyElement == NULL) {
            ptrLastElement_a = ptrNewElement;
        }
    }

    return;
}

template<class Type>
inline
LinkList<Type>&
LinkList<Type>::operator = (const LinkList<Type> &copyMe)
{
    return(*this);
}

template<class Type>
inline
bool
LinkList<Type>::operator == (const LinkList<Type> &compareMe)
{
    bool           equal = false;
    ELEMENT<Type> *ptrThisElement = ptrFirstElement_a;
    ELEMENT<Type> *ptrCompareElement = compareMe.GetFirstElement();

    if ( (ordering_a == compareMe.ordering_a) &&
            (size_a == compareMe.size_a) &&
            (listDesignator_a == compareMe.listDesignator_a) ) {
        equal = true;

        while ( (equal == true) &&
                (ptrThisElement != NULL) &&
                (ptrCompareElement != NULL) ) {
            if ( (ptrThisElement->sortValue == ptrCompareElement->sortValue) &&
                    (*(ptrThisElement->ptrObject) == *(ptrCompareElement->ptrObject)) ) {
                ptrThisElement = ptrThisElement->ptrNext;
                ptrCompareElement = ptrCompareElement->ptrNext;
            } else {
                equal = false;
            }
        }
    }

    return(equal);
}

template<class Type>
inline
Type&
LinkList<Type>::operator [] (int arrayElement)
{
    int           element = 0;
    Type          *ptrReturnObject = NULL;
    ELEMENT<Type> *ptrArrayElement = ptrFirstElement_a;

    if ((arrayElement < 0) || (arrayElement >= size_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "operator []",
                                            size_a, arrayElement);
    }

    while ((ptrArrayElement != NULL) && (element < arrayElement)) {
        ptrArrayElement = ptrArrayElement->ptrNext;
        ++element;
    }

    if (ptrArrayElement != NULL) {
        ptrReturnObject = ptrArrayElement->ptrObject;
    }

    if (ptrReturnObject == NULL) {
        throw new NullPointerException(GetClassName(), "operator[] (object not found)");
    }

    return(*ptrReturnObject);
}


template<class Type>
void
LinkList<Type>::Print(ostream &outputStream) const
{
    outputStream << "Name of List:          " << listDesignator_a << NEW_LINE;
    outputStream << "Ordering of List:      ";

    if (ordering_a == ASCENDING) {
        outputStream << "Ascending" << NEW_LINE;
    } else if (ordering_a == DESCENDING) {
        outputStream << "Descending" << NEW_LINE;
    } else if (ordering_a == FIRST_IN_FIRST_OUT) {
        outputStream << "FIFO" << NEW_LINE;
    } else if (ordering_a == LAST_IN_FIRST_OUT) {
        outputStream << "LIFO" << NEW_LINE;
    } else {
        outputStream << "Unknown" << NEW_LINE;
    }

    outputStream << "Current # of Elements: " << size_a << NEW_LINE;
    outputStream << "Maximum # of Elements: " << maximumSize_a << NEW_LINE;
    return;
}

template<class Type>
inline
int
LinkList<Type>::Size() const
{
    return(size_a);
}

template<class Type>
inline
int
LinkList<Type>::GetMaximumSize() const
{
    return(maximumSize_a);
}

template<class Type>
inline
string
LinkList<Type>::GetClassName() const
{
    string className = LINK_LIST_H;
    className += "<";
    className += typeid(Type).name();
    className += ">";
    return(className);
}

template<class Type>
inline
const string
LinkList<Type>::GetListDesignator() const
{
    return(listDesignator_a);
}

template<class Type>
inline
void
LinkList<Type>::SetOrdering(const ORDERING &ordering)
{
    ordering_a = ordering;
    return;
}

template<class Type>
inline
ELEMENT<Type>*
LinkList<Type>::GetFirstElement() const
{
    return(ptrFirstElement_a);
}

template<class Type>
inline
ELEMENT<Type>*
LinkList<Type>::GetLastElement() const
{
    return(ptrLastElement_a);
}

template<class Type>
inline
void
LinkList<Type>::Append(Type *ptrNewObject,
                       int value)
{
    ELEMENT<Type> *ptrPreviousElement = NULL;
    ELEMENT<Type> *ptrSearchElement = ptrFirstElement_a;
    ELEMENT<Type> *ptrNewElement = new ELEMENT<Type>;
    ptrNewElement->sortValue = value;
    ptrNewElement->ptrObject = ptrNewObject;
    ptrNewElement->ptrNext = NULL;

    if (ptrFirstElement_a == NULL) {
        ptrFirstElement_a = ptrNewElement;
        ptrLastElement_a = ptrNewElement;
    } else if ( (ordering_a == FIRST_IN_FIRST_OUT) || (value < 0) ||
                ((ordering_a == ASCENDING) && (value >= ptrLastElement_a->sortValue)) ||
                ((ordering_a == DESCENDING) && (value <= ptrLastElement_a->sortValue)) ) {
        ptrLastElement_a->ptrNext = ptrNewElement;
        ptrLastElement_a = ptrNewElement;
    } else if (ordering_a == LAST_IN_FIRST_OUT) {
        ptrNewElement->ptrNext = ptrFirstElement_a;
        ptrFirstElement_a = ptrNewElement;
    } else {
        while (ptrSearchElement != NULL) {
            if ( ((ordering_a == ASCENDING) && (value < ptrSearchElement->sortValue)) ||
                    ((ordering_a == DESCENDING) && (value > ptrSearchElement->sortValue)) ) {
                ptrNewElement->ptrNext = ptrSearchElement;

                if (ptrPreviousElement != NULL) {
                    ptrPreviousElement->ptrNext = ptrNewElement;
                } else {
                    ptrFirstElement_a = ptrNewElement;
                }

                ptrSearchElement = NULL;
            } else {
                ptrPreviousElement = ptrSearchElement;
                ptrSearchElement = ptrSearchElement->ptrNext;
            }
        }
    }

    ++size_a;
    ++maximumSize_a;
    return;
}

template<class Type>
inline
void
LinkList<Type>::AppendUnique(Type *ptrNewObject,
                             int value)
{
    bool           uniqueObject = true;
    ELEMENT<Type> *ptrSearchElement = ptrFirstElement_a;

    while ((ptrSearchElement != NULL) && (uniqueObject == true)) {
        if (*(ptrSearchElement->ptrObject) == *ptrNewObject) {
            uniqueObject = false;
        } else {
            ptrSearchElement = ptrSearchElement->ptrNext;
        }
    }

    if (uniqueObject == true) {
        Append(ptrNewObject, value);
    }

    return;
}

template<class Type>
inline
bool
LinkList<Type>::ContainsObject(const Type *ptrObject) const
{
    bool           foundObject = false;
    ELEMENT<Type> *ptrElement = ptrFirstElement_a;

    while ((ptrElement != NULL) && (foundObject == false)) {
        if (*(ptrElement->ptrObject) == *ptrObject) {
            foundObject = true;
        } else {
            ptrElement = ptrElement->ptrNext;
        }
    }

    return(foundObject);
}

template<class Type>
inline
Type*
LinkList<Type>::GetFirstObject() const
{
    Type *ptrReturnObject = NULL;

    if (ptrFirstElement_a != NULL) {
        ptrReturnObject = ptrFirstElement_a->ptrObject;
    }

    return(ptrReturnObject);
}

template<class Type>
inline
Type*
LinkList<Type>::GetLastObject() const
{
    Type *ptrReturnObject = NULL;

    if (ptrLastElement_a != NULL) {
        ptrReturnObject = ptrLastElement_a->ptrObject;
    }

    return(ptrReturnObject);
}

template<class Type>
inline
Type*
LinkList<Type>::GetObjectAt(int index) const
{
    int           sortIndex = 0;
    bool           foundObject = false;
    Type          *ptrReturnObject = NULL;
    ELEMENT<Type> *ptrElement = ptrFirstElement_a;

    if ((index < 0) || (index >= size_a)) {
        throw new IndexOutOfBoundsException(GetClassName(), "GetObject(int)",
                                            size_a, index);
    }

    while (sortIndex < index) {
        ptrElement = ptrElement->ptrNext;
        ++sortIndex;
    }

    ptrReturnObject = ptrElement->ptrObject;

    if (ptrReturnObject == NULL) {
        throw new NullPointerException(GetClassName(), "GetObject(int)");
    }

    return(ptrReturnObject);
}

template<class Type>
inline
void
LinkList<Type>::RemoveAll()
{
    ELEMENT<Type> *ptrElement = ptrFirstElement_a;
    ELEMENT<Type> *ptrDeleteElement = NULL;

    while (ptrElement != NULL) {
        ptrDeleteElement = ptrElement;
        ptrElement = ptrElement->ptrNext;
        delete ptrDeleteElement;
        ptrDeleteElement = NULL;
    }

    ptrFirstElement_a = NULL;
    ptrLastElement_a = NULL;
    size_a = 0;
    return;
}

template<class Type>
inline
Type*
LinkList<Type>::Retrieve()
{
    Type          *ptrReturnObject = NULL;
    ELEMENT<Type> *ptrElement = ptrFirstElement_a;

    if (ptrElement != NULL) {
        ptrFirstElement_a = ptrFirstElement_a->ptrNext;
        ptrReturnObject = ptrElement->ptrObject;
        delete ptrElement;

        if (ptrFirstElement_a == NULL) {
            ptrLastElement_a = NULL;
        }

        --size_a;
    }

    return(ptrReturnObject);
}

template<class Type>
inline
Type*
LinkList<Type>::RetrieveBack()
{
    int           index = 0;
    Type          *ptrReturnObject = NULL;
    ELEMENT<Type> *ptrElement = ptrLastElement_a;
    ELEMENT<Type> *ptrNewLastElement = NULL;

    if (ptrElement != NULL) {
        if (size_a > 1) {
            ptrNewLastElement = ptrFirstElement_a;

            while (index < (size_a - 2)) {
                ptrNewLastElement = ptrNewLastElement->ptrNext;
                ++index;
            }
        }

        ptrLastElement_a = ptrNewLastElement;
        ptrReturnObject = ptrElement->ptrObject;
        delete ptrElement;

        if (ptrLastElement_a == NULL) {
            ptrFirstElement_a = NULL;
        } else {
            ptrLastElement_a->ptrNext = NULL;
        }

        --size_a;
    }

    return(ptrReturnObject);
}

template<class Type>
inline
void
LinkList<Type>::Delete()
{
    Type *ptrDeleteObject = NULL;
    
    
    ptrDeleteObject = Retrieve();

    while (ptrDeleteObject != NULL) {
        delete ptrDeleteObject;
        ptrDeleteObject = NULL;
        ptrDeleteObject = Retrieve();
    }

    return;
}

template<class Type>
inline
LinkList<Type>::~LinkList()
{
    return;
}

#endif
