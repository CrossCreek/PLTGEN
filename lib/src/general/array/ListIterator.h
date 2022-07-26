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
 * ListIterator.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef LIST_ITERATOR_H
#define LIST_ITERATOR_H "ListIterator"

#include "LinkList.h"

using namespace std;

//------------------------------------------------------------------------------
// Class Attribute Descriptions
//
// Name                    Description (units)
// -------------------     -----------------------------------------------------
// ptrCurrentElement_a*    pointer to the current element in the list
// ptrCurrentList_a*       pointer to the list being iterated on
//
//------------------------------------------------------------------------------

template<class Type>
class ListIterator
{
public:

    inline          ListIterator();
    inline          ListIterator(const LinkList<Type> *ptrList);
    inline virtual ~ListIterator();

    inline int   GetListSize() const;

    inline Type*  First();
    inline Type*  Last();
    inline Type*  Current();
    inline Type*  Next();
    inline Type*  Previous();
    inline Type*  PeekNext();
    inline Type*  PeekLast();
    inline Type*  Find(int findIndex);

protected:

private:

    inline string GetClassName() const;

    inline Type*  GetObject(const ELEMENT<Type> *ptrElement);

    ELEMENT<Type>        *ptrCurrentElement_a;
    const LinkList<Type> *ptrCurrentList_a;
};

template<class Type>
inline
ListIterator<Type>::ListIterator()
    : ptrCurrentElement_a(NULL), ptrCurrentList_a(NULL)
{
    return;
}

template<class Type>
inline
ListIterator<Type>::ListIterator(const LinkList<Type> *ptrList)
    : ptrCurrentElement_a(NULL), ptrCurrentList_a(ptrList)
{
    return;
}

template<class Type>
inline
Type*
ListIterator<Type>::First()
{
    if (ptrCurrentList_a != NULL) {
        ptrCurrentElement_a = ptrCurrentList_a->GetFirstElement();
    }

    return(GetObject(ptrCurrentElement_a));
}

template<class Type>
inline
Type*
ListIterator<Type>::Last()
{
    if (ptrCurrentList_a != NULL) {
        ptrCurrentElement_a = ptrCurrentList_a->GetLastElement();
    }

    return(GetObject(ptrCurrentElement_a));
}

template<class Type>
inline
Type*
ListIterator<Type>::PeekLast()
{
    Type *ptrLastElement = NULL;

    if (ptrCurrentList_a != NULL) {
        ptrLastElement = ptrCurrentList_a->GetLastElement();
    }

    return(GetObject(ptrLastElement));
}

template<class Type>
inline
Type*
ListIterator<Type>::Current()
{
    return(GetObject(ptrCurrentElement_a));
}

template<class Type>
inline
Type*
ListIterator<Type>::Next()
{
    if (ptrCurrentElement_a != NULL) {
        ptrCurrentElement_a = ptrCurrentElement_a->ptrNext;
    }

    return(GetObject(ptrCurrentElement_a));
}

template<class Type>
inline
Type*
ListIterator<Type>::PeekNext()
{
    Type *ptrReturnObject = NULL;

    if (ptrCurrentElement_a != NULL) {
        ptrReturnObject = GetObject(ptrCurrentElement_a->ptrNext);
    }

    return(ptrReturnObject);
}

template<class Type>
inline
Type*
ListIterator<Type>::Previous()
{
    ELEMENT<Type> *ptrPreviousElement = NULL;

    if ((ptrCurrentElement_a != NULL) && (ptrCurrentList_a != NULL)) {
        if (ptrCurrentElement_a != ptrCurrentList_a->GetFirstElement()) {
            ptrPreviousElement = ptrCurrentList_a->GetFirstElement();

            while (ptrPreviousElement->ptrNext != ptrCurrentElement_a) {
                ptrPreviousElement = ptrPreviousElement->ptrNext;
            }

            ptrCurrentElement_a = ptrPreviousElement;
        }
    }

    return(GetObject(ptrPreviousElement));
}

template<class Type>
inline
Type*
ListIterator<Type>::GetObject(const ELEMENT<Type> *ptrElement)
{
    Type *ptrReturnObject = NULL;

    if (ptrElement != NULL) {
        ptrReturnObject = ptrElement->ptrObject;
    }

    return(ptrReturnObject);
}

template<class Type>
inline
Type*
ListIterator<Type>::Find(int findIndex)
{
    int  index = 0;
    Type *ptrReturnObject = NULL;

    if ((findIndex >= 0) && (findIndex < GetListSize())) {
        ptrCurrentElement_a = ptrCurrentList_a->GetFirstElement();

        while (index < findIndex) {
            ptrCurrentElement_a = ptrCurrentElement_a->ptrNext;
            ++index;
        }

        ptrReturnObject = GetObject(ptrCurrentElement_a);
    }

    return(ptrReturnObject);
}

template<class Type>
inline
int
ListIterator<Type>::GetListSize() const
{
    int listSize = 0;

    if (ptrCurrentList_a != NULL) {
        listSize = ptrCurrentList_a->Size();
    }

    return(listSize);
}

template<class Type>
inline
string
ListIterator<Type>::GetClassName() const
{
    string className = LIST_ITERATOR_H;
    className += "<";
    className += typeid(Type).name();
    className += ">";
    return(className);
}

template<class Type>
inline
ListIterator<Type>::~ListIterator()
{
    ptrCurrentElement_a = NULL;
    ptrCurrentList_a = NULL;
    return;
}

#endif
