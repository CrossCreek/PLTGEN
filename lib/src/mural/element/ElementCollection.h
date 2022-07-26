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
 * ElementCollection.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ELEMENT_COLLECTION_H
#define ELEMENT_COLLECTION_H "ElementCollection"

#include <memory>
#include <string>
#include <vector>

#include "mural/MURALTypes.h"

class Element;

using namespace std;

/**
 * Contains a collection of elements to that are part of a specified group, system, or type.
 * It has a string to represent the group/system/type name and a list of elements (users/relays/ground)
 * that are a part of that group.
 *
 * @author Brian Fowler
 * @date 11/09/11
 */
class ElementCollection
{
public:



    explicit ElementCollection(const string &designator);

    virtual ~ElementCollection();

    /**
     * Gets the designator of the element group
     *
     * @return designator
     */
    string GetCollectionDesignator() const;

    /**
     * Adds an element to the collection
     *
     * @param ptrElement the element to add
     */
    void AddElement(shared_ptr<Element> ptrElement);

    /**
     * Removes an element from the collection
     *
     * @param ptrElement the element to remove
     */
    void RemoveElement(shared_ptr<Element> ptrElement);

    /**
     * Gets the element vector
     *
     * @return ElementVector
     */
    shared_ptr<MURALTypes::ElementVector> GetElementVector();

    /**
     * Gets the element at the position in the vector
     *
     * @int index the position in the ElementVector
     * @return Element
     */
    shared_ptr<Element> GetElement(int index);

    /**
     * Gets the number of elements in the collection
     *
     * @return int
     */

    int Size();

protected:

    string                                collectionDesignator_a;
    shared_ptr<MURALTypes::ElementVector> ptrElements_a;

private:



};

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
ElementCollection::GetClassName()
{
    return(ELEMENT_COLLECTION_H);
}
*/

#endif
