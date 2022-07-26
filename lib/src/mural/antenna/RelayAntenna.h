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
 * RelayAntenna.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef RELAY_ANTENNA_H
#define RELAY_ANTENNA_H "RelayAntenna"

#include "Antenna.h"

using namespace std;

class RelaySatellite;

/**
 * RelayAntenna is the parent class created and inherited by either the
 * UserPointingAntenna, RelayPointingAntenna, or GroundPointingAntenna
 * child classes.  It is a child of the Antenna class.  This maintains the
 * data that is common to the three children classes.
 *
 * Class Attribute Descriptions
 *
 * Name                        Description (units)
 * -------------               -------------------------------------------------
 * numberOfTransmitChannels_a  available channels to transmit data used for
 *                               throughput output
 * ptrRelaySatellite_a*        pointer to the relay that owns this antenna
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class RelayAntenna : public Antenna
{
public:

    RelayAntenna();
    RelayAntenna(shared_ptr<RelaySatellite> ptrRelay,
                 int previousCapacity = 0);
    virtual ~RelayAntenna();

    RelayAntenna&   operator =  (const RelayAntenna &copyMe);
    bool            operator == (const RelayAntenna &compareMe);
    bool            operator != (const RelayAntenna &compareMe);

    void   PrintAttributes(ostream &outfile) const;

    int  GetDataProviderIndex() const;
    int  GetDataProviderNumber() const;

    LinkList<Link>* GetCrosslinks(const string &userDesignator) const;

    void   SetNumberOfTransmitChannels(int numberOfTransmitChannels);

    virtual inline void DetermineTransmitLink(const string &transmitDesignator);

    virtual inline bool IsTransmittingAntenna() const;
    virtual inline bool IsReceiveAntenna() const;

    inline void   ResetNumberOfTransmitChannels();
    inline void   SetMinimumNumberOfTransmitChannels();

    inline int  GetNumberOfTransmitChannels() const;

    inline double GetDataRatePerChannel() const;

    /**
     * Method cast the element on Antenna base to a RelaySatellite
     * (for MURAL compatability)
     *
     * @return shared_ptr<RelaySatellite>
     */
    shared_ptr<RelaySatellite> GetRelay() const;

protected:

    int               numberOfTransmitChannels_a;

private:



};

inline
void
RelayAntenna::DetermineTransmitLink(const string &transmitDesignator)
{
    return;
}

inline
void
RelayAntenna::ResetNumberOfTransmitChannels()
{
    numberOfTransmitChannels_a = 0;
}

inline
void
RelayAntenna::SetMinimumNumberOfTransmitChannels()
{
    if (numberOfTransmitChannels_a == 0) {
        numberOfTransmitChannels_a = 1;
    }
}

inline
int
RelayAntenna::GetNumberOfTransmitChannels() const
{
    return(numberOfTransmitChannels_a);
}

inline
double
RelayAntenna::GetDataRatePerChannel() const
{
    double dataRatePerChannel = 0.0;

    if (numberOfTransmitChannels_a > 0) {
        dataRatePerChannel = dataRate_a / (double)numberOfTransmitChannels_a;
    }

    return(dataRatePerChannel);
}

inline
bool
RelayAntenna::IsTransmittingAntenna() const
{
    return(false);
}

inline
bool
RelayAntenna::IsReceiveAntenna() const
{
    return(false);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
RelayAntenna::GetClassName()
{
    return(RELAY_ANTENNA_H);
}
*/

#endif
