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
 * OverheadData.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef OVERHEAD_DATA_H
#define OVERHEAD_DATA_H "OverheadData"

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

/**
 * Data class to store overhead information of the user
 *
 * OverheadData is the class that contains the overhead data (acquire and
 *     droplink times) when making a link with a resource (i.e. relay, ground
 *     station).
 *
 * Name                           Description (units)
 * -------------                  ----------------------------------------------
 * acquisitionTime_a              time it takes to acquire the input resource
 * droplinkTime_a                 time it takes to droplink the input resource
 * makeBeforeBreakDroplinkTime_a  time it takes to droplink the input resource
 *                                  when conducting an MBB
 * makeBeforeBreakStartUpTime_a   time it takes to start up the input resource
 *                                  when conducting an MBB
 * ptrWithReourceDesignator_a*    the designator of the resource linked with
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class OverheadData
{
public:

    OverheadData();
    OverheadData(const string &withDesignator,
                 int acquireTime = -1,
                 int dropTime = -1,
                 int makeBeforeBreakDropTime = -1,
                 int makeBeforeBreakStartUpTime = -1);
    OverheadData(const OverheadData &copyMe);
    virtual ~OverheadData();

    OverheadData& operator =  (const OverheadData &copyMe);
    bool          operator == (const OverheadData &compareMe) const;
    bool          operator != (const OverheadData &compareMe) const;

    inline void   SetAcquistionTime(int acquireTime);
    inline void   SetDroplinkTime(int dropTime);
    inline void   SetMakeBeforeBreakDroplinkTime(int dropTime);
    inline void   SetMakeBeforeBreakStartUpTime(int startUp);
    inline void   SetWithResourceDesignator(const string &designator);

    inline int  GetAcquistionTime() const;
    inline int  GetDroplinkTime() const;
    inline int  GetMakeBeforeBreakDroplinkTime() const;
    inline int  GetMakeBeforeBreakStartUpTime() const;

    inline string GetWithResourceDesignator() const;

protected:

private:




    int               acquisitionTime_a;
    int               droplinkTime_a;
    int               makeBeforeBreakDroplinkTime_a;
    int               makeBeforeBreakStartUpTime_a;
    string             *ptrWithReourceDesignator_a;
};

inline
int
OverheadData::GetAcquistionTime() const
{
    return(acquisitionTime_a);
}

inline
void
OverheadData::SetAcquistionTime(int acquireTime)
{
    acquisitionTime_a = acquireTime;
}

inline
int
OverheadData::GetDroplinkTime() const
{
    return(droplinkTime_a);
}

inline
void
OverheadData::SetDroplinkTime(int dropTime)
{
    droplinkTime_a = dropTime;
}

inline
int
OverheadData::GetMakeBeforeBreakDroplinkTime() const
{
    return(makeBeforeBreakDroplinkTime_a);
}

inline
void
OverheadData::SetMakeBeforeBreakDroplinkTime(int dropTime)
{
    makeBeforeBreakDroplinkTime_a = dropTime;
}

inline
int
OverheadData::GetMakeBeforeBreakStartUpTime() const
{
    return(makeBeforeBreakStartUpTime_a);
}

inline
void
OverheadData::SetMakeBeforeBreakStartUpTime(int startUp)
{
    makeBeforeBreakStartUpTime_a = startUp;
}

inline
string
OverheadData::GetWithResourceDesignator() const
{
    return(*ptrWithReourceDesignator_a);
}

inline
void
OverheadData::SetWithResourceDesignator(const string &designator)
{
    if (ptrWithReourceDesignator_a != NULL) {
        delete ptrWithReourceDesignator_a;
    }

    ptrWithReourceDesignator_a = new string(designator);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
OverheadData::GetClassName()
{
    return(OVERHEAD_DATA_H);
}
*/

#endif
