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
 * DataProviderExtractor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#ifndef DATA_PROVIDER_EXTRACTOR_H
#define DATA_PROVIDER_EXTRACTOR_H "DataProviderExtractor"

#include <fstream>
#include <vector>
#include <memory>

using namespace std;

class DataProvider;
class UserVehicle;
class RelaySatellite;
class GroundStation;
class InputDataElement;
class AntennaExtractor;

/**
 * Class which extracts the Data Provider (user/relay/ground) information from an
 * input data element
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class DataProviderExtractor
{
public:

    DataProviderExtractor();
    virtual ~DataProviderExtractor();

    /**
     * Extract the user vehicle from an input data element
     *
     * @param ptrUser the user vehicle to populate
     * @param ptrInputDataElement the input data element
     */
    void  ExtractUserVehicleInfo(const shared_ptr<UserVehicle> &ptrUser,
                                 const shared_ptr<InputDataElement> &ptrInputDataElement,
                                 const bool &isTrailVehicle);

    /**
     * Extract the relay satellite from an input data element
     *
     * @param ptrRelay the relay satellite to populate
     * @param ptrInputDataElement the input data element
     */
    void  ExtractRelaySatelliteInfo(const shared_ptr<RelaySatellite> &ptrRelay,
                                    const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Extract the ground station from an input data element
     *
     * @param ptrGroundStation the ground station to populate
     * @param ptrInputDataElement the input data element
     */
    void  ExtractGroundStationInfo(const shared_ptr<GroundStation> &ptrGround,
                                   const shared_ptr<InputDataElement> &ptrInputDataElement);

    /**
     * Checks the user vehicle inputs
     *
     * @param ptrUser the user vehicle to check
     */
    void  CheckUserVehicleInputs(const shared_ptr<UserVehicle> &ptrUser,
                                 const bool &isTrailVehicle);

    /**
     * Checks the relay satellite inputs
     *
     * @param ptrRelay the relay satellite to check
     */
    void  CheckRelaySatelliteInputs(const shared_ptr<RelaySatellite> &ptrRelay);

    /**
     * Checks the ground station inputs
     *
     * @param ptrUser the ground station to check
     */
    void  CheckGroundStationInputs(const shared_ptr<GroundStation> &ptrGround);

protected:

private:

    /**
     * Extract the data provider from an input data element
     *
     * @param ptrDataProvider the data provider to populate
     * @param ptrInputDataElement the input data element
     */
    void ExtractDataProviderInfo(const shared_ptr<DataProvider> &ptrDataProvider,
                                 const shared_ptr<InputDataElement> &ptrInputDataElement,
                                 const bool &dataTransmitter,
                                 const bool &dataCollector);

    unique_ptr<AntennaExtractor> ptrAntennaExtractor_a;

    static inline string GetClassName();
};

inline
string
DataProviderExtractor::GetClassName()
{
    return(DATA_PROVIDER_EXTRACTOR_H);
}

#endif
