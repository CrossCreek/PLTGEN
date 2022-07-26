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
 * VectorReal3D.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// 3-Space Vector class
// Refer to VectorReal3D.h for more info.
// =============================================================================
#include "VectorReal3D.h"
#include "Angle.h"
#include "IoSeparator.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

// =============================================================================
// A3 namespace start
namespace A3
{
// -------------------------------------------------------------------------
// exceptions
template <typename FPT>
void Vector3DFP<FPT>
::ThrowBadIndex(typename Vector3DFP<FPT>::size_type index)
{
    std::ostringstream stmMessage;
    stmMessage << "<<Vector3DFP>> bad array index\n"
               << "index = " << index
               << "\nvalid indices are 0, 1, 2" << std::ends;
    std::out_of_range oor(stmMessage.str());
    throw oor;
}

template <typename FPT>
void Vector3DFP<FPT>
::ThrowBadSize(typename Vector3DFP<FPT>::size_type size)
{
    std::ostringstream stmMessage;
    stmMessage << "<<Vector3DFP>> bad input array size\n"
               << "size = " << size
               << "\nsize must be " << VECTORREAL3D_SZ_ARRAY << std::ends;
    std::out_of_range oor(stmMessage.str());
    throw oor;
}

// -------------------------------------------------------------------------
// insertion & extraction operators
template <typename FPT>
std::istream&
operator>>(std::istream& is, Vector3DFP<FPT> &vec)
{
    return is >> vec[0] >> vec[1] >> vec[2];
}

template <typename FPT>
std::ostream&
operator<<(std::ostream& os, const Vector3DFP<FPT> &vec)
{
    //WARNING_FIX:
    //1>src\VectorReal3D.cpp(58): warning C4244: 'initializing' : conversion from 'std::streamsize' to 'int', possible loss of data
    long long int w = os.width();
    return os << vec[0] << ioSeparator << std::setw(w)
           << vec[1] << ioSeparator << std::setw(w)
           << vec[2];
}

// -------------------------------------------------------------------------
// explicit instantiation
template class Vector3DFP<Angle>;
template std::ostream& operator<<(std::ostream&, const Vector3DFP<Angle>&);
template std::istream& operator>>(std::istream&, Vector3DFP<Angle>&);

template class Vector3DFP<double>;
template std::ostream& operator<<(std::ostream&, const Vector3DFP<double>&);
template std::istream& operator>>(std::istream&, Vector3DFP<double>&);

template class Vector3DFP<float>;
template std::ostream& operator<<(std::ostream&, const Vector3DFP<float>&);
template std::istream& operator>>(std::istream&, Vector3DFP<float>&);

}
// A3 namespace end
// =============================================================================
