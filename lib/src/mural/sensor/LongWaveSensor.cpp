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
 * LongWaveSensor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "LongWaveSensor.h"

#include "general/array/ListIterator.h"
#include "general/data/UnitData.h"
#include "general/math/SimpleMath.h"

#include "mural/sensor/iqe/LongWaveIQE.h"

//------------------------------------------------------------------------------
//
//  Method Name: LongWaveSensor::LongWaveSensor
//
//  Purpose:
//    Constructor for the LongWaveSensor class.
//
//  Calling Method:
//     LongWaveSensor();
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

LongWaveSensor::LongWaveSensor()
    : OpticalSensor()
{
    return;
}

LongWaveSensor::LongWaveSensor(shared_ptr<InputDataElement> ptrInputDataElement,
                               const SENSOR_TYPE &type)
    : OpticalSensor(ptrInputDataElement, type)
{
    ExtractLongWaveSensorInfo(ptrInputDataElement);
    // Init the IQE here
    ptrIQE_a = shared_ptr<LongWaveIQE>(new LongWaveIQE(ptrInputDataElement));
    return;
}

LongWaveSensor::LongWaveSensor(const LongWaveSensor &copyMe)
    : OpticalSensor(copyMe)
{
    return;
}

LongWaveSensor&
LongWaveSensor::operator = (const LongWaveSensor &copyMe)
{
    OpticalSensor::operator = (copyMe);
    return(*this);
}

bool
LongWaveSensor::operator == (const LongWaveSensor &compareMe) const
{
    return( (OpticalSensor::operator == (compareMe)) );
}

bool
LongWaveSensor::operator != (const LongWaveSensor &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
LongWaveSensor::ExtractLongWaveSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement)
{
    return;
}

void
LongWaveSensor::CheckInputData(bool runTimelineGenerator) const
{
    string errorString = "";
    double niirsA = UNDEFINED_COEFFICIENT;
    double niirsB = UNDEFINED_COEFFICIENT;
    double niirsC = UNDEFINED_COEFFICIENT;
    double niirsD = UNDEFINED_COEFFICIENT;
    double niirsE = UNDEFINED_COEFFICIENT;
    double niirsF = UNDEFINED_COEFFICIENT;
    double noiseGain = UNDEFINED_COEFFICIENT;
    double noiseToSignalRatio = UNDEFINED_COEFFICIENT;
    double responseEdgeRatio = UNDEFINED_COEFFICIENT;

    if ( ptrIQE_a != NULL ) {
        niirsA = ptrIQE_a->GetNIIRSA();
        niirsB = ptrIQE_a->GetNIIRSB();
        niirsC = ptrIQE_a->GetNIIRSC();
        niirsD = ptrIQE_a->GetNIIRSD();
        niirsE = ptrIQE_a->GetNIIRSE();
        niirsF = ptrIQE_a->GetNIIRSF();
        noiseGain = ptrIQE_a->GetNoiseGain();
        noiseToSignalRatio = ptrIQE_a->GetNoiseToSignalRatio();
        responseEdgeRatio = ptrIQE_a->GetResposneEdgeRatio();
    }

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

    if (niirsE == UNDEFINED_COEFFICIENT) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing 'NIIRS E' input";
    }

    if (niirsF == UNDEFINED_COEFFICIENT) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing 'NIIRS F' input";
    }

    if (noiseGain == UNDEFINED_COEFFICIENT) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing 'Noise Gain' input";
    }

    if (noiseToSignalRatio == UNDEFINED_COEFFICIENT) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'Noise To Signal Ratio' input";
    }

    if (responseEdgeRatio == UNDEFINED_COEFFICIENT) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing 'Response Edge Ratio' input";
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "CheckInputData()", errorString);
    }

    return;
}

void
LongWaveSensor::PrintAttributes(ostream &outfile) const
{
    // Get the attributes from the IQE
    double niirsE = UNDEFINED_COEFFICIENT;
    double niirsF = UNDEFINED_COEFFICIENT;
    double noiseGain = UNDEFINED_COEFFICIENT;
    double noiseToSignalRatio = UNDEFINED_COEFFICIENT;
    double responseEdgeRatio = UNDEFINED_COEFFICIENT;

    if ( ptrIQE_a != NULL ) {
        niirsE = ptrIQE_a->GetNIIRSE();
        niirsF = ptrIQE_a->GetNIIRSF();
        noiseGain = ptrIQE_a->GetNoiseGain();
        noiseToSignalRatio = ptrIQE_a->GetNoiseToSignalRatio();
        responseEdgeRatio = ptrIQE_a->GetResposneEdgeRatio();
    }

    outfile << "   <SENSOR_OBJECT_BEGIN>" << NEW_LINE;
    OpticalSensor::PrintAttributes(outfile);

    if (niirsE != UNDEFINED_COEFFICIENT) {
        outfile << "      NIIRS E                           : ";
        outfile << niirsE << NEW_LINE;
    }

    if (niirsF != UNDEFINED_COEFFICIENT) {
        outfile << "      NIIRS F                           : ";
        outfile << niirsF << NEW_LINE;
    }

    if (noiseGain != UNDEFINED_COEFFICIENT) {
        outfile << "      Noise Gain (NGAIN)                : ";
        outfile << noiseGain << NEW_LINE;
    }

    if (noiseToSignalRatio != UNDEFINED_COEFFICIENT) {
        outfile << "      Noise To Signal Ratio (NOSDR)     : ";
        outfile << noiseToSignalRatio << NEW_LINE;
    }

    if (responseEdgeRatio != UNDEFINED_COEFFICIENT) {
        outfile << "      Response Edge Ratio (RERRER0)     : ";
        outfile << responseEdgeRatio << NEW_LINE;
    }

    outfile << "   <SENSOR_OBJECT_STOP>" << NEW_LINE;
    return;
}

/**
 * Method to return the index which maps to the target deck and the SENSOR enum.  In the case of the Longwave Sensor
 * the sensor type of LONG_WAVE is always returned
      LONG_WAVE = 1,
 *
 * @return int sensor Index
 */
int
LongWaveSensor::GetSensorIndex() const
{
    return(LONG_WAVE);
}

LongWaveSensor::~LongWaveSensor()
{
    return;
}
