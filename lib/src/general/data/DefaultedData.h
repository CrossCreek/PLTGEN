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
 * DefaultedData.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef DEFAULTED_DATA_H
#define DEFAULTED_DATA_H "DefaultedData"

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>

using namespace std;

/**
 * Data class which holds a default value and values based on specific strings.  These strings can
 * represent missions, sensor types, groups, etc.  The default will be used if no specific string
 * value is specified.
 *
 * @author Brian Fowler
 * @date 09/23/2011
 */
template<class Type>
class DefaultedData
{
public:

    inline DefaultedData();
    inline DefaultedData(const Type defaultValue);
    inline DefaultedData(const DefaultedData<Type> &copyMe);
    inline virtual ~DefaultedData();

    /**
     * Adds a specific string and value to the hash map
     *
     * @param key the string for the key
     * @param value the value to add
     */
    inline void AddSpecific(const string &key, Type value);

    /**
     * Method to return the desired value.  If the key
     * is not found the default will be returned
     *
     * @param key the key to look for
     * @return the value (default if the key is not found)
     */
    inline Type GetValue(const string &key) const;

    /**
     * Method to simply return the default value
     *
     * @return the default value
     */
    inline Type GetDefaultValue() const;


    /**
     * Method to check if the spcified mission key is in the map or not
     *
     * @param missionKey the key to check
     * @return bool
     */
    inline bool HasSpecificKey(const string &key) const;

    /**
     * Method to return a vector of all the specific mission tags in the map
     *
     * @return vector of the tags
     */
    inline shared_ptr<std::vector<string>>  GetSpecificTags() const;

    /**
     * Sets the default value
     *
     * @param value the value to set
     */
    inline void   SetDefault(Type value);

protected:

private:

    inline string GetClassName();

    Type                            defaultValue_a;
    shared_ptr<map<string, Type>>   ptrSpecficValueMap_a;
    shared_ptr<vector<string>>      ptrSpecficTags_a;
};

template<class Type>
inline
string
DefaultedData<Type>::GetClassName()
{
    return(DEFAULTED_DATA_H);
}

/* ***************************************************************************** */

template<class Type>
inline
DefaultedData<Type>::DefaultedData()
    : defaultValue_a(NULL),
      ptrSpecficValueMap_a(new std::map<string, Type>()),
      ptrSpecficTags_a(new std::vector<string>())
{
    return;
}

/* ***************************************************************************** */

template<class Type>
inline
DefaultedData<Type>::DefaultedData(const DefaultedData<Type> &copyMe)
{
    defaultValue_a       = *(new Type(copyMe.defaultValue_a));
    ptrSpecficValueMap_a = shared_ptr<map<string, Type>>(new std::map<string, Type>(*copyMe.ptrSpecficValueMap_a));
    ptrSpecficTags_a     = shared_ptr<vector<string>>(new std::vector<string>(*copyMe.ptrSpecficTags_a));
    return;
}

/* ***************************************************************************** */

template<class Type>
inline
DefaultedData<Type>::DefaultedData(const Type defaultValue)
    : defaultValue_a(defaultValue),
      ptrSpecficValueMap_a(new std::map<string, Type>()),
      ptrSpecficTags_a(new std::vector<string>())
{
    return;
}

/* ***************************************************************************** */

template<class Type>
inline
void
DefaultedData<Type>::AddSpecific(const string &key, Type value)
{
    if ( ptrSpecficValueMap_a->find(key) == ptrSpecficValueMap_a->end()) {
        ptrSpecficValueMap_a->insert(std::pair<string, Type>(key, value));
        ptrSpecficTags_a->push_back(key);
    }
}

/* ***************************************************************************** */

template<class Type>
void
DefaultedData<Type>::SetDefault(Type value)
{
    defaultValue_a = value;
}

/* ***************************************************************************** */

template<class Type>
inline
Type
DefaultedData<Type>::GetValue(const string &key) const
{
    Type value = defaultValue_a;

    if ( ptrSpecficValueMap_a->find(key) != ptrSpecficValueMap_a->end()) {
        value = ptrSpecficValueMap_a->find(key)->second;
    }

    return value;
}

/* ***************************************************************************** */

template<class Type>
inline
bool
DefaultedData<Type>::HasSpecificKey(const string &key) const
{
    bool returnValue = false;

    if ( ptrSpecficValueMap_a->find(key) != ptrSpecficValueMap_a->end()) {
        returnValue = true;
    }

    return returnValue;
}

/* ***************************************************************************** */

template<class Type>
inline
Type
DefaultedData<Type>::GetDefaultValue() const
{
    return defaultValue_a;
}

/* ***************************************************************************** */

template<class Type>
inline
shared_ptr<std::vector<string>>
                             DefaultedData<Type>::GetSpecificTags() const
{
    return ptrSpecficTags_a;
}

/* ***************************************************************************** */
template<class Type>
inline
DefaultedData<Type>::~DefaultedData()
{
    return;
}

#endif
