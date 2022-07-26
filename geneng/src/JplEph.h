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
 * JplEph.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
/// @file
/// @brief Jpl namespace, functions and classes for processing JPL ephemeris data
///
/// @details The JPL Ephemeris tables, describe the orbits of the
/// sun and planets with very high precision over relatively long time.  The
/// JPL data sets used with this library include coefficients for a pair of
/// nutation angles used for coordinate transformations and physical libration
/// angles for the moon.
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
/// - Initialization Files:  defined per file i/f spec
// =============================================================================
#ifndef JPLEPH_H
/// header guard macro
#define JPLEPH_H "JphEph V5.0"

#include "JplEphHdr.h"
#include "Handle.h"
#include "TimeJ2000.h"
#include "constants.h"
#include "VectorCartesian.h"
#include "Angle.h"
#include <map>
#include <vector>
#include <iosfwd>
#include <string>
#include <ostream>
#include <istream>
#include <math.h>
#include <utility>

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// JPL Ephemeris
namespace Jpl
{
// ---------------------------------------------------------------------
/// JPL Chebyshev Coefficient Record
class EphRec
{
public:
    typedef double * iterator;
    typedef const double * const_iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    EphRec();
    EphRec(const EphRec& er);
    /// Read chebyshev coefficient record from stream and initialize
    /// @param [in] sz size of record
    /// @param [in,out] is input stream
    EphRec(unsigned sz, std::istream& is );
    virtual ~EphRec();

    EphRec& operator=(const EphRec& er);

    /// Start of interpolation interval
    /// @return interval start time, J2000 TT sec
    const TimeJ2000& TmStart() const
    {
        return m_tmStr;
    }
    /// End of interpolation interval
    /// @return interval end time, J2000 TT sec
    const TimeJ2000& TmEnd() const
    {
        return m_tmEnd;
    }

    /// Is time within interval
    /// @param [in] tm time to test, J2000 TT sec
    /// @return true if tm is within interval, else false
    bool IsInInterval(const TimeJ2000& tm) const;

    /// Coefficients begin
    /// @return iterator to 1st coefficient
    const_iterator begin() const
    {
        return m_pCoeff;
    }
    /// Coefficients end
    /// @return iterator beyond last coefficient
    const_iterator end() const
    {
        return m_pCoeff+m_sz;
    }
    /// Count of coefficients
    /// @return number of coefficients in record
    size_type size() const
    {
        return m_sz;
    }

protected:
private:
    TimeJ2000 m_tmStr;
    TimeJ2000 m_tmEnd;
    double *m_pCoeff;
    size_type m_sz;
};

typedef std::vector<EphRec> EphRecVec;

// ---------------------------------------------------------------------
/// JPL Object Description and Coefficient Set
class EphCoeff
{
public:
    typedef double * iterator;
    typedef const double * const_iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    EphCoeff();
    EphCoeff(const EphCoeff& ec);
    /// Initialize from Ephemeris Object Description and
    /// Coefficient Record
    /// @param [in] hDesc handle to an ephemeris object description
    /// @param [in] tmStr interpolation period start, J2000 TT sec
    /// @param [in] tmEnd interpolation period end, J2000 TT sec
    /// @param [in] pCoeff Chebyshev coefficient recort
    EphCoeff(Handle<EphObjDesc> hDesc,
             const TimeJ2000& tmStr, const TimeJ2000& tmEnd,
             EphRec::const_iterator pCoeff);
    virtual ~EphCoeff();

    EphCoeff& operator=(const EphCoeff& ec);

    /// Interpolated component value
    /// @param [in] tm interpolation time, J2000 TT sec
    /// @param [in] index component index
    /// @param [out] s value of component at tm
    template <typename T>
    void Component(const TimeJ2000& tm, unsigned index,
                   T& s) const;

    /// Interpolated component value and rate
    /// @param [in] tm interpolation time, J2000 TT sec
    /// @param [in] index component index
    /// @param [out] s value of component at tm
    /// @param [out] r component rate of change at tm, per sec
    template <typename T>
    void Component(const TimeJ2000& tm, unsigned index,
                   T& s, T& r) const;

    /// Interpolated vector value for objects with 3 components
    /// @param [in] tm interpolation time, J2000 TT sec
    /// @param [out] s value of vector at tm
    void Component(const TimeJ2000& tm,
                   VectorCartesian& s) const;

    /// Interpolated vector value and rate for objects with
    /// 3 components
    /// @param [in] tm interpolation time, J2000 TT sec
    /// @param [out] s value of vector at tm
    /// @param [out] r vector rate of change at tm, per sec
    void Component(const TimeJ2000& tm,
                   VectorCartesian& s,
                   VectorCartesian& r) const;

