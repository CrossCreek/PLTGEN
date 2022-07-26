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
 * IoBitBucket.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
// UNITS 030109
// Function Arg & Return: N/A
// Stream I/O:            N/A
// Initialization Files:  N/A

// ============================================================================
//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : GenEngr

//  MODULE(s) : GenUtility

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V4.4-001  2008-10-03  JAM  1st SATSIM build

//  NOTES : simulation class.



//==============================================================================
// The Bit Bucket
#ifndef IOBITBUCKET_H
#define IOBITBUCKET_H "IoBitBucket V4.4-001  2008-10-03"

#include <ostream>

// ============================================================================
// A3 namespace start
namespace A3
{

// ========================================================================
// TEMPLATE CLASS basic_bitbucket
template<class ChTyp, class ChTr>
class basic_bitbucketbuf : public std::basic_streambuf<ChTyp, ChTr>
{
public:
    typedef ChTyp char_type;
    typedef ChTr  traits_type;
    typedef typename traits_type::int_type int_type;
    typedef typename traits_type::pos_type pos_type;
    typedef typename traits_type::off_type off_type;

    basic_bitbucketbuf() { ; }

    virtual ~basic_bitbucketbuf() { ; }

protected:
    virtual int_type overflow( int_type ch = ChTr::eof() )
    {
        return ChTr::not_eof(ch);
    }

    virtual int_type pbackfail( int_type )
    {
        return ChTr::eof();
    }

    virtual int_type underflow()
    {
        return ChTr::eof();
    }

    virtual pos_type seekoff( off_type,
                              std::ios_base::seekdir,
                              std::ios_base::openmode )
    {
        return static_cast< pos_type >( ChTr::eof() );
    }

    virtual pos_type seekpos( pos_type,
                              std::ios_base::openmode)
    {
        return static_cast< pos_type >( ChTr::eof() );
    }

private:

};

// ========================================================================
// TEMPLATE CLASS basic_obitbucketstream
template<class ChTyp, class ChTr>
class basic_obitbucketstream : public std::basic_ostream<ChTyp, ChTr>
{
public:
    typedef basic_bitbucketbuf< ChTyp, ChTr > BUFFER;

    basic_obitbucketstream()
        : std::basic_ostream< ChTyp, ChTr >( &m_buffer ) { ; }

    virtual ~basic_obitbucketstream() { ; }

    basic_bitbucketbuf< ChTyp, ChTr > *rdbuf() const
    {
        return &m_buffer;
    }

private:
    basic_bitbucketbuf< ChTyp, ChTr > m_buffer;
};

// ========================================================================
typedef basic_bitbucketbuf< char, std::char_traits<char> >  IoBitBucketBuf;
typedef basic_obitbucketstream< char, std::char_traits<char> > IoBitBucket;

// ============================================================================
}
// A3 namespace end
// ============================================================================

#endif // IOBITBUCKET_H
