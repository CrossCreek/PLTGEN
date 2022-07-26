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
 * InputDataElement.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "InputDataElement.h"
#include "general/array/ListIterator.h"
#include "general/exception/InputException.h"
#include "general/utility/StringEditor.h"

InputDataElement::InputDataElement()
    : numberOfIndents_a(0), ptrBlockName_a(NULL), ptrInputDataList_a(NULL),
      ptrInputDataElementList_a(NULL)
{
    ptrBlockName_a = shared_ptr<string>(new string("Undefined Block Tag"));
    return;
}

InputDataElement::InputDataElement(const string &newBlockTag,
                                   int indent)
    : numberOfIndents_a(indent), ptrBlockName_a(NULL), ptrInputDataList_a(NULL),
      ptrInputDataElementList_a(NULL)
{
    ptrBlockName_a = shared_ptr<string>(new string(StripOutBlockName(newBlockTag)));
    return;
}

InputDataElement::InputDataElement(const InputDataElement &copyMe)
    : numberOfIndents_a(copyMe.numberOfIndents_a), ptrBlockName_a(NULL),
      ptrInputDataList_a(NULL), ptrInputDataElementList_a(NULL)
{
    ptrBlockName_a = shared_ptr<string>(new string(*(copyMe.ptrBlockName_a)));

    if (copyMe.ptrInputDataList_a != NULL) {
        ptrInputDataList_a = shared_ptr<InputDataList>(new InputDataList(*(copyMe.ptrInputDataList_a)));
    }

    if (copyMe.ptrInputDataElementList_a != NULL) {
        ptrInputDataElementList_a
            = shared_ptr<InputDataElementList>(new InputDataElementList(*(copyMe.ptrInputDataElementList_a)));
    }

    return;
}

InputDataElement&
InputDataElement::operator = (const InputDataElement &copyMe)
{
    numberOfIndents_a  = copyMe.numberOfIndents_a;
    return(*this);
}

bool
InputDataElement::operator == (const InputDataElement &compareMe) const
{
    return( (numberOfIndents_a == compareMe.numberOfIndents_a) &&
            (*ptrBlockName_a == *(compareMe.ptrBlockName_a)) &&
            (*ptrInputDataList_a == *(compareMe.ptrInputDataList_a)) &&
            (*ptrInputDataElementList_a == *(compareMe.ptrInputDataElementList_a)) );
}

