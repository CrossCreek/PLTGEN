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
 * IQE.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "IQE.h"

#include "general/exception/NullPointerException.h"
#include "general/math/SimpleMath.h"
#include "general/data/InputDataElement.h"
#include "general/data/PositionState.h"
#include "general/data/DefaultedData.h"

IQE::IQE(shared_ptr<InputDataElement> ptrInputDataElement) :
    pitch_a(UNDEFINED_DISTANCE), ptrFocalLength_a(NULL),
    niirsA_a(UNDEFINED_COEFFICIENT), niirsB_a(UNDEFINED_COEFFICIENT),
    niirsC_a(UNDEFINED_COEFFICIENT), niirsD_a(UNDEFINED_COEFFICIENT),
    niirsE_a(UNDEFINED_COEFFICIENT), niirsF_a(UNDEFINED_COEFFICIENT),
    noiseGain_a(UNDEFINED_COEFFICIENT), noiseToSignalRatio_a(UNDEFINED_COEFFICIENT),
    responseEdgeRatio_a(UNDEFINED_COEFFICIENT), nrisD1_a(UNDEFINED_COEFFICIENT), nrisD2_a(UNDEFINED_COEFFICIENT),
    nrisD3_a(UNDEFINED_COEFFICIENT), nrisD4_a(UNDEFINED_COEFFICIENT),
    nrisD5_a(UNDEFINED_COEFFICIENT), nrisD6_a(UNDEFINED_COEFFICIENT),
    imageProductResolution_a(UNDEFINED_COEFFICIENT)
{
    ExtractEquationInfo(ptrInputDataElement);
    return;
}

/* ********************************************************************************* */

void
IQE::ExtractEquationInfo(shared_ptr<InputDataElement> ptrInputDataElement)
{
    string      typeString = "";
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>              ptrInputData = NULL;

    //CPPCHECK_FIX Performance - prefix iterator change
    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("NIIRS A") == true) {
            niirsA_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("NIIRS B") == true) {
            niirsB_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("NIIRS C") == true) {
            niirsC_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("NIIRS D") == true) {
            niirsD_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("NIIRS E") == true) {
            niirsE_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("NIIRS F") == true) {
            niirsF_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Noise Gain") == true) {
            noiseGain_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Noise To Signal Ratio") == true) {
            noiseToSignalRatio_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Response Edge Ratio") == true) {
            responseEdgeRatio_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("NRIS D1") == true) {
            nrisD1_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("NRIS D2") == true) {
            nrisD2_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("NRIS D3") == true) {
            nrisD3_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("NRIS D4") == true) {
            nrisD4_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("NRIS D5") == true) {
            nrisD5_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("NRIS D6") == true) {
            nrisD6_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Image Product Resolution") == true) {
            imageProductResolution_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Pitch") == true) {
            pitch_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Default Focal Length") == true) {
            ptrFocalLength_a = shared_ptr<DefaultedData<double>>(new DefaultedData<double>(ptrInputData->GetDoubleValue()));
        } else if (ptrInputData->LabelContainsString("Focal Length") == true) {
            if (ptrFocalLength_a == NULL) {
                throw new InputException(GetClassName(),
                                         "ExtractOpticalSensorInfo(InputDataElement*)",
                                         "Must input Default Focal Length before mission specific");
            }

            if (ptrInputData->GetSpecificType(typeString) == true) {
                ptrFocalLength_a->AddSpecific(typeString, ptrInputData->GetDoubleValue());
            }
        }
    }

    return;
}

/* ********************************************************************************* */

double
IQE::GetGroundSampleDistance(double elevationAngle,
                             double range,
                             const string &missionType) const
{
    const double focalLength = ptrFocalLength_a->GetValue(missionType);
    return((range / SimpleMath::SquareRoot(sin(elevationAngle))) * (pitch_a / focalLength));
}

/* ********************************************************************************* */

double
IQE::ComputeRange(const SpaceVector &targetPos,
                  const PositionState &ecfVechileState) const
{
    const SpaceVector ecfPosition = *(ecfVechileState.GetPosition());
    return ( targetPos.GetSeparationDistance(ecfPosition) );
}

/* ********************************************************************************* */

double
IQE::ComputeSquintAngle(const SpaceVector &targetECF,
                        const PositionState &ecfVechileState) const
{
    const SpaceVector positionECF = *(ecfVechileState.GetPosition());
    const SpaceVector velocityECF = *(ecfVechileState.GetVelocity());
    SpaceVector lineOfSightECF = targetECF - positionECF;
    SpaceVector horizontalLineOfSight = lineOfSightECF.ProjectIntoPlane(positionECF);
    SpaceVector horizontalVelocity = velocityECF.ProjectIntoPlane(positionECF);
    return(horizontalLineOfSight.GetSeparationAngle(horizontalVelocity));
}

/* **************************************************************************************** */

double
IQE::ComputeDopplerConeAngle(const SpaceVector &targetECF,
                             const PositionState &ecfVechileState) const
{
    const SpaceVector positionECF = *(ecfVechileState.GetPosition());
    const SpaceVector velocityECF = *(ecfVechileState.GetVelocity());
    SpaceVector lineOfSightECF = targetECF - positionECF;
    return(lineOfSightECF.GetSeparationAngle(velocityECF));
}

/* **************************************************************************************** */

const double
IQE::GetPitch() const
{
    return(pitch_a);
}

/* **************************************************************************************** */

const shared_ptr<DefaultedData<double>>
                                     IQE::GetFocalLength() const
{
    return(ptrFocalLength_a);
}

/* **************************************************************************************** */

const double
IQE::GetFocalLength(const string &missionType) const
{
    return(ptrFocalLength_a->GetValue(missionType));
}

/* **************************************************************************************** */

const double
IQE::GetNIIRSA() const
{
    return(niirsA_a);
}

/* **************************************************************************************** */

const double
IQE::GetNIIRSB() const
{
    return(niirsB_a);
}

/* **************************************************************************************** */

const double
IQE::GetNIIRSC() const
{
    return(niirsC_a);
}

/* **************************************************************************************** */

const double
IQE::GetNIIRSD() const
{
    return(niirsD_a);
}

/* **************************************************************************************** */

const double
IQE::GetNIIRSE() const
{
    return(niirsE_a);
}

/* **************************************************************************************** */

double
const IQE::GetNIIRSF() const
{
    return(niirsF_a);
}

/* **************************************************************************************** */

double
const IQE::GetNoiseGain() const
{
    return(noiseGain_a);
}

/* **************************************************************************************** */

double
const IQE::GetNoiseToSignalRatio() const
{
    return(noiseToSignalRatio_a);
}

/* **************************************************************************************** */

double
const IQE::GetResposneEdgeRatio() const
{
    return(responseEdgeRatio_a);
}

/* **************************************************************************************** */

const double
IQE::GetNRISD1() const
{
    return(nrisD1_a);
}

/* **************************************************************************************** */

const double
IQE::GetNRISD2() const
{
    return(nrisD2_a);
}

/* **************************************************************************************** */

const double
IQE::GetNRISD3() const
{
    return(nrisD3_a);
}

/* **************************************************************************************** */

const double
IQE::GetNRISD4() const
{
    return(nrisD4_a);
}

/* **************************************************************************************** */

const double
IQE::GetNRISD5() const
{
    return(nrisD5_a);
}

/* **************************************************************************************** */

const double
IQE::GetNRISD6() const
{
    return(nrisD6_a);
}

/* **************************************************************************************** */

const double
IQE::GetIPR() const
{
    return(imageProductResolution_a);
}

/* **************************************************************************************** */

IQE::~IQE()
{
    // Do nothing
}
