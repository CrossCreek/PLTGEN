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
 * AntennaExtractor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#ifndef ANTENNA_EXTRACTOR_H
#define ANTENNA_EXTRACTOR_H "AntennaExtractor"

#include <fstream>
#include <vector>
#include <memory>

#include "mural/antenna/Antenna.h"

using namespace std;

class InputDataElement;
class Antenna;
class UserTransmitAntenna;
class UserPointingAntenna;
class RelayPointingAntenna;
class GroundPointingAntenna;
class ReceiveFacilityAntenna;
class SelfRelayAntenna;
class DirectDownlinkAntenna;
class AzimuthAngleData;
class LinkConstraintData;

/**
 * Class which extracts the Antenna information from an
 * input data element
 *
 * Common types of antennas in MURAL are:
 * UTA - User Transmit Antenna (transmitter on user)
 * UPA - User Pointing Antenna (receiver on relay)
 * GPA - Ground Pointer Antenna (transmitter on relay)
 * RF - Receive Facility Antenna (receiver on ground)
 *
 * Other antenna types:
 * RPA - Relay Pointing Antenna (transmitter/receiver on relay)
 * SRA - Self Relay Antenna (transmitter/receiver on user)
 * DDA - Direct Downlink Antenna (transmitter on user)
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class AntennaExtractor
{
public:

    AntennaExtractor();
    virtual ~AntennaExtractor();

    /**
     * Predicate for the std::sort method to sort AzimuthAngleData by Azimuth value.  Returns true if
     * ptrI->GetAzimuthValue() < ptrJ->GetAzimuthValue()
     *
     * @param ptrI the left AzimuthAngleData
     * @param ptrJ the right AzimuthAngleData
     * @return bool
     */
    static bool sortAzEl(const shared_ptr<AzimuthAngleData> &ptrI, const shared_ptr<AzimuthAngleData> ptrJ);

    /**
     * Extract the user transmit antenna from an input data element
     *
     * @param ptrElement pointer to the containing element (for the coordinate frame)
     * @param ptrUTA the user transmit antenna to populate
     * @param ptrInputDataElement the input data element
     */
    void  ExtractUTA(const shared_ptr<Element> &ptrElement,
                     const shared_ptr<UserTransmitAntenna> &ptrUTA,
                     const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Extract the direct downlink antenna from an input data element
     *
     * @param ptrElement pointer to the containing element (for the coordinate frame)
     * @param ptrDDA the direct downlink antenna to populate
     * @param ptrInputDataElement the input data element
     */
    void  ExtractDDA(const shared_ptr<Element> &ptrElement,
                     const shared_ptr<DirectDownlinkAntenna> &ptrDDA,
                     const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Extract the self relay antenna from an input data element
     *
     * @param ptrElement pointer to the containing element (for the coordinate frame)
     * @param ptrSRA the self relay antenna to populate
     * @param ptrInputDataElement the input data element
     */
    void  ExtractSRA(const shared_ptr<Element> &ptrElement,
                     const shared_ptr<SelfRelayAntenna> &ptrSRA,
                     const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Extract the user pointing antenna from an input data element
     *
     * @param ptrElement pointer to the containing element (for the coordinate frame)
     * @param ptrUPA the user pointing antenna to populate
     * @param ptrInputDataElement the input data element
     */
    void  ExtractUPA(const shared_ptr<Element> &ptrElement,
                     const shared_ptr<UserPointingAntenna> &ptrUPA,
                     const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Extract the ground pointing antenna from an input data element
     *
     * @param ptrElement pointer to the containing element (for the coordinate frame)
     * @param ptrGPA the ground pointing antenna to populate
     * @param ptrInputDataElement the input data element
     */
    void  ExtractGPA(const shared_ptr<Element> &ptrElement,
                     const shared_ptr<GroundPointingAntenna> &ptrGPA,
                     const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Extract the relay pointing antenna from an input data element
     *
     * @param ptrElement pointer to the containing element (for the coordinate frame)
     * @param ptrRPA the relay pointing antenna to populate
     * @param ptrInputDataElement the input data element
     */
    void  ExtractRPA(const shared_ptr<Element> &ptrElement,
                     const shared_ptr<RelayPointingAntenna> &ptrRPA,
                     const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Extract the receive facility antenna from an input data element.  The MURAL
     * ground station file is where this block should reside.
     *
     * @param ptrElement pointer to the containing element (for the coordinate frame)
     * @param ptrRF the receive facility antenna to populate
     * @param ptrInputDataElement the input data element
     */
    void  ExtractRF(const shared_ptr<Element> &ptrElement,
                    const shared_ptr<ReceiveFacilityAntenna> &ptrRF,
                    const shared_ptr<InputDataElement> &ptrInputDataElement);


    /**
     * Checks the user transmit antenna for errors
     *
     * @param ptrUTA the user transmit antenna to check
     */
    void  CheckUTAInputs(const shared_ptr<UserTransmitAntenna> &ptrUTA);

    /**
     * Checks the direct downlink antenna for errors
     *
     * @param ptrDDA the direct downlink antenna to check
     */
    void  CheckDDAInputs(const shared_ptr<DirectDownlinkAntenna> &ptrDDA);

    /**
     * Checks the self relay antenna for errors
     *
     * @param ptrSRA the self relay antenna to check
     */
    void  CheckSRAInputs(const shared_ptr<SelfRelayAntenna> &ptrSRA);

    /**
     * Checks the user pointing antenna for errors
     *
     * @param ptrUPA the user pointing antenna to check
     */
    void  CheckUPAInputs(const shared_ptr<UserPointingAntenna> &ptrUPA);

    /**
     * Checks the ground pointing antenna for errors
     *
     * @param ptrGPA the ground pointing antenna to check
     */
    void  CheckGPAInputs(const shared_ptr<GroundPointingAntenna> &ptrGPA);

    /**
     * Checks the relay pointing antenna for errors
     *
     * @param ptrRPA the relay pointing antenna to check
     */
    void  CheckRPAInputs(const shared_ptr<RelayPointingAntenna> &ptrRPA);

    /**
     * Checks the receive facility antenna for errors
     *
     * @param ptrRF the receive facility antenna to check
     */
    void  CheckRFInputs(const shared_ptr<ReceiveFacilityAntenna> &ptrRF);


protected:

private:

    /**
     * Extract general antenna information from an input data element
     *
     * @param ptrElement pointer to the containing element (for the coordinate frame)
     * @param ptrAntenna the antenna to populate
     * @param ptrInputDataElement the input data element
     */
    void ExtractAntennaInfo(const shared_ptr<Element> &ptrElement,
                            const shared_ptr<Antenna> &ptrAntenna,
                            const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Check general antenna information
     *
     * @param ptrAntenna the antenna to check
     */




    /**
     * Checks the azimuth angle data given the max/min elevations specified
     *
     * @param ptrAntenna the antenna to check
     * @param maximumMaxElev the maximum elevation
     * @param minimumMaxElev the minimum elevation
     */



    /**
     * Method to convert a string to a FREQUENCY_BAND enum.
     */
    Antenna::FREQUENCY_BAND GetFrequencyBandValue(const string &type) const;

    /**
     * Method to convert a string to an Antenna Capability
     */
    AntennaBase::ANTENNA_CAPABILITY GetAntennaCapabilityValue(const string &type) const;

    static inline string GetClassName();
};

inline
string
AntennaExtractor::GetClassName()
{
    return(ANTENNA_EXTRACTOR_H);
}

#endif
