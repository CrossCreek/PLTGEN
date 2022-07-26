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
 * WideFOVSensor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef WIDE_FOV_SENSOR_H
#define WIDE_FOV_SENSOR_H "WideFOVSensor"

#include "Sensor.h"

using namespace std;

/**
 * Class to represent a Wide Field of View Sensor
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class WideFOVSensor : public Sensor
{
public:

    WideFOVSensor();
    WideFOVSensor(shared_ptr<InputDataElement> ptrInputDataElement,
                  const SENSOR_TYPE &type);
    WideFOVSensor(const WideFOVSensor &copyMe);
    virtual ~WideFOVSensor();

    WideFOVSensor&  operator =  (const WideFOVSensor &copyMe);
    bool            operator == (const WideFOVSensor &compareMe) const;
    bool            operator != (const WideFOVSensor &compareMe) const;

    int    GetSensorIndex() const;

    void   PrintAttributes(ostream &outfile) const;

    virtual double  GetCollectionRate(int missionIndex,
                                      int qualityIndex) const;
    virtual double  GetResourceWeight(int missionIndex,
                                      int qualityIndex) const;

protected:

private:

    void ExtractWideFOVSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement);



};

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
WideFOVSensor::GetClassName()
{
    return(WIDE_FOV_SENSOR_H);
}
*/

#endif
