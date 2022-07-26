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
 * ElementCollection.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "ElementCollection.h"

#include <algorithm>
#include "general\utility\Utility.h"

ElementCollection::ElementCollection(const string& collectionDesignator) :
    collectionDesignator_a(collectionDesignator), ptrElements_a(NULL)
{
    return;
}

/* ************************************************************************************* */

string
ElementCollection::GetCollectionDesignator() const
{
    return (collectionDesignator_a);
}

/* ************************************************************************************* */

void
ElementCollection::AddElement(shared_ptr<Element> ptrElement)
{
    if ( ptrElements_a == NULL ) {
        ptrElements_a = shared_ptr<MURALTypes::ElementVector>( new MURALTypes::ElementVector() );
    }

    ptrElements_a->push_back(ptrElement);
}

/* ************************************************************************************* */

void
ElementCollection::RemoveElement(shared_ptr<Element> ptrElement)
{
    if ( ptrElements_a != NULL ) {
        MURALTypes::ElementVector::iterator posIter;
        posIter = std::find(ptrElements_a->begin(), ptrElements_a->end(), ptrElement);
        ptrElements_a->erase(posIter);
    }
}

/* ************************************************************************************* */

shared_ptr<MURALTypes::ElementVector>
ElementCollection::GetElementVector()
{
    return ptrElements_a;
}

/* ************************************************************************************* */

shared_ptr<Element>
ElementCollection::GetElement(int index)
{
    return (*ptrElements_a)[index];
}

/* ************************************************************************************* */


int
ElementCollection::Size()
{
    int size = 0;

    if ( ptrElements_a != NULL ) {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        size = Utility::CastSizetToInt(ptrElements_a->size());
    }

    return size;
}

/* ************************************************************************************* */

ElementCollection::~ElementCollection()
{
    return;
}

