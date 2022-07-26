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
 * SpatialJpl.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
/// @file
/// @brief Spatial class representing position and velocity of JPL bodies as
/// a function of time
///
/// @author J. McCormick
/// @version V4.5 Initial release
/// @date 2009-07-01
///
/// @par Recommended enhancement (JAM)
/// Most values computed from the JPL DE don't need to be recomputed each time
/// step since their change relative to the Earth is relatively small, add
/// an frequency option to to reduce runtime cost
///
/// @par Units
/// - Function Arg & Return: standard length, sec, radians
/// - Stream I/O:            standard length, sec, manipulator controlled angles
/// - Initialization Files:  standard length, sec, manipulator controlled angles
// =============================================================================
#ifndef SPATIALJPL_H
/// header guard macro
#define SPATIALJPL_H "SpatialJpl V4.5"

#include "SpatialTimeDep.h"
#include "JplEph.h"
#include "CoorTransEciJ2000.h"

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// @brief Position and velocity of JPL bodies as a function of time
/// @details See EphObjId in JphEph.h for list of valid actual template
/// arguments
template <Jpl::EphObjId id>
class SpatialJpl : public SpatialTimeDep
{
public:
    /// @brief Initialize with handle to governing time object
    /// @details Note that the target coordinate system is
    /// defaulted to ECI
    /// @param [in] hdlTime handle to governing time object
    /// @param [in] bDoLightTime true include light-time in spatial
    /// computations to provide apparent position and velocity,
    /// false excludes light-time, provide "actual" position and
    /// velocity
    SpatialJpl(HandleConst<TimeJ2000> hdlTime,
               bool bDoLightTime);

    /// Initialize with handle to governing time object and
    /// transformation to the targeted coordinate system
    /// @param [in] hdlTime handle to governing time object
    SpatialJpl(HandleConst<TimeJ2000> hdlTime,
               Handle<CoorTransBaseEci> hdlCoorTrans,
               bool bDoLightTime);

    virtual ~SpatialJpl();

    bool operator==(const SpatialJpl& spatial) const;
    bool operator!=(const SpatialJpl& spatial) const;

    virtual const char* GetVersion() const;

protected:
    virtual void UpdateState();

    virtual const VectorCartesian& GetAcceleration() const;

private:
    Jpl::EphObjGC<id> m_eogc;

    VectorCartesian  m_j2000Pos;
    VectorCartesian  m_j2000Vel;

    CoorTransEciJ2000 m_xEciJ2000;

    bool m_bDoLightTime;
};

//--------------------------------------------------------------------------
template <Jpl::EphObjId id>
inline const char* SpatialJpl<id>::GetVersion() const
{
    return SPATIALJPL_H;
}
} // A3 namespace end

#endif // SPATIALJPL_H
