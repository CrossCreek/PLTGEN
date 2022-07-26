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
 * RelayPointingAntenna.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef RELAY_POINTING_ANTENNA_H
#define RELAY_POINTING_ANTENNA_H "RelayPointingAntenna"

#include "RelayAntenna.h"

using namespace std;

/**
 * Child Antenna class which represents relay to relay communication.  Used with
 * the Network allocator and heritage "RATT" capabilities (non-standard)
 *
 * RelayPointingAntenna is created and owned by an instance of the
 *     RelaySatellite class and is a child of the RelayAntenna class.
 *
 * Class Attribute Descriptions
 *
 * Name                    Description (units)
 * -------------           -----------------------------------------------------
 * ptrTransmitLinkArray_a* array of booleans indicating whether a link is being
 *                           transmitted from this RPA (true) or being received
 *                           by this RPA (false) and matches up with the Link
 *                           instances on ptrLinksList_a to assist with the
 *                           destruction of these links
 * antennaCapability_a     indicates if this is a TRANS, RECV, or DUPLEX antenna
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class RelayPointingAntenna : public RelayAntenna
{
public:

    RelayPointingAntenna();



    explicit RelayPointingAntenna(shared_ptr<RelaySatellite> ptrRelay);

    virtual ~RelayPointingAntenna();

    RelayPointingAntenna& operator =  (const RelayPointingAntenna &copyMe);
    bool                  operator == (const RelayPointingAntenna &compareMe);
    bool                  operator != (const RelayPointingAntenna &compareMe);

    void   PrintAttributes(ostream &outfile) const;

    inline void DetermineTransmitLink(const string &transmitDesignator);

    inline bool IsTransmittingAntenna() const;
    inline bool IsReceiveAntenna() const;

protected:

private:




    Array<bool>         *ptrTransmitLinkArray_a;
    ANTENNA_CAPABILITY   antennaCapability_a;
};

inline
void
RelayPointingAntenna::DetermineTransmitLink(const string &transmitDesignator)
{
    // only concerned if this antenna can transmit AND receive (i.e. DUPLEX)
    if (antennaCapability_a == DUPLEX) {
        bool isTransmitLink = false;

        if (ptrTransmitLinkArray_a == NULL) {
            ptrTransmitLinkArray_a = new Array<bool>();
        }

        if (transmitDesignator == GetDataProviderDesignator()) {
            isTransmitLink = true;
        }

        ptrTransmitLinkArray_a->AddElement(isTransmitLink);
    }

    return;
}

inline
bool
RelayPointingAntenna::IsTransmittingAntenna() const
{
    return(antennaCapability_a != RECEIVE);
}

inline
bool
RelayPointingAntenna::IsReceiveAntenna() const
{
    return(antennaCapability_a != TRANSMIT);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
RelayPointingAntenna::GetClassName()
{
    return(RELAY_POINTING_ANTENNA_H);
}
*/

#endif
