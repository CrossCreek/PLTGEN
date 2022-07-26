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
 * Quaternion.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Quaternion class
// Refer to Quaternion.h for more info.
// =============================================================================
#include "Quaternion.h"
#include "Angle.h"
#include "IoDelim.h"
#include "MatrixFP.h"
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <float.h>
#include <memory.h>

namespace A3
{
const Quaternion::size_type Quaternion::s_szQuaternion(4);
const Quaternion::size_type Quaternion::s_szDcm(9);
const Quaternion::size_type Quaternion::s_szDcmDiag(3);
const Quaternion::size_type Quaternion::s_szVector(3);

Quaternion::Quaternion()
{
    m_q[0] = 0.0;
    m_q[1] = 0.0;
    m_q[2] = 0.0;
    m_q[3] = 1.0;
}

Quaternion::Quaternion(const MatrixFP<double>& m)
{
    if (m.IsSquare(s_szDcmDiag)) {
        FromDcm(m);
    } else {
        THROW_RUNTIME_ERROR("Bad DCM");
    }
}

Quaternion::Quaternion(const Quaternion& q)
{
    static const size_t QUATBYTES(s_szQuaternion*sizeof(double));
    memcpy(m_q, q.m_q, QUATBYTES);
}

Quaternion::Quaternion(const VectorCartesian& u, const Angle& a)
{
    static const double NEAR_ZERO(10.0*DBL_EPSILON);

    if (NEAR_ZERO < fabs(1.0-u.MagnitudeSquared())) {
        THROW_RUNTIME_ERROR("Rotation vector must be unit vector");
    }

    m_q[0] = u[0] * sin(a/2.0);
    m_q[1] = u[1] * sin(a/2.0);
    m_q[2] = u[2] * sin(a/2.0);
    m_q[3] = cos(a/2.0);

    if (!IsValid()) {
        THROW_RUNTIME_ERROR("Bad Euler axis rotation");
    }
}

void Quaternion::FromDcm(const MatrixBase<double>& m)
{
    // V5.0 changed internal quaternion representation in MatrixFP, Quaternion
    // and Simulation for consistency with internal CoorTrans implementation;
    // there is no change to input or output
    double k[s_szQuaternion];
    k[0] = 1.0+m[0][0]+m[1][1]+m[2][2];
    k[1] = 1.0+m[0][0]-m[1][1]-m[2][2];
    k[2] = 1.0-m[0][0]+m[1][1]-m[2][2];
    k[3] = 1.0-m[0][0]-m[1][1]+m[2][2];
    double *p(std::max_element(k, k+4));
    double qp(sqrt(*p)/2.0);
    double qp4(4.0*qp);

    switch (p-k) {
        case 0:
            m_q[0] = (m[1][2]-m[2][1])/qp4;
            m_q[1] = (m[2][0]-m[0][2])/qp4;
            m_q[2] = (m[0][1]-m[1][0])/qp4;
            m_q[3] = qp;
            break;

        case 1:
            m_q[0] = qp;
            m_q[1] = (m[0][1]+m[1][0])/qp4;
            m_q[2] = (m[2][0]+m[0][2])/qp4;
            m_q[3] = (m[1][2]-m[2][1])/qp4;
            break;

        case 2:
            m_q[0] = (m[0][1]+m[1][0])/qp4;
            m_q[1] = qp;
            m_q[2] = (m[1][2]+m[2][1])/qp4;
            m_q[3] = (m[2][0]-m[0][2])/qp4;
            break;

        case 3:
            m_q[0] = (m[2][0]+m[0][2])/qp4;
            m_q[1] = (m[1][2]+m[2][1])/qp4;
            m_q[2] = qp;
            m_q[3] = (m[0][1]-m[1][0])/qp4;
            break;

        default:
            THROW_RUNTIME_ERROR("Bad switch");
    }

    if (!IsValid()) {
        THROW_RUNTIME_ERROR("Bad DCM");
    }
}

void Quaternion::GetEulerAxis(VectorCartesian& u, Angle& a)
{
    static const Angle SM_ANGLE(1.0e-16);
    a = 2.0 * acos(m_q[3]);

    if (SM_ANGLE < a) {
        double sa(sin(a/2.0));
        u[0] = m_q[0] / sa;
        u[1] = m_q[1] / sa;
        u[2] = m_q[2] / sa;
    } else {
        u[0] = 1.0;
        u[1] = 0.0;
        u[2] = 0.0;
    }
}

void Quaternion::GetConjugate(Quaternion& invQ) const
{
    invQ[0] = -m_q[0];
    invQ[1] = -m_q[1];
    invQ[2] = -m_q[2];
    invQ[3] =  m_q[3];
}

void Quaternion::ConjugateThis()
{
    m_q[0] = -m_q[0];
    m_q[1] = -m_q[1];
    m_q[2] = -m_q[2];
}

bool Quaternion::IsValid() const
{
    static const double tol(1.0e-6);
    double m(m_q[0]*m_q[0]+m_q[1]*m_q[1]+m_q[2]*m_q[2]+m_q[3]*m_q[3]-1.0);
    return tol >= fabs(m);
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
    Quaternion c;
    Multiply(q,c);
    return c;
}

void Quaternion::Multiply(const Quaternion& b, Quaternion& c) const
{
    c[3] = -m_q[0]*b[0] - m_q[1]*b[1] - m_q[2]*b[2] + m_q[3]*b[3];
    c[0] =  m_q[0]*b[3] + m_q[1]*b[2] - m_q[2]*b[1] + m_q[3]*b[0];
    c[1] = -m_q[0]*b[2] + m_q[1]*b[3] + m_q[2]*b[0] + m_q[3]*b[1];
    c[2] =  m_q[0]*b[1] - m_q[1]*b[0] + m_q[2]*b[3] + m_q[3]*b[2];
}

void Quaternion::Multiply(double b, Quaternion& c) const
{
    c[0] = m_q[0]*b;
    c[1] = m_q[1]*b;
    c[2] = m_q[2]*b;
    c[3] = m_q[3]*b;
}

void Quaternion::Add(const Quaternion& b, Quaternion& c) const
{
    c[0] = m_q[0]+b[0];
    c[1] = m_q[1]+b[1];
    c[2] = m_q[2]+b[2];
    c[3] = m_q[3]+b[3];
}

void Quaternion::Transform(const VectorCartesian& vi,
                           VectorCartesian& vt) const
{
    Quaternion qt;
    qt[3] = -vi[0]*m_q[0] - vi[1]*m_q[1] - vi[2]*m_q[2];
    qt[0] =  vi[0]*m_q[3] + vi[1]*m_q[2] - vi[2]*m_q[1];
    qt[1] = -vi[0]*m_q[2] + vi[1]*m_q[3] + vi[2]*m_q[0];
    qt[2] =  vi[0]*m_q[1] - vi[1]*m_q[0] + vi[2]*m_q[3];
    vt[0] = m_q[3]*qt[0] - m_q[0]*qt[3] - m_q[1]*qt[3] + m_q[2]*qt[1];
    vt[1] = m_q[3]*qt[1] + m_q[0]*qt[3] - m_q[1]*qt[3] - m_q[2]*qt[0];
    vt[2] = m_q[3]*qt[3] - m_q[0]*qt[1] + m_q[1]*qt[0] - m_q[2]*qt[3];
}

void Quaternion::TransformInverse(const VectorCartesian& vi,
                                  VectorCartesian& vt) const
{
    Quaternion qt(*this);
    qt.ConjugateThis();
    qt.Transform(vi, vt);
}

double Quaternion::DotProduct(const Quaternion& q) const
{
    return m_q[0] * q.m_q[0]
           + m_q[1] * q.m_q[1]
           + m_q[2] * q.m_q[2]
           + m_q[3] * q.m_q[3];
}

std::ostream& operator<<(std::ostream& os, const Quaternion& q)
{
    std::streamsize w(os.width());
    return os << q[0] << ioDelim << std::setw(w)
           << q[1] << ioDelim << std::setw(w)
           << q[2] << ioDelim << std::setw(w)
           << q[3];
}

std::istream& operator>>(std::istream& is, Quaternion& q)
{
    return is >> q[0] >> q[1] >> q[2] >> q[3];
}
}


