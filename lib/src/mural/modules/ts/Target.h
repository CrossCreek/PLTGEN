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
 * Target.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef TARGET_H
#define TARGET_H "Target"


#include "general/array/Array.h"
#include "general/array/ArrayOfLinkLists.h"

using namespace std;

class CollectionProblemSet;
class Sensor;
class TargetRegion;

/**
 * Target container object which contains Collection Problem Sets
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class Target
{
public:

    enum MISSION_INDEX {
        INTEL_POINT = 0,
        MCG_POINT = 1,
        GROUP_POINT = 2,
        BROAD_AREA_SEARCH = 3,
        MCG_MONO_BOX = 4,
        LARGE_AREA_TARGET = 5,
        MCG_STEREO_AREA = 6,
        DIRECTED_SEARCH_AREA = 7,
        LINE_OF_COMMUNICATION = 8,
        //RAGUSA 1/9/2018 - Add CRISIS mission pattern for using Crisis Region Mode, this is
        //a slightly different way this enum is used but it works for our purposes here
        //as simply a secondary access pattern for Crisis Region Mode
        CRISIS = 9,
        NUMBER_OF_MISSIONS = 10
    };

    enum MODE_INDEX {
        MONO = 0,
        STEREO = 1,
        NUMBER_OF_MODES = 2
    };

    Target();




    Target(const Target &target);

    Target(const string &targetID,
           int missionNumber,
           int deckIndex,
           const string &fcpsID,
           const Array<double> *ptrQuality,
           const Array<int> *ptrPriority,
           const Array<char> *ptrPreference,
           const Array<char> *ptrMode,
           const Array<double> *ptrSensorScore,
           const Array<double> *ptrRegionNumber,
           const Array<double> *ptrRegionArea,
           const bool &scheduleTargets);

    virtual ~Target();

    static int GetQualityLevel(int qualityIndex,
                               int sensorIndex);

    void   AddCollectionProblemSet(const string &fcpsID,
                                   const Array<double> *ptrQuality,
                                   const Array<int> *ptrPriority,
                                   const Array<char> *ptrPreference,
                                   const Array<char> *ptrMode,
                                   const Array<double> *ptrSensorScore);
    void   DetermineBestCollectionProblemSet();

    int  GetCollectedPriority() const;
    int  GetRequiredQuality(int sensorIndex) const;

    int   GetMissionIndex() const;
    int   GetMissionIndex(int sensorIndex) const;
    int   GetModeIndex(int sensorIndex) const;
    int   GetQualityIndex(int sensorIndex) const;
    int   GetPriorityIndex(int sensorIndex) const;
    int   GetScoreValueForSort(const Sensor *ptrSensor) const;

    double  GetTotalTargetArea() const;
    double  GetTargetAreaCollected() const;
    double  GetTargetAreaCollected(int sensorIndex) const;
    double  GetTotalScore() const;
    double  GetSensorScore(int sensorIndex) const;

    bool   CanCollectWithSensor(int sensorIndex) const;
    bool   IsCommonCountdown() const;
    bool   IsStereoMode(int sensorIndex) const;

    static inline int  GetMissionNumber(int missionIndex);
    static inline int  GetPriorityLevel(int priorityIndex);

    static inline int   GetNumberOfMissions();
    static inline int   GetNumberOfQualities();
    static inline int   GetNumberOfPriorities();
    static inline int   GetNumberOfModes();
    static inline int   ConvertMissionToIndex(int missionNumber);
    static inline int   ConvertMissionStringToIndex(const string &missionString);
    static inline int   ConvertQualityToIndex(int qualityLevel);
    static inline int   ConvertPriorityToIndex(int priorityNumber);
    static inline int   ConvertModeToIndex(const char &modeCharacter);
    static inline int   ConvertModeStringToIndex(const string &modeString);

    static inline double  GetScoreMultiplier();

    static inline bool   IsStereoMode(const char &modeCharacter);
    static inline bool   IsPointTarget(int missionIndex);

    static inline string GetMissionString(int missionIndex);
    static inline string GetModeString(int modeIndex);

    inline void   SetAchievableQuality(int sensorIndex,
                                       int quality);
    inline void   SetCollectSensorIndex(int sensorIndex);

    inline int  GetAchievableQuality(int sensorIndex) const;
    inline int  GetMissionNumber() const;

    inline int   GetDeckIndex() const;
    inline int   GetMissionIndex(const char &collectionMode) const;
    inline int   GetNumberOfCPSs() const;
    inline int   GetScoreValueForSort() const;

    inline bool   HasAchievableCollectionProblemSet() const;
    inline bool   IsPointTarget() const;
    inline bool   IsAreaTarget() const;

    inline string GetTargetID() const;

    inline CollectionProblemSet* GetBestAchievableCPS() const;

    inline const LinkList<TargetRegion>* GetTargetRegionList() const;

    inline const LinkList<CollectionProblemSet>* GetCollectionProblemSetList() const;

protected:

private:

    void  DetermineSensorPreference(const Array<char> *ptrPreference);

    static inline string GetClassName();

    static const int  NUMBER_OF_QUALITIES_s;
    static const int  NUMBER_OF_PRIORITIES_s;
    static const double  SCORE_MULTIPLIER_s;
    static const char   MONO_MODE_IDENTIFIER_s;
    static const char   STEREO_MODE_IDENTIFIER_s;

    string                          targetID_a;
    int                           missionNumber_a;
    int                           deckIndex_a;
    int                           collectSensorIndex_a;
    Array<int>                   *ptrAchievableQuality_a;
    CollectionProblemSet           *ptrBestAchievableCPS_a;
    LinkList<CollectionProblemSet> *ptrCollectionProblemSetList_a;
    LinkList<TargetRegion>         *ptrTargetRegionList_a;
};

inline
void
Target::SetAchievableQuality(int sensorIndex,
                             int quality)
{
    if (quality > ptrAchievableQuality_a->ElementAt(sensorIndex)) {
        (*ptrAchievableQuality_a)[sensorIndex] = quality;
    }

    return;
}

inline
void
Target::SetCollectSensorIndex(int sensorIndex)
{
    if (IsCommonCountdown() == false) {
        collectSensorIndex_a = sensorIndex;
    }

    return;
}

inline
int
Target::GetAchievableQuality(int sensorIndex) const
{
    int qualityLevel = 0;

    if (ptrAchievableQuality_a != NULL) {
        qualityLevel = ptrAchievableQuality_a->ElementAt(sensorIndex);
    }

    return(qualityLevel);
}

inline
int
Target::GetNumberOfMissions()
{
    return(NUMBER_OF_MISSIONS);
}

inline
int
Target::GetNumberOfQualities()
{
    return(NUMBER_OF_QUALITIES_s);
}

inline
int
Target::GetNumberOfPriorities()
{
    return(NUMBER_OF_PRIORITIES_s);
}

inline
int
Target::GetNumberOfModes()
{
    return(NUMBER_OF_MODES);
}

inline
int
Target::ConvertMissionToIndex(int missionNumber)
{
    int missionIndex = -1;

    if ((missionNumber > 0) && (missionNumber <= NUMBER_OF_MISSIONS)) {
        missionIndex = missionNumber - 1;
    }

    return(missionIndex);
}

inline
int
Target::ConvertQualityToIndex(int qualityLevel)
{
    int qualityIndex = -1;

    if (qualityLevel > 0) {
        qualityIndex = ((qualityLevel + 9) / 10) - 1;
    }

    return(qualityIndex);
}

inline
int
Target::ConvertModeToIndex(const char &modeCharacter)
{
    int modeIndex = -1;

    if (modeCharacter == MONO_MODE_IDENTIFIER_s) {
        modeIndex = MONO;
    } else if (modeCharacter == STEREO_MODE_IDENTIFIER_s) {
        modeIndex = STEREO;
    }

    return(modeIndex);
}

inline
int
Target::ConvertPriorityToIndex(int priorityNumber)
{
    int priorityIndex = -1;

    if (priorityNumber > 0) {
        priorityIndex = (priorityNumber - 100) / 50;
    }

    return(priorityIndex);
}

inline
int
Target::ConvertModeStringToIndex(const string &modeString)
{
    int modeIndex = -1;

    if (modeString == "MONO") {
        modeIndex = MONO;
    } else if (modeString == "STEREO") {
        modeIndex = STEREO;
    }

    return(modeIndex);
}

inline
double
Target::GetScoreMultiplier()
{
    return(SCORE_MULTIPLIER_s);
}

inline
bool
Target::IsStereoMode(const char &modeCharacter)
{
    return(modeCharacter == STEREO_MODE_IDENTIFIER_s);
}

inline
bool
Target::IsPointTarget(int missionIndex)
{
    return(missionIndex <= GROUP_POINT);
}

inline
int
Target::ConvertMissionStringToIndex(const string &missionString)
{
    int missionIndex = -1;

    if (missionString == "INTEL") {
        missionIndex = INTEL_POINT;
    } else if (missionString == "MCG_PT") {
        missionIndex = MCG_POINT;
    } else if (missionString == "GRP_PT") {
        missionIndex = GROUP_POINT;
    } else if (missionString == "BAS") {
        missionIndex = BROAD_AREA_SEARCH;
    } else if (missionString == "MCG_MN") {
        missionIndex = MCG_MONO_BOX;
    } else if (missionString == "LAT") {
        missionIndex = LARGE_AREA_TARGET;
    } else if (missionString == "MCG_ST") {
        missionIndex = MCG_STEREO_AREA;
    } else if (missionString == "DSA") {
        missionIndex = DIRECTED_SEARCH_AREA;
    } else if (missionString == "LOC") {
        missionIndex = LINE_OF_COMMUNICATION;
    }//RAGUSA 1/10/2018 - Add CRISIS Mission for Crisis Region Mode
    else if (missionString == "CRISIS") {
        missionIndex = CRISIS;
    }

    return(missionIndex);
}

inline
string
Target::GetMissionString(int missionIndex)
{
    string missionString = "";

    if (missionIndex == INTEL_POINT) {
        missionString = "INTEL";
    } else if (missionIndex == MCG_POINT) {
        missionString = "MCG_PT";
    } else if (missionIndex == GROUP_POINT) {
        missionString = "GRP_PT";
    } else if (missionIndex == BROAD_AREA_SEARCH) {
        missionString = "BAS";
    } else if (missionIndex == MCG_MONO_BOX) {
        missionString = "MCG_MN";
    } else if (missionIndex == LARGE_AREA_TARGET) {
        missionString = "LAT";
    } else if (missionIndex == MCG_STEREO_AREA) {
        missionString = "MCG_ST";
    } else if (missionIndex == DIRECTED_SEARCH_AREA) {
        missionString = "DSA";
    } else if (missionIndex == LINE_OF_COMMUNICATION) {
        missionString = "LOC";
    }//RAGUSA 1/10/2018 - Add CRISIS Mission for Crisis Region Mode
    else if (missionIndex == CRISIS) {
        missionString = "CRISIS";
    }

    return(missionString);
}

inline
string
Target::GetModeString(int modeIndex)
{
    string modeString = "";

    if (modeIndex == MONO) {
        modeString = "MONO";
    } else if (modeIndex == STEREO) {
        modeString = "STEREO";
    }

    return(modeString);
}

inline
int
Target::GetMissionNumber(int missionIndex)
{
    return(missionIndex + 1);
}

inline
int
Target::GetPriorityLevel(int priorityIndex)
{
    return((priorityIndex * 50) + 100);
}

inline
int
Target::GetMissionNumber() const
{
    return(missionNumber_a);
}

inline
int
Target::GetDeckIndex() const
{
    return(deckIndex_a);
}

inline
int
Target::GetMissionIndex(const char &collectionMode) const
{
    int missionIndex = ConvertMissionToIndex(missionNumber_a);

    if ( (missionIndex == MCG_MONO_BOX) &&
            (collectionMode == STEREO_MODE_IDENTIFIER_s) ) {
        missionIndex = MCG_STEREO_AREA;
    }

    return(missionIndex);
}

inline
bool
Target::HasAchievableCollectionProblemSet() const
{
    return(ptrBestAchievableCPS_a != NULL);
}

inline
bool
Target::IsPointTarget() const
{
    return(missionNumber_a <= BROAD_AREA_SEARCH);
}

inline
bool
Target::IsAreaTarget() const
{
    return(missionNumber_a >= MCG_MONO_BOX);
}

inline
string
Target::GetTargetID() const
{
    return(targetID_a);
}

inline
const LinkList<TargetRegion>*
Target::GetTargetRegionList() const
{
    return(ptrTargetRegionList_a);
}

inline
int
Target::GetNumberOfCPSs() const
{
    int numberOfCPSs = 0;

    if (ptrCollectionProblemSetList_a != NULL) {
        numberOfCPSs = ptrCollectionProblemSetList_a->Size();
    }

    return(numberOfCPSs);
}

inline
int
Target::GetScoreValueForSort() const
{
    return((int)(GetTotalScore() * SCORE_MULTIPLIER_s));
}

inline
CollectionProblemSet*
Target::GetBestAchievableCPS() const
{
    return(ptrBestAchievableCPS_a);
}

inline
const LinkList<CollectionProblemSet>*
Target::GetCollectionProblemSetList() const
{
    return(ptrCollectionProblemSetList_a);
}

inline
string
Target::GetClassName()
{
    return(TARGET_H);
}

#endif
