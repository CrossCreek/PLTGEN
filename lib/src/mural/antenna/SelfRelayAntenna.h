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
 * SelfRelayAntenna.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef SELF_RELAY_ANTENNA_H
#define SELF_RELAY_ANTENNA_H "SelfRelayAntenna"

#include "UserAntenna.h"

using namespace std;

/**
 * Child class of UserAntenna which represents User to User communication.
 * Used with the Network allocator (non-standard)
 *
 * SelfRelayAntenna is created and owned by an instance of the
 * UserVehicle class and is a child of the UserAntenna class.
 *
 * Class Attribute Descriptions
 *
 * Name                    Description (units)
 * -------------           -----------------------------------------------------
 * ptrTransmitLinkArray_a* array of booleans indicating whether a link is being
 *                           transmitted from this SRA (true) or being received
 *                           by this SRA (false).  Matches up with the Link
 *                           instances on ptrLinksList_a and assist with the
 *                           destruction of these links
 * antennaCapability_a     indicates if this is a TRANS, RECV, or DUPLEX antenna
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class SelfRelayAntenna : public UserAntenna
{
public:

    SelfRelayAntenna();

    explicit SelfRelayAntenna(shared_ptr<UserVehicle> ptrUser);

    virtual ~SelfRelayAntenna();

    SelfRelayAntenna& operator =  (const SelfRelayAntenna &copyMe);
    bool              operator == (const SelfRelayAntenna &compareMe);
    bool              operator != (const SelfRelayAntenna &compareMe);

    void   PrintAttributes(ostream &outfile) const;

    inline void DetermineTransmitLink(const string &transmitDesignator);

    inline bool IsTransmittingAntenna() const;
    inline bool IsReceiveAntenna() const;

	string GetClassName();

protected:

private:

    Array<bool>         *ptrTransmitLinkArray_a;
};

inline
void
SelfRelayAntenna::DetermineTransmitLink(const string &transmitDesignator)
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
SelfRelayAntenna::IsTransmittingAntenna() const
{
    return(antennaCapability_a != RECEIVE);
}

inline
bool
SelfRelayAntenna::IsReceiveAntenna() const
{
    return(antennaCapability_a != TRANSMIT);
}


inline
string
SelfRelayAntenna::GetClassName()
{
    return(SELF_RELAY_ANTENNA_H);
}


#endif
