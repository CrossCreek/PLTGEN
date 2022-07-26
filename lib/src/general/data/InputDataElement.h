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
 * InputDataElement.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef INPUT_DATA_ELEMENT_H
#define INPUT_DATA_ELEMENT_H "InputDataElement"

#include <list>

#include "InputData.h"
#include "general/array/LinkList.h"
#include "general\utility\Utility.h"

using namespace std;

/**
 * Class which encapsulates a grouping of Data.  It holds a begin and end tags as
 * well as a list of InputData values asscoiated with them.
 *
 * Example:
 *     <TAG_BEGIN>
 *         <InputData objects here>
 *     <TAG_STOP>
 *
 * Class Attribute Descriptions
 *
 * Name                        Description (units)
 * -------------               -------------------------------------------------
 * GetClassName()                 String containing this class' name
 * numberOfIndents_a           Number of times this block is indented (zero
 *                               indicates STAR/END block, one or more indicates
 *                               BEGIN/STOP block).
 * ptrBlockName_a*             String containing the associated Block name
 * ptrInputDataList_a*         LinkList of InputData's
 * ptrInputDataElementList_a*  LinkList of InputDataElement's
 *
 * @author Rob Lowry
 * @author Brian Fowler
 * @date 08/16/11
 */
class InputDataElement
{
public:

    /**
     * Type definitions for common InputData types
     */
    typedef std::list<shared_ptr<InputData>> InputDataList;
    typedef std::list<shared_ptr<InputDataElement>> InputDataElementList;

    /**
     * Default Constructor
     */
    InputDataElement();
    InputDataElement(const string &newBlockTag,
                     int indents = 0);
    InputDataElement(const InputDataElement &copyMe);
    virtual ~InputDataElement();

    InputDataElement& operator=(const InputDataElement &copyMe);
    bool              operator == (const InputDataElement &compareMe) const;
    bool              operator != (const InputDataElement &compareMe) const;

    static string StripOutBlockName(const string &inputString);
    static string StripOffLabel(const string &inputString,
                                const string &delimiter);
    static string StripOffValue(const string &inputString,
                                const string &delimiter);

    void        AddInputData(const string &inputString,
                             string delimiter = ":");
    void        AddInputData(shared_ptr<InputData> ptrNewInput);
    void        AddInputDataElement(shared_ptr<InputDataElement> ptrNewElement);
    void        PrintInputDataElement(ostream &outfile) const;

    int       GetMaximumLabelSize() const;

    string      GetOpeningBlockTag() const;
    string      GetClosingBlockTag() const;

    InputData&   RetrieveInputData(const string &label);

    inline int GetNumberOfIndents() const;


    inline int  GetNumberOfInputData() const;

    inline int  GetNumberOfInputDataElements() const;

    inline bool  BlockTagContainsString(const string &checkString);

    inline shared_ptr<string>                 GetBlockTag(bool check = false) const;
    inline shared_ptr<InputDataList>          GetInputDataList(bool check = false);
    inline shared_ptr<InputDataElementList>   GetInputDataElementList(bool check = false);

protected:

private:




    static inline string GetClassName();

    int                                    numberOfIndents_a;
    shared_ptr<string>                     ptrBlockName_a;
    shared_ptr<InputDataList>              ptrInputDataList_a;
    shared_ptr<InputDataElementList>       ptrInputDataElementList_a;
};

inline
shared_ptr<InputDataElement::InputDataList>
InputDataElement::GetInputDataList(bool check)
{
    if ((check == true) && (ptrInputDataList_a == NULL)) {
        throw new NullPointerException(GetClassName(), "GetInputDataList(bool)",
                                       "ptrInputDataList_a");
    }

    return(ptrInputDataList_a);
}

inline
shared_ptr<InputDataElement::InputDataElementList>
InputDataElement::GetInputDataElementList(bool check)
{
    if ((check == true) && (ptrInputDataElementList_a == NULL)) {
        throw new NullPointerException(GetClassName(), "GetInputDataElementList(bool)",
                                       "ptrInputDataElementList_a");
    }

    return(ptrInputDataElementList_a);
}

inline

int
InputDataElement::GetNumberOfInputData() const
{
    int numberOfInputData = 0;

    if (ptrInputDataList_a != NULL) {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        numberOfInputData = Utility::CastSizetToInt(ptrInputDataList_a->size());
    }

    return(numberOfInputData);
}

inline

int
InputDataElement::GetNumberOfInputDataElements() const
{
    int numberOfInputDataElements = 0;

    if (ptrInputDataElementList_a != NULL) {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        numberOfInputDataElements = Utility::CastSizetToInt(ptrInputDataElementList_a->size());
    }

    return(numberOfInputDataElements);
}

inline
shared_ptr<string>
InputDataElement::GetBlockTag(bool check) const
{
    if ((check == true) && (ptrBlockName_a == NULL)) {
        throw new NullPointerException(GetClassName(), "GetBlockTag(bool)",
                                       "ptrBlockName_a");
    }

    return(ptrBlockName_a);
}

//CPPCHECK_RELATED
//Removed SetBlockTag() since it wasn't called in code base
/*
inline
void
InputDataElement::SetBlockTag(const string &newBlockTag)
{
    ptrBlockName_a = shared_ptr<string>(new string(newBlockTag));
    return;
}
*/

inline
bool
InputDataElement::BlockTagContainsString(const string &checkString)
{
    if (ptrBlockName_a == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "BlockTagContainsString(const string&)",
                                       "ptrBlockName_a");
    }

    return(ptrBlockName_a->find(checkString) != string::npos);
}

inline
int
InputDataElement::GetNumberOfIndents() const
{
    return(numberOfIndents_a);
}

inline
string
InputDataElement::GetClassName()
{
    return(INPUT_DATA_ELEMENT_H);
}

#endif
