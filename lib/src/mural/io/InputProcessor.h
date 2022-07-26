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
 * InputProcessor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef INPUT_PROCESSOR_H
#define INPUT_PROCESSOR_H "InputProcessor"

#include <fstream>


#include "general/data/InputDataElement.h"
#include "general/exception/InputException.h"

using namespace std;

/**
 * Base class for the MURAL run-control processor
 *
 * Class Attribute Descriptions
 *
 * Name                            Description (units)
 * -------------                   ---------------------------------------------
 * ptrMainInputFileName_s*         String containing the main input filename
 * ptrInputDataFileNameElement_s*  InputDataElement containing input data file
 *                                   names (i.e. files with data in IDE format)
 * ptrInputFileNameElement_s*      InputDataElement containing input file names
 * ptrDebugFileNameElement_s*      InputDataElement containing debug file names
 * ptrOutputFileNameElement_s*     InputDataElement containing output file names
 * ptrInputDataElementList_s*      LinkList of input data elements
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class InputProcessor
{
public:

    InputProcessor();



    explicit InputProcessor(const string &mainInputFileName);

    InputProcessor(const InputProcessor &copyMe);
    virtual ~InputProcessor();

    InputProcessor& operator =  (const InputProcessor &copyMe);
    bool            operator == (const InputProcessor &compareMe) const;

    static void   PrintAttributes(ostream &outfile);

protected:

    static shared_ptr<InputDataElement>                        ptrInputDataFileNameElement_s;
    static shared_ptr<InputDataElement>                        ptrInputFileNameElement_s;
    static shared_ptr<InputDataElement>                        ptrOutputFileNameElement_s;
    static shared_ptr<InputDataElement>                        ptrDebugFileNameElement_s;
    static shared_ptr<InputDataElement::InputDataElementList>  ptrInputDataElementList_s;

private:

    static void   RetrieveFileNameElements();
    static void   RetrieveInputDataElements();

    static shared_ptr<InputDataElement> ExtractInputDataElement(ifstream &inputFile,
            const string &startLine,
            int numberOfIndents = 0);

    static inline string GetClassName();
};

inline
string
InputProcessor::GetClassName()
{
    return(INPUT_PROCESSOR_H);
}

#endif
