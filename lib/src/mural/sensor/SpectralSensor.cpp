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
 * SpectralSensor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "SpectralSensor.h"

#include "general/array/ListIterator.h"

#include "mural/sensor/iqe/SpectralIQE.h"

Array<double>* SpectralSensor::ptrQualityBinArray_s = NULL;
bool          SpectralSensor::anySpectralSensors_s = false;

//----------------------------------------------------------------------
//
//  Method Name: SpectralSensor::SpectralSensor
//
//  Purpose:
//    Constructor for the SpectralSensor class.
//
//  Calling Method:
//     MultispectralSensor();
//
//  Calling Parameters
//  Parameters      i/o  description
//  ----------      ---  -----------
//
//  Returns:
//     None
//
//  Limitations:
//     None
//
//----------------------------------------------------------------------

SpectralSensor::SpectralSensor()
    : OpticalSensor()
{
    return;
}

SpectralSensor::SpectralSensor(shared_ptr<InputDataElement> ptrInputDataElement,
                               const SENSOR_TYPE &type)
    : OpticalSensor(ptrInputDataElement, type)
{
    try {
        ExtractSpectralSensorInfo(ptrInputDataElement);
        // Init the IQE here
        ptrIQE_a = shared_ptr<SpectralIQE>(new SpectralIQE(ptrInputDataElement));
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "SpectralSensor(InputDataElement*, const SENSOR_TYPE&)");
        throw;
    }

    anySpectralSensors_s = true;
    return;
}

SpectralSensor::SpectralSensor(const SpectralSensor &copyMe)
    : OpticalSensor(copyMe)
{
    return;
}

SpectralSensor&
SpectralSensor::operator = (const SpectralSensor &copyMe)
{
    OpticalSensor::operator = (copyMe);
    return(*this);
}

bool
SpectralSensor::operator == (const SpectralSensor &compareMe) const
{
    return(OpticalSensor::operator == (compareMe));
}

bool
SpectralSensor::operator != (const SpectralSensor &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
SpectralSensor::ExtractSpectralSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement)
{
    return;
}

void
SpectralSensor::PrintAttributes(ostream &outfile) const
{
    outfile << "   <SENSOR_OBJECT_BEGIN>" << NEW_LINE;
    OpticalSensor::PrintAttributes(outfile);
    outfile << "   <SENSOR_OBJECT_STOP>" << NEW_LINE;
    return;
}

/**
 * Method to return the index which maps to the target deck and the SENSOR enum.  In the case of the Specral Sensor
 * the sensor type is returned to denote which target deck index to map to
      MS_REFLECTIVE = 3,
      MS_EMISSIVE   = 4,
      US_REFLECTIVE = 5,
      US_EMISSIVE   = 6,
      HS_REFLECTIVE = 7,
      HS_EMISSIVE   = 8,
 *
 * @return int sensor Index
 */
int
SpectralSensor::GetSensorIndex() const
{
    return(sensorType_a);
}

SpectralSensor::~SpectralSensor()
{
    return;
}
