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
 * JplEphGrp.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
/// @file
/// @brief Jpl namespace, functions and classes for processing JPL
/// ephemeris header groups
///
/// @details The JPL Ephemeris tables, describe the orbits of the
/// sun and planets with very high precision over relatively long time.  The
/// JPL data sets used with this library include coefficients for a pair of
/// nutation angles used for coordinate transformations and physical libration
/// angles for the moon.  Classes and functions in this header are used to
/// read and process JPL header groups
///
/// @author J. McCormick
/// @version V4.5 Initial release
/// @date 2009-07-01
///
/// @par Units
/// - Function Arg & Return: sec, km
/// - Stream I/O:            manipulator set
/// - Initialization Files:  defined per file i/f spec
// =============================================================================
#ifndef JPLEPHGRP_H
/// header guard macro
#define JPLEPHGRP_H "JphEphGrp V4.5"

#include <string>
#include <vector>
#include <iosfwd>

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// JPL Ephemeris
namespace Jpl
{
// ---------------------------------------------------------------------
/// JPL Ephemeris Group 0000
class G0000
{
public:
    G0000();
    virtual ~G0000() {;}

    /// Header record length, ignored
    /// @return header KSIZE value
    unsigned KSize() const
    {
        return m_ksize;
    }
    /// Number of coefficients per ephemeris record
    /// @return header NCOEFF value
    unsigned NCoeff() const
    {
        return m_ncoeff;
    }

    /// Initialize this object from stream
    /// @param [in] is header input stream
    /// @return input stream
    std::istream& Init(std::istream& is);
    /// Report the state of this object to stream
    /// @param [in, out] os output stream receiving report
    /// @return output stream
    std::ostream& ReportState(std::ostream& os) const;

protected:

private:
    unsigned m_ksize;
    unsigned m_ncoeff;
};

// ---------------------------------------------------------------------
/// JPL Ephemeris Group 1010
class G1010
{
public:
    G1010();
    virtual ~G1010() {;}

    /// Ephemeris name
    /// @return name of JPL ephemeris
    const std::string& EphName() const
    {
        return m_sEphName;
    }
    /// Ephemeris start date
    /// @return start date as text
    const std::string& EphStr() const
    {
        return m_sEphStr;
    }
    /// Ephemeris end date
    /// @return end date as text
    const std::string& EphEnd() const
    {
        return m_sEphEnd;
    }

    /// Initialize this object from stream
    /// @param [in] is header input stream
    /// @return input stream
    std::istream& Init(std::istream& is);
    /// Report the state of this object to stream
    /// @param [in, out] os output stream receiving report
    /// @return output stream
    std::ostream& ReportState(std::ostream& os) const;

protected:

private:
    std::string m_sEphName;
    std::string m_sEphStr;
    std::string m_sEphEnd;
};

// ---------------------------------------------------------------------
/// JPL Ephemeris Group 1030
class G1030
{
public:
    G1030();
    virtual ~G1030() {;}

    /// Ephemeris start date
    /// @return start date as a Julian Date
    double EphStr() const
    {
        return m_jdEphStr;
    }
    /// Ephemeris end date
    /// @return end date as a Julian Date
    double EphEnd() const
    {
        return m_jdEphEnd;
    }

    /// Chebyshev interpolation interval
    /// @return interpolation interval, days
    unsigned Interval() const
    {
        return m_intervalDays;
    }

    /// Initialize this object from stream
    /// @param [in] is header input stream
    /// @return input stream
    std::istream& Init(std::istream& is);
    /// Report the state of this object to stream
    /// @param [in, out] os output stream receiving report
    /// @return output stream
    std::ostream& ReportState(std::ostream& os) const;

protected:

private:
    double    m_jdEphStr;
    double    m_jdEphEnd;
    unsigned  m_intervalDays;
};

// ---------------------------------------------------------------------
/// JPL Ephemeris Group 1040
class G1040
{
public:
    typedef std::vector<std::string>::iterator iterator;
    typedef std::vector<std::string>::const_iterator const_iterator;
    typedef std::vector<std::string>::size_type size_type;
    typedef std::vector<std::string>::value_type value_type;
    typedef std::vector<std::string>::reference reference;

