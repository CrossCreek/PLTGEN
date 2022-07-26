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
 * JplEphHdr.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
//  FILE      : JplEphHhdr.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V4.5  2009-07-01  JAM  Initial release
//              V5.0  2010-01-18  JAM  Replaced A3::Jpl::ephObjIds global
//                                     with function

// Doxygen documentation
/// @file
/// @brief Jpl namespace, functions and classes for representing JPL objects
///
/// @details The JPL Ephemeris tables, describe the orbits of the
/// sun and planets with very high precision over relatively long time.  The
/// JPL data sets used with this library include coefficients for a pair of
/// nutation angles used for coordinate transformations and physical libration
/// angles for the moon.  The classes and functions in this header represent
/// ephemeris body, nutation angle and lunar libration angle characteristics
///
/// @author J. McCormick
/// @version V5.0 <BR> Replaced A3::Jpl::ephObjIds global with function
/// @date 2010-01-18
///
/// @author J. McCormick
/// @version V4.5 <BR> Initial release
/// @date 2009-07-01
///
/// @par Units
/// - Function Arg & Return: sec, km
/// - Stream I/O:            manipulator set
/// - Initialization Files:  defined per file i/f spec
// =============================================================================
#ifndef JPLEPHHDR_H
/// header guard macro
#define JPLEPHHDR_H "JphEphHdr V4.5"

#include "JplEphGrp.h"
#include "Handle.h"
#include "EnumMap.h"
#include <string>
#include <map>
#include <iosfwd>

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// JPL Ephemeris
namespace Jpl
{
/// Provides an id for each ephemeris and is in the same order
/// as the JPL ephemeris database
enum EphObjId {
    MERCURY, VENUS, EMB, MARS, JUPITER, SATURN, URANUS, NEPTUNE,
    PLUTO, MOON, SUN, NUTATIONS, LIBRATIONS, EARTH, INVALID_OBJECT
};

/// Contains map of EphObjId's and corresponding string representation
class EphObjIdMap : public EnumMap<EphObjId>
{
public:
    EphObjIdMap();
};

/// JPL Ephemeris Object ID/Name Map
/// @return object containing map of Ephemeris Object ID/Names
inline const EphObjIdMap& ephObjIds()
{
    static EphObjIdMap eoi;
    return eoi;
}

// ---------------------------------------------------------------------
/// JPH Ephemeris Object Description
class EphObjDesc
{
public:
    EphObjDesc();
    /// Initialize object's id, number of components and days per
    /// interval description
    /// @param [in] id object id, see EphObjIdMap
    /// @param [in] nComp number of object components, e.g.,
    /// a planet contains 3 spatial components, nutation contains
    /// 2 angle components
    /// @param [in] nIntervalDay number of day in each interpolation
    /// interval, note each interpolation interval may contain N
    /// granule intervals
    EphObjDesc(EphObjId id, unsigned nComp, unsigned nIntervalDays);
    virtual ~EphObjDesc() {;}

    /// Initialize object with an LCG that partially describes the
    /// object, see LCG description
    /// @param lcg LCG object cooresponding to this
    void Lcg(const G1050::LCG& lcg);

    /// Object id
    /// @return object id
    EphObjId Id() const
    {
        return m_idObj;
    }
    /// Location in record
    /// @return relative location of the Chebyshev coefficients
    unsigned Location() const
    {
        return m_tblLoc;
    }
    /// Components in object
    /// @return number of component in object
    unsigned NComponents() const
    {
        return m_nComponents;
    }
    /// Coefficients per component
    /// @return number of Chebyshev coefficients per component per
    /// granule interpolation interval
    unsigned NCoefficients() const
    {
        return m_nCoeff;
    }
    /// Total coefficients per granule interval
    /// @return total number of Chebyshev coefficients per granule
    /// interpolation interval
    unsigned TCoefficients() const
    {
        return m_nComponents*m_nCoeff;
    }
    /// Count of Granules
    /// @return number of granules per record interpolation interval
    unsigned NGranules() const
    {
        return m_nGranules;
    }
    /// Record interpolation interval
    /// @return number of days per record interpolation interval
    unsigned RecordInterval() const
    {
        return m_nIntervalDays;
    }
    /// Granule interpolation interval
    /// @return number of days per granule interpolation interval
    unsigned GranuleInterval() const
    {
        return m_nGranuleDays;
    }

    /// Report the state of this object to stream
    /// @param [in, out] os output stream receiving report
    /// @return output stream
    std::ostream& ReportState(std::ostream& os) const;

protected:

private:
    EphObjId m_idObj;
    unsigned m_nComponents;  // bodies 3, nutations 2, librations 3
    unsigned m_tblLoc;
    unsigned m_nCoeff;
    unsigned m_nGranules;
    unsigned m_nIntervalDays;
    unsigned m_nGranuleDays;
};

// ---------------------------------------------------------------------
/// EphObjDescMap: JPL Object Description
typedef std::map< EphObjId, Handle<EphObjDesc> > EphObjDescMap;
/// EphConstMap: JPL Ephemeris Constants
typedef std::map<std::string,double> EphConstMap;

// ---------------------------------------------------------------------
/// JPL Ephemeris Header
class EphHdr
{
public:
    /// Initial header object with the name of the JPL ephemeris
    /// header file name, reference to an object description map
    /// and constants map
    /// @param [in] fileNm header file name
    /// @param [in,out] odm ephemeris object description map
    /// @param [in,out] ecm JPL header constants map
    EphHdr(const std::string fileNm, EphObjDescMap &odm,
           EphConstMap &ecm);

    virtual ~EphHdr() {;}

    /// Report the state of this object to stream
    /// @param [in, out] os output stream receiving report
    /// @return output stream
    std::ostream& ReportState(std::ostream& os) const;

protected:
    std::istream& Init(std::istream& is, EphObjDescMap& odm);
    std::istream& Init(std::istream& is, EphConstMap& ecm);

private:
    EphHdr() {;}

    EphObjDescMap m_odm;
    EphConstMap m_ecm;

    G0000 m_g0000;
    G1010 m_g1010;
    G1030 m_g1030;

    std::string m_fileNm;
};
}
}

#endif // JPLEPHHDR_H
