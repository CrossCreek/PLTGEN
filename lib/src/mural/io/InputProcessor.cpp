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
 * InputProcessor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "InputProcessor.h"
#include "mural/io/FileNames.h"


#include "general/array/ListIterator.h"
#include "general/exception/FileNotFoundException.h"
#include "general/exception/NullPointerException.h"
#include "general/parse/ParseUtility.h"
#include "general/utility/Constant.h"
#include "general/utility/StringEditor.h"

shared_ptr<InputDataElement>                     InputProcessor::ptrInputDataFileNameElement_s = NULL;
shared_ptr<InputDataElement>                     InputProcessor::ptrInputFileNameElement_s = NULL;
shared_ptr<InputDataElement>                     InputProcessor::ptrOutputFileNameElement_s = NULL;
shared_ptr<InputDataElement>                     InputProcessor::ptrDebugFileNameElement_s = NULL;
shared_ptr<list<shared_ptr<InputDataElement>>>   InputProcessor::ptrInputDataElementList_s = NULL;

//------------------------------------------------------------------------------
//
//  Method Name: InputProcessor::InputProcessor
//
//  Purpose:
//    Default constructor for the InputProcessor class.
//
//  Calling Method:
//     InputProcessor();
//
//  Calling Parameters
//  Parameters      i/o  description
//  ----------      ---  -----------
//
//  Returns:
//     None
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

InputProcessor::InputProcessor()
{
    return;
}

//------------------------------------------------------------------------------
//
//  Method Name: InputProcessor::InputProcessor
//
//  Purpose:
//    Alternate constructor for the InputProcessor class.
//
//  Calling Method:
//     InputProcessor();
//
//  Calling Parameters
//  Parameters      i/o  description
//  ----------      ---  -----------
//
//  Returns:
//     None
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

InputProcessor::InputProcessor(const string &mainInputFileName)
{
    FileNames::SetFileName(FileNames::MAIN_INPUT_FILE, mainInputFileName);

    try {
        FileNames::VerifyFile(FileNames::MAIN_INPUT_FILE);
        RetrieveFileNameElements();

        if (ptrInputDataFileNameElement_s != NULL) {
            RetrieveInputDataElements();
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "InputProcessor(const string&)");
        throw;
    }

    return;
}

InputProcessor::InputProcessor(const InputProcessor &copyMe)
{
    return;
}

InputProcessor&
InputProcessor::operator = (const InputProcessor &copyMe)
{
    return(*this);
}

bool
InputProcessor::operator == (const InputProcessor &compareMe) const
{
    return(false);
}

void
InputProcessor::PrintAttributes(ostream &outfile)
{
    if (FileNames::HasFileNameDefined(FileNames::MAIN_INPUT_FILE) == true) {
        outfile << "                  MAIN INPUT FILE : "
                << FileNames::GetFileName(FileNames::MAIN_INPUT_FILE) << NEW_LINE << NEW_LINE;
    }

    if (ptrInputDataFileNameElement_s != NULL) {
        ptrInputDataFileNameElement_s->PrintInputDataElement(outfile);
        outfile << NEW_LINE;
    }

    if (ptrInputFileNameElement_s != NULL) {
        ptrInputFileNameElement_s->PrintInputDataElement(outfile);
        outfile << NEW_LINE;
    }

    if (ptrOutputFileNameElement_s != NULL) {
        ptrOutputFileNameElement_s->PrintInputDataElement(outfile);
        outfile << NEW_LINE;
    }

    if (ptrDebugFileNameElement_s != NULL) {
        ptrDebugFileNameElement_s->PrintInputDataElement(outfile);
        outfile << NEW_LINE;
    }

    return;
}

void
InputProcessor::RetrieveFileNameElements()
{
    string   line = "";
    string   errorString = "";
    ifstream mainInputFile;

    try {
        ParseUtility::OpenInputFile(mainInputFile, FileNames::GetFileName(FileNames::MAIN_INPUT_FILE));

        while (getline(mainInputFile, line, NEW_LINE) && (errorString.size() == 0)) {
            // find a line that has a start tag and does not begin with a '#'
            if ((line.find("_START>") != string::npos) && (line.find("#") == string::npos)) {
                if (line.find("<INPUT_DATA_FILE_NAMES_START>") != string::npos) {
                    ptrInputDataFileNameElement_s = ExtractInputDataElement(mainInputFile, line);
                } else if (line.find("<INPUT_FILE_NAMES_START>") != string::npos) {
                    ptrInputFileNameElement_s = ExtractInputDataElement(mainInputFile, line);
                } else if (line.find("<OUTPUT_FILE_NAMES_START>") != string::npos) {
                    ptrOutputFileNameElement_s = ExtractInputDataElement(mainInputFile, line);
                } else if (line.find("<DEBUG_FILE_NAMES_START>") != string::npos) {
                    ptrDebugFileNameElement_s = ExtractInputDataElement(mainInputFile, line);
                } else {
                    errorString = "Unknown start block tag '" + line + "' found!";
                }
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "RetrieveFileNameElements()");
        throw;
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "RetrieveFileNameElements()", errorString);
    }

    return;
}

