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
 * ParseUtility.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef PARSE_UTILITY_H
#define PARSE_UTILITY_H "ParseUtility"

#include <fstream>
#include "general/data/InputDataElement.h"
#include "general/exception/InputException.h"

using namespace std;

/**
 * Utility class which acts like a utility class.  It contains convenience methods for
 * extracting heritage MURAL InputDataElements and InputData objects.
 *
 * @author Rob Lowry
 * @author Brian Fowler
 * @date 08/16/11
 */
class ParseUtility
{
public:

    /**
     * Default Constructor
     */
    ParseUtility();

    /**
     * Copy Constructor
     */
    ParseUtility(const ParseUtility &copyMe);

    /**
     * Default Destructor
     */
    virtual ~ParseUtility();

    ParseUtility& operator =  (const ParseUtility &copyMe);
    bool            operator == (const ParseUtility &compareMe) const;

    /**
     * Opens an input file stream with a pointer to a string.  Used with heritage MURAL
     * parsers
     *
     * @param &infile the stream which gets returned
     * @param *ptrFileName the pointer to the file name
     */
    static void   OpenInputFile(ifstream &infile,
                                const string *ptrFileName);

    /**
     * Opens an input file stream with a reference to a string.  Used with heritage MURAL
     * parsers
     *
     * @param &infile the stream which gets returned
     * @param &fileName the reference to the file name
     */
    static void   OpenInputFile(ifstream &infile,
                                const string &fileName);

    /**
     * Opens an output file stream with a pointer to a string.  Used with heritage MURAL
     * parsers
     *
     * @param &outfile the stream which gets returned
     * @param *ptrFileName the pointer to the file name
    * @param openAppend append the current ofstream
     */
    static void   OpenOutputFile(ofstream &outfile,
                                 const string *ptrFileName,
                                 bool openAppend = false);

    /**
     * Opens an output file stream with a reference to a string.  Used with heritage MURAL
     * parsers
     *
     * @param &outfile the stream which gets returned
     * @param &fileName the reference to the file name
    * @param openAppend append the current ofstream
     */
    static void   OpenOutputFile(ofstream &outfile,
                                 const string &fileName,
                                 bool openAppend = false);

    /**
     * Method to verify the existance of a file name.  Will throw an exception if it fails
     * Used with heritage MURAL parsers
     *
     * @param *ptrFileName the pointer to the file name
     */
    static void   VerifyInputFile(const string *ptrFileName);

    /**
     * Method to verify the existance of a file name.  Will throw an exception if it fails
     * Used with heritage MURAL parsers
     *
     * @param *ptrFileName the pointer to the file name
     */
    static void   VerifyInputFile(const string &fileName);

    /**
     * Method to verify the existance of a file name.  Will return a boolean T/F upon the
    * existance of the file
     *
     * @param *ptrFileName the pointer to the file name
     */
    static bool   FileExists(const string &fileName);

    /**
     * Method to generically extract an InputDataElement from the input file stream
     *
     * @param &inputFile the input file stream
     * @param &startLine the line to start on
     * @param numberOfIndents the number of indents
     */
    static shared_ptr<InputDataElement> ExtractInputDataElement(ifstream &inputFile,
            const string &startLine,
            int numberOfIndents = 0);
protected:

private:

    /**
     * Gets the class name
     *
     * @return string class name
     */
    static inline string GetClassName();
};

inline
string
ParseUtility::GetClassName()
{
    return(PARSE_UTILITY_H);
}

#endif
