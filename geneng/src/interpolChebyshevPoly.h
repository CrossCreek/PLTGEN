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
 * interpolChebyshevPoly.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
/// @file
/// @brief Chebyshev polynomial interpolation template functions
///
/// @details Interpolation of the form SUM(c[i]*t[i](x))
/// or  SUM(c[i]*t[i](x))-0.5*c[0] with option to compute the 1st derivative
///
/// @author J. McCormick
/// @version V4.5 Initial release
/// @date 2009-07-01
///
/// @par Units
/// - Function Arg & Return: N/A
/// - Stream I/O:            N/A
/// - Initialization Files:  N/A
// =============================================================================
#ifndef INTERPOLCHEBYSHEVPOLY_H
/// Include guard macro
#define INTERPOLCHEBYSHEVPOLY_H  "interpolChebyshevPoly V4.3-009"

#include <algorithm>
#include <iterator>
#include <stdexcept>

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// @brief Chebyshev polynomial interpolation
///
/// Interpolation of the form <i>SUM(c[i]*t[i](x))</i>, this assumes that
/// the 0th coefficient has already been halved, if not, use
/// interpolChebyshevPolyCZF.
/// Slower but higher precision used, code for the faster but lower
/// precision is included as commented out code below this implementation
/// @param [in] cBegin Chebyshev coefficient bi-directional begin iterator
/// @param [in] cEnd Chebyshev coefficient bi-directional end iterator
/// @param [in] x scaled Chebyshev argument, scalar, floating point type
/// @param [out] y interpolated value, scalar, floating point type
template <class InpItrCoeff, class NumTypeX, class NumTypeY>
inline void
interpolChebyshevPoly(InpItrCoeff cBegin,
                      InpItrCoeff cEnd,
                      const NumTypeX &x,  // scaled
                      NumTypeY &y )
{
    size_t sz(std::distance(cBegin, cEnd));

    if (1 < sz) {
        NumTypeX *ps(new NumTypeX[sz]);
        NumTypeX *psi(ps), *pse(ps+sz), *psl(pse-1);
        const NumTypeX xx(static_cast<NumTypeX>(2.0)*x);
        InpItrCoeff pcl(cEnd-1);
        *psi = static_cast<NumTypeX>(1.0);
        *(++psi) = static_cast<NumTypeX>(x);

        while (pse != ++psi) {
            *psi = xx * *(psi-1) - *(psi-2);
        }

        y = static_cast<NumTypeY>(*psl * *pcl);

        while (--psl >= ps) {
            y += static_cast<NumTypeY>(*psl * *(--pcl));
        }

        delete [] ps;
    } else {
        y = static_cast<NumTypeY>(*cBegin);
    }
}

// -------------------------------------------------------------------------
// Faster but lower precision implementation
// template <class InpItrCoeff, class NumTypeX, class NumTypeY>
// inline void
// interpolChebyshevPoly(InpItrCoeff cBegin,
//                       InpItrCoeff cEnd,
//                       const NumTypeX &x,  // scaled
//                       NumTypeY &y )
// {
//     NumTypeX s0(static_cast<NumTypeX>(1.0));
//     NumTypeX s1(x);
//     NumTypeX s2(0.0);
//     const NumTypeX xx(static_cast<NumTypeX>(2.0)*x);

//     InpItrCoeff i(cBegin);

//     y = static_cast<NumTypeY>(*i);

//     while (cEnd != (++i)) {
//         y += static_cast<NumTypeY>(static_cast<NumTypeX>(*i)*s1);
//         s2 = xx * s1 - s0;
//         s0 = s1;
//         s1 = s2;
//     }
//}
// -------------------------------------------------------------------------

