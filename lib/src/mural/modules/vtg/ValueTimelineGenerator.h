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
 * ValueTimelineGenerator.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef VALUE_TIMELINE_GENERATOR_H
#define VALUE_TIMELINE_GENERATOR_H "ValueTimelineGenerator"

#include "TimelineGenerator.h"

using namespace std;

class DataProviderCollection;

/**
 * This is the module that creates the timeline (score profile) to feed the request
 * generator
 *
 * Name                    Description (units)
 * -------------           -----------------------------------------------------
 * resetCrosslinkStatus_s  indicates if any crosslink inview status was changed
 *                           due to pointing at a specific Region instead of
 *                           nadir pointing
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */
class ValueTimelineGenerator : public TimelineGenerator
{
public:

    ValueTimelineGenerator();
    ValueTimelineGenerator(const ValueTimelineGenerator &copyMe);
    virtual ~ValueTimelineGenerator();

    ValueTimelineGenerator& operator =  (const ValueTimelineGenerator &copyMe);
    bool            operator == (const ValueTimelineGenerator &compareMe) const;
    bool            operator != (const ValueTimelineGenerator &compareMe) const;

    void GenerateValueTimelines(const shared_ptr<DataProviderCollection> &ptrDataProviderCollection);

    static inline bool         ResetCrosslinkStatus();

protected:

    double FactorWhenPointingAtRegion(Link *ptrTransmitLink,
                                      const shared_ptr<UserVehicle> &ptrUserVehicle,
                                      Region *ptrRegion,
                                      int timeIndex,
                                      double regionScore,
                                      ofstream &valtLogFile,
                                      double azimuthOffsetAngle = 0.0,
                                      double cableWrapOffsetAngle = 0.0);

private:

    static inline string GetClassName();

    static bool          resetCrosslinkStatus_s;
};

inline
bool
ValueTimelineGenerator::ResetCrosslinkStatus()
{
    return(resetCrosslinkStatus_s);
}

inline
string
ValueTimelineGenerator::GetClassName()
{
    return(VALUE_TIMELINE_GENERATOR_H);
}

#endif