    /// Component begin
    /// @param [in] index component index
    /// @return iterator to 1st component
    const_iterator begin_component(unsigned index) const;
    /// Component end
    /// @param [in] index component index
    /// @return iterator beyond last component
    const_iterator end_component(unsigned index) const;

    /// Count of components
    /// @return number of components
    unsigned NComponents() const
    {
        return m_hDesc->NComponents();
    }

    /// Count of coefficients
    /// @return number of coefficients per component per granule
    unsigned NCoefficients() const
    {
        return m_hDesc->NCoefficients();
    }

    /// Total count of coefficients
    /// @return total number of coefficients per granule
    unsigned TCoefficients() const
    {
        return m_hDesc->TCoefficients();
    }

    /// Ephemeris Object Description
    /// @return handle to Ephemeris Object Description
    const Handle<EphObjDesc> Description() const
    {
        return m_hDesc;
    }

    /// Is time within interpolation period
    /// @param tm time to be tested, J2000 TT sec
    /// @return true if time is within interpolation interval
    /// else false
    bool IsInSpan(const TimeJ2000& tm) const
    {
        return m_tmStr<=tm && (m_tmStr+m_dt)>=tm;
    }

    /// Interpolation period
    /// @return interpolation start, end, J2000 TT sec
    std::pair<TimeJ2000, TimeJ2000> TimePeriod() const
    {
        return std::pair<TimeJ2000,TimeJ2000>(m_tmStr,m_tmStr+m_dt);
    }

    /// Report the state of this object to stream
    /// @param [in, out] os output stream receiving report
    /// @return output stream
    std::ostream& ReportState(std::ostream& os) const;

protected:
private:
    Handle<EphObjDesc> m_hDesc;
    TimeJ2000 m_tmStr;
    double m_dt;
    double* m_pCoeff;
};

// ---------------------------------------------------------------------
/// Ephemeris Interpolation Interval
class EphInterval
{
public:
    EphInterval();
    /// Initialize from an Ephemeris Object Description and
    /// Ephemeris record
    /// @param [in] handle to an Ephemeris Object Description
    /// @param [in] handle to an Ephemeris Record
    EphInterval(Handle<EphObjDesc> hDesc, const EphRec& rec);
    /// Initialize from an Ephemeris Object Description and
    /// a container of Ephemeris Records
    /// @param [in] handle to an Ephemeris Object Description
    /// @param [in] iterator to the 1st ephemeris record
    /// @param [in] iterator beyond the last ephemeris record
    EphInterval(Handle<EphObjDesc> hDesc,
                EphRecVec::const_iterator itr,
                EphRecVec::const_iterator etr);

    /// Ephemeris object description
    /// @return handle to the Ephemeris Object Description
    const Handle<EphObjDesc> Description() const
    {
        return m_hDesc;
    }

    /// Ephemeris coefficients at specified time
    /// @param [in] tm time, J2000 TT tm
    /// @param [out] ec set of ephemeris coefficients valid at tm
    void Coefficients(const TimeJ2000& tm, EphCoeff& ec) const;

    /// Is time within interpolation period
    /// @param tm time to be tested, J2000 TT sec
    /// @return true if time is within interpolation interval
    /// else false
    bool IsInSpan(const TimeJ2000& tm) const
    {
        return m_tmStr<=tm && m_tmEnd>=tm;
    }

    /// Interpolation period
    /// @return interpolation start, end, J2000 TT sec
    std::pair<TimeJ2000, TimeJ2000> TimePeriod() const
    {
        return std::pair<TimeJ2000,TimeJ2000>(m_tmStr,m_tmEnd);
    }

    /// Report the state of this object to stream
    /// @param [in, out] os output stream receiving report
    /// @return output stream
    std::ostream& ReportState(std::ostream& os) const;

protected:
    void AddCoefficients(double t, double dt,
                         EphRec::const_iterator itr, EphRec::const_iterator etr);
private:
    typedef std::map<TimeJ2000,EphCoeff>::iterator iterator;
    typedef std::map<TimeJ2000,EphCoeff>::const_iterator
    const_iterator;
    typedef std::map<TimeJ2000,EphCoeff>::size_type size_type;
    typedef std::map<TimeJ2000,EphCoeff>::difference_type
    difference_type;

    Handle<EphObjDesc> m_hDesc;
    TimeJ2000 m_tmStr;
    TimeJ2000 m_tmEnd;
    std::map<TimeJ2000,EphCoeff> m_ctCoeff;
};

// ---------------------------------------------------------------------
/// JPL Ephemeris Dataset
class Eph
{
public:
    Eph();

