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
 * BasicLink.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef BASIC_LINK_H
#define BASIC_LINK_H "BasicLink"

#include <memory>
#include <vector>
#include <string>

using namespace std;

#include "general/array/Array.h"
#include "general/time/TimePiece.h"

class Antenna;
class SpaceVector;
class DataProvider;

/**
 * Base class of Link to which handles the geometric link between a transmit and receive antenna.
 * It holds the inveiw data for the geometric link.
 *
 * Name                               Description (units)
 * -------------                      --------------------------------------------
 * ptrTransmitAntenna_a*              Pointer to the transmitting antenna of the link (shared_ptr)
 * ptrReceiveAntenna_a*               Pointer to the receive antenna of the link (shared_ptr)
 * ptrInviewStatus_a*                 Status array containing the inview codes of the link
 *
 * @author Brian Fowler
 * @date 10/27/11
 */
class BasicLink
{
public:

    enum LINK_STATUS {
        UNKNOWN = -1,
        NO_LINK = 0,
        LINK = 1,
        MAX_ELEVATION_TRANSMIT = 2,
        MIN_ELEVATION_TRANSMIT = 3,
        MAX_ELEVATION_RECEIVE = 4,
        MIN_ELEVATION_RECEIVE = 5,
        MIN_GRAZING_ALTITUDE = 6,
        MAX_RANGE_TRANSMIT = 7,
        MIN_RANGE_TRANSMIT = 8,
        MAX_SUN_SEPARATION_TRANSMIT = 9,
        MIN_SUN_SEPARATION_TRANSMIT = 10,
        MAX_SUN_SEPARATION_RECEIVE = 11,
        MIN_SUN_SEPARATION_RECEIVE = 12,
        MIN_AZIMUTH_TRANSMIT = 13,
        MIN_AZIMUTH_RECEIVE = 14,
        OUTAGE_TIME_STEP = 15,
        FULLY_DEDICATED_TIME_STEP = 16,
        NO_SUPPORTING_DOWNLINK = 17,
        STATE_OF_HEALTH_TIME_STEP = 18,
        REQUIRED_DOWNLINK_OVERHEAD = 19,
        MIN_SOLAR_INTRUSION = 20,
        POSSE_SHORT_INVIEW = 50,
        POSSE_NOT_REQUESTING_INVIEW = 51
    };

    BasicLink();
    BasicLink(const shared_ptr<Antenna> &ptrFromAntenna,
              const shared_ptr<Antenna> &ptrToAntenna,
              const Array<int> *ptrStatusArray);
    BasicLink(const BasicLink &copyMe);
    virtual ~BasicLink();

    /**
     * Method to convert LINK_STATUS to its string representation
     */
    static string GetLinkStatusString(const LINK_STATUS &status);

    /**
     * Gets the numeric value of LINK_STATUS
     */
    const int GetLinkStatusNumber(int timeIndex) const;

    /**
     * Returns true if at least one inview timestep in the range
     */
    const bool AnyInview(int startTimeIndex, int endTimeIndex) const;

    /**
     * Returns true if the timestep is inview at the timeIndex
     */
    const bool IsInview(int timeIndex) const;

    /**
     * Returns true if ALL timesteps in the range are inview
     */
    const bool IsInview(int startTimeIndex, int endTimeIndex) const;

    /**
     * Creates an inview period from the current time.  The period is returned via the
     * start/endInveiewPeriod arguments.  The first inview timestep after the currentTime
     * is the start.  The end is the first timestep after the start where the inview
     * is broken
     *
     * @param currentTime the time index to start finding an inview period for
     * @param &startInviewPeriod [return] the start of the inview period
     * @param &endInviewPeriod [return] the end of the inview period
     * @param endCheckTime a short circut timestep to stop checking
     */
    const bool GetInviewPeriod(int currentTime,
                               int &startInviewPeriod,
                               int &endInviewPeriod,
                               int endCheckTime = TimePiece::GetEndIndexTime()) const;

    /**
     * Setters for LINK_STATUS array
     */
    void SetLinkStatus(int timeIndex, const LINK_STATUS &status);
    void UpdateLinkStatus(int timeIndex, const LINK_STATUS &linkStatus);
    void ResetInviews(int startTime, int endTime, const LINK_STATUS &status);

    /**
     * Gets the first inview timestep counting up from the parameter
     */
    int GetFirstInviewTimeStep(int startingTime) const;

    /**
     * Gets the last inview timestep counting down from the parameter
     */
    int GetLastInviewTimeStep(int endingTime) const;

    /**
     * Gets the first inview timestep counting up from the start time and terminating
     * at the stopCheckTime
     */
    int GetEarliestInviewTimeStep(int startingTime, int stopCheckTime) const;

    /**
     * Gets the last inview timestep counting down from the end time and terminating
     * at the stopCheckTime
     */
    int GetLatestInviewTimeStep(int endingTime, int stopCheckTime) const;

    /**
     * Gets the last inview timestep counting from the end of the simualation day
     */
    int GetLastInview(int startTime) const;

    LINK_STATUS       GetLinkStatus(int timeIndex) const;

    /**
     * Getters to get the Antennas
     */
    const shared_ptr<Antenna> GetTransmitAntenna() const;
    const shared_ptr<Antenna> GetReceiveAntenna() const;
    string GetTransmitAntennaDesignator() const;
    string GetReceiveAntennaDesignator() const;

    /**
     * Getters to get the Providers/Elements
     */
    shared_ptr<DataProvider> GetTransmitProvider() const;
    shared_ptr<DataProvider> GetReceiveProvider() const;
    string GetTransmitDesignator() const;
    string GetReceiveDesignator() const;

    SpaceVector   GetTransmitPostion(int timeIndex) const;
    SpaceVector   GetTransmitVelocity(int timeIndex) const;
    SpaceVector   GetReceivePostion(int timeIndex) const;
    SpaceVector   GetReceiveVelocity(int timeIndex) const;

    /**
     * Gets the line of sight vector between the transmitter and reciever
     *      <recieve position> - <transmit position>
     *
     * @param timeIndex the time
     * @return SpaceVector the vector representing the line of sight
     */
    SpaceVector   GetLineOfSightVector(int timeIndex) const;

protected:

    shared_ptr<Antenna>       ptrTransmitAntenna_a;
    shared_ptr<Antenna>       ptrReceiveAntenna_a;
    shared_ptr<vector<int>>   ptrInviewStatus_a;

private:

    static inline string GetClassName();
};

inline
string
BasicLink::GetClassName()
{
    return(BASIC_LINK_H);
}

#endif
