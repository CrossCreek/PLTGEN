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
 * IQE.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef IQE_H
#define IQE_H "IQE"

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <memory>

#include "general/math/CoordinateFrame.h"


class PositionState;
class InputDataElement;

template<class Type> class DefaultedData;
/**
 * Class which serves as the base class for for Image Quality Equations (IQE).  It holds all
 * the attributes to make it easier (Instead of having a bunch of broken out attributes with a
 * few common ones on the base)
 *
 * @author Brian Fowler
 * @date 10/03/11
 */
class IQE
{
public:

    /**
     * Constructor which takes the input data element and extracts the data for all
     * the iqe types (see note below)
     */


    explicit IQE(shared_ptr<InputDataElement> ptrInputDataElement);

    ~IQE();

    /**
     * Main abstract method which computes the quality based on the given parameters
     *
     * @param elevationAngle
     * @param &missionType
     * @param enhancedAreaMode
     * @return double the calculated quality
     */
    virtual double CalculateQuality(double elevationAngle,
                                    const string &missionType,
                                    const SpaceVector &targetPos,
                                    const PositionState &ecfVehilceState,
                                    bool enhancedAreaMode) = 0;

    /**
     * This method returns either the range or the squnit angle depending on the IQE.  This method
     * is to support the detailedValt debug file.  Because it is used for an output file the conversion
     * to "output" units happens in this method.
     *
     * @param targetPos the vector of the target Position (typically wac center)
     * @param PositionState the state of the vechile (position & velocity in ecf)
     * @returen quality paramter (range or squint)
     */
    virtual double GetDebugQualityParameter(const SpaceVector &targetPos,
                                            const PositionState &ecfVechileState) = 0;


    /**
     * Computes the range for OPtical IQE's
     *
     * @param targetPos the vector of the target Position (typically wac center)
     * @param PositionState the state of the vechile (position & velocity in ecf)
     * @return double the computed range.
     */
    double ComputeRange(const SpaceVector &targetPos,
                        const PositionState &ecfVechileState) const;

    /**
     * Computes the squint angle for Radar IQE's
     *
     * @param targetPos the vector of the target Position (typically wac center)
     * @param PositionState the state of the vechile (position & velocity in ecf)
     * @return double the computed range.
     */
    double ComputeSquintAngle(const SpaceVector &targetECF,
                              const PositionState &ecfVechileState) const;

    /**
     * Computes the doppler angle for Radar IQE's
     *
     * @param targetPos the vector of the target Position (typically wac center)
     * @param PositionState the state of the vechile (position & velocity in ecf)
     * @return double the computed range.
     */
    double ComputeDopplerConeAngle(const SpaceVector &targetECF,
                                   const PositionState &ecfVechileState) const;

    /**
     * Getter methods
     */
    // Optical Sensors (VIS/LW)
    const shared_ptr<DefaultedData<double>> GetFocalLength() const;
    const double GetPitch() const;
    const double GetFocalLength(const string &missionType) const;
    const double GetNIIRSA() const;
    const double GetNIIRSB() const;
    const double GetNIIRSC() const;
    const double GetNIIRSD() const;
    const double  GetNIIRSE() const;
    const double  GetNIIRSF() const;
    const double  GetNoiseGain() const;
    const double  GetNoiseToSignalRatio() const;
    const double  GetResposneEdgeRatio() const;

    // Radar (SAR/SSAR)
    const double GetNRISD1() const;
    const double GetNRISD2() const;
    const double GetNRISD3() const;
    const double GetNRISD4() const;
    const double GetNRISD5() const;
    const double GetNRISD6() const;
    const double GetIPR() const;

protected:
    /**
     * Extracts the attributes of the equation (on the SENSOR block in current MURAL)
     *
     * @param ptrInputDataElement ptr to the input data element
     */
    void ExtractEquationInfo(shared_ptr<InputDataElement> ptrInputDataElement);

    /**
     * Computes the ground sample distance, used in the optical IQE:
     *
     * GSD = range / SquareRoot(sin(elevationAngle))) * (pitch_a / focalLength))
     *
     * @param elevationAngle
     * @param range the range to the target
     * @param missionType the specific mission type
     * @return double the ground sample distance
     */
    double GetGroundSampleDistance(double elevationAngle,
                                   double range,
                                   const string &missionType) const;

    /**
     * Common attributes among all the iqe's.
     *
     * NOTE: I did not break them all out to keep it simple (all primatives).  If the individual equations
     * start to get more detail then it might make sence to remove these attributes from the base class
     */
    double                             pitch_a;
    shared_ptr<DefaultedData<double>>  ptrFocalLength_a;

    // Optical/LW attributes
    double               niirsA_a;
    double               niirsB_a;
    double               niirsC_a;
    double               niirsD_a;
    double               niirsE_a;
    double               niirsF_a;
    double               noiseGain_a;
    double               noiseToSignalRatio_a;
    double               responseEdgeRatio_a;

    // Radar attributes
    double               nrisD1_a;
    double               nrisD2_a;
    double               nrisD3_a;
    double               nrisD4_a;
    double               nrisD5_a;
    double               nrisD6_a;
    double               imageProductResolution_a;

private:

    static inline std::string GetClassName();
};

inline
std::string
IQE::GetClassName()
{
    return(IQE_H);
}

#endif

