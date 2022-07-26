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
 * MultipleRequirement.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef MULTIPLE_REQUIREMENT_H
#define MULTIPLE_REQUIREMENT_H "MultipleRequirement"


#include "general/array/Array.h"
#include "general/array/LinkList.h"
#include "mural/MURALTypes.h"

using namespace std;

class Constraint;

/**
 * Data class which represents a MultipleRequirement.  Used when creating the LP
 * for the requested and allocated MVR process.  Represents the 'Q' cplex indices.
 *
 * Name                           Description (units)
 * -------------                  ----------------------------------------------
 * requirementIndex_a             index assigned to the requirement
 * ptrVariableArray_a*            array of solution variable indices assigned to
 *                                  this requirement (different time steps)
 * timeToCollect_a                remaining time to collect the requirement
 * totalUpperBound_a              total possible collection time for this
 *                                  requirement across the interested time steps
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */
class MultipleRequirement
{
public:

    MultipleRequirement();
    MultipleRequirement(int requirementIndex,
                        int variableIndex,
                        double timeToCollect,
                        double upperBound);
    MultipleRequirement(const MultipleRequirement &copyIndex);
    virtual ~MultipleRequirement();

    MultipleRequirement& operator =  (const MultipleRequirement &copyMe);
    bool                 operator == (const MultipleRequirement &compareMe);
    bool                 operator != (const MultipleRequirement &compareMe);

    inline void  AddVariableIndex(int variableIndex,
                                  double upperBound);

    inline int  GetRequirementIndex() const;
    inline int  GetNumberOfVariables() const;

    inline double GetTimeToCollect() const;

    inline bool  NeedConstraint() const;

protected:

private:
    int                requirementIndex_a;
    Array<int>        *ptrVariableArray_a;
    double              timeToCollect_a;
    double              totalUpperBound_a;
};

inline
void
MultipleRequirement::AddVariableIndex(int variableIndex,
                                      double upperBound)
{
    ptrVariableArray_a->AddElement(variableIndex);
    totalUpperBound_a += upperBound;
}

inline
int
MultipleRequirement::GetRequirementIndex() const
{
    return(requirementIndex_a);
}

inline
int
MultipleRequirement::GetNumberOfVariables() const
{
    return(ptrVariableArray_a->Size());
}

inline
double
MultipleRequirement::GetTimeToCollect() const
{
    return(timeToCollect_a);
}

inline
bool
MultipleRequirement::NeedConstraint() const
{
    return( (ptrVariableArray_a->Size() > 1) &&
            (totalUpperBound_a > timeToCollect_a) );
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
MultipleRequirement::GetClassName()
{
    return(MULTIPLE_REQUIREMENT_H);
}
*/

#endif
