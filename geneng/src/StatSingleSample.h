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
 * StatSingleSample.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : StatSingleSample.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  2000-03-19  JAM  Initial Release
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :

//          note: internal angular units are radians

//****************************************************************************//

//=============================================================================
#ifndef STATSINGLESAMPLE_H
#define STATSINGLESAMPLE_H     "StatSingleSample V2.0"

// ----------------------------------------------------------------------------
// Headers

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class StatSingleSample
{
public:
    StatSingleSample();

    void AddSample( double x );
    void RemoveSample( double x );

    inline unsigned GetNumOfSamples() const;

    inline double GetMinimum() const;
    inline double GetMaximum() const;

    inline double GetMean() const;
    inline double GetMeanXSq() const;

    inline double GetStdDeviation() const;

private:
    unsigned m_numX;

    double m_sumX;
    double m_sumX2;
    double m_minX;
    double m_maxX;
};

// ----------------------------------------------------------------------------
inline unsigned StatSingleSample::GetNumOfSamples() const
{
    return m_numX;
}

inline double StatSingleSample::GetMinimum() const
{
    return m_minX;
}

inline double StatSingleSample::GetMaximum() const
{
    return m_maxX;
}

inline double StatSingleSample::GetMean() const
{
    if ( 0 == m_numX ) {
        return 0;
    }

    return m_sumX / m_numX;
}

inline double StatSingleSample::GetMeanXSq() const
{
    if ( 0 == m_numX ) {
        return 0;
    }

    return m_sumX2 / m_numX;
}

inline double StatSingleSample::GetStdDeviation() const
{
    if ( 0 == m_numX ) {
        return 0;
    }

    return ( m_sumX2 - m_sumX * m_sumX / m_numX ) / m_numX;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif // STATSINGLESAMPLE_H
