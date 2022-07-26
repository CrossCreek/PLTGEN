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
 * ParseUtility.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "ParseUtility.h"
#include "general/utility/Constant.h"
#include "general/utility/StringEditor.h"
#include "general/exception/FileNotFoundException.h"
#include "general/exception/NullPointerException.h"
#include "general\exception\Exception.h"

ParseUtility::ParseUtility()
{
    return;
}

ParseUtility::ParseUtility(const ParseUtility &copyMe)
{
    return;
}

ParseUtility&
ParseUtility::operator = (const ParseUtility &copyMe)
{
    return(*this);
}

bool
ParseUtility::operator == (const ParseUtility &compareMe) const
{
    return(false);
}

/* **************************************************************************** */

void
ParseUtility::OpenInputFile(ifstream &infile,
                            const string *ptrFileName)
{
    if (ptrFileName == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "OpenInputFile(ifstream&, const string*)",
                                       "ptrFileName");
    }

    infile.open(ptrFileName->c_str(), ios::in);

    if (!infile) {
        throw new FileNotFoundException(GetClassName(),
                                        "OpenInputFile(ifstream&, const string*)",
                                        *ptrFileName);
    }

    return;
}

/* **************************************************************************** */

void
ParseUtility::OpenInputFile(ifstream &infile,
                            const string &fileName)
{
    infile.open(fileName.c_str(), ios::in);

    if (!infile) {
        throw new FileNotFoundException(GetClassName(),
                                        "OpenInputFile(ifstream&, const string&)",
                                        fileName);
    }

    return;
}

/* **************************************************************************** */

void
ParseUtility::OpenOutputFile(ofstream &outfile,
                             const string *ptrFileName,
                             bool openAppend)
{
    if (ptrFileName == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "OpenInputFile(ifstream&, const string*)",
                                       "ptrFileName");
    }

    // open the file
    if (openAppend == false) {
        outfile.open(ptrFileName->c_str(), ios::out);
    } else {
        outfile.open(ptrFileName->c_str(), ios::app);
    }

    if (!outfile) {
        throw new FileNotFoundException(GetClassName(),
                                        "OpenInputFile(ifstream&, const string*)",
                                        *ptrFileName);
    }

    outfile.setf(ios::fixed);
    outfile.setf(ios::right);
    return;
}

/* **************************************************************************** */

void
ParseUtility::OpenOutputFile(ofstream &outfile,
                             const string &fileName,
                             bool openAppend)
{
    // open the file
    if (openAppend == false) {
        outfile.open(fileName.c_str(), ios::out);
    } else {
        outfile.open(fileName.c_str(), ios::app);
    }

    if (!outfile) {
        throw new FileNotFoundException(GetClassName(),
                                        "OpenInputFile(ifstream&, const string&)",
                                        fileName);
    }

    outfile.setf(ios::fixed);
    outfile.setf(ios::right);
    return;
}

/* **************************************************************************** */

void
ParseUtility::VerifyInputFile(const string *ptrFileName)
{
    ifstream inputFile;

    try {
        OpenInputFile(inputFile, ptrFileName);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "VerifyInputFile(const string*)");
        throw;
    }

    inputFile.close();
    return;
}

/* **************************************************************************** */

void
ParseUtility::VerifyInputFile(const string &fileName)
{
    ifstream inputFile;

    try {
        OpenInputFile(inputFile, fileName);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "VerifyInputFile(const string*)");
        throw;
    }

    inputFile.close();
    return;
}

/* **************************************************************************** */

bool
ParseUtility::FileExists(const string &fileName)
{
    ifstream inputFile;

    try {
        OpenInputFile(inputFile, fileName);
    } catch (Exception *ptrError) {
        return false;
    }

    inputFile.close();
    return true;
}

/* **************************************************************************** */

shared_ptr<InputDataElement>
ParseUtility::ExtractInputDataElement(ifstream &inputFile,
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

/* **************************************************************************** */

ParseUtility::~ParseUtility()
{
    return;
}