/// @brief Chebyshev polynomial interpolation + 1st derivative
///
/// Interpolation of the form <i>SUM(c[i]*t[i](x))</i>, this assumes that
/// the 0th coefficient has already been halved, if not, use
/// interpolChebyshevPolyDCZF.
/// Slower but higher precision used, code for the faster but lower
/// precision is included as commented out code below this implementation
/// @param [in] cBegin Chebyshev coefficient bi-directional begin iterator
/// @param [in] cEnd Chebyshev coefficient bi-directional end iterator
/// @param [in] x scaled Chebyshev argument, scalar, floating point type
/// @param [out] y interpolated value, scalar, floating point type
/// @param [out] dydx interpolated dy/dx for a SCALED X!
template <class InpItrCoeff, class NumTypeX, class NumTypeY>
inline void
interpolChebyshevPolyD(InpItrCoeff cBegin,
                       InpItrCoeff cEnd,
                       const NumTypeX &x, // scaled
                       NumTypeY &y,
                       NumTypeY &dydx)
{
    size_t sz(std::distance(cBegin, cEnd));

    if (1 < sz) {
        NumTypeX *ps(new NumTypeX[sz]);
        NumTypeX *pu(new NumTypeX[sz]);
        NumTypeX *psi(ps), *pse(ps+sz), *psl(pse-1);
        NumTypeX *pui(pu), *pue(pu+sz), *pul(pue-1);
        const NumTypeX xx(static_cast<NumTypeX>(2.0)*x);
        InpItrCoeff pcl(cEnd-1);
        *psi = static_cast<NumTypeX>(1.0);
        *(++psi) = static_cast<NumTypeX>(x);
        *pui = static_cast<NumTypeX>(0.0);
        *(++pui) = static_cast<NumTypeX>(1.0);

        while (pse != ++psi) {
            *psi = xx * *(psi-1) - *(psi-2);
            ++pui;
            *pui = xx * *(pui-1) - *(pui-2)
                   + static_cast<NumTypeX>(2.0) * *(psi-1);
        }

        y = static_cast<NumTypeY>(*psl * *pcl);
        dydx = static_cast<NumTypeY>(*pul * *pcl);

        while (--psl >= ps) {
            y += static_cast<NumTypeY>(*psl * *(--pcl));
            dydx += static_cast<NumTypeY>(*(--pul) * *pcl);
        }

        delete [] ps;
        delete [] pu;
    } else {
        y = static_cast<NumTypeY>(*cBegin);
        dydx = static_cast<NumTypeY>(0.0);
    }
}

// -------------------------------------------------------------------------
// Faster but lower precision implementation
// template <class InpItrCoeff, class NumTypeX, class NumTypeY>
// inline void
// interpolChebyshevPolyD(InpItrCoeff cBegin,
//                        InpItrCoeff cEnd,
//                        const NumTypeX &x, // scaled
//                        NumTypeY &y,
//                        NumTypeY &dydx)
// {
//     NumTypeX s0(static_cast<NumTypeX>(1.0));
//     NumTypeX s1(x);
//     NumTypeX s2(0.0);
//     NumTypeX u0(static_cast<NumTypeX>(0.0));
//     NumTypeX u1(static_cast<NumTypeX>(1.0));
//     NumTypeX u2(0.0);
//     const NumTypeX xx(static_cast<NumTypeX>(2.0)*x);
//
//     InpItrCoeff i(cBegin);
//
//     y = static_cast<NumTypeY>(*i);
//     dydx = static_cast<NumTypeY>(0.0);
//
//     while (cEnd != (++i)) {
//         y += static_cast<NumTypeY>(static_cast<NumTypeX>(*i)*s1);
//         dydx += static_cast<NumTypeY>(static_cast<NumTypeX>(*i)*u1);
//         s2 = xx * s1 - s0;
//         u2 = xx * u1 - u0 + static_cast<NumTypeX>(2.0) * s1;
//         s0 = s1;
//         s1 = s2;
//         u0 = u1;
//         u1 = u2;
//     };
// }
// -------------------------------------------------------------------------

} // A3
#endif  // INTERPOLCHEBYSHEVPOLY_H
