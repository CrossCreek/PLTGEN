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
 * JourneyExtractor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#ifndef JOURNEY_EXTRACTOR_H
#define JOURNEY_EXTRACTOR_H "JourneyExtractor"

#include <fstream>
#include <vector>
//RAGUSA 7/24/2017 - Needed to resolve shared_ptr removal from std library.
#include <memory>
#include "general/array/LinkList.h"
#include "general/utility/Constant.h"

using namespace std;

class InputDataElement;
class Revolution;
class SpaceVector;
class PositionState;
class Journey;
class Orbit;
class Trek;

/**
 * Class which extracts the Journey information from an
 * input data element
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class JourneyExtractor
{
public:

    JourneyExtractor(const string &opsDesignator = "",
                     bool dataCollector = false);

    virtual ~JourneyExtractor();

    /**
     * Exposed method to extract the journey (orbit or trek)
     *
     * @param ptrInputDataElement the inputDataElement to extract
     */
    shared_ptr<Journey> ExtractJourney(const shared_ptr<InputDataElement> &ptrInputDataElement);

protected:

private:

    /**
     * Extracts the Orbit
     */
    shared_ptr<Orbit>   ExtractOrbitInfo(const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Extracts the Trek (does not retrieve trek nodes anymore)
     */
    shared_ptr<Trek>    ExtractTrekInfo(const shared_ptr<InputDataElement> &ptrInputDataElement);

    static inline string GetClassName();

    bool dataCollector_a;
    string opsDesignator_a;
};

inline
string
JourneyExtractor::GetClassName()
{
    return(JOURNEY_EXTRACTOR_H);
}

#endif
