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
 * MultipleRequirement.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "MultipleRequirement.h"

MultipleRequirement::MultipleRequirement()
    : requirementIndex_a(-1), ptrVariableArray_a(NULL), timeToCollect_a(0.0),
      totalUpperBound_a(0.0)
{
    return;
}

MultipleRequirement::MultipleRequirement(int requirementIndex,
        int variableIndex,
        double timeToCollect,
        double upperBound)
    : requirementIndex_a(requirementIndex),
      ptrVariableArray_a(new Array<int>(1, variableIndex)),
      timeToCollect_a(timeToCollect), totalUpperBound_a(upperBound)
{
    return;
}

MultipleRequirement::MultipleRequirement(const MultipleRequirement &copyMe)
    : requirementIndex_a(copyMe.requirementIndex_a),
      ptrVariableArray_a(new Array<int>(*(copyMe.ptrVariableArray_a))),
      timeToCollect_a(copyMe.timeToCollect_a)
{
    return;
}

MultipleRequirement&
MultipleRequirement::operator = (const MultipleRequirement &copyMe)
{
    requirementIndex_a = requirementIndex_a;
    ptrVariableArray_a = new Array<int>(*(copyMe.ptrVariableArray_a));
    timeToCollect_a    = copyMe.timeToCollect_a;
    return(*this);
}

bool
MultipleRequirement::operator == (const MultipleRequirement &compareMe)
{
    return( (requirementIndex_a == compareMe.requirementIndex_a) &&
            (timeToCollect_a == compareMe.timeToCollect_a) );
}

bool
MultipleRequirement::operator != (const MultipleRequirement &compareMe)
{
    return(!(operator == (compareMe)));
}

MultipleRequirement::~MultipleRequirement()
{
    if (ptrVariableArray_a != NULL) {
        delete ptrVariableArray_a;
        ptrVariableArray_a = NULL;
    }

    return;
}
