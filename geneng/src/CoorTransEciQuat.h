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
 * CoorTransEciQuat.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================

//  FILE      : CoorTransEciQuat.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciQuat

//  AUTHOR(s) : J. McCormick

//  UPDATES   :
//      V5.0    2010-06-01  JAM  initial release

//
/// @file
/// @brief Quaternion w.r.t. Earth Centered Inertial (TOD)
/// Rotation
///
/// @details Provides quaternion interpolation for attitude representation.
/// Displacement transformation only.
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
#ifndef COORTRANSECIQUAT_H
#define COORTRANSECIQUAT_H  __FILE__ " " __DATE__

#include "CoorTransBaseEci.h"
#include "HandleConst.h"
#include "TimeJ2000.h"
#include "Quaternion.h"
#include <string>
#include <vector>
#include <iosfwd>

namespace A3
{
class CoorTransEciQuat : public CoorTransBaseEci
{
public:
    CoorTransEciQuat(HandleConst<TimeJ2000> hTime,
                     std::istream& is);

    explicit CoorTransEciQuat(HandleConst<TimeJ2000> hTime);

    CoorTransEciQuat(const CoorTransEciQuat& ct);

    CoorTransEciQuat& operator=(const CoorTransEciQuat& ct);

    void InsertTimeQuat(const TimeJ2000& tm, const Quaternion& q);

    virtual bool IsStale() const;

    std::ostream& DumpTimeQuat(std::ostream& os);

    bool operator==(const CoorTransEciQuat& coorTrans) const;
    bool operator!=(const CoorTransEciQuat& coorTrans) const;

protected:
    virtual void UpdateState();

private:
    HandleConst<TimeJ2000> m_hTime;
    TimeJ2000  m_tmUpdate;
    Quaternion m_qtrn;
    std::vector<TimeJ2000>  m_vTime;
    std::vector<Quaternion> m_vQuat;
    std::vector<TimeJ2000>::iterator m_hint;
};

inline bool
CoorTransEciQuat::IsStale() const
{
    return CoorTransBaseEci::IsStale() || m_tmUpdate != *m_hTime;
}

} // A3 namespace end
#endif  // COORTRANSECIQUAT_H

