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
 * EulerAngles.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Euler Angles class
// Refer to EulerAngles.h for more info.
// =============================================================================
#include "EulerAngles.h"
#include "Angle.h"
#include "IoSeparator.h"

#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <memory.h>

// A3 namespace start
namespace A3
{
// constructors & destructors
EulerAngles::EulerAngles() : m_seq(0) {;}

EulerAngles::EulerAngles( unsigned rotationSequence )
    : m_seq( rotationSequence ) {;}

EulerAngles::EulerAngles( unsigned rotationSequence,
                          Angle alpha, Angle beta, Angle gamma  )
    : Vector3DFP<Angle>(alpha, beta, gamma), m_seq( rotationSequence ) {;}

// operators
bool EulerAngles::operator==( const EulerAngles& eulerAngles ) const
{
    return m_seq == eulerAngles.m_seq
           && Vector3DFP<Angle>::operator==(eulerAngles);
}

bool EulerAngles::operator!=( const EulerAngles& eulerAngles ) const
{
    return !operator==( eulerAngles );
}

// inline i/o stream operators
std::istream&
operator>>( std::istream& is, EulerAngles& eulerAngles )
{
    unsigned sequence;
    is >> sequence;
    is >> static_cast<Vector3DFP<Angle>&>(eulerAngles);
    eulerAngles.SetRotationSequence(sequence);
    return is;
}

std::ostream&
operator<<( std::ostream& os, const EulerAngles& eulerAngles )
{
    //WARNING_FIX:
    //1>src\EulerAngles.cpp(55): warning C4244: 'initializing' : conversion from 'std::streamsize' to 'int', possible loss of data
    long long int w = os.width();
    os << eulerAngles.GetRotationSequence() << std::setw(w) << ioSeparator
       << static_cast<const Vector3DFP<Angle>&>(eulerAngles);
    return os;
}

} // A3 namespace end

