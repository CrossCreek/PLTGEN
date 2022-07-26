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
 * CoorTrans.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Coordinate transformation base class
// Refer to CoorTrans.h for more info.
// =============================================================================
#include "CoorTrans.h"
#include <sstream>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
CoorTrans::CoorTrans( CoorSystem::Type baseSystem,
                      CoorSystem::Type sourceSystem )
    : m_dcm(MatrixOption::IDENTITY),
      m_vecAngularVelocity(0.0),
      m_vecAngularAcceleration(0.0),
      m_baseSystem(baseSystem),
      m_sourceSystem(sourceSystem),
      m_fgEIS(POS_EI|ANGVEL0|ANGACC0|STALE)
{ ; }

CoorSystem::Type
CoorTrans::GetBaseSystem() const
{
    return m_baseSystem;
}

void
CoorTrans::SetSourceSystem( CoorSystem::Type sourceType )
{
    m_sourceSystem = sourceType;
}

void
CoorTrans::SetBaseSystem( CoorSystem::Type baseType )
{
    m_baseSystem = baseType;
}

CoorSystem::Type
CoorTrans::GetSourceSystem() const
{
    return m_sourceSystem;
}

bool CoorTrans::operator==( const CoorTrans& coorTrans ) const
{
    return            m_baseSystem == coorTrans.m_baseSystem
                      &&          m_sourceSystem == coorTrans.m_sourceSystem
                      &&                   m_dcm == coorTrans.m_dcm
                      &&                 m_fgEIS == coorTrans.m_fgEIS;
}

bool CoorTrans::operator!=( const CoorTrans& coorTrans ) const
{
    return !operator==( coorTrans );
}

void
CoorTrans::ToBase( const VectorCartesian& vecPositionSource,
                   VectorCartesian& vecPositionBase )
{
    Update();
    m_dcm.Multiply(vecPositionSource, vecPositionBase);
}

void
CoorTrans::ToBase( const VectorCartesian& vecPositionSource,
                   VectorCartesian& vecPositionBase ) const
{
    if (areBitsOn(STALE, m_fgEIS)) {
        ThrowToBasePos();
    }

    m_dcm.Multiply(vecPositionSource, vecPositionBase);
}

void
CoorTrans::ToBase( const VectorCartesian& vecPositionSource,
                   const VectorCartesian& vecVelocitySource,
                   VectorCartesian& vecPositionBase,
                   VectorCartesian& vecVelocityBase )
{
    if (!areBitsOn(VEL_EI, m_fgEIS)) {
        ThrowNotEnabled(__FUNCSIG__);
    }

    Update();
    m_dcm.Multiply(vecPositionSource, vecPositionBase);
    m_dcm.Multiply(vecVelocitySource, vecVelocityBase);

    if ( !areBitsOn(ANGVEL0, m_fgEIS) ) {
        vecVelocityBase += m_vecAngularVelocity.CrossProduct(vecPositionBase);
    }
}

void
CoorTrans::ToBase( const VectorCartesian& vecPositionSource,
                   const VectorCartesian& vecVelocitySource,
                   VectorCartesian& vecPositionBase,
                   VectorCartesian& vecVelocityBase ) const
{
    if (!areBitsOn(VEL_EI, m_fgEIS)) {
        ThrowNotEnabled(__FUNCSIG__);
    }

    if (areBitsOn(STALE, m_fgEIS)) {
        ThrowToBasePosVel();
    }

    m_dcm.Multiply(vecPositionSource, vecPositionBase);
    m_dcm.Multiply(vecVelocitySource, vecVelocityBase);

    if ( !areBitsOn(ANGVEL0, m_fgEIS) ) {
        vecVelocityBase += m_vecAngularVelocity.CrossProduct(vecPositionBase);
    }
}

