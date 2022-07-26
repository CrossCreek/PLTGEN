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
 * SSARSensor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#ifndef SSAR_SENSOR_H
#define SSAR_SENSOR_H "SSARSensor"

#include "Sensor.h"

#include "general/array/Array2D.h"

using namespace std;


/* sensor that can be attached to a data provider. This class is modeled identically to
* SAR class.
*
* Name                         Description (units)
* -------------                ------------------------------------------
* nrisD1_a                     NRIS Calculation coefficient D1
* nrisD2_a                     NRIS Calculation coefficient D2
* nrisD3_a                     NRIS Calculation coefficient D3
* nrisD4_a                     NRIS Calculation coefficient D4
* nrisD5_a                     NRIS Calculation coefficient D5
* nrisD6_a                     NRIS Calculation coefficient D6
* imageProductResolution_a     SSAR gsd
* ptrCollectionRate_a*         rate at which targets are collected by
*                                collection rate type and quality
* ptrResourceWeight_a*         score multiplier to targets by quality
*
* @author Anthony Ragusa
* @date 11/2/2017
*/
class SSARSensor : public Sensor
{
public:

    enum COLLECTION_RATE_TYPE {
        POINT = 0,
        AREA = 1,
        NUMBER_OF_COLLECTION_TYPES = 2
    };

    SSARSensor();
    SSARSensor(shared_ptr<InputDataElement> ptrInputDataElement,
               const SENSOR_TYPE &type);
    SSARSensor(const SSARSensor &copyMe);
    virtual ~SSARSensor();

    SSARSensor&      operator =  (const SSARSensor &copyMe);
    bool            operator == (const SSARSensor &compareMe) const;
    bool            operator != (const SSARSensor &compareMe) const;
    bool            operator <  (const SSARSensor &compareMe) const;

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

    void ExtractSSARSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement);

    static inline string GetClassName();

    Array2D<double>     *ptrCollectionRate_a;
    Array<double>       *ptrResourceWeight_a;
};

inline
string
SSARSensor::GetClassName()
{
    return(SSAR_SENSOR_H);
}

#endif

