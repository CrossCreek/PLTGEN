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
 * SpatialJpl.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Jpl Spatial Object
// Refer to SpatialJpl.h for more info.
//===============================================================================
#include "SpatialJpl.h"
#include "constants.h"

// ============================================================================
// A3 namespace start
namespace A3
{
template <Jpl::EphObjId id>
SpatialJpl<id>::SpatialJpl(HandleConst<TimeJ2000> hdlTime,
                           bool bDoLightTime)
    : SpatialTimeDep(hdlTime), m_xEciJ2000(hdlTime),
      m_bDoLightTime(bDoLightTime)
{
    Update();
}

template <Jpl::EphObjId id>
SpatialJpl<id>::SpatialJpl(HandleConst<TimeJ2000> hdlTime,
                           Handle<CoorTransBaseEci> hdlCoorTrans,
                           bool bDoLightTime)
    : SpatialTimeDep(hdlTime, hdlCoorTrans), m_xEciJ2000(hdlTime),
      m_bDoLightTime(bDoLightTime)
{
    Update();
}

template <Jpl::EphObjId id>
SpatialJpl<id>::~SpatialJpl() { ; }

//--------------------------------------------------------------------------
template <Jpl::EphObjId id>
const VectorCartesian& SpatialJpl<id>::GetAcceleration() const
{
    throw std::runtime_error(
        "SpatialJpl::GetAcceleration() not implemented");
    // return m_vecAcceleration;
}

//--------------------------------------------------------------------------
// operators
template <Jpl::EphObjId id>
bool SpatialJpl<id>::operator==(const SpatialJpl& spatial) const
{
    return SpatialTimeDep::operator==(spatial);
}

template <Jpl::EphObjId id>
bool SpatialJpl<id>::operator!=(const SpatialJpl& spatial) const
{
    return !operator==(spatial);
}

template <Jpl::EphObjId id>
void SpatialJpl<id>::UpdateState()
{
    // update base class
    SpatialTimeDep::UpdateState();
    TimeJ2000 tm(GetTime());

    if (IsEnabled(VELOCITY)) {
        m_eogc.PosVel(tm, m_j2000Pos, m_j2000Vel);

        if (m_bDoLightTime) {
            double dt(m_j2000Pos.Magnitude()/SPEED_OF_LIGHT);
            tm -= dt;
            m_eogc.PosVel(tm, m_j2000Pos, m_j2000Vel);
        }

        if (m_hdlCoorTrans->GetSourceSystem() == CoorSystem::J2000) {
            m_vecPosition = m_j2000Pos;
            m_vecVelocity = m_j2000Vel;
        } else {
            VectorCartesian p, v;
            m_xEciJ2000.ToBase(m_j2000Pos, m_j2000Vel, p, v);
            m_hdlCoorTrans->ToSource(p, v, m_vecPosition, m_vecVelocity);
        }
    } else {
        m_eogc.Position(tm, m_j2000Pos);

        if (m_bDoLightTime) {
            double dt(m_j2000Pos.Magnitude()/SPEED_OF_LIGHT);
            tm -= dt;
            m_eogc.Position(tm, m_j2000Pos);
        }

        if (m_hdlCoorTrans->GetSourceSystem() == CoorSystem::J2000) {
            m_vecPosition = m_j2000Pos;
        } else {
            VectorCartesian p;
            m_xEciJ2000.ToBase(m_j2000Pos, p);
            m_hdlCoorTrans->ToSource(p, m_vecPosition);
        }
    }
}

template class SpatialJpl<Jpl::MERCURY>;
template class SpatialJpl<Jpl::VENUS>;
template class SpatialJpl<Jpl::EMB>;
template class SpatialJpl<Jpl::MOON>;
template class SpatialJpl<Jpl::MARS>;
template class SpatialJpl<Jpl::JUPITER>;
template class SpatialJpl<Jpl::SATURN>;
template class SpatialJpl<Jpl::URANUS>;
template class SpatialJpl<Jpl::NEPTUNE>;
template class SpatialJpl<Jpl::PLUTO>;
template class SpatialJpl<Jpl::SUN>;

} // A3
