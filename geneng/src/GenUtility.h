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
 * GenUtility.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : GenEngr

//  MODULE(s) : GenUtility

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.3-018  05/17/2000  JAM  stream utility functions
//              V4.4-001  2008-10-03  JAM  1st SATSIM build
//              V4.5      2009-07-01  JAM  dropped support for MS CPP Ver 4/5
//                                         compilers (removed typename switch)
//                                         added support for GCC compiler 3.x
//                                         (added this-> prefix to resolve
//                                         GCC limitation)
//                                         removed unused functions from
//                                         NMD IRAD
//              V5.0      2010-02-12  JAM  switched to STL version of toupper
//                                         and tolower, added __FUNCSIG__ macro
// Doxygen documentation
/// @file
/// @brief Utility functions and classes

/// @author J. McCormick
/// @version V5.0 switched to STL version of toupper and tolower,
/// added __FUNCSIG__ macro
/// @date 2010-06-01

/// @author J. McCormick
/// @version V4.5 dropped support for MS CPP Ver 4/5 compilers (removed
/// typename switch) added support for GCC compiler 3.x (added this->
/// prefix to resolve GCC limitation) removed unused functions from NMD IRAD
/// @date 2009-07-01

/// @par Units
/// - Function Arg & Return: n/a
/// - Stream I/O:            n/a
/// - Initialization Files:  n/a

// =============================================================================
#ifndef GENUTILITY_H
#define GENUTILITY_H   __FILE__ " " __DATE__

// required headers
#include "Handle.h"

#include <algorithm>
#include <iosfwd>
#include <string>
#include <locale>
#include <ctype.h>
#include <stdexcept>

#ifndef __FUNCSIG__
#   define  __FUNCSIG__ __FUNCTION__
#endif

#define THROW_RUNTIME_ERROR(msg) \
    throw A3::ExceptExt::runtime_error(__FILE__, __LINE__, __FUNCSIG__, msg)

#define TRACE_EXCEPT(e) \
    throw A3::ExceptExt::caught_throw(__FILE__, __LINE__, __FUNCSIG__, e)

