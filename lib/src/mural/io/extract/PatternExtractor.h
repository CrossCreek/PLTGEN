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
 * PatternExtractor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef PATTERN_EXTRACTOR_H
#define PATTERN_EXTRACTOR_H "PatternExtractor"

#include <iostream>
#include <iomanip>
#include "general/array/LinkList.h"
#include "mural/access/AccessPattern.h"

using namespace std;

class InputDataElement;
class ConePattern;
class DonutPattern;
class AxebladePattern;
class ButterflyPattern;

/**
 * Class which extracts the Access pattern information from an
 * input data element
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class PatternExtractor
{
public:

    PatternExtractor();
    virtual ~PatternExtractor();

    /**
     * Exposed method to extract a pattern from an input data element
     *
     * @parse *ptrInputDataElement the input data element to parse
     */
    shared_ptr<AccessPattern> ExtractPattern(const shared_ptr<InputDataElement> &ptrInputDataElement);

protected:

private:

    /**
     * Parse a cone angle data from the input data element (common among the various patterns)
     *
     * @param keyString the string to get the cone angle data for ("Outer" or "Inner")
     * @parse *ptrInputDataElement the input data element to parse
     *
     * @return shared_ptr<ConeAngleData> Cone angle data created
     */
    shared_ptr<ConeAngleData> ParseConeAngleData(const string &keyString, const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Method which parses a Cone Pattern from a MURAL input data element.
     *
     * @parse *ptrInputDataElement the input data element to parse
     */
    shared_ptr<ConePattern>  ParseConePattern(const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Method which parses a Donut Pattern from a MURAL input data element.
     *
     * @parse *ptrInputDataElement the input data element to parse
     */
    shared_ptr<DonutPattern>  ParseDonutPattern(const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Method which parses a Axeblade Pattern from a MURAL input data element.
     *
     * @parse *ptrInputDataElement the input data element to parse
     */
    shared_ptr<AxebladePattern>  ParseAxebladePattern(const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Method which parses a Butterfly Pattern from a MURAL input data element.
     *
     * @parse *ptrInputDataElement the input data element to parse
     */
    shared_ptr<ButterflyPattern>  ParseButterflyPattern(const shared_ptr<InputDataElement> &ptrInputDataElement);

    static inline string GetClassName();
};

inline
string
PatternExtractor::GetClassName()
{
    return(PATTERN_EXTRACTOR_H);
}

#endif