bool
InputDataElement::operator != (const InputDataElement &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
InputDataElement::PrintInputDataElement(ostream &outfile) const
{
    int                           indent = 0;
    const int                     maxLabelSize = GetMaximumLabelSize() + 3;
    string                          inputLabel = "";
    InputDataList::iterator inputDataIter;
    InputDataElementList::iterator inputDataElementIter;

    while (indent < numberOfIndents_a) {
        outfile << "   ";
        ++indent;
    }

    outfile << GetOpeningBlockTag() << NEW_LINE;

    if ( ptrInputDataList_a != NULL ) {
        for( inputDataIter = ptrInputDataList_a->begin(); inputDataIter != ptrInputDataList_a->end(); ++inputDataIter) {
            shared_ptr<InputData> ptrInputData = (*inputDataIter);
            indent = 0;

            while (indent <= numberOfIndents_a) {
                outfile << "   ";
                ++indent;
            }

            inputLabel = ptrInputData->GetLabel();
            outfile << inputLabel;
            outfile << setw(maxLabelSize - (int)inputLabel.size()) << " : ";
            (ptrInputData->GetValueArray())->Print(outfile);
            outfile << NEW_LINE;
        }
    }

    if (ptrInputDataElementList_a != NULL) {
        outfile << NEW_LINE;

        for( inputDataElementIter = ptrInputDataElementList_a->begin(); inputDataElementIter != ptrInputDataElementList_a->end(); ++inputDataElementIter) {
            shared_ptr<InputDataElement> ptrInputDataElement = (*inputDataElementIter);
            ptrInputDataElement->PrintInputDataElement(outfile);
        }
    }

    indent = 0;

    while (indent < numberOfIndents_a) {
        outfile << "   ";
        ++indent;
    }

    outfile << GetClosingBlockTag() << NEW_LINE;
    return;
}

void
InputDataElement::AddInputData(const string &inputString,
                               string delimiter)
{
    string     label = StripOffLabel(inputString, delimiter);
    string     value = StripOffValue(inputString, delimiter);
    shared_ptr<InputData> ptrInputData = NULL;
    ptrInputData = shared_ptr<InputData>(new InputData(label, value));

    if (ptrInputDataList_a == NULL) {
        ptrInputDataList_a = shared_ptr<InputDataList>(new InputDataList());
    }

    ptrInputDataList_a->push_back(ptrInputData);
    return;
}

void
InputDataElement::AddInputData(shared_ptr<InputData> ptrNewInput)
{
    if (ptrInputDataList_a == NULL) {
        ptrInputDataList_a = shared_ptr<InputDataList>(new InputDataList());
    }

    ptrInputDataList_a->push_back(ptrNewInput);
    return;
}

void
InputDataElement::AddInputDataElement(shared_ptr<InputDataElement> ptrNewElement)
{
    if (ptrInputDataElementList_a == NULL) {
        ptrInputDataElementList_a = shared_ptr<InputDataElementList>(new InputDataElementList());
    }

    ptrInputDataElementList_a->push_back(ptrNewElement);
    return;
}

InputData&
InputDataElement::RetrieveInputData(const string &label)
{
    bool                    found = false;
    InputDataList::iterator inputDataIter;   // Iterator of InputData
    shared_ptr<InputData>   ptrReturnData = NULL;

    // Initialize the iterator
    if ( ptrInputDataList_a != NULL ) {
        inputDataIter = ptrInputDataList_a->begin();
    }

    while (((*inputDataIter) != NULL) && (found == false)) {
        if ((*inputDataIter)->GetLabel() == label) {
            found = true;
            ptrReturnData = (*inputDataIter);
        } else {
            // Increment the iterator
            ++inputDataIter;
        }
    }

    return(*ptrReturnData);
}

int
InputDataElement::GetMaximumLabelSize() const
{
    int                   maximumLabelSize = 0;
    InputDataList::iterator inputDataIter; // Iterator of InputData

    if ( ptrInputDataList_a != NULL ) {
        for( inputDataIter = ptrInputDataList_a->begin(); inputDataIter != ptrInputDataList_a->end(); ++inputDataIter) {
            int labelSize = 0;
            labelSize = (*inputDataIter)->GetLabelSize();

            if (labelSize > maximumLabelSize) {
                maximumLabelSize = labelSize;
            }
        }
    }

    return(maximumLabelSize);
}

string
InputDataElement::StripOutBlockName(const string &inputString)
{
    string                  blockName = "";
    const string::size_type startPosition = inputString.find_first_of("<");
    const string::size_type endPosition = inputString.find_last_of("_");

    if ( (startPosition == string::npos) ||
            (endPosition == string::npos) ||
            (startPosition > endPosition) ) {
        throw new InputException(GetClassName(), "StripOutBlockName(const string&)", inputString);
    }

    blockName = inputString.substr((startPosition + 1), (endPosition - startPosition - 1));
    return(blockName);
}

string
InputDataElement::StripOffLabel(const string &inputString,
                                const string &delimiter)
{
    string                  labelString = "";
    const string::size_type startPosition = 0;
    const string::size_type endPosition = inputString.find_first_of(delimiter);

    if (endPosition == string::npos) {
        throw new InputException(GetClassName(), "StripOffLabel(const string&, const string&)",
                                 inputString);
    }

    labelString = inputString.substr(startPosition, endPosition);
    StringEditor::TrimBlankSpaces(labelString);
    return(labelString);
}

string
InputDataElement::StripOffValue(const string &inputString,
                                const string &delimiter)
{
    string            valueString = "";
    string::size_type startPosition = inputString.find_first_of(delimiter);
    string::size_type endPosition = inputString.size();

    if ((startPosition == string::npos) || (startPosition >= endPosition)) {
        throw new InputException(GetClassName(), "StripOffValue(const string&, const string&)",
                                 inputString);
    }

    valueString = inputString.substr((startPosition + 1), (endPosition - startPosition));
    StringEditor::TrimBlankSpaces(valueString);
    return(valueString);
}

string
InputDataElement::GetOpeningBlockTag() const
{
    string returnString = "";

    if (ptrBlockName_a != NULL) {
        returnString = "<" + (*ptrBlockName_a);

        if (numberOfIndents_a == 0) {
            returnString += "_START>";
        } else {
            returnString += "_BEGIN>";
        }
    }

    return(returnString);
}

string
InputDataElement::GetClosingBlockTag() const
{
    string returnString = "";

    if (ptrBlockName_a != NULL) {
        returnString = "<" + (*ptrBlockName_a);

        if (numberOfIndents_a == 0) {
            returnString += "_END>";
        } else {
            returnString += "_STOP>";
        }
    }

    return(returnString);
}

InputDataElement::~InputDataElement()
{
    return;
}
