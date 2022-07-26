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
 * CoorTrans.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTrans.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTrans

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  02/27/1998  JAM  Initial Release
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//              Change: reordered data members
//              Reason: stops pointless initialized here warnings

//              V5.0    2010-06-01  JAM deleted unused DCM

//  NOTES :
//      m_dcm describes the transformation from the source system
//      to the base system such that
//      [pos_base]   = [dcm]  * [pos_source]
//      [pos_source] = [dcm]T * [pos_base]
//      where T = transpose

//      m_vecAngularVelocity describe the rate of rotation of the
//      source system wrt the base system, described in the base
//      system frame
//      [vel_base]   = [dcm]  *  [vel_source] + [angVel] X [pos_base]
//      [vel_source] = [dcm]T * ([vel_base]   - [angVel] X [pos_base])
//      where T = transpose
//      and   X = cross product

//      m_vecAngularAcceleration describes the rotational acceleration
//      of the source system w.r.t. the base system, described in the
//      base system frame
//      [acc_base]   = [dcm] * [acc_source] + [angAcc] X [pos_base]
//                   + 2 * [angVel] X [vel_base]
//                   - [angVel] X ([angVel] X [pos_base])
//      [acc_source] = [dcm]T * ( [acc_base] - [angAcc] X [pos_base]
//                              - 2 * [angVel] X [vel_base]
//                              + [angVel] X ([angVel] X [pos_base]) )

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//

//=============================================================================
#ifndef COORTRANS_H
#define COORTRANS_H	__FILE__ " " __DATE__