namespace A3
{
/// Error log
/// @details Defaults to cerr
/// @return reference to the error log stream
std::ostream& errLog();

/// Error log handle
/// @details Defaults to cerr handle
/// @return handle to error log stream
Handle<std::ostream> getHdlErrLog();

/// Set error log handle
/// @param[in] handle to error log stream
void setHdlErrLog(Handle<std::ostream> hLog);

/// Replace back slashes with slashes
/// @param[in,out] s target string
void replaceBackSlash(std::string& s);

/// Sets a custom path to avoid using Environment Variables
/// @param[in] sPath path to set
void addCustomPath(std::string sPath);

/// Separate path in directory and file name parts
/// @param[in] sPath path to evaluate
/// @param[out] sDir directory portion of path
/// @param[out] sFile file name portion of path
void pathToDirFile(std::string sPath, std::string& sDir,
                   std::string& sFile);

/// Current Working Directory
/// @param[out] wd if successful, working directory name
/// @return true if working directory found, false otherwise
bool getWorkDir(std::string& wd);

/// Environment Variable Value
/// @param[in] sEnvVar environment variable
/// @param[out] sEnvVal environment variable value
/// @return value of specified environment variable
bool getEnvVal(const std::string& sEnvVar, std::string& sEnvVal);

/// Check existance of file in directory
/// @param[in] sDir directory to check
/// @param[in] sFile file name to check
/// @param[out] sPath path (dir+file) if successful, empty string otherwise
/// @return true if successful, false otherwise
bool doesFileExist(const std::string& sDir, std::string sFile,
                   std::string& sPath);

/// Search directory names stored in an environment variable for file
/// @param[in] sEnvVar environment variable name
/// @param[in] sFile file name to search for
/// @param[out] sPath path (dir+file) if successful, empty string otherwise
/// @return true if successful, false otherwise
bool searchForFile(const std::string& sEnvVar,const std::string& sFile,
                   std::string& sPath);

/// Search standard directories file
/// @param[in] sFile file name to search for
/// @param[out] sPath path (dir+file) if successful, empty string otherwise
/// @return true if successful, false otherwise
bool searchForFileInStdPath(const std::string& sFile, std::string& sPath);

/// Attempt to open a standard GenEngr file, searches default
/// directories
/// @param[in] sFile file to open specified without a
/// path, search standard paths
/// @return handle to open file stream or null object if
/// file open failed
Handle<std::istream> findOpenStdFile(const std::string& sFile);

/// Read a quoted string
/// @param[in] is input stream
/// @param[in] bKeepQuotes true = keep quotes, false = strip quotes
/// @return extracted string
std::string getQuotedString( std::istream& is, bool bKeepQuotes );

/// Compare string to length of string characters in stream
/// @param[in] is input stream
/// @param[in] str comparison string
/// @return true if character match, false otherwise
bool peekCompareString( std::istream& is, const std::string& str );

/// @brief Search for string in stream
/// @details The search begins at the current get-pointer and ends when the
/// string is found or eof is reached, if the string is found, the
/// get-pointer is set to the 1st character of the string in the stream and
/// true is returned, otherwise, the get-pointer is set to its original
/// position and false is returned
/// @param[in] is input stream
/// @param[in] str conparison string
/// @param[in] throwOnFail true = throw an exception if the string is not
/// found, false = don't throw, just return false
/// @return true = string found, else false
bool findString(std::istream& is, const std::string& str,
                bool throwOnFail=false);

/// @brief Search for string in stream
/// @details The search begins at the current get-pointer and ends when the
/// string is found or the end postion is reached, if the string is found,
/// the get-pointer is set to the 1st character of the string in the stream
/// and true is returned, otherwise, the get-pointer is set to its original
/// position and false is returned
/// @param[in] is input stream
/// @param[in] str conparison string
/// @param[in] throwOnFail true = throw an exception if the string is not
/// found, false = don't throw, just return false
/// @return true = string found, else false
bool findString(std::istream& is, const std::string& str,
                std::streampos endPos, bool throwOnFail=false);

/// @brief Find block bounded by start and end tags
/// @details if the search is successful the get-pointer is set to the 1st
/// character of the start tag in the stream and the stream position of the
/// last char of the end tag is returned
/// @param[in] is input stream
/// @param[in] startTag string that represents the start of a block
/// @param[in] endTag string that represents the end of a block
/// @return stream position of the last character of the end tag
std::streampos findBlock(std::istream& is, std::string startTag,
                         std::string endTag);

/// Remove the outer quotes from a string
/// @param[in] str string with quotes
/// @return string with quotes removed
std::string quotesRemove( const std::string& str );

/// Encase a string in quotes
/// @param[in] str string without surrounding quotes
/// @return string with quotes added
inline std::string quotesInsert( const std::string& str )
{
    return "\"" + str + "\"";
}

/// Change lower case characters to upper case
/// @param[in] b begin point to 1st character
/// @param[in] e end pointer 1 past last character
inline void makeUpper(char* b, char* e)
{
    std::locale loc;
    std::use_facet<std::ctype<char> >(loc).toupper(b,e);
}

/// Change lower case characters to upper case
/// @param[in] b begin iterator to character container
/// @param[in] e end iterator to character container
template <typename ITR>
inline void makeUpper(ITR b, ITR e)
{
    if (e != b) {
        std::locale loc;

        do {
            *b = std::toupper(*b, loc);
        } while (e != ++b);
    }
}

/// Change upper case characters to lower case
/// @param[in] b begin point to 1st character
/// @param[in] e end pointer 1 past last character
inline void makeLower(char* b, char* e)
{
    std::locale loc;
    std::use_facet<std::ctype<char> >(loc).tolower(b,e);
}

/// Change upper case characters to lower case
/// @param[in] b begin iterator to character container
/// @param[in] e end iterator to character container
template <typename ITR>
inline void makeLower(ITR b, ITR e)
{
    if (e != b) {
        std::locale loc;

        do {
            *b = std::tolower(*b, loc);
        } while (e != ++b);
    }
}

//==========================================================================
/// @brief Abstract base class for creating compare function objects
/// @details Classes derived from CompareValue are used with STL find
/// algorithm to search a container for a element that satisfies the
/// comparison test
template < class C, class VALTYPE >
class CompareValue
{
public:
    CompareValue() {;}

