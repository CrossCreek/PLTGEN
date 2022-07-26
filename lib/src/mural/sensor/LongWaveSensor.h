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
 * LongWaveSensor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef LONG_WAVE_SENSOR_H
#define LONG_WAVE_SENSOR_H "LongWaveSensor"

#include "OpticalSensor.h"

using namespace std;

/**
 * LongWaveSensor is the class that models a int wave sensor that
 * can be attached to a data provider.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------------
 * niirsE_a                     NIIRS equation coefficient E
 * niirsF_a                     NIIRS equation coefficient F
 * noiseGain_a                  Noise gain (NGAIN)
 * noiseToSignalRatio_a         noise to signal delta ratio (NOSDR)
 * responseEdgeRatio_a          response edge ratio (RERRER0)
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class LongWaveSensor : public OpticalSensor
{
public:

    LongWaveSensor();
    LongWaveSensor(shared_ptr<InputDataElement> ptrInputDataElement,
                   const SENSOR_TYPE &type);
    LongWaveSensor(const LongWaveSensor &copyMe);
    virtual ~LongWaveSensor();

    LongWaveSensor& operator =  (const LongWaveSensor &copyMe);
    bool            operator == (const LongWaveSensor &compareMe) const;
    bool            operator != (const LongWaveSensor &compareMe) const;

    int    GetSensorIndex() const;

    void   CheckInputData(bool runTimelineGenerator = false) const;
    void   PrintAttributes(ostream &outfile) const;

protected:

private:

    void ExtractLongWaveSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement);

    static inline string GetClassName();
};

inline
string
LongWaveSensor::GetClassName()
{
    return(LONG_WAVE_SENSOR_H);
}

#endif
