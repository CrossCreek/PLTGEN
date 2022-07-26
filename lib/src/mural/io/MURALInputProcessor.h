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
 * MURALInputProcessor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef MURAL_INPUT_PROCESSOR_H
#define MURAL_INPUT_PROCESSOR_H "MURALInputProcessor"

#include <vector>

#include "mural/io/InputProcessor.h"
#include "mural/MURALTypes.h"

#include "general/utility/Constant.h"

using namespace std;

class DataProviderCollection;
class DataProvider;
class UserVehicle;

/**
 * Child of InputProcessor which handles the MURAL run control
 *
 * Class Attribute Descriptions
 *
 * Name                        Description (units)
 * -------------               -------------------------------------------------
 * lastModuleToComplete_s      The last RUN_MODULE to complete
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class MURALInputProcessor : public InputProcessor
{
public:

    enum RUN_MODULE {
        UNKNOWN_RUN_MODULE  = -1,
        POSITION_GENERATOR    = 0,
        LINK_PATH_GENERATOR = 1,
        TIMELINE_GENERATOR  = 2,
        REQUEST_GENERATOR   = 3,
        RESOURCE_ALLOCATOR  = 4,
        TARGET_SCHEDULER  = 5
    };

    MURALInputProcessor();



    explicit MURALInputProcessor(const string &mainInputFileName);

    MURALInputProcessor(const MURALInputProcessor &copyMe);
    virtual ~MURALInputProcessor();

    MURALInputProcessor& operator =  (const MURALInputProcessor &copyMe);
    bool               operator == (const MURALInputProcessor &compareMe) const;

    static void   PrintAttributes(ostream &outfile);

    static bool   CreateEarth();
    static bool   FindValidDataProvider(const shared_ptr<InputData> ptrInputData,
                                        string &providerDesignator);
    static bool   FindValidDataProviders(const shared_ptr<InputData> ptrInputData,
                                         string &providerDesignator1,
                                         string &providerDesignator2);

    static bool   ValidGroundStation(const string &groundDesignator);
    static bool   ValidRelaySatellite(const string &relayDesignator);
    static bool   ValidUserVehicle(const string &userDesignator);
    static bool   ValidTransmitter(const string &transmitterDesignator);
    static bool   ValidReceiver(const string &receiverDesignator);
    static bool   ValidDataProvider(const string &providerDesignator);

    shared_ptr<DataProviderCollection> CreateDataProviderList();

    static inline bool   JPLFilePathDefined();
    static inline bool   RunModule(const RUN_MODULE &module);
    static inline bool   RunPositionGenerator();
    static inline bool   RunLinkPathGenerator();
    static inline bool   RunTimelineGenerator();

    static inline RUN_MODULE      GetLastModuleToComplete();

protected:

private:

    static RUN_MODULE GetRunModule(const string &valueString);

    void   ExtractInputRunVariables();
    void   SetInputFileNames();
    void   SetDebugFileNames();
    void   ExtractGlobalRunVariables();

    string FindDesignatorString(shared_ptr<InputDataElement> ptrInputDataElement) const;
    string FindTrailVehicleDesignatorString(shared_ptr<InputDataElement> ptrInputDataElement) const;

    Array<string>*   CheckAllActiveAssetsExist();

    shared_ptr<UserVehicle>     FindUserVehicle(const shared_ptr<MURALTypes::UserVector> &ptrUserVehicleList,
            const string &userDesignator) const;

    static inline string GetClassName();

    static bool                  jplHeaderDefined_s;
    static RUN_MODULE            lastModuleToComplete_s;
};

inline
bool
MURALInputProcessor::JPLFilePathDefined()
{
    return(jplHeaderDefined_s);
}

inline
bool
MURALInputProcessor::RunModule(const RUN_MODULE &module)
{
    return(module <= lastModuleToComplete_s);
}

inline
bool
MURALInputProcessor::RunPositionGenerator()
{
    return(RunModule(POSITION_GENERATOR));
}

inline
bool
MURALInputProcessor::RunLinkPathGenerator()
{
    return(RunModule(LINK_PATH_GENERATOR));
}

inline
bool
MURALInputProcessor::RunTimelineGenerator()
{
    return(RunModule(TIMELINE_GENERATOR));
}

inline
MURALInputProcessor::RUN_MODULE
MURALInputProcessor::GetLastModuleToComplete()
{
    return(lastModuleToComplete_s);
}

inline
string
MURALInputProcessor::GetClassName()
{
    return(MURAL_INPUT_PROCESSOR_H);
}

#endif