    /// Conversion constructor setting the comparison object
    explicit CompareValue( const VALTYPE& valToCompare ) : m_val( valToCompare ) {;}

    virtual ~CompareValue() {;}

    /// Set the comparison object
    /// @param[in] valToCompare comparsion object
    virtual void Set( const VALTYPE& valToCompare )
    {
        m_val = valToCompare;
    }

    /// Pure virtual function defining the functor interface
    /// @param[in] classWithGetFfctn reference to container element
    /// object with the get function invoked for the comparison test
    /// @return true for match, false otherwise
    virtual bool operator()( const C& classWithGetFctn ) const = 0;

    // Compare object managed by a handle
    // @param[in] hdlWithGetFctn handle to object to be compared
    // @return true for match, false otherwise
    // virtual bool operator()( Handle<C>& hdlWithGetFctn ) const
    // { return operator()( *hdlWithGetFctn ); }

    /// Casting operator returning a reference to the comparison object
    /// @return reference to the comparison object
    const VALTYPE& operator()()
    {
        return m_val;
    }

protected:
    VALTYPE m_val;
};

//==========================================================================
/// Function object used to compare objects with a GetName function
template < class T >
class CompareName : public CompareValue< T, std::string >
{
public:
    CompareName() {;}

    explicit CompareName( const std::string& sName ) : CompareValue< T, std::string >( sName ) {;}

    virtual ~CompareName() {;}

    bool operator()( const T& classWithGetName ) const
    {
        return classWithGetName.GetName() == this->m_val;
    }

    bool operator()( Handle<T>& hdlWithGetFctn ) const
    {
        return operator()( *hdlWithGetFctn );
    }
};

//==========================================================================
/// Function object used to compare objects with a GetTag function
template < class T >
class CompareTag : public CompareValue< T, std::string >
{
public:
    CompareTag() {;}

    explicit CompareTag( const std::string& sTag ) : CompareValue< T, std::string >( sTag ) {;}

    virtual ~CompareTag() {;}

    bool operator()( const T& classWithGetTag ) const
    {
        return classWithGetTag.GetTag() == this->m_val;
    }
};

//==========================================================================
/// Function object used to compare objects with a GetAttribute function
template < class T >
class CompareAttribute : public CompareValue< T, std::string >
{
public:
    CompareAttribute( const std::string& sAttTyp,
                      const std::string& sAttVal )
        : CompareValue< T, std::string >( sAttVal ),
          m_sAttrib( sAttTyp ) {;}

    virtual ~CompareAttribute() {;}

    bool operator()( const T& classWithGetAttribute ) const
    {
        return classWithGetAttribute.GetAttribute( m_sAttrib )
               == this->m_val;
    }

private:

    explicit CompareAttribute( const std::string& sAttTyp ) : m_sAttrib( sAttTyp ) {;}

    std::string m_sAttrib;
};

//==========================================================================
/// Function object used to compare the second value of a STL pair
template < class PAIR_TYPE >
class CompareSecond
    : public CompareValue< PAIR_TYPE, typename PAIR_TYPE::second_type >
{
public:
    CompareSecond() {;}

    explicit CompareSecond( const typename PAIR_TYPE::second_type& value ) : CompareValue<PAIR_TYPE, typename PAIR_TYPE::second_type>(value) {;}

    virtual ~CompareSecond() {;}

    bool operator()( const PAIR_TYPE& cmpPair ) const
    {
        return cmpPair.second == this->m_val;
    }

private:
};

//==========================================================================
/// Function object used to compare objects with a GetId/ function
template < class T, class ID_TYPE >
class CompareId : public CompareValue< T, ID_TYPE >
{
public:
    CompareId() {;}
    explicit CompareId( const ID_TYPE& id ) : CompareValue< T, ID_TYPE >( id ) {;}

    virtual ~CompareId() {;}

    bool operator()( const T& classWithGetId ) const
    {
        return classWithGetId.GetId() == this->m_val;
    }
};

//==========================================================================
namespace ExceptExt
{
std::runtime_error runtime_error(const std::string& file, int line,
                                 const std::string& fn, const std::string& msg);

std::runtime_error caught_throw(const std::string& file, int line,
                                const std::string& fn, const std::exception& e);
}

}
#endif  // GENUTILITY_H

//==============================================================================
