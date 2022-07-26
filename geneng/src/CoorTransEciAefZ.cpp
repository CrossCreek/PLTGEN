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
 * CoorTransEciAefZ.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// ECI(TOD)-Azimuth Elevation Fixed Geocentric coordinate transformation class
// Refer to CoorTransEciAefZ.h for more info.
// =============================================================================
#include "CoorTransEciAefZ.h"
#include "VectorGeocentric.h"

namespace A3
{
CoorTransEciAefZ::CoorTransEciAefZ(HandleConst<TimeJ2000> hdlTime,
                                   Angle geocentricLatitude, Angle longitude,
                                   Angle azimuth, Angle elevation)
    : CoorTransBaseEci(CoorSystem::AEFZ),
      m_ctEciNwzF(hdlTime, geocentricLatitude, longitude),
      m_azimuth(azimuth),
      m_elevation(elevation)
{
    EIClrSet(POS_EI|VEL_EI|ACC_EI);
    double caz(cos(m_azimuth));
    double saz(sin(m_azimuth));
    double cel(cos(m_elevation));
    double sel(sin(m_elevation));
    CoorTransDcm dcmAefzToNwzf;
    dcmAefzToNwzf[0][0] =  caz*cel;
    dcmAefzToNwzf[0][1] =  saz;
    dcmAefzToNwzf[0][2] = -caz*sel;
    dcmAefzToNwzf[1][0] = -saz*cel;
    dcmAefzToNwzf[1][1] =  caz;
    dcmAefzToNwzf[1][2] =  saz*sel;
    dcmAefzToNwzf[2][0] =  sel;
    dcmAefzToNwzf[2][1] =  0.0;
    dcmAefzToNwzf[2][2] =  cel;
    Dcm() = dcmAefzToNwzf;
}

//--------------------------------------------------------------------------
void
CoorTransEciAefZ::ToBase(const VectorCartesian& vecPositionSource,
                         VectorCartesian& vecPositionBase)
{
    VectorCartesian vecPositionNwz;
    CoorTrans::ToBase(vecPositionSource, vecPositionNwz);
    m_ctEciNwzF.ToBase(vecPositionNwz, vecPositionBase);
}

void
CoorTransEciAefZ::ToBase(const VectorCartesian& vecPositionSource,
                         const VectorCartesian& vecVelocitySource,
                         VectorCartesian& vecPositionBase,
                         VectorCartesian& vecVelocityBase)
{
    VectorCartesian vecPositionNwz, vecVelocityNwz;
    CoorTrans::ToBase(vecPositionSource, vecVelocitySource,
                      vecPositionNwz,    vecVelocityNwz);
    m_ctEciNwzF.ToBase(vecPositionNwz,  vecVelocityNwz,
                       vecPositionBase, vecVelocityBase);
}

void
CoorTransEciAefZ::ToBase(const VectorCartesian& vecPositionSource,
                         const VectorCartesian& vecVelocitySource,
                         const VectorCartesian& vecAccelerationSource,
                         VectorCartesian& vecPositionBase,
                         VectorCartesian& vecVelocityBase,
                         VectorCartesian& vecAccelerationBase)
{
    VectorCartesian vecPositionNwz, vecVelocityNwz, vecAccelerationNwz;
    CoorTrans::ToBase(vecPositionSource, vecVelocitySource,
                      vecAccelerationSource,
                      vecPositionNwz,    vecVelocityNwz,
                      vecAccelerationNwz);
    m_ctEciNwzF.ToBase(vecPositionNwz,  vecVelocityNwz,
                       vecAccelerationNwz,
                       vecPositionBase, vecVelocityBase,
                       vecAccelerationBase);
}

void
CoorTransEciAefZ::ToSource(const VectorCartesian& vecPositionBase,
                           VectorCartesian& vecPositionSource)
{
    VectorCartesian vecPositionNwz;
    m_ctEciNwzF.ToSource(vecPositionBase, vecPositionNwz);
    CoorTrans::ToSource(vecPositionNwz, vecPositionSource);
}

void
CoorTransEciAefZ::ToSource(const VectorCartesian& vecPositionBase,
                           const VectorCartesian& vecVelocityBase,
                           VectorCartesian& vecPositionSource,
                           VectorCartesian& vecVelocitySource)
{
    VectorCartesian vecPositionNwz, vecVelocityNwz;
    m_ctEciNwzF.ToSource(vecPositionBase, vecVelocityBase,
                         vecPositionNwz,  vecVelocityNwz);
    CoorTrans::ToSource(vecPositionNwz,    vecVelocityNwz,
                        vecPositionSource, vecVelocitySource);
}

void
CoorTransEciAefZ::ToSource(const VectorCartesian& vecPositionBase,
                           const VectorCartesian& vecVelocityBase,
                           const VectorCartesian& vecAccelerationBase,
                           VectorCartesian& vecPositionSource,
                           VectorCartesian& vecVelocitySource,
                           VectorCartesian& vecAccelerationSource)
{
    VectorCartesian vecPositionNwz, vecVelocityNwz, vecAccelerationNwz;
    m_ctEciNwzF.ToSource(vecPositionBase, vecVelocityBase,
                         vecAccelerationBase,
                         vecPositionNwz,  vecVelocityNwz,
                         vecAccelerationNwz);
    CoorTrans::ToSource(vecPositionNwz,    vecVelocityNwz,
                        vecAccelerationNwz,
                        vecPositionSource, vecVelocitySource,
                        vecAccelerationSource);
}

void
CoorTransEciAefZ::ToBase(const VectorCartesian& vecPositionSource,
                         VectorCartesian& vecPositionBase) const
{
    VectorCartesian vecPositionNwz;
    CoorTrans::ToBase(vecPositionSource, vecPositionNwz);
    m_ctEciNwzF.ToBase(vecPositionNwz, vecPositionBase);
}

void
CoorTransEciAefZ::ToBase(const VectorCartesian& vecPositionSource,
                         const VectorCartesian& vecVelocitySource,
                         VectorCartesian& vecPositionBase,
                         VectorCartesian& vecVelocityBase) const
{
    VectorCartesian vecPositionNwz, vecVelocityNwz;
    CoorTrans::ToBase(vecPositionSource, vecVelocitySource,
                      vecPositionNwz,    vecVelocityNwz);
    m_ctEciNwzF.ToBase(vecPositionNwz,  vecVelocityNwz,
                       vecPositionBase, vecVelocityBase);
}

void
CoorTransEciAefZ::ToBase(const VectorCartesian& vecPositionSource,
                         const VectorCartesian& vecVelocitySource,
                         const VectorCartesian& vecAccelerationSource,
                         VectorCartesian& vecPositionBase,
                         VectorCartesian& vecVelocityBase,
                         VectorCartesian& vecAccelerationBase) const
{
    VectorCartesian vecPositionNwz, vecVelocityNwz, vecAccelerationNwz;
    CoorTrans::ToBase(vecPositionSource, vecVelocitySource,
                      vecAccelerationSource,
                      vecPositionNwz,    vecVelocityNwz,
                      vecAccelerationNwz);
    m_ctEciNwzF.ToBase(vecPositionNwz,  vecVelocityNwz,
                       vecAccelerationNwz,
                       vecPositionBase, vecVelocityBase,
                       vecAccelerationBase);
}

void
CoorTransEciAefZ::ToSource(const VectorCartesian& vecPositionBase,
                           VectorCartesian& vecPositionSource) const
{
    VectorCartesian vecPositionNwz;
    m_ctEciNwzF.ToSource(vecPositionBase, vecPositionNwz);
    CoorTrans::ToSource(vecPositionNwz, vecPositionSource);
}

void
CoorTransEciAefZ::ToSource(const VectorCartesian& vecPositionBase,
                           const VectorCartesian& vecVelocityBase,
                           VectorCartesian& vecPositionSource,
                           VectorCartesian& vecVelocitySource) const
{
    VectorCartesian vecPositionNwz, vecVelocityNwz;
    m_ctEciNwzF.ToSource(vecPositionBase, vecVelocityBase,
                         vecPositionNwz,  vecVelocityNwz);
    CoorTrans::ToSource(vecPositionNwz,    vecVelocityNwz,
                        vecPositionSource, vecVelocitySource);
}

void
CoorTransEciAefZ::ToSource(const VectorCartesian& vecPositionBase,
                           const VectorCartesian& vecVelocityBase,
                           const VectorCartesian& vecAccelerationBase,
                           VectorCartesian& vecPositionSource,
                           VectorCartesian& vecVelocitySource,
                           VectorCartesian& vecAccelerationSource) const
{
    VectorCartesian vecPositionNwz, vecVelocityNwz, vecAccelerationNwz;
    m_ctEciNwzF.ToSource(vecPositionBase, vecVelocityBase,
                         vecAccelerationBase,
                         vecPositionNwz,  vecVelocityNwz,
                         vecAccelerationNwz);
    CoorTrans::ToSource(vecPositionNwz,    vecVelocityNwz,
                        vecAccelerationNwz,
                        vecPositionSource, vecVelocitySource,
                        vecAccelerationSource);
}

bool CoorTransEciAefZ::operator==(const CoorTransEciAefZ& coorTrans) const
{
    return CoorTransBaseEci::operator==(coorTrans)
           && m_ctEciNwzF  == coorTrans.m_ctEciNwzF
           && m_azimuth    == coorTrans.m_azimuth
           && m_elevation  == coorTrans.m_elevation;
}

bool CoorTransEciAefZ::operator!=(const CoorTransEciAefZ& coorTrans) const
{
    return !operator==(coorTrans);
}

} // A3 namespace end