    /// JPL ephemeris interval for a specified object and time
    /// @param [in] id ephemeris object
    /// @param [in] tmRef time, J2000 TT sec
    /// @param [out] itvl ephemeris interval valid for id and tmRef
    void Interval(EphObjId id, const TimeJ2000& tmRef,
                  EphInterval& itvl);

    /// JPL ephemeris constant
    /// @param [in] nm name of ephemeris constant
    /// @return value of constant
    double Constant(std::string nm)
    {
        ChkInit();
        return m_ecm[nm];
    }

    /// Ephemeris object description
    /// @return handle to the Ephemeris Object Description
    Handle<EphObjDesc> Description(EphObjId id)
    {
        return m_odm[id];
    }

    /// Ephemeris record containing all objects
    /// @param [in] tm time, J2000 TT sec
    /// @return ephemeris record valid at tm
    const EphRec& Record(const TimeJ2000& tm);

    /// Report the state of ephemeris header to stream
    /// @param [in, out] os output stream receiving report
    /// @return output stream
    std::ostream&
    ReportHeader(std::ostream& os);

    /// Count of ephmeris records in buffer
    /// @return number of records in buffer
    static EphRecVec::size_type NRecBuf()
    {
        return s_nRec;
    }

protected:

    struct CoeffHdr {
        char name[16];
        char version[16];
        double t0;
        double t1;
        unsigned intervalDays;
        unsigned recLen;
        unsigned recCnt;
        unsigned posRec1;
        char cReserve[32];
    };

private:
    void ChkInit()
    {
        if (m_odm.empty()) {
            Init();
        }
    }
    void Init();

    CoeffHdr m_hdr;

    EphRecVec m_ctRec;
    //Handle<EphRec> m_hCurRec;

    EphObjDescMap m_odm;
    EphConstMap m_ecm;

    std::string m_fileNm;

    static const EphRecVec::size_type s_nRec;
    static const int       s_nRecBack;
};

/// JPL Ephemeris Dataset Static Object
// extern Eph eph;
inline Eph& eph()
{
    static Eph e;
    return e;
}

// ---------------------------------------------------------------------
/// Base Class for JPL Objects
class EphObjBase
{
public:
    /// Initialize class with object id
    /// @param [in] id object id, see EphObjId



    explicit EphObjBase(EphObjId id) : m_id(id) {;}

    virtual ~EphObjBase() {;}

protected:
    void CondUpdate(const TimeJ2000 &tm);

    EphObjId m_id;

    EphInterval m_itvl;
    EphCoeff m_coeff;

private:
    EphObjBase() {;}
};

// ---------------------------------------------------------------------
/// Nutations
class Nutations : public EphObjBase
{
public:
    Nutations()
        : EphObjBase(NUTATIONS), m_long(0.0), m_oblq(0.0) {;}

    /// Nutation in longitude
    /// @param [in] tm time, J2000 TT sec
    /// @return longitude nutation angle, rad
    Angle Longitude(const TimeJ2000 &tm);
    /// Nutation in obliquity
    /// @param [in] tm time, J2000 TT sec
    /// @return obliquity nutation angle, rad
    Angle Obliquity(const TimeJ2000 &tm);

    /// Nutation and rate in longitude
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] longitude nutation angle, rad
    /// @param [out] longitude nutation rate, rad/sec
    void LongRate(const TimeJ2000 &tm, Angle &lon, Angle &rate);
    /// Nutation and rate in obliquity
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] obliquity nutation angle, rad
    /// @param [out] obliquity nutation rate, rad/sec
    void OblqRate(const TimeJ2000 &tm, Angle &oblq, Angle &rate);

protected:

private:
    TimeJ2000 m_tmL;
    Angle m_long;
    TimeJ2000 m_tmQ;
    Angle m_oblq;
};

// extern Nutations nutations;
inline Nutations& nutations()
{
    static Nutations nut;
    return nut;
}

// ---------------------------------------------------------------------
/// Body Position & Velocity w.r.t. the Solar System Barycenter
template <EphObjId id>
class EphObjSSB : public EphObjBase
{
public:
    EphObjSSB() : EphObjBase(id) {;}

    /// Position given time
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] p position, standard length
    void Position(const TimeJ2000 &tm, VectorCartesian& p);
    /// Velocity given time
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] v position, standard length per sec
    void Velocity(const TimeJ2000 &tm, VectorCartesian& v);
    /// Position & velocity given time
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] p position, standard length
    /// @param [out] v position, standard length per sec
    void PosVel(const TimeJ2000 &tm,
                VectorCartesian& p, VectorCartesian& v);