// acceleration
void
CoorTrans::ToBase( const VectorCartesian& vecPositionSource,
                   const VectorCartesian& vecVelocitySource,
                   const VectorCartesian& vecAccelerationSource,
                   VectorCartesian& vecPositionBase,
                   VectorCartesian& vecVelocityBase,
                   VectorCartesian& vecAccelerationBase )
{
    static VectorCartesian wxr;
    static VectorCartesian wxwxr;
    static VectorCartesian wxv;
    static VectorCartesian axr;

    if (!areBitsOn(ACC_EI, m_fgEIS)) {
        ThrowNotEnabled(__FUNCSIG__);
    }

    Update();
    m_dcm.Multiply(vecPositionSource, vecPositionBase);
    m_dcm.Multiply(vecVelocitySource, vecVelocityBase);
    m_dcm.Multiply(vecAccelerationSource, vecAccelerationBase);

    if ( !areBitsOn(ANGVEL0, m_fgEIS) ) {
        wxr   = m_vecAngularVelocity.CrossProduct( vecPositionBase );
        wxwxr = m_vecAngularVelocity.CrossProduct( wxr );
        wxv   = m_vecAngularVelocity.CrossProduct( vecVelocityBase );
        vecVelocityBase += wxr;
        vecAccelerationBase += 2 * wxv - wxwxr;
    }

    if ( !areBitsOn(ANGACC0, m_fgEIS) ) {
        axr   = m_vecAngularAcceleration.CrossProduct( vecPositionBase );
        vecAccelerationBase += axr;
    }
}

void
CoorTrans::ToBase( const VectorCartesian& vecPositionSource,
                   const VectorCartesian& vecVelocitySource,
                   const VectorCartesian& vecAccelerationSource,
                   VectorCartesian& vecPositionBase,
                   VectorCartesian& vecVelocityBase,
                   VectorCartesian& vecAccelerationBase ) const
{
    static VectorCartesian wxr;
    static VectorCartesian wxwxr;
    static VectorCartesian wxv;
    static VectorCartesian axr;

    if (!areBitsOn(ACC_EI, m_fgEIS)) {
        ThrowNotEnabled(__FUNCSIG__);
    }

    if (areBitsOn(STALE, m_fgEIS)) {
        ThrowToBasePosVelAcc();
    }

    m_dcm.Multiply(vecPositionSource, vecPositionBase);
    m_dcm.Multiply(vecVelocitySource, vecVelocityBase);
    m_dcm.Multiply(vecAccelerationSource, vecAccelerationBase);

    if ( !areBitsOn(ANGVEL0, m_fgEIS) ) {
        wxr   = m_vecAngularVelocity.CrossProduct( vecPositionBase );
        wxwxr = m_vecAngularVelocity.CrossProduct( wxr );
        wxv   = m_vecAngularVelocity.CrossProduct( vecVelocityBase );
        vecVelocityBase += wxr;
        vecAccelerationBase += 2 * wxv - wxwxr;
    }

    if ( !areBitsOn(ANGACC0, m_fgEIS) ) {
        axr   = m_vecAngularAcceleration.CrossProduct( vecPositionBase );
        vecAccelerationBase += axr;
    }
}

void
CoorTrans::ToSource( const VectorCartesian& vecPositionBase,
                     VectorCartesian& vecPositionSource )
{
    Update();
    m_dcm.TransposeMultiply(vecPositionBase, vecPositionSource);
}

void
CoorTrans::ToSource( const VectorCartesian& vecPositionBase,
                     VectorCartesian& vecPositionSource ) const
{
    if (areBitsOn(STALE, m_fgEIS)) {
        ThrowToSourcePos();
    }

    m_dcm.TransposeMultiply(vecPositionBase, vecPositionSource);
}

void
CoorTrans::ToSource( const VectorCartesian& vecPositionBase,
                     const VectorCartesian& vecVelocityBase,
                     VectorCartesian& vecPositionSource,
                     VectorCartesian& vecVelocitySource )
{
    if (!areBitsOn(VEL_EI, m_fgEIS)) {
        ThrowNotEnabled(__FUNCSIG__);
    }

    Update();
    m_dcm.TransposeMultiply(vecPositionBase, vecPositionSource);

    if ( !areBitsOn(ANGVEL0, m_fgEIS) ) {
        m_dcm.TransposeMultiply(vecVelocityBase, vecVelocitySource);
    } else
        m_dcm.TransposeMultiply(
            vecVelocityBase-m_vecAngularVelocity.CrossProduct(vecPositionBase),
            vecVelocitySource);
}