#include "AutoUpdate.h"
#include "CoorSystem.h"
#include "CoorTransDcm.h"
#include "GenMath.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class CoorTrans : public AutoUpdate
{
    //-------------------------------------------------------------------------
public:
    // constructor set the DCM to identity and
    // angular velocity vector to zero
    CoorTrans( CoorSystem::Type baseSystem,
               CoorSystem::Type sourceSystem );

    virtual ~CoorTrans() {;}

    //----------------------------------------------------------------------
    // enumerated types to enable/disable Get functions
    // disabling a Get function may reduce run time
    // position is always enable
    // by default, velocity function is enabled;
    // by default, acceleration is disabled
    // if acceleration is enabled, velocity is automatically enabled
    // if velocity is disabled, acceleration is automatically disabled
    // enum VecType { POSITION, VELOCITY, ACCELERATION };
    void Enable( VecType vecType );
    void Disable( VecType vecType );
    bool IsEnabled( VecType vecType ) const;
    bool IsImplemented( VecType vecType ) const;

    //----------------------------------------------------------------------
    // Returns true if the state of the spatial object is stale, the object
    // will automatically update itself the next time a non-const Get
    // function is called
    virtual bool IsStale() const;

    //----------------------------------------------------------------------
    virtual void
    ToBase( const VectorCartesian& vecPositionSource,
            VectorCartesian& vecPositionBase );

    virtual void
    ToBase( const VectorCartesian& vecPositionSource,
            const VectorCartesian& vecVelocitySource,
            VectorCartesian& vecPositionBase,
            VectorCartesian& vecVelocityBase );

    virtual void
    ToBase( const VectorCartesian& vecPositionSource,
            const VectorCartesian& vecVelocitySource,
            const VectorCartesian& vecAccelerationSource,
            VectorCartesian& vecPositionBase,
            VectorCartesian& vecVelocityBase,
            VectorCartesian& vecAccelerationBase );

    virtual void
    ToBase( const VectorCartesian& vecPositionSource,
            VectorCartesian& vecPositionBase ) const;

    virtual void
    ToBase( const VectorCartesian& vecPositionSource,
            const VectorCartesian& vecVelocitySource,
            VectorCartesian& vecPositionBase,
            VectorCartesian& vecVelocityBase ) const;

    virtual void
    ToBase( const VectorCartesian& vecPositionSource,
            const VectorCartesian& vecVelocitySource,
            const VectorCartesian& vecAccelerationSource,
            VectorCartesian& vecPositionBase,
            VectorCartesian& vecVelocityBase,
            VectorCartesian& vecAccelerationBase ) const;

    virtual void
    ToSource( const VectorCartesian& vecPositionBase,
              VectorCartesian& vecPositionSource );

    virtual void
    ToSource( const VectorCartesian& vecPositionBase,
              const VectorCartesian& vecVelocityBase,
              VectorCartesian& vecPositionSource,
              VectorCartesian& vecVelocitySource );

    virtual void
    ToSource( const VectorCartesian& vecPositionBase,
              const VectorCartesian& vecVelocityBase,
              const VectorCartesian& vecAccelerationBase,
              VectorCartesian& vecPositionSource,
              VectorCartesian& vecVelocitySource,
              VectorCartesian& vecAccelerationSource  );

    virtual void
    ToSource( const VectorCartesian& vecPositionBase,
              VectorCartesian& vecPositionSource ) const;

    virtual void
    ToSource( const VectorCartesian& vecPositionBase,
              const VectorCartesian& vecVelocityBase,
              VectorCartesian& vecPositionSource,
              VectorCartesian& vecVelocitySource ) const;

    virtual void
    ToSource( const VectorCartesian& vecPositionBase,
              const VectorCartesian& vecVelocityBase,
              const VectorCartesian& vecAccelerationBase,
              VectorCartesian& vecPositionSource,
              VectorCartesian& vecVelocitySource,
              VectorCartesian& vecAccelerationSource  ) const;

    //----------------------------------------------------------------------
    CoorSystem::Type GetBaseSystem() const;
    CoorSystem::Type GetSourceSystem() const;

    virtual const CoorTransDcm& Dcm() const
    {
        return m_dcm;
    }

    //----------------------------------------------------------------------
    // operators
    bool operator==( const CoorTrans& coorTrans ) const;
    bool operator!=( const CoorTrans& coorTrans ) const;

    //----------------------------------------------------------------------
    const char* GetVersion() const;

protected:
    virtual CoorTransDcm& Dcm()
    {
        return m_dcm;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    enum COMPONENTS {POS_ENBL = 0x0001, POS_IMPL = 0x0010, POS_EI = POS_ENBL|POS_IMPL,
          VEL_ENBL = 0x0002, VEL_IMPL = 0x0020, VEL_EI = VEL_ENBL|VEL_IMPL,
          ACC_ENBL = 0x0004, ACC_IMPL = 0x0040, ACC_EI = ACC_ENBL|ACC_IMPL,
          BIT_ENBL = POS_ENBL|VEL_ENBL|ACC_ENBL,
          BIT_IMPL = POS_IMPL|VEL_IMPL|ACC_IMPL,
          BIT_EI   = BIT_ENBL|BIT_IMPL,
          STALE    = 0x0080,
          ANGVEL0  = 0x0100,
          ANGACC0  = 0x0200
         };

    // Implementation status
    void ImplClrSet( unsigned bitsImpl );
    void EnblClrSet( unsigned bitsImpl );
    void EIClrSet( unsigned bitsImpl );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // AutoUpdate calls this function when the state of the spatial object
    // needs to be recomputed.  Derived classes must implement this
    // function such that the base class UpdateState class is called and
    // then recompute is data members
    virtual void UpdateState();
    virtual void MakeStale();

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    void ThrowStaleCoorTrans( const std::string& strFunctionName ) const;
    void ThrowToBasePos() const;
    void ThrowToBasePosVel() const;
    void ThrowToBasePosVelAcc() const;
    void ThrowToSourcePos() const;
    void ThrowToSourcePosVel() const;
    void ThrowToSourcePosVelAcc() const;
    void ThrowNotEnabled( const std::string& strFunctionName ) const;
    void ThrowNotImplemented( const std::string& strFunctionName ) const;

    void SetAngularVelocity( const VectorCartesian& vec );
    void SetAngularVelocityZero();

    void SetAngularAcceleration( const VectorCartesian& acc );
    void SetAngularAccelerationZero();

    void SetSourceSystem( CoorSystem::Type sourceType );
    void SetBaseSystem( CoorSystem::Type baseType );

private:
    // m_dcm describes the transformation from the source system
    // to the base system such that
    // [pos_base]   = [dcm]  * [pos_source]
    // [pos_source] = [dcm]T * [pos_base]
    // where T = transpose
    CoorTransDcm m_dcm;

    // m_vecAngularVelocity describes the rotational rate of the
    // source system w.r.t. the base system, described in the base
    // system frame
    // [vel_base]   = [dcm]  *  [vel_source] + [angVel] X [pos_base]
    // [vel_source] = [dcm]T * ([vel_base]   - [angVel] X [pos_base])
    // where T = transpose
    // and   X = cross product
    VectorCartesian m_vecAngularVelocity;

    // m_vecAngularAcceleration describes the rotational acceleration
    // of the source system w.r.t. the base system, described in the
    // base system frame
    // [acc_base]   = [dcm] * [acc_source] + [angAcc] X [pos_base]
    //              + 2 * [angVel] X [vel_base]
    //              - [angVel] X ([angVel] X [pos_base])
    // [acc_source] = [dcm]T * ( [acc_base] - [angAcc] X [pos_base]
    //                         - 2 * [angVel] X [vel_base]
    //                         + [angVel] X ([angVel] X [pos_base]) )
    VectorCartesian m_vecAngularAcceleration;

    CoorSystem::Type m_baseSystem;
    CoorSystem::Type m_sourceSystem;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Enabled & Implemented bits
    unsigned m_fgEIS;
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// constructors & destructors

//-----------------------------------------------------------------------------
// access
inline void
CoorTrans::SetAngularVelocity( const VectorCartesian& vec )
{
    m_vecAngularVelocity = vec;

    if (m_vecAngularVelocity.MagnitudeSquared() < 1.0e-300) {
        setBitsOn(ANGVEL0, m_fgEIS);
    } else {
        setBitsOff(ANGVEL0, m_fgEIS);
    }
}

inline void
CoorTrans::SetAngularVelocityZero()
{
    m_vecAngularVelocity.Set( 0.0, 0.0, 0.0 );
    setBitsOn(ANGVEL0, m_fgEIS);
}

inline void
CoorTrans::SetAngularAcceleration( const VectorCartesian& vec )
{
    m_vecAngularAcceleration = vec;

    if (m_vecAngularAcceleration.MagnitudeSquared() < 1.0e-300) {
        setBitsOn(ANGACC0, m_fgEIS);
    } else {
        setBitsOff(ANGACC0, m_fgEIS);
    }
}

inline void
CoorTrans::SetAngularAccelerationZero()
{
    m_vecAngularAcceleration.Set( 0.0, 0.0, 0.0 );
    setBitsOn(ANGACC0, m_fgEIS);
}

//------------------------------------------------------------------------------
inline void CoorTrans::Enable( VecType vecType )
{
    switch ( vecType ) {
        case VELOCITY:
            setBitsOn(VEL_ENBL, m_fgEIS);
            break;

        case ACCELERATION:
            setBitsOn(VEL_ENBL | ACC_ENBL, m_fgEIS);
            break;

        case POSITION:
            break;

        default:
            throw std::invalid_argument(
                "CoorTrans::Enable(VecType) Invalid VecType");
    }
}

inline void CoorTrans::Disable( VecType vecType )
{
    switch ( vecType ) {
        case VELOCITY:
            setBitsOff(VEL_ENBL | ACC_ENBL, m_fgEIS);
            break;

        case ACCELERATION:
            setBitsOff(ACC_ENBL, m_fgEIS);
            break;

        case POSITION:
            throw std::invalid_argument(
                "CoorTrans::Disable(VecType) VecType POSITION invalid");

        default:
            throw std::invalid_argument(
                "CoorTrans::Disable(VecType) Invalid VecType");
    }
}

inline bool CoorTrans::IsEnabled( VecType vecType ) const
{
    bool bIsEnabled(false);

    switch ( vecType ) {
        case VELOCITY:
            bIsEnabled = areBitsOn(VEL_EI, m_fgEIS);
            break;

        case ACCELERATION:
            bIsEnabled = areBitsOn(ACC_EI, m_fgEIS);
            break;

        case POSITION:
            bIsEnabled = areBitsOn(POS_EI, m_fgEIS);
            break;

        default:
            throw std::invalid_argument(
                "CoorTrans::IsEnabled(VecType) Invalid VecType");
    }

    return bIsEnabled;
}

//------------------------------------------------------------------------------
inline void CoorTrans::ImplClrSet( unsigned bitsImpl )
{
    setBitsOn(bitsImpl&BIT_IMPL, m_fgEIS);
}

inline void CoorTrans::EnblClrSet( unsigned bitsImpl )
{
    setBitsOn(bitsImpl&BIT_ENBL, m_fgEIS);
}

inline void CoorTrans::EIClrSet( unsigned bitsImpl )
{
    setBitsOn(bitsImpl&BIT_EI, m_fgEIS);
}

inline bool CoorTrans::IsImplemented( VecType vecType ) const
{
    bool bIsImpl(false);

    switch ( vecType ) {
        case VELOCITY:
            bIsImpl = areBitsOn(VEL_IMPL, m_fgEIS);
            break;

        case ACCELERATION:
            bIsImpl = areBitsOn(ACC_IMPL, m_fgEIS);
            break;

        case POSITION:
            bIsImpl = true;
    }

    return bIsImpl;
}

//-----------------------------------------------------------------------------
inline void
CoorTrans::UpdateState()
{
    setBitsOff(STALE, m_fgEIS);
}

inline void
CoorTrans::MakeStale()
{
    setBitsOn(STALE, m_fgEIS);
}

inline bool
CoorTrans::IsStale() const
{
    return areBitsOn(STALE, m_fgEIS);
}

//-----------------------------------------------------------------------------
// misc functions
inline const char* CoorTrans::GetVersion() const
{
    return COORTRANS_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // COORTRANS_H
