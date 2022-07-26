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
 * SpectralSensor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef SPECTRAL_SENSOR_H
#define SPECTRAL_SENSOR_H "SpectralSensor"

#include "OpticalSensor.h"

using namespace std;

/**
 * SpectralSensor is the parent class to the types of spectral
 *     sensors (hyperspectral, multispectral, and ultraspectral).
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * ptrQualityBinArray_s*        array of values used for binning quality of
 *                                spectral targets
 * anySpectralSensors_s         indicates if any sensor in the user
 *                                constellation is spectral
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class SpectralSensor : public OpticalSensor
{
public:

    SpectralSensor();
    SpectralSensor(shared_ptr<InputDataElement> ptrInputDataElement,
                   const SENSOR_TYPE &type);
    SpectralSensor(const SpectralSensor &copyMe);
    virtual ~SpectralSensor();

    SpectralSensor& operator =  (const SpectralSensor &copyMe);
    bool            operator == (const SpectralSensor &compareMe) const;
    bool            operator != (const SpectralSensor &compareMe) const;

    void   PrintAttributes(ostream &outfile) const;

    int    GetSensorIndex() const;

    static inline void   PrintQualityBinArray(ostream &outfile);
    static inline void   SetQualityBinArray(const Array<double> *ptrArray);

    static inline int   GetQualityIndex(double quality);

    static inline double  GetQualityBinValue(int qualityIndex);

    static inline bool   AnySpectralSensors();

protected:

private:

    void ExtractSpectralSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement);

    static inline string GetClassName();

    static Array<double> *ptrQualityBinArray_s;
    static bool          anySpectralSensors_s;
};

inline
void
SpectralSensor::PrintQualityBinArray(ostream &outfile)
{
    if (ptrQualityBinArray_s != NULL) {
        ptrQualityBinArray_s->Print(outfile);
    }

    return;
}

inline
void
SpectralSensor::SetQualityBinArray(const Array<double> *ptrArray)
{
    if (ptrQualityBinArray_s != NULL) {
        delete ptrQualityBinArray_s;
    }

    ptrQualityBinArray_s = new Array<double>(*ptrArray);
    return;
}

inline
int
SpectralSensor::GetQualityIndex(double quality)
{
    int qualityIndex = -1;

    if (ptrQualityBinArray_s != NULL) {
        bool foundQualityBin = false;
        qualityIndex = 0;

        while ( (qualityIndex < ptrQualityBinArray_s->Size()) &&
                (foundQualityBin == false) ) {
            if (quality <= ptrQualityBinArray_s->ElementAt(qualityIndex)) {
                foundQualityBin = true;
            } else {
                ++qualityIndex;
            }
        }

        if (foundQualityBin == false) {
            qualityIndex = -1;
        }
    }

    return(qualityIndex);
}

inline
double
SpectralSensor::GetQualityBinValue(int qualityIndex)
{
    double qualityBinValue = 0.0;

    if (ptrQualityBinArray_s != NULL) {
        if ((qualityIndex >= 0) && (qualityIndex < ptrQualityBinArray_s->Size())) {
            qualityBinValue = ptrQualityBinArray_s->ElementAt(qualityIndex);
        }
    }

    return(qualityBinValue);
}

inline
bool
SpectralSensor::AnySpectralSensors()
{
    return(anySpectralSensors_s);
}

inline
string
SpectralSensor::GetClassName()
{
    return(SPECTRAL_SENSOR_H);
}

#endif
