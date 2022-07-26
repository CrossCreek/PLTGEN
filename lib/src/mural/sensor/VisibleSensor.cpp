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
 * VisibleSensor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "VisibleSensor.h"

#include "general/array/ListIterator.h"
#include "general/math/SimpleMath.h"
#include "general/data/UnitData.h"

#include "mural/sensor/iqe/VisibleIQE.h"

//------------------------------------------------------------------------------
//
//  Method Name: VisibleSensor::VisibleSensor
//
//  Purpose:
//    Constructor for the VisibleSensor class.
//
//  Calling Method:
//     VisibleSensor();
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
//------------------------------------------------------------------------------

VisibleSensor::VisibleSensor()
    : OpticalSensor()
{
    return;
}

VisibleSensor::VisibleSensor(shared_ptr<InputDataElement> ptrInputDataElement,
                             const SENSOR_TYPE &type)
    : OpticalSensor(ptrInputDataElement, type)
{
    ExtractVisibleSensorInfo(ptrInputDataElement);
    // Init the IQE here
    ptrIQE_a = shared_ptr<VisibleIQE>(new VisibleIQE(ptrInputDataElement));
    return;
}

VisibleSensor::VisibleSensor(const VisibleSensor &copyMe)
    : OpticalSensor(copyMe)
{
    return;
}

VisibleSensor&
VisibleSensor::operator = (const VisibleSensor &copyMe)
{
    OpticalSensor::operator = (copyMe);
    return(*this);
}

bool
VisibleSensor::operator == (const VisibleSensor &compareMe) const
{
    return(OpticalSensor::operator == (compareMe));
}

bool
VisibleSensor::operator != (const VisibleSensor &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
VisibleSensor::ExtractVisibleSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement)
{
    return;
}

void
VisibleSensor::CheckInputData(bool runTimelineGenerator) const
{
    // Get the attributes from the IQE
    double niirsA = UNDEFINED_COEFFICIENT;
    double niirsB = UNDEFINED_COEFFICIENT;
    double niirsC = UNDEFINED_COEFFICIENT;
    double niirsD = UNDEFINED_COEFFICIENT;

    if ( ptrIQE_a != NULL ) {
        niirsA = ptrIQE_a->GetNIIRSA();
        niirsB = ptrIQE_a->GetNIIRSB();
        niirsC = ptrIQE_a->GetNIIRSC();
        niirsD = ptrIQE_a->GetNIIRSD();
    }

    string errorString = "";
    OpticalSensor::CheckInputData();

    if (niirsA == UNDEFINED_COEFFICIENT) {
        errorString = "Missing 'NIIRS A' input";
    }

    if (niirsB == UNDEFINED_COEFFICIENT) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing 'NIIRS B' input";
    }

    if (niirsC == UNDEFINED_COEFFICIENT) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing 'NIIRS C' input";
    }

    if (niirsD == UNDEFINED_COEFFICIENT) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing 'NIIRS D' input";
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "CheckInputData()",
                                 errorString);
    }

    return;
}

void
VisibleSensor::PrintAttributes(ostream &outfile) const
{
    outfile << "   <SENSOR_OBJECT_BEGIN>" << NEW_LINE;
    OpticalSensor::PrintAttributes(outfile);
    outfile << "   <SENSOR_OBJECT_STOP>" << NEW_LINE;
    return;
}

/**
 * Method to return the index which maps to the target deck and the SENSOR enum.  In the case of the Visible Sensor
 * the sensor type of VISIBLE is always returned
      VISIBLE = 0,
 *
 * @return int sensor Index
 */
int
VisibleSensor::GetSensorIndex() const
{
    return(VISIBLE);
}

VisibleSensor::~VisibleSensor()
{
    return;
}
