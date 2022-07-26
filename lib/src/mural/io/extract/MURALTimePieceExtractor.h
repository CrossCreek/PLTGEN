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
 * MURALTimePieceExtractor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef MURAL_TIMEPIECE_EXTRACTOR_H
#define MURAL_TIMEPIECE_EXTRACTOR_H "MURALTimePieceExtractor"

#include <string>
#include <memory>

class InputDataElement;

using namespace std;

/**
 * Class to extract the TimePiece data using the traditional InputDataElement approach
 *
 * @author Rob Lowry
 * @author Brian Fowler
 * @date 08/16/11
 */
class MURALTimePieceExtractor
{
public:

    /**
     * Default Constructor
     */
    MURALTimePieceExtractor();

    /**
     * Copy Constructor
     */
    MURALTimePieceExtractor(const MURALTimePieceExtractor &copyMe);

    /**
     * Default Destructor
     */
    virtual ~MURALTimePieceExtractor();

    /**
     * Method which extracts the Time Piece Data
     */
    void ExtractTimePieceInfo(shared_ptr<InputDataElement> ptrInputDataElement);

protected:

private:

    /**
     * Method which extracts the Leap Second Information
     */
    void ExtractLeapSecondFile(const std::string &leapSecondFileName);

    /**
     * Gets the class name
     *
     * @return string class name
     */
    static inline std::string GetClassName();
};

inline
std::string
MURALTimePieceExtractor::GetClassName()
{
    return(MURAL_TIMEPIECE_EXTRACTOR_H);
}

#endif
