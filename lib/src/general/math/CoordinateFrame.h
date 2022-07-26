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
 * CoordinateFrame.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef COORDINATE_FRAME_H
#define COORDINATE_FRAME_H "CoordinateFrame"

#include <iostream>
#include <memory>

#include "general/math/SpaceVector.h"

using namespace std;

/**
 * CoordinateFrame is the class that is used to represent the axes
 * of a particular coordinate system..
 *
 * Class Attribute Descriptions
 *
 * Name                   Description (units)
 * -------------          ------------------------------------------------------
 * *ptrXAxis_a            Unit vector representing the x-axis of the frame
 * *ptrYAxis_a            Unit vector representing the y-axis of the frame
 * *ptrZAxis_a            Unit vector representing the z-axis of the frame
 *
 * @author Rob Lowry
 * @author Brian Fowler
 * @date 08/16/11
 */
class CoordinateFrame
{
public:



    explicit CoordinateFrame(REFERENCE_FRAME frame = UnitData::GetInternalReferenceFrame());

    CoordinateFrame(const SpaceVector &xAxis,
                    const SpaceVector &yAxis,
                    const SpaceVector &zAxis,
                    REFERENCE_FRAME frame = UnitData::GetInternalReferenceFrame());

    CoordinateFrame(double azimuthAngle,
                    double elevationAngle);

    CoordinateFrame(const CoordinateFrame &copyMe);

    virtual ~CoordinateFrame();

    CoordinateFrame& operator =  (const CoordinateFrame &copyMe);
    bool             operator == (const CoordinateFrame &compareMe) const;
    bool             operator != (const CoordinateFrame &compareMe) const;

    /**
     * Perform a Euler Transform About the X axis.  MURAL used a standard where
     * positive values are clockwise.  The DCM reflects this.
     *
     * @param rotationAngle angle to rotate
     */
    void EulerTransformAboutX(double rotationAngle);

    /**
     * Perform a Euler Transform About the Y axis.  MURAL used a standard where
     * positive values are clockwise.  The DCM reflects this.
     *
     * @param rotationAngle angle to rotate
     */
    void EulerTransformAboutY(double rotationAngle);

    /**
     * Perform a Euler Transform About the Z axis.  MURAL used a standard where
     * positive values are clockwise.  The DCM reflects this.
     *
     * @param rotationAngle angle to rotate
     */
    void EulerTransformAboutZ(double rotationAngle);
    void Print(ostream &outfile) const;

    CoordinateFrame MultiplyBy(const CoordinateFrame &multiplyMatrix);

    SpaceVector     MultiplyBy(const SpaceVector &multiplyVector);

    const SpaceVector GetXAxis() const;
    const SpaceVector GetYAxis() const;
    const SpaceVector GetZAxis() const;

private:

    static inline string GetClassName();

    shared_ptr<SpaceVector> ptrXAxis_a;
    shared_ptr<SpaceVector> ptrYAxis_a;
    shared_ptr<SpaceVector> ptrZAxis_a;
};

inline
string
CoordinateFrame::GetClassName()
{
    return(COORDINATE_FRAME_H);
}

#endif
