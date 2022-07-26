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
 * FileNames.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "InputProcessor.h"
#include "FileNames.h"


#include "general/parse/ParseUtility.h"
#include "general/exception/UpdateException.h"
#include "general/exception/Exception.h"
#include "general\utility\Utility.h"

// Init the static file Maps
shared_ptr<FileNames::FileMap>     FileNames::ptrFileNameMap_s = shared_ptr<FileNames::FileMap>(new FileNames::FileMap() );
shared_ptr<FileNames::FileListMap> FileNames::ptrFileListMap_s = shared_ptr<FileNames::FileListMap>(new FileNames::FileListMap() );

FileNames::FileNames()
{
}

/* ************************************************************************************** */

bool
FileNames::HasFileNameDefined(const FILE_TAG &fileTag)
{
    bool requestsExist = false;

    if ( ptrFileNameMap_s->find(fileTag) != ptrFileNameMap_s->end()) {
        requestsExist = true;
    }

    return ( requestsExist );
}

/* ************************************************************************************** */

const string
FileNames::GetFileName(const FILE_TAG &fileTag)
{
    string fileName = "";

    if ( ptrFileNameMap_s->find(fileTag) != ptrFileNameMap_s->end()) {
        fileName = *(ptrFileNameMap_s->find(fileTag)->second);
    }

    return ( fileName );
}

/* ************************************************************************************** */

void
FileNames::SetFileName(const FILE_TAG &fileTag, const string &fileName)
{
    if ( ptrFileNameMap_s->find(fileTag) == ptrFileNameMap_s->end()) {
        shared_ptr<string> fileNamePtr = shared_ptr<string>(new string(fileName));
        ptrFileNameMap_s->insert(std::pair<FILE_TAG, shared_ptr<string>>(fileTag, fileNamePtr));
    } else {
        throw new UpdateException(GetClassName(), "SetFileName(...)", "Duplicate File Name Path in file map");
    }
}

/* ************************************************************************************** */

void
FileNames::VerifyFile(const FILE_TAG &fileTag)
{
    string fileName = GetFileName(fileTag);

    try {
        ParseUtility::VerifyInputFile(fileName);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "VerifyFileName(" + fileName + ")");
        throw;
    }
}

/* ************************************************************************************** */

shared_ptr<MURALTypes::FileList>
FileNames::GetFileList(const FILE_TAG &fileTag)
{
    shared_ptr<MURALTypes::FileList> ptrFileList = NULL;

    if ( ptrFileListMap_s->find(fileTag) != ptrFileListMap_s->end()) {
        ptrFileList = ptrFileListMap_s->find(fileTag)->second;
    }

    return ptrFileList;
}

/* ************************************************************************************** */


int
FileNames::GetFileListSize(const FILE_TAG &fileTag)
{
    int listSize = 0;
    shared_ptr<MURALTypes::FileList> ptrFileList = GetFileList(fileTag);

    if ( ptrFileList != NULL ) {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        listSize = Utility::CastSizetToInt(ptrFileList->size());
    }

    return listSize;
}

/* ************************************************************************************** */

int
FileNames::FindIndexInList(const FILE_TAG &fileTag, const string &fileName)
{
    int index = 0;
    shared_ptr<MURALTypes::FileList> ptrFileList = GetFileList(fileTag);

    if ( ptrFileList != NULL ) {
        bool found = false;
        MURALTypes::FileList::iterator listIter;

        for(listIter = ptrFileList->begin(); ((listIter != ptrFileList->end()) && (found == false)); ++listIter) {
            if ( *(*listIter) == fileName ) {
                found = true;
            } else {
                index++;
            }
        }
    }

    return index;
}

/* ************************************************************************************** */

void
FileNames::AddFileToList(const FILE_TAG &fileTag, const string &fileName)
{
    shared_ptr<MURALTypes::FileList> ptrFileList = GetFileList(fileTag);
    shared_ptr<string> ptrFileName = shared_ptr<string>(new string(fileName));

    // If the list does not exist in the map, create it and add it to the map
    if ( ptrFileList == NULL ) {
        ptrFileList = shared_ptr<MURALTypes::FileList>(new MURALTypes::FileList());
        ptrFileListMap_s->insert(std::pair<FILE_TAG, shared_ptr<MURALTypes::FileList>>(fileTag, ptrFileList));
    }

    // Add the file to the list (uniquely)
    bool foundValue = false;
    MURALTypes::FileList::iterator listIter;

    for ( listIter = ptrFileList->begin(); listIter != ptrFileList->end(); ++listIter) {
        if ( *(*listIter) == fileName ) {
            foundValue = true;
        }
    }

    if ( foundValue == false ) {
        ptrFileList->push_back(ptrFileName);
    }
}

/* ************************************************************************************** */
/**
 * Destructor
 */
FileNames::~FileNames()
{
    // Do nothing
}
