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
 * TargetDeck.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef TARGET_DECK_H
#define TARGET_DECK_H "TargetDeck"

#include <fstream>
#include <string>
#include <sstream>
#include <memory>


#include "general/array/Array2D.h"
#include "general/array/LinkList.h"

using namespace std;

class CountryFactor;
class InputDataElement;
class Target;
class UserVehicle;

/**
 *  Class to parse a Target deck and retain the relevent data.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class TargetDeck
{
public:

    enum EFFECTIVITY {
        UNKNOWN_EFFECTIVITY = -1,
        MONTH_ONLY = 0,
        MONTH_AND_DAY = 1,
        ALL_ACTIVE_TARGETS = 2,
        ALL_TARGETS = 3
    };

    TargetDeck();




    TargetDeck(const TargetDeck &targetDeck);

    TargetDeck(int targetDeckIndex,
               const Array2D<bool> *ptrActiveRequirements,
               int numberOfResources);
    virtual ~TargetDeck();

    static void   DestroyTargetDeck();
    static void   ExtractTargetDeckInfo(shared_ptr<InputDataElement> ptrElement,
                                        const bool &runTargetScheduler);
    static void   PrintAttributes(ostream &outfile);

    static string GetTargetDeckFileName(int deckIndex);

    void   PrintSummary(ofstream &outfile,
                        const Array<bool> *ptrActiveSensor) const;
    void   DetermineBestCollectionProblemSet() const;

    static inline void   AddTargetDeckFileName(const string &fileName);

    static inline int  GetSubPriorityLevel(int priorityNumber);

    static inline int   GetNumberOfTargetDecks();
    static inline int   GetTargetDeckIndex(const string &fileName);

    static inline bool   IsTargetGroup(const string &groupName);
    static inline bool   PrintDeckAsRequirements();
    static inline bool   ScheduleTargets();

    static inline string GetEffectivityString();

    static inline string* GetDeckFileBaseDirectory();

    inline string GetDeckFileName() const;

    inline LinkList<Target>* GetTargetList() const;

protected:

private:

    void   RetrieveTargetsAndBuildRequirements(int targetDeckIndex,
            const Array2D<bool> *ptrActiveRequirements,
            int numberOfResources);
    void   RetrieveCollectionData(istringstream &inputStream,
                                  const Array<double> *ptrQuality,
                                  Array<int> *&ptrPriority,
                                  Array<char> *&ptrPreference,
                                  Array<char> *&ptrMode,
                                  int &sampleSize,
                                  double &periodicity,
                                  string &countryCode,
                                  string &fcpsID) const;
    void   CalculateSensorScore(Array<double> *&ptrSensorScore,
                                const Array<char> *ptrMode,
                                const Array<int> *ptrPriority,
                                const string &countryCode,
                                int duration,
                                int sampleSize,
                                double periodicity,
                                double totalTargetArea) const;
    void   BuildRequirements(Target *ptrTarget,
                             const Array2D<bool> *ptrActiveRequirements,
                             int numberOfResources,
                             double totalTargetArea) const;

    int  GetEffectiveSimulationTimes(int &simulationStart,
                                     int &simulationEnd) const;

    bool   ValidTarget(istringstream &inputStream,
                       int simulationStart,
                       int simulationEnd,
                       int startYear,
                       const Array2D<bool> *ptrActiveRequirements,
                       string &targetID,
                       int &duration,
                       int &missionNumber,
                       Array<double> *&ptrQuality,
                       const bool &anySpectralSensors) const;
    bool   EffectiveTarget(const char &statusFlag,
                           const string &dateString,
                           int duration,
                           int simulationStart,
                           int simulationEnd,
                           int startYear) const;

    static double  GetCountryFactor(const string &countryCode);

    static inline string GetClassName();

    static inline EFFECTIVITY GetEffectivity(const string &effectivityString);

    static Array<int>            *ptrSubPriorityThreshold_s;
    static double                    opticalPriorityFactorCoeff_s;
    static double                    opticalPriorityFactorExpnt_s;
    static double                    radarPriorityFactorCoeff_s;
    static double                    radarPriorityFactorExpnt_s;
    static double                    areaFactor_s;
    static const char               ACTIVE_TARGET_ID_s;
    static bool                     printDeckAsRequirements_s;
    static bool                     scheduleTargets_s;
    static bool                     useFullOpticalScoreEquation_s;
    static string                  *ptrDeckFileBaseDirectory_s;
    static Array<string>           *ptrTargetDeckFileNameArray_s;
    static Array<string>           *ptrTargetGroupArray_s;
    static EFFECTIVITY              effectivity_s;
    static LinkList<CountryFactor> *ptrCountryFactorList_s;

    string                deckFileName_a;
    LinkList<Target>     *ptrTargetList_a;
};

inline
void
TargetDeck::AddTargetDeckFileName(const string &fileName)
{
    if (ptrTargetDeckFileNameArray_s == NULL) {
        ptrTargetDeckFileNameArray_s = new Array<string>(1, fileName);
    } else {
        ptrTargetDeckFileNameArray_s->AddUniqueElement(fileName);
    }
}

inline
int
TargetDeck::GetSubPriorityLevel(int priorityNumber)
{
    int subPriorityLevel = 0;

    if (ptrSubPriorityThreshold_s != NULL) {
        bool foundLevel = false;

        while ( (subPriorityLevel < ptrSubPriorityThreshold_s->Size()) &&
                (foundLevel == false) ) {
            if (priorityNumber <= ptrSubPriorityThreshold_s->ElementAt(subPriorityLevel)) {
                foundLevel = true;
            }

            ++subPriorityLevel;
        }
    }

    return(subPriorityLevel);
}

inline
int
TargetDeck::GetNumberOfTargetDecks()
{
    int numberOfTargetDecks = 0;

    if (ptrTargetDeckFileNameArray_s != NULL) {
        numberOfTargetDecks = ptrTargetDeckFileNameArray_s->Size();
    }

    return(numberOfTargetDecks);
}

inline
int
TargetDeck::GetTargetDeckIndex(const string &fileName)
{
    int targetDeckIndex = -1;

    if (ptrTargetDeckFileNameArray_s != NULL) {
        targetDeckIndex = ptrTargetDeckFileNameArray_s->FindIndex(fileName);
    }

    return(targetDeckIndex);
}

inline
bool
TargetDeck::IsTargetGroup(const string &groupName)
{
    bool isTargetGroup = false;

    if (ptrTargetGroupArray_s != NULL) {
        isTargetGroup = ptrTargetGroupArray_s->ValueExists(groupName);
    }

    return(isTargetGroup);
}

inline
bool
TargetDeck::PrintDeckAsRequirements()
{
    return(printDeckAsRequirements_s);
}

inline
bool
TargetDeck::ScheduleTargets()
{
    return(scheduleTargets_s);
}

inline
string
TargetDeck::GetDeckFileName() const
{
    return(deckFileName_a);
}

inline
TargetDeck::EFFECTIVITY
TargetDeck::GetEffectivity(const string &effectivityString)
{
    EFFECTIVITY returnEffecitivity = UNKNOWN_EFFECTIVITY;

    if (effectivityString == "MO") {
        returnEffecitivity = MONTH_ONLY;
    } else if (effectivityString == "MAD") {
        returnEffecitivity = MONTH_AND_DAY;
    } else if (effectivityString == "ACTIVE") {
        returnEffecitivity = ALL_ACTIVE_TARGETS;
    } else if (effectivityString == "ALL") {
        returnEffecitivity = ALL_TARGETS;
    }

    return(returnEffecitivity);
}

inline
string
TargetDeck::GetEffectivityString()
{
    string effectivityString = "UNKNOWN";

    if (effectivity_s == MONTH_ONLY) {
        effectivityString = "MO";
    } else if (effectivity_s == MONTH_AND_DAY) {
        effectivityString = "MAD";
    } else if (effectivity_s == ALL_ACTIVE_TARGETS) {
        effectivityString = "ACTIVE";
    } else if (effectivity_s == ALL_TARGETS) {
        effectivityString = "ALL";
    }

    return(effectivityString);
}

inline
string*
TargetDeck::GetDeckFileBaseDirectory()
{
    return(ptrDeckFileBaseDirectory_s);
}

inline
LinkList<Target>*
TargetDeck::GetTargetList() const
{
    return(ptrTargetList_a);
}

inline
string
TargetDeck::GetClassName()
{
    return(TARGET_DECK_H);
}

#endif
