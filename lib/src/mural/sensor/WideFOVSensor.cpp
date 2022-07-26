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
 * WideFOVSensor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "WideFOVSensor.h"

#include "general/data/InputDataElement.h"

#include "mural/sensor/iqe/WideFOVIQE.h"

WideFOVSensor::WideFOVSensor()
    : Sensor()
{
    return;
}

WideFOVSensor::WideFOVSensor(shared_ptr<InputDataElement> ptrInputDataElement,
                             const SENSOR_TYPE &type)
    : Sensor(ptrInputDataElement, type)
{
    ExtractWideFOVSensorInfo(ptrInputDataElement);
    // Init the IQE here
    ptrIQE_a = shared_ptr<WideFOVIQE>(new WideFOVIQE(ptrInputDataElement));
    return;
}

WideFOVSensor::WideFOVSensor(const WideFOVSensor &copyMe)
    : Sensor(copyMe)
{
    return;
}

WideFOVSensor&
WideFOVSensor::operator = (const WideFOVSensor &copyMe)
{
    Sensor::operator = (copyMe);
    return(*this);
}

bool
WideFOVSensor::operator == (const WideFOVSensor &compareMe) const
{
    return(Sensor::operator == (compareMe));
}

bool
WideFOVSensor::operator != (const WideFOVSensor &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
WideFOVSensor::ExtractWideFOVSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement)
{
    return;
}

void
WideFOVSensor::PrintAttributes(ostream &outfile) const
{
    outfile << "   <SENSOR_OBJECT_BEGIN>" << NEW_LINE;
    Sensor::PrintAttributes(outfile);
    outfile << "   <SENSOR_OBJECT_STOP>" << NEW_LINE;
    return;
}

double
WideFOVSensor::GetCollectionRate(int missionIndex,
                                 int qualityIndex) const
{
    return(1.0);
}

double
WideFOVSensor::GetResourceWeight(int missionIndex,
                                 int qualityIndex) const
{
    return(sensorWeightFactor_a);
}

/**
 * Method to return the index which maps to the target deck and the SENSOR enum.  In the case of the WideFOV Sensor
 * the sensor type of WIDE_FOV is always returned.  It should also be noted that the WideFOV is not in the target deck
 * and does not map to the target deck arrays for preferenceing and modes
      WIDE_FOV = 10,
 *
 * @return int sensor Index
 */
int
WideFOVSensor::GetSensorIndex() const
{
    return(WIDE_FOV);
}

WideFOVSensor::~WideFOVSensor()
{
    return;
}

