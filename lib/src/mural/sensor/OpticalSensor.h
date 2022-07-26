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
 * OpticalSensor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef OPTICAL_SENSOR_H
#define OPTICAL_SENSOR_H "OpticalSensor"

#include "Sensor.h"

#include "general/array/Array2D.h"

/**
 * OpticalSensor is the class that models an visible sensor that
 * can be attached to a data provider.
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * numberOfDetectorElements_a   number of detector elements (not currently used)
 * opticalFieldOfView_a         optical field of view
 * pitch_a                      pitch
 * niirsA_a                     NIIRS Equation Coefficient A
 * niirsB_a                     NIIRS Equation Coefficient B
 * niirsC_a                     NIIRS Equation Coefficient C
 * niirsD_a                     NIIRS Equation Coefficient D
 * ptrFocalLength_a*            array of focal lengths based on mission
 * ptrCollectionRate_a*         rate at which targets are collected by
 *                                collection rate type and quality
 * ptrResourceWeight_a*         score multiplier to targets by resource weight
 *                                type and quality
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class OpticalSensor : public Sensor
{
public:

    enum COLLECTION_RATE_TYPE {
        POINT_CR = 0,
        LARGE_AREA_CR = 1,
        MCG_AREA_CR = 2,
        SMALL_AREA_CR = 3,
        NUMBER_OF_COLLECTION_TYPES = 4
    };

    enum RESOURCE_WEIGHT_TYPE {
        POINT_RW = 0,
        AREA_RW = 1,
        LOC_RW = 2,
        NUMBER_OF_RESOURCE_WEIGHT_TYPES = 3
    };

    OpticalSensor();
    OpticalSensor(shared_ptr<InputDataElement> ptrInputDataElement,
                  const SENSOR_TYPE &type);
    OpticalSensor(const OpticalSensor &copyMe);
    virtual ~OpticalSensor();

    OpticalSensor&  operator =  (const OpticalSensor &copyMe);
    bool            operator == (const OpticalSensor &compareMe) const;
    bool            operator != (const OpticalSensor &compareMe) const;

    static string  GetCollectionRateString(int typeIndex);
    static string  GetResourceWeightString(int typeIndex);

    virtual void   CheckInputData(bool runTimelineGenerator = false) const;
    virtual void   PrintAttributes(ostream &outfile) const;

    virtual double GetCollectionRate(int missionIndex,
                                     int qualityIndex) const;
    virtual double GetResourceWeight(int missionIndex,
                                     int qualityIndex) const;

    virtual inline bool  IsReflectiveEnabled() const;
    virtual inline bool  IsEmissiveEnabled() const;

    inline const int  GetNumberOfDetectorElements() const;

    inline const double GetOpticalFieldOfView() const;

protected:

    void   ExtractOpticalSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement);

    /*double GetGroundSampleDistance(double elevationAngle,
                                   double range,
                                   int missionIndex) const;*/

    inline void SetOpticalFieldOfView(double newValue);
    inline void SetPitch(double newValue);
    inline void SetFocalLength(int missionIndex,
                               double newValue);
    inline void SetNIIRSA(double newValue);
    inline void SetNIIRSB(double newValue);
    inline void SetNIIRSC(double newValue);
    inline void SetNIIRSD(double newValue);

    int            numberOfDetectorElements_a;
    double           opticalFieldOfView_a;
    Array2D<double> *ptrCollectionRate_a;
    Array2D<double> *ptrResourceWeight_a;

private:

    static int  GetCollectionRateTypeIndex(const string &typeString);
    static int  GetResourceWeightTypeIndex(const string &typeString);
    static int  ConvertCollectionRateIndex(int missionIndex);
    static int  ConvertResourceWeightIndex(int missionIndex);

    static inline string GetClassName();
};

inline
bool
OpticalSensor::IsReflectiveEnabled() const
{
    return(true);
}

inline
bool
OpticalSensor::IsEmissiveEnabled() const
{
    return(true);
}

inline
const int
OpticalSensor::GetNumberOfDetectorElements() const
{
    return(numberOfDetectorElements_a);
}

inline
void
OpticalSensor::SetOpticalFieldOfView(double newValue)
{
    opticalFieldOfView_a = newValue;
}

inline
const double
OpticalSensor::GetOpticalFieldOfView() const
{
    return(opticalFieldOfView_a);
}

inline
string
OpticalSensor::GetClassName()
{
    return(OPTICAL_SENSOR_H);
}

#endif
