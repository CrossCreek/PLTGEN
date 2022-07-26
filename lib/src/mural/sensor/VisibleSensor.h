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
 * VisibleSensor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef VISIBLE_SENSOR_H
#define VISIBLE_SENSOR_H "VisibleSensor"

#include "OpticalSensor.h"

/**
 *  VisibleSensor is the class that models an visible sensor that
 *  can be attached to a data provider.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class VisibleSensor : public OpticalSensor
{
public:

    VisibleSensor();
    VisibleSensor(shared_ptr<InputDataElement> ptrInputDataElement,
                  const SENSOR_TYPE &type);
    VisibleSensor(const VisibleSensor &copyMe);
    virtual ~VisibleSensor();

    VisibleSensor&  operator =  (const VisibleSensor &copyMe);
    bool            operator == (const VisibleSensor &compareMe) const;
    bool            operator != (const VisibleSensor &compareMe) const;

    int   GetSensorIndex() const;
    void   CheckInputData(bool runTimelineGenerator) const;
    void   PrintAttributes(ostream &outfile) const;

protected:

private:

    void ExtractVisibleSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement);

    static inline string GetClassName();
};

inline
string
VisibleSensor::GetClassName()
{
    return(VISIBLE_SENSOR_H);
}

#endif
