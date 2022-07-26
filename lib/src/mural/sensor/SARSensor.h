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
 * SARSensor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef SAR_SENSOR_H
#define SAR_SENSOR_H "SARSensor"

#include "Sensor.h"

#include "general/array/Array2D.h"

using namespace std;

/**
 * SARSensor is the class that models an synthetic aperature radar
 * sensor that can be attached to a data provider.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * nrisD1_a                     NRIS Calculation coefficient D1
 * nrisD2_a                     NRIS Calculation coefficient D2
 * nrisD3_a                     NRIS Calculation coefficient D3
 * nrisD4_a                     NRIS Calculation coefficient D4
 * nrisD5_a                     NRIS Calculation coefficient D5
 * nrisD6_a                     NRIS Calculation coefficient D6
 * imageProductResolution_a     SAR gsd
 * ptrCollectionRate_a*         rate at which targets are collected by
 *                                collection rate type and quality
 * ptrResourceWeight_a*         score multiplier to targets by quality
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class SARSensor : public Sensor
{
public:

    enum COLLECTION_RATE_TYPE {
        POINT = 0,
        AREA = 1,
        NUMBER_OF_COLLECTION_TYPES = 2
    };

    SARSensor();
    SARSensor(shared_ptr<InputDataElement> ptrInputDataElement,
              const SENSOR_TYPE &type);
    SARSensor(const SARSensor &copyMe);
    virtual ~SARSensor();

    SARSensor&      operator =  (const SARSensor &copyMe);
    bool            operator == (const SARSensor &compareMe) const;
    bool            operator != (const SARSensor &compareMe) const;
    bool            operator <  (const SARSensor &compareMe) const;

    static string  GetCollectionRateString(int typeIndex);

    int    GetSensorIndex() const;

    void   CheckInputData(bool runTimelineGenerator = false) const;
    void   PrintAttributes(ostream &outfile) const;

    virtual double  GetCollectionRate(int missionIndex,
                                      int qualityIndex) const;
    virtual double  GetResourceWeight(int missionIndex,
                                      int qualityIndex) const;

protected:

private:

    static int  GetCollectionRateTypeIndex(const string &typeString);
    static int  ConvertCollectionRateIndex(int missionIndex);

    void ExtractSARSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement);

    static inline string GetClassName();

    Array2D<double>     *ptrCollectionRate_a;
    Array<double>       *ptrResourceWeight_a;
};

inline
string
SARSensor::GetClassName()
{
    return(SAR_SENSOR_H);
}

#endif