    G1040();
    virtual ~G1040() {;}

    /// Count of constant names
    /// @return number of constants in header
    size_type size() const
    {
        return m_ctConstNm.size();
    }

    /// Constant names begin
    /// @return iterator to the 1st constant name
    const_iterator begin() const
    {
        return m_ctConstNm.begin();
    }
    /// Constant names begin
    /// @return iterator beyond the last constant name
    const_iterator end() const
    {
        return m_ctConstNm.end();
    }

    /// Initialize this object from stream
    /// @param [in] is header input stream
    /// @return input stream
    std::istream& Init(std::istream& is);
    /// Report the state of this object to stream
    /// @param [in, out] os output stream receiving report
    /// @return output stream
    std::ostream& ReportState(std::ostream& os) const;

protected:

private:
    std::vector<std::string> m_ctConstNm;
};

// ---------------------------------------------------------------------
/// JPL Ephemeris Group 1041
class G1041
{
public:
    typedef std::vector<double>::iterator iterator;
    typedef std::vector<double>::const_iterator const_iterator;
    typedef std::vector<double>::size_type size_type;
    typedef std::vector<double>::value_type value_type;
    typedef std::vector<double>::reference reference;

    G1041();
    virtual ~G1041() {;}

    /// Count of constant values
    /// return number of constants in header
    size_type size() const
    {
        return m_ctConst.size();
    }

    /// Constant value begin
    /// @return iterator to the 1st constant value
    const_iterator begin() const
    {
        return m_ctConst.begin();
    }
    /// Constant value begin
    /// @return iterator beyond the last constant value
    const_iterator end() const
    {
        return m_ctConst.end();
    }

    /// Initialize this object from stream
    /// @param [in] is header input stream
    /// @return input stream
    std::istream& Init(std::istream& is);
    /// Report the state of this object to stream
    /// @param [in, out] os output stream receiving report
    /// @return output stream
    std::ostream& ReportState(std::ostream& os) const;

protected:

private:
    std::vector<double> m_ctConst;
};

// ---------------------------------------------------------------------
/// JPL Ephemeris Group 1050
class G1050
{
public:
    /// @brief LCG = Location, Coefficients, Granules
    /// @details Contains the relative record location, number of
    /// Chebyshev coefficients per record and number of granules
    /// per record for either an ephemeris body, nutation angles
    /// or lunar libration angles
    struct LCG {
        LCG()
            : m_tblLoc(0), m_nCoeff(0), m_nGranules(0) {;}

        LCG(unsigned loc, unsigned nCoeff, unsigned nGran)
            : m_tblLoc(loc), m_nCoeff(nCoeff), m_nGranules(nGran) {;}

        unsigned m_tblLoc;
        unsigned m_nCoeff;
        unsigned m_nGranules;
    };

    typedef std::vector<LCG>::iterator iterator;
    typedef std::vector<LCG>::const_iterator const_iterator;
    typedef std::vector<LCG>::size_type size_type;
    typedef std::vector<LCG>::value_type value_type;
    typedef std::vector<LCG>::reference reference;

    G1050() {;}
    virtual ~G1050() {;}

    /// Count of LCGs
    /// @return number of LCGs in header
    size_type size() const
    {
        return m_ctLcg.size();
    }

    /// Constant value begin
    /// @return iterator to the 1st LCG
    const_iterator begin() const
    {
        return m_ctLcg.begin();
    }
    /// Constant value begin
    /// @return iterator beyond the last LCG
    const_iterator end() const
    {
        return m_ctLcg.end();
    }

    /// Initialize this object from stream
    /// @param [in] is header input stream
    /// @return input stream
    std::istream& Init(std::istream& is);
    /// Report the state of this object to stream
    /// @param [in, out] os output stream receiving report
    /// @return output stream
    std::ostream& ReportState(std::ostream& os) const;

protected:

private:
    std::vector<LCG> m_ctLcg;
};

}
}

#endif // JPLEPHGRP_H
