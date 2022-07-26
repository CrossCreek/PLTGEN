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
 * VisibleIQE.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef VISIBLE_IQE_H
#define VISIBLE_IQE_H "VisibleIQE"

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <memory>

#include "IQE.h"

class InputDataElement;

/**
 * Class which represents the visible IQE
 *
 * @author Brian Fowler
 * @date 10/03/11
 */
class VisibleIQE : public IQE
{
public:



    explicit VisibleIQE(shared_ptr<InputDataElement> ptrInputDataElement);


    ~VisibleIQE();

    double CalculateQuality(double elevationAngle,
                            const string &missionType,
                            const SpaceVector &targetPos,
                            const PositionState &ecfVehicleState,
                            bool enhancedAreaMode);

    double GetDebugQualityParameter(const SpaceVector &targetPos,
                                    const PositionState &ecfVehicleState);
protected:

private:




};

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
std::string
VisibleIQE::GetClassName()
{
    return(VISIBLE_IQE_H);
}
*/

#endif