void
CoorTrans::ToSource( const VectorCartesian& vecPositionBase,
                     const VectorCartesian& vecVelocityBase,
                     VectorCartesian& vecPositionSource,
                     VectorCartesian& vecVelocitySource ) const
{
    if (!areBitsOn(VEL_EI, m_fgEIS)) {
        ThrowNotEnabled(__FUNCSIG__);
    }

    if (areBitsOn(STALE, m_fgEIS)) {
        ThrowToSourcePosVel();
    }

    m_dcm.TransposeMultiply(vecPositionBase, vecPositionSource);

    if ( !areBitsOn(ANGVEL0, m_fgEIS) ) {
        m_dcm.TransposeMultiply(vecVelocityBase, vecVelocitySource);
    } else
        m_dcm.TransposeMultiply(
            vecVelocityBase-m_vecAngularVelocity.CrossProduct(vecPositionBase),
            vecVelocitySource);
}

// acceleration
void
CoorTrans::ToSource( const VectorCartesian& vecPositionBase,
                     const VectorCartesian& vecVelocityBase,
                     const VectorCartesian& vecAccelerationBase,
                     VectorCartesian& vecPositionSource,
                     VectorCartesian& vecVelocitySource,
                     VectorCartesian& vecAccelerationSource  )
{
    static VectorCartesian wxr;
    static VectorCartesian wxwxr;
    static VectorCartesian wxv;
    static VectorCartesian axr;
    static VectorCartesian x;

    if (!areBitsOn(ACC_EI, m_fgEIS)) {
        ThrowNotEnabled(__FUNCSIG__);
    }

    Update();
    m_dcm.TransposeMultiply(vecPositionBase, vecPositionSource);
    m_dcm.TransposeMultiply(vecVelocityBase, vecVelocitySource);
    m_dcm.TransposeMultiply(vecAccelerationBase, vecAccelerationSource);

    if ( !areBitsOn(ANGVEL0, m_fgEIS) ) {
        wxr   = m_vecAngularVelocity.CrossProduct( vecPositionBase );
        wxwxr = m_vecAngularVelocity.CrossProduct( wxr );
        wxv   = m_vecAngularVelocity.CrossProduct( vecVelocityBase );
        m_dcm.TransposeMultiply(wxr, x);
        vecVelocitySource -= x;
        m_dcm.TransposeMultiply(wxwxr-2.0*wxv, x);
        vecAccelerationSource += x;
    }

    if ( !areBitsOn(ANGACC0, m_fgEIS) ) {
        axr = m_vecAngularAcceleration.CrossProduct( vecPositionBase );
        m_dcm.TransposeMultiply(axr, x);
        vecAccelerationSource -= x;
    }
}

void
CoorTrans::ToSource( const VectorCartesian& vecPositionBase,
                     const VectorCartesian& vecVelocityBase,
                     const VectorCartesian& vecAccelerationBase,
                     VectorCartesian& vecPositionSource,
                     VectorCartesian& vecVelocitySource,
                     VectorCartesian& vecAccelerationSource  ) const
{
    static VectorCartesian wxr;
    static VectorCartesian wxwxr;
    static VectorCartesian wxv;
    static VectorCartesian axr;
    static VectorCartesian x;

    if (!areBitsOn(ACC_EI, m_fgEIS)) {
        ThrowNotEnabled(__FUNCSIG__);
    }

    if (areBitsOn(STALE, m_fgEIS)) {
        ThrowToSourcePosVelAcc();
    }

    m_dcm.TransposeMultiply(vecPositionBase, vecPositionSource);
    m_dcm.TransposeMultiply(vecVelocityBase, vecVelocitySource);
    m_dcm.TransposeMultiply(vecAccelerationBase, vecAccelerationSource);

    if ( !areBitsOn(ANGVEL0, m_fgEIS) ) {
        wxr   = m_vecAngularVelocity.CrossProduct( vecPositionBase );
        wxwxr = m_vecAngularVelocity.CrossProduct( wxr );
        wxv   = m_vecAngularVelocity.CrossProduct( vecVelocityBase );
        m_dcm.TransposeMultiply(wxr, x);
        vecVelocitySource -= x;
        m_dcm.TransposeMultiply(wxwxr-2.0*wxv, x);
        vecAccelerationSource += x;
    }

    if ( !areBitsOn(ANGACC0, m_fgEIS) ) {
        axr = m_vecAngularAcceleration.CrossProduct( vecPositionBase );
        m_dcm.TransposeMultiply(axr, x);
        vecAccelerationSource -= x;
    }
}