protected:
private:
    TimeJ2000 m_tmP;
    VectorCartesian m_pos;
    TimeJ2000 m_tmV;
    VectorCartesian m_vel;

    void UpdatePosVel(const TimeJ2000 &tm);
};

// ---------------------------------------------------------------------
/// @brief Earth Specialization of Body Position & Velocity
/// w.r.t. the Solar System Barycenter
/// @details The JPL DE datasets don't directly provide the Earth's
/// position w.r.t. the solar system barycenter (SSB); the Earth's
/// position and velocity is derived from the Earth-Moon barycenter
/// w.r.t. the SSB  and the Moon's center w.r.t. the geocenter
/// which are provided
template <>
class EphObjSSB<EARTH> : public EphObjBase
{
public:
    EphObjSSB() : EphObjBase(EMB) {;}

    /// Position given time
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] p position, standard length
    void Position(const TimeJ2000 &tm, VectorCartesian& p);
    /// Velocity given time
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] v position, standard length per sec
    void Velocity(const TimeJ2000 &tm, VectorCartesian& v);
    /// Position & velocity given time
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] p position, standard length
    /// @param [out] v position, standard length per sec
    void PosVel(const TimeJ2000 &tm,
                VectorCartesian& p, VectorCartesian& v);

protected:
private:
    /// Moon geocentric position and velocity is given in the
    /// JPL DE datasets; use with the Earth-Moon barycenter given
    /// in the JPL DE datasets to determine the Earth's position
    /// w.r.t. the solar system barycenter
    class MoonGC : public EphObjBase
    {
    public:
        MoonGC() : EphObjBase(MOON) {;}
        void Position(const TimeJ2000 &tm, VectorCartesian& p);
        void PosVel(const TimeJ2000 &tm,
                    VectorCartesian& p, VectorCartesian& v);
    private:
        TimeJ2000 m_tmP;
        VectorCartesian m_pos;
        TimeJ2000 m_tmV;
        VectorCartesian m_vel;
    };

    TimeJ2000 m_tmP;
    VectorCartesian m_pos;
    TimeJ2000 m_tmV;
    VectorCartesian m_vel;

    MoonGC m_moon;

    void UpdatePosVel(const TimeJ2000 &tm);
};

// ---------------------------------------------------------------------
/// Body Position & Velocity w.r.t. the Geocenter
template <EphObjId id>
class EphObjGC : public EphObjBase
{
public:
    EphObjGC() : EphObjBase(id) {;}

    /// Position given time
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] p position, standard length
    void Position(const TimeJ2000 &tm, VectorCartesian& p);
    /// Velocity given time
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] v position, standard length per sec
    void Velocity(const TimeJ2000 &tm, VectorCartesian& v);
    /// Position & velocity given time
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] p position, standard length
    /// @param [out] v position, standard length per sec
    void PosVel(const TimeJ2000 &tm,
                VectorCartesian& p, VectorCartesian& v);

protected:
private:
    TimeJ2000 m_tmP;
    VectorCartesian m_pos;
    TimeJ2000 m_tmV;
    VectorCartesian m_vel;

    EphObjSSB<EARTH> m_earth;

    void UpdatePosVel(const TimeJ2000 &tm);
};

/// @brief Moon Specialization of Body Position & Velocity
/// w.r.t. the Geocenter
/// @details Unlike other JPL DE bodies, the JPL DE directly provides
/// the Moon's center w.r.t. the geocenter
template <>
class EphObjGC<MOON> : public EphObjBase
{
public:
    EphObjGC() : EphObjBase(MOON) {;}

    /// Position given time
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] p position, standard length
    void Position(const TimeJ2000 &tm, VectorCartesian& p);
    /// Velocity given time
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] v position, standard length per sec
    void Velocity(const TimeJ2000 &tm, VectorCartesian& v);
    /// Position & velocity given time
    /// @param [in] tm time, J2000 TT sec
    /// @param [out] p position, standard length
    /// @param [out] v position, standard length per sec
    void PosVel(const TimeJ2000 &tm,
                VectorCartesian& p, VectorCartesian& v);

protected:
private:
    TimeJ2000 m_tmP;
    VectorCartesian m_pos;
    TimeJ2000 m_tmV;
    VectorCartesian m_vel;

    void UpdatePosVel(const TimeJ2000 &tm);
};

// ---------------------------------------------------------------------
}
}

#endif // JPLEPH_H
