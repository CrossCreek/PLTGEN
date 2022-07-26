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
 * FileNames.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef FILENAMES_H
#define FILENAMES_H "FileNames"

#include "mural/MURALTypes.h"

#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <list>

using namespace std;

/**
 * Class which holds two maps.  One maps a FILE_TAG value to a string represneting the file name.
 * The other map, maps a FILE_TAG to a list of string representing multiple file names associated with it
 *
 * The class is solely made of static methods.  This was to replace the static strings in the various
 * modules scatterd across MURAL
 *
 * @author Brian Fowler
 * @date 08/16/11
 */
class FileNames
{
public:

    /**
     * Enum to map output file names
     */
     enum FILE_TAG {
        // Main File Tags
        MAIN_INPUT_FILE                 = 1,

        // Input File Tags
        POSITION_INPUTFILE              = 100,
        POSITION_P470_INPUTFILE         = 101,
        CROSSLINK_INPUTFILE             = 102,
        DIRECTDOWNLINK_INPUTFILE        = 103,
        SELFRELAY_INPUTFILE             = 104,
        DOWNLINK_INPUTFILE              = 105,
        MULTIHOP_INPUTFILE              = 106,
        UPA_CONJ_INPUTFILE              = 107,
        RF_CONJ_INPUTFILE               = 108,
        OPS_REQUEST_INPUTFILE           = 109,
        OPS_WORTH_INPUTFILE             = 110,
        MURAL_REQUEST_INPUTFILE         = 111,
        TIMELINE_INPUTFILE              = 112,
        INITPLAN_INPUTFILE              = 113,

        // Output File Tags
        MAIN_MRL_OUTPUTFILE             = 200,
        MAIN_AVERAGING_OUTPUTFILE       = 201,
        POSITION_OUTPUTFILE             = 202,
        REV_TIME_OUTPUTFILE             = 203,
        REV_GPL_OUTPUTFILE              = 204,
        UPDATE_ORBIT_OUTPUTFILE         = 205,
        RELAY_COVERAGE_OUTPUTFILE       = 206,
        CROSSLINK_OUTPUTFILE            = 207,
        DOWNLINK_OUTPUTFILE             = 208,
        DIRECTDOWNLINK_OUTPUTFILE       = 209,
        MULTIHOP_OUTPUTFILE             = 210,
        SELFRELAY_OUTPUTFILE            = 211,
        UPA_CONJ_OUTPUTFILE             = 212,
        RF_CONJ_OUTPUTFILE              = 213,
        VALUE_TIMELINE_OUTPUTFILE       = 214,
        REQUEST_OUTPUTFILE              = 215,
        GPL_REQUEST_OUTPUTFILE          = 216,
        RIV_OUTPUTFILE                  = 217,
        ALLOCATION_SUMMARY_OUTPUTFILE   = 218,
        FINAL_ALLOCATION_OUTPUTFILE     = 219,
        REVISIT_OUTPUTFILE              = 220,
        SATISFACTION_P470_OUTPUTFILE    = 221,
        ACCESS_METRICS_OUTPUTFILE       = 222,
        SATSIM_OUTPUTFILE               = 223,
        TARGET_SUMMARY_OUTPUTFILE       = 224,

        // Debug File Tags
        FOR_REGIONS_INVIEW_DEBUGFILE    = 300,
        FOR_AZIMUTH_ELEVATION_DEBUGFILE = 301,
        OPS_WORTH_DEBUGFILE             = 302,
        OOC_WINDOW_DEBUGFILE            = 303,
        ACCESS_DEBUGFILE                = 304,
        REVISIT_DEBUGFILE               = 305,
        LINKS_DEBUGFILE                 = 306,
        CONJ_DEBUGFILE                  = 307,
        VALT_DEBUGFILE                  = 308,
        REQ_MVR_DEBUGFILE               = 309,
        ALL_MVR_DEBUGFILE               = 310,
        ELEVATION_FACTOR_DEBUGFILE      = 311,
        COLLECTION_DEBUGFILE            = 312,
        PROB_MATRIX_DEBUGFILE           = 313,
        PROB_DEBUGFILE                  = 314,

        // Multiple List File Tags
        REQUIREMENT_DECK                = 400
    };

    typedef map<FILE_TAG, shared_ptr<string>> FileMap;
    typedef map<FILE_TAG, shared_ptr<MURALTypes::FileList>> FileListMap;

    FileNames();
    ~FileNames();

    /**
     * Method to determine if the file name is defined for a given tag
     *
     * @param fileTag the file tag
     * @return bool
     */
    static bool HasFileNameDefined(const FILE_TAG &fileTag);

    /**
     * Gets the file name as a string from the file tag
     *
     * @param fileTag the file tag
     * @return string
     */
    static const string GetFileName(const FILE_TAG &fileTag);

    /**
     * Sets the file name as a string to the file tag
     *
     * @param fileTag the file tag
     * @param fileName the file name
     */
    static void SetFileName(const FILE_TAG &fileTag, const string &fileName);


    /**
     * Method to verify the existance of a file name.  Will get the stored file
     * from the file name map
     *
     * @param fileTag the file tag
     */
    static void VerifyFile(const FILE_TAG &fileTag);

    /**
     * Gets a particular file list.  Returns NULL if the file tag does not exist
     *
     * @param fileTag the file tag
     */
    static shared_ptr<MURALTypes::FileList> GetFileList(const FILE_TAG &fileTag);

    /**
     * Gets size of a particular file list
     *
     * @param fileTag the file tag
     */

    static int GetFileListSize(const FILE_TAG &fileTag);

    /**
     * Gets position index of fileName in the list
     *
     * @param fileTag the file tag
     * @param fileName the file name to find
     */
    static int FindIndexInList(const FILE_TAG &fileTag, const string &fileName);

    /**
     * Adds a file name to list given the file tag.  It will add it uniquely.
     *
     * @param fileTag the file tag
     * @param fileName the file name to add
     */
    static void AddFileToList(const FILE_TAG &fileTag, const string &fileName);



protected:

private:
    // Map to hold the FILE_TAG to file name path string
    static shared_ptr<FileMap> ptrFileNameMap_s;

    // Map to hold the FILE_TAG to list of file name paths
    static shared_ptr<FileListMap> ptrFileListMap_s;

    static inline string GetClassName();
};

inline
string
FileNames::GetClassName()
{
    return(FILENAMES_H);
}

#endif

