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
 * CoorTransEciAefZ.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================

//  FILE      : CoorTransEciAefZ.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciAefZ

//  AUTHOR(s) : J. McCormick

//  UPDATES   :
//      V5.0    2010-06-01  JAM  initial release

//
/// @file
/// @brief Azimuth-Elevation-Fixed to/from Earth Centered Inertial (TOD)
/// Rotation
///
/// @details Defines the Azimuth Elevation Fixed, AEF, coordinate system with
/// respect to the Earth Centered Inertial (TOD) using geocentric coordinates
/// for the origin and a rotation from the topocentric-horizon system defined
/// in terms of azimuth and elevation.  Refer to "Azimuth Elevation Fixed
/// to/from Topocentric Coordinate System Transformations – Rotation Only"
/// by Joseph McCormick for more details.
///
/// @author J. McCormick
/// @version V5.0  initial release
/// @date 2010-06-01
///
/// @par Units
/// - Function Arg & Return: standard GenEngr (see constants.h)
/// - Stream I/O:            L = n/a, T, A = manipulator controlled
/// - Initialization Files:  N/A
// =============================================================================
#ifndef COORTRANSECIAEFZ_H
#define COORTRANSECIAEFZ_H  __FILE__ " " __DATE__

#include "CoorTransEciNwzF.h"
#include "Angle.h"

namespace A3
{
class CoorTransEciAefZ : public CoorTransBaseEci
{
public:
    CoorTransEciAefZ(HandleConst<TimeJ2000> hdlTime,
                     Angle geocentricLatitude, Angle longitude,
                     Angle azimuth, Angle elevation);

    virtual void
    ToBase(const VectorCartesian& vecPositionSource,
           VectorCartesian& vecPositionBase);

    virtual void
    ToBase(const VectorCartesian& vecPositionSource,
           const VectorCartesian& vecVelocitySource,
           VectorCartesian& vecPositionBase,
           VectorCartesian& vecVelocityBase);

    virtual void
    ToBase(const VectorCartesian& vecPositionSource,
           const VectorCartesian& vecVelocitySource,
           const VectorCartesian& vecAccelerationSource,
           VectorCartesian& vecPositionBase,
           VectorCartesian& vecVelocityBase,
           VectorCartesian& vecAccelerationBase);

    virtual void
    ToSource(const VectorCartesian& vecPositionBase,
             VectorCartesian& vecPositionSource);

    virtual void
    ToSource(const VectorCartesian& vecPositionBase,
             const VectorCartesian& vecVelocityBase,
             VectorCartesian& vecPositionSource,
             VectorCartesian& vecVelocitySource);

    virtual void
    ToSource(const VectorCartesian& vecPositionBase,
             const VectorCartesian& vecVelocityBase,
             const VectorCartesian& vecAccelerationBase,
             VectorCartesian& vecPositionSource,
             VectorCartesian& vecVelocitySource,
             VectorCartesian& vecAccelerationSource);

    virtual void
    ToBase(const VectorCartesian& vecPositionSource,
           VectorCartesian& vecPositionBase) const;

    virtual void
    ToBase(const VectorCartesian& vecPositionSource,
           const VectorCartesian& vecVelocitySource,
           VectorCartesian& vecPositionBase,
           VectorCartesian& vecVelocityBase) const;

    virtual void
    ToBase(const VectorCartesian& vecPositionSource,
           const VectorCartesian& vecVelocitySource,
           const VectorCartesian& vecAccelerationSource,
           VectorCartesian& vecPositionBase,
           VectorCartesian& vecVelocityBase,
           VectorCartesian& vecAccelerationBase) const;

    virtual void
    ToSource(const VectorCartesian& vecPositionBase,
             VectorCartesian& vecPositionSource) const;

    virtual void
    ToSource(const VectorCartesian& vecPositionBase,
             const VectorCartesian& vecVelocityBase,
             VectorCartesian& vecPositionSource,
             VectorCartesian& vecVelocitySource) const;

    virtual void
    ToSource(const VectorCartesian& vecPositionBase,
             const VectorCartesian& vecVelocityBase,
             const VectorCartesian& vecAccelerationBase,
             VectorCartesian& vecPositionSource,
             VectorCartesian& vecVelocitySource,
             VectorCartesian& vecAccelerationSource) const;

    virtual bool IsStale() const;

    bool operator==(const CoorTransEciAefZ& coorTrans) const;
    bool operator!=(const CoorTransEciAefZ& coorTrans) const;

protected:
    virtual void UpdateState();

private:
    CoorTransEciNwzF m_ctEciNwzF;
    double m_azimuth;
    double m_elevation;
};

// -------------------------------------------------------------------------
inline void
CoorTransEciAefZ::UpdateState()
{
    CoorTransBaseEci::UpdateState();
    m_ctEciNwzF.Update();
}

inline bool
CoorTransEciAefZ::IsStale() const
{
    return m_ctEciNwzF.IsStale() || CoorTransBaseEci::IsStale();
}

} // A3 namespace end
#endif  // COORTRANSECIAEFZ_H