// -----------------------------------------------------------------------------
void CoorTrans::ThrowStaleCoorTrans( const std::string& strFunctionName ) const
{
    std::ostringstream stmMsg;
    stmMsg << "Exception thrown from\n" << typeid( *this ).name()
           << "by function\n"
           << strFunctionName << std::endl
           << "The internal coordinate transformation parameters are stale.\n"
           << "The const version of this function was called, to non-const\n"
           << "version must be used."
           << std::endl << std::ends;
    std::runtime_error re( stmMsg.str() );
    throw re;
}

void CoorTrans::ThrowToBasePos() const
{
    std::string strFcnNm( "ToBase( const VectorCartesian& vecPositionSource,\n" );
    strFcnNm +=      "        VectorCartesian& vecPositionBase ) const";
    ThrowStaleCoorTrans( strFcnNm );
}

void CoorTrans::ThrowToBasePosVel() const
{
    std::string strFcnNm( "ToBase( const VectorCartesian& vecPositionSource,\n" );
    strFcnNm +=      "        const VectorCartesian& vecVelocitySource,\n";
    strFcnNm +=      "        VectorCartesian& vecPositionBase,\n";
    strFcnNm +=      "        VectorCartesian& vecVelocityBase ) const";
    ThrowStaleCoorTrans( strFcnNm );
}

void CoorTrans::ThrowToBasePosVelAcc() const
{
    std::string strFcnNm( "ToBase( const VectorCartesian& vecPositionSource,\n" );
    strFcnNm +=      "        const VectorCartesian& vecVelocitySource,\n";
    strFcnNm +=      "        const VectorCartesian& vecAccelerationSource,\n";
    strFcnNm +=      "        VectorCartesian& vecPositionBase,\n";
    strFcnNm +=      "        VectorCartesian& vecVelocityBase,\n";
    strFcnNm +=      "        VectorCartesian& vecAccelerationBase ) const";
    ThrowStaleCoorTrans( strFcnNm );
}

void CoorTrans::ThrowToSourcePos() const
{
    std::string strFcnNm( "ToSource( const VectorCartesian& vecPositionSource,\n" );
    strFcnNm +=      "          VectorCartesian& vecPositionBase ) const";
    ThrowStaleCoorTrans( strFcnNm );
}

void CoorTrans::ThrowToSourcePosVel() const
{
    std::string strFcnNm( "ToSource( const VectorCartesian& vecPositionSource,\n" );
    strFcnNm +=      "          const VectorCartesian& vecVelocitySource,\n";
    strFcnNm +=      "          VectorCartesian& vecPositionBase,\n";
    strFcnNm +=      "          VectorCartesian& vecVelocityBase ) const";
    ThrowStaleCoorTrans( strFcnNm );
}

void CoorTrans::ThrowToSourcePosVelAcc() const
{
    std::string strFcnNm( "ToSource( const VectorCartesian& vecPositionSource,\n" );
    strFcnNm +=      "          const VectorCartesian& vecVelocitySource,\n";
    strFcnNm +=      "          const VectorCartesian& vecAccelerationSource,\n";
    strFcnNm +=      "          VectorCartesian& vecPositionBase,\n";
    strFcnNm +=      "          VectorCartesian& vecVelocityBase,\n";
    strFcnNm +=      "          VectorCartesian& vecAccelerationBase ) const";
    ThrowStaleCoorTrans( strFcnNm );
}

void CoorTrans::ThrowNotEnabled( const std::string& strFunctionName ) const
{
    std::ostringstream stmMsg;
    stmMsg << "Exception thrown from\n" << typeid( *this ).name() << std::endl
           << "by function\n"
           << strFunctionName << std::endl
           << "Unexpected call to a disabled function.\n"
           << "This function was disabled to reduce runtime.\n"
           << "To use this function, call the SetEnabled( ... ) "
           << "member function.\n"
           << "To disable again, call SetDisabled( ... ) member function.\n"
           << std::ends;
    std::runtime_error re( stmMsg.str() );
    throw re;
}

void CoorTrans::ThrowNotImplemented( const std::string& strFunctionName ) const
{
    std::ostringstream stmMsg;
    stmMsg << "Exception thrown from\n" << typeid( *this ).name() << std::endl
           << "by function\n"
           << strFunctionName << std::endl
           << "This function is not implemented.\n"
           << std::ends;
    std::runtime_error re( stmMsg.str() );
    throw re;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
