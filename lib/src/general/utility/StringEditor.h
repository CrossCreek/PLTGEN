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
 * StringEditor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef STRING_EDITOR_H
#define STRING_EDITOR_H "StringEditor"

#include <string>

using namespace std;

/**
 * StringEditor is the class that performs generic string manipulation
 *  functions.
 *
 * @author Rob Lowry
 * @author Brian Fowler
 * @date 08/16/11
 */
class StringEditor
{
public:

    virtual ~StringEditor();

    static void   RemoveCharacter(string &inputString,
                                  const char &removeCharacter);
    static void   TrimBlankSpaces(string &inputString);

    static int   ConvertToInt(const string &valueString);

    static double ConvertToDouble(const string &valueString);

    static bool   ConvertToBoolean(const string &valueString);

    static string ConvertToString(int value,
                                  int totalFillZeros = 0);
    static string ConvertToString(double value);
    static string ConvertToString(const bool &value);
    static string ConvertToString(const char &value,
                                  int multiplicity = 1);
    static string ConvertToString(const char *value);

    static bool IsEmpty(const string *inStr);

protected:

private:

    static inline string GetClassName();
};

inline
string
StringEditor::GetClassName()
{
    return(STRING_EDITOR_H);
}

#endif