void
InputProcessor::RetrieveInputDataElements()
{
    string                                    line = "";
    shared_ptr<InputData>                     ptrInputData = NULL;
    shared_ptr<InputDataElement>              ptrInputDataElement = NULL;
    shared_ptr<InputDataElement::InputDataList>       inputDataList = ptrInputDataFileNameElement_s->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;

    try {
        for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
            ptrInputData = (*inputDataIter);
            ifstream inputFile;
            ParseUtility::OpenInputFile(inputFile, ptrInputData->GetStringValue());

            while (getline(inputFile, line, NEW_LINE)) {
                // find a line that has a start tag and does not begin with a '#'
                if ((line.find("_START>") != string::npos) && (line.find("#") == string::npos)) {
                    ptrInputDataElement = ExtractInputDataElement(inputFile, line);

                    if (ptrInputDataElement != NULL) {
                        if (ptrInputDataElementList_s == NULL) {
                            ptrInputDataElementList_s = shared_ptr<InputDataElement::InputDataElementList>(
                                                            new InputDataElement::InputDataElementList());
                        }

                        ptrInputDataElementList_s->push_back(ptrInputDataElement);
                        ptrInputDataElement = NULL;
                    }
                }
            }

            inputFile.close();
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "RetrieveInputDataElements()");
        throw;
    }

    return;
}

shared_ptr<InputDataElement>
InputProcessor::ExtractInputDataElement(ifstream &inputFile,
                                        const string &startLine,
                                        int numberOfIndents)
{
    bool              foundEndOfBlock = false;
    string            line = "";
    string            errorString = "";
    string closingBlockTag;
    shared_ptr<InputDataElement> ptrSubElement = NULL;
    shared_ptr<InputDataElement> ptrInputDataElement = NULL;
    ptrInputDataElement = shared_ptr<InputDataElement>(new InputDataElement(startLine, numberOfIndents));
    closingBlockTag = ptrInputDataElement->GetClosingBlockTag();

    while (!inputFile.eof() && (foundEndOfBlock == false) && (errorString.size() == 0)) {
        getline(inputFile, line, NEW_LINE);
        StringEditor::TrimBlankSpaces(line);

        if ((line.size() > 0) && (line.find("#") == string::npos)) {
            if (line.find(" : ") != string::npos) {
                ptrInputDataElement->AddInputData(line);
            } else if (line.find("_BEGIN>") != string::npos) {
                ptrSubElement = ExtractInputDataElement(inputFile, line, (numberOfIndents + 1));

                if (ptrSubElement != NULL) {
                    ptrInputDataElement->AddInputDataElement(ptrSubElement);
                    ptrSubElement = NULL;
                }
            } else if (line.find(closingBlockTag) != string::npos) {
                foundEndOfBlock = true;
            } else if (line.find("_START>") != string::npos) {
                errorString = "Nested START block tag '" + line + "' found!  Must be a BEGIN block tag!";
            } else if ((line.find("_END>") != string::npos) || (line.find("_STOP>") != string::npos)) {
                errorString = "Unexpected closing block tag '" + line + "' found!  Was looking for '";
                errorString += (closingBlockTag + "'!");
            } else {
                errorString = "Unknown input '" + line + "' found!";
            }
        }
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "ExtractInputDataElement(ifstream&, const string&, int)",
                                 errorString);
    }

    if (foundEndOfBlock == false) {
        errorString = "Expected closing block tag '" + closingBlockTag + "' was not found!";
        throw new InputException(GetClassName(),
                                 "ExtractInputDataElement(ifstream&, const string&, int)",
                                 errorString);
    }

    return(ptrInputDataElement);
}

InputProcessor::~InputProcessor()
{
    return;
}
