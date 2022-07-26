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
 * GenUtility.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Utility functions and classes
// Refer to GenUtility.h for more info.
//==============================================================================
#include "GenUtility.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <limits.h>

//RAGUSA 4/30/2018 - Improving MURAL to 64-bit processing
#ifdef WIN64
#   include <direct.h>
#else
#   include <unistd.h>
#endif

namespace A3
{
namespace
{
// default data locations
// needed for SATSIM file search
// GenEngr implementation needs improvement
const char* pStrDataDir[] = {
    ".\\",
    ".\\resources\\data\\",
    "..\\resources\\data\\",
    "..\\..\\resources\\data\\",
    "..\\..\\..\\resources\\data\\",
    "..\\..\\..\\..\\resources\\data\\"
};
unsigned nStrDataDir(sizeof(pStrDataDir)/ sizeof(const char*));

static std::vector<std::string> customPaths;

// utility class to support writing to a global log
// without creating a global object
class ErrLog
{
public:
    ErrLog();
    Handle<std::ostream> GetHdlStream()
    {
        return m_hLog;
    }
    void SetHdlStream(Handle<std::ostream> hLog)
    {
        m_hLog=hLog;
    }
    std::ostream& GetStream()
    {
        return *m_hLog;
    }

protected:

private:
    Handle<std::ostream> m_hLog;
};

ErrLog::ErrLog()
{
    m_hLog = Handle<std::ostream>(&std::cerr);
}

ErrLog& GetErrLog()
{
    static ErrLog el;
    return el;
}
}

std::ostream& errLog()
{
    return GetErrLog().GetStream();
}

Handle<std::ostream> getHdlErrLog()
{
    return GetErrLog().GetHdlStream();
}

void setHdlErrLog(Handle<std::ostream> hLog)
{
    GetErrLog().SetHdlStream(hLog);
}

// Replace back slashes with slashes
void replaceBackSlash(std::string& s)
{
    std::replace(s.begin(), s.end(), '\\', '/');
}

void addCustomPath(std::string sPath)
{
    customPaths.push_back(sPath);
}

void pathToDirFile(std::string sPath, std::string& sDir, std::string& sFile)
{
    replaceBackSlash(sPath);
    std::string::size_type iPos = sPath.rfind('/');

    if (std::string::npos != iPos) {
        sDir = sPath.substr(0, iPos+1);
        sFile = sPath.substr(iPos+1);
    } else {
        sDir.clear();
        sFile = sPath;
    }
}

bool getWorkDir(std::string& wd)
{
    static const unsigned nCharBuf(8192);
    char *pCharBuf(new char[nCharBuf]);
    bool bSuccess(false);
#   ifdef WIN32
    bSuccess = NULL != _getcwd(pCharBuf, nCharBuf);
#   else
    bSuccess = NULL != _getcwd(pCharBuf, nCharBuf);
#   endif

    if (bSuccess) {
        wd = pCharBuf;
        replaceBackSlash(wd);

        if (*wd.rbegin()!='/') {
            wd.append(1,'/');
        }
    } else {
        wd.clear();
    }

    delete [] pCharBuf;
    return bSuccess;
}

bool getEnvVal(const std::string& sEnvVar, std::string& sEnvVal)
{
    char *pEnv(getenv(sEnvVar.c_str()));

    if (NULL != pEnv) {
        sEnvVal = pEnv;
    } else {
        sEnvVal.clear();
    }

    return NULL != pEnv;
}

bool doesFileExist(const std::string& sDir, std::string sFile,
                   std::string& sPath)
{
    sPath = sDir;
    replaceBackSlash(sPath);
    replaceBackSlash(sFile);
    unsigned n(('/'==sFile[0]?1:0)+('/'==*sPath.rbegin()?1:0));

    if (0 == n) {
        sPath.append(1,'/').append(sFile);
    } else {
        sPath.append(sFile, n-1, sFile.size()+1-n);
    }

    std::fstream fsTest(sPath.c_str(), std::ios_base::in);
    bool bFileFound(fsTest.is_open());
    fsTest.close();

    if (!bFileFound) {
        sPath.clear();
    }

    return bFileFound;
}

bool searchForFile(const std::string& sEnvVar, const std::string& sFile,
                   std::string& sPath)
{
    std::string sDir, sEnvVal;
    std::string::size_type npos(std::string::npos);
    std::string::size_type iPosA(0), iPosB(npos);
    bool bSuccess(false);
    getEnvVal(sEnvVar, sEnvVal);
    bool bDoSearch(!sEnvVal.empty());

    while (bDoSearch) {
        iPosB = sEnvVal.find(';', iPosA);

        if (npos == iPosB) {
            iPosB = sEnvVal.size();
        }

        sDir = sEnvVal.substr(iPosA, iPosB-iPosA);
        bSuccess = doesFileExist(sDir, sFile, sPath);
        iPosA = iPosB+1;

        if (bSuccess || sEnvVal.size() <= iPosA) {
            bDoSearch = false;
        }
    }

    return bSuccess;
}

template <typename ITR>
bool searchForFile(ITR btr, ITR etr, const std::string& sFile,
                   std::string& sPath)
{
    bool bSuccess(false);

    for (ITR itr=btr; !bSuccess && etr != itr; ++itr) {
        bSuccess = doesFileExist(*itr, sFile, sPath);
    }

    return bSuccess;
}

template
bool searchForFile(char**, char**, const std::string&, std::string&);

bool searchForFileInStdPath(const std::string& sFile, std::string& sPath)
{
    const char **itr = pStrDataDir;
    const char **ied = pStrDataDir+nStrDataDir;
    bool bSuccess(searchForFile(itr, ied, sFile, sPath));
    std::string sEnvVal;

    // Search Custom Paths
    if ( (customPaths.size() > 0) && (!bSuccess) ) {
        std::vector<std::string>::iterator cPath;

        for( cPath = customPaths.begin(); (cPath != customPaths.end() && !bSuccess); ++cPath)	{
            bSuccess = doesFileExist(*cPath, sFile, sPath);
        }
    }

    // Search Environment Variables
    if (!bSuccess) {
        getEnvVal("SATSIMPATH", sEnvVal);
        replaceBackSlash(sEnvVal);

        if ('/' != *sEnvVal.rbegin()) {
            sEnvVal += '/';
        }

        sEnvVal += "resources/data/";
        bSuccess = doesFileExist(sEnvVal, sFile, sPath);
    }

    if (!bSuccess) {
        bSuccess = searchForFile("SATSIM2000", sFile, sPath);
    }

    if (!bSuccess) {
        bSuccess = searchForFile("SATSIMPATH", sFile, sPath);
    }

    if (!bSuccess) {
        bSuccess = searchForFile("PATH", sFile, sPath);
    }

    return bSuccess;
}

Handle<std::istream> findOpenStdFile(const std::string& sFile)
{
    std::string sPath;
    std::vector<std::string> ctPrefix(1, "");
    Handle<std::fstream> his;
    bool bSuccess(searchForFileInStdPath(sFile, sPath));

    if (bSuccess)
        his = new std::fstream(sPath.c_str(),
                               std::ios_base::in | std::ios_base::binary);

    if (!his.IsNullObject() && !his->good()) {
        his = Handle<std::fstream>();
    }

    return his;
}

std::string quotesRemove( const std::string& str )
{
    std::string::size_type pos0 = str.find( '\"' );

    if ( str.npos == pos0 ) {
        return str;
    }

    std::string::size_type pos1 = str.find( '\"', pos0+1 );
    std::string sBuf( str, pos0+1, pos1-pos0-1 );
    return sBuf;
}

bool peekCompareString( std::istream& is, const std::string& str )
{
    std::streampos pos = is.tellg();
    std::string::const_iterator itr = str.begin();
    std::string::const_iterator etr = str.end();
    char c;

    while( itr != etr && is.get( c ) && *itr == c ) {
        ++itr;
    }

    is.clear();
    is.seekg( pos );
    return itr==etr;
}

bool findString(std::istream& is, const std::string& str, bool throwOnFail)
{
    std::streampos pos = is.tellg();
    std::string::const_iterator btr = str.begin();
    std::string::const_iterator etr = str.end();
    std::string::const_iterator itr = btr;
    std::ios_base::iostate iost(is.exceptions());

    try {
        if (throwOnFail)
            is.exceptions(std::ios_base::eofbit|std::ios_base::failbit
                          |std::ios_base::badbit);

        //  std::cout << "\n------------------------\nstr = " << str
        //            << "\npos = " << pos << std::endl;

        while ( itr != etr && is ) {
            // std::cout << "peek= " << char(is.peek()) << std::endl;
            if ( *itr == is.get() ) {
                ++itr;
            } else {
                itr = btr;
            }

            // std::cout << "cur = " << is.tellg() << std::endl;
        }

        is.clear();

        if ( itr == etr )
            is.seekg( -static_cast<std::streamoff>(str.length()),
                      std::ios_base::cur );
        else {
            is.seekg( pos );
        }

        // std::cout << "lst = " << is.tellg() << std::endl;
    } catch (...) {
        is.clear();
        is.exceptions(iost);
        std::stringstream stmMsg;
        stmMsg << "\nbool findString(std::istream&, const std::string&,"
               << " bool)\nfailed to find string in stream\nstring = "
               << str << "\ninitial stream position = " << pos
               << "\ncurrent stream position = " << is.tellg() << std::endl;
        stmMsg.setf(std::ios_base::hex|std::ios_base::showbase,
                    std::ios_base::basefield);
        stmMsg << "character at current position = " << is.peek()
               << " (" << unsigned(is.peek()) << ')' << std::endl;
        THROW_RUNTIME_ERROR(stmMsg.str());
    }

    if (throwOnFail) {
        is.exceptions(iost);
    }

    return itr == etr;
}

bool findString(std::istream& is, const std::string& str, std::streampos endPos,
                bool throwOnFail)
{
    std::streampos pos = is.tellg();
    std::string::const_iterator btr = str.begin();
    std::string::const_iterator etr = str.end();
    std::string::const_iterator itr = btr;
    std::ios_base::iostate iost(is.exceptions());

    try {
        if (throwOnFail)
            is.exceptions(std::ios_base::eofbit|std::ios_base::failbit
                          |std::ios_base::badbit);

        while ( itr != etr && is && is.tellg() < endPos ) {
            if ( *itr == is.get() ) {
                ++itr;
            } else {
                itr = btr;
            }
        }

        is.clear();

        if ( itr == etr )
            is.seekg( -static_cast<std::streamoff>(str.length()),
                      std::ios_base::cur );
        else {
            is.seekg( pos );
        }
    } catch (...) {
        is.clear();
        is.exceptions(iost);
        std::stringstream stmMsg;
        stmMsg << "\nbool findString(std::istream&, const std::string&,"
               << " bool)\nfailed to find string in stream\nstring = "
               << str << "\ninitial stream position = " << pos
               << "\ncurrent stream position = " << is.tellg() << std::endl;
        stmMsg.setf(std::ios_base::hex|std::ios_base::showbase,
                    std::ios_base::basefield);
        stmMsg << "character at current position = " << is.peek()
               << " (" << unsigned(is.peek()) << ')' << std::endl;
        THROW_RUNTIME_ERROR(stmMsg.str());
    }

    if (throwOnFail) {
        is.exceptions(iost);
    }

    return itr == etr;
}

std::streampos
findBlock( std::istream& is, std::string startTag, std::string endTag )
{
    // store the initial stream position
    std::streampos pos = is.tellg();
    std::pair< std::streampos, std::streampos > se( -1, -1 );
    std::pair< std::streampos, std::streampos > tp( -1, -1 );

    // search for the start tag
    // if found, search for the end tag
    // else return failed
    if ( findString( is, startTag ) ) {
        int count = 0;
        // store the location of the 1st char in the start tag
        se.first = is.tellg();
        ++count;

        // loop until the block is found
        // ignore nested blocks with the same start-end tags
        while ( 0 < count && is ) {
            // store the current location as the temporary start
            tp.first = is.tellg();

            // seach for an end tag
            // if found, determine whether the found tag is part
            // of a nested block or the match to the requested block
            // when count = 0, the end tag belongs to the requested block
            // else terminate search and return failed
            if ( findString( is, endTag ) ) {
                // store the location of the 1st char in the end tag
                // as the temporary end
                tp.second = is.tellg();
                --count;
                // set the stream postion to the char after the last char
                // of the start tag
                is.seekg( tp.first + std::streamoff( startTag.size() ) );

                // count the number of nested blocks
                while ( findString( is, startTag, tp.second ) ) {
                    is.seekg( static_cast<std::streamoff>(startTag.size()),
                              std::ios_base::cur );
                    ++count;
                }

                is.seekg( tp.second + std::streamoff( endTag.size() ) );
            } else {
                count = -1;
            }
        }

        is.clear();

        // if count is not 0 then the block was not found
        // else the block was found and se contains the
        // stream positions of the block start/end
        if ( count ) {
            se.first = -1;
            is.seekg( pos );
        } else {
            se.second = is.tellg();
            is.seekg( se.first );
        }
    }

    return se.second;
}

std::string getQuotedString( std::istream& is, bool bKeepQuotes )
{
    std::string s;
    std::ios_base::iostate ioOriginalState = is.exceptions();
    // any failure means can't find string
    is.exceptions( std::ios_base::badbit );

    try {
        char c = ' ';

        while ( is >> c && c != '\"' ) ;

        if ( is.good() ) {
            std::getline( is, s, '\"' );

            if ( is.good() && bKeepQuotes ) {
                s.insert( s.begin(), '\"' );
                s.insert( s.end(), '\"' );
            } else if ( !is.good() ) {
                s.erase();
            }
        }
    } catch ( ... ) {
        std::cerr << "getQuotedString(std::istream&, bool)" << std::endl;
        throw;
    }

    is.exceptions( ioOriginalState );
    return s;
}

namespace ExceptExt
{
std::runtime_error runtime_error(const std::string& file, int line,
                                 const std::string& fn, const std::string& msg)
{
    std::ostringstream oss;
    oss << "File: " << file
        << "\nLine: " << line
        << "\nFunction: " << fn
        << "\nMessage:\n" << msg << '\n';
    return std::runtime_error(oss.str());
}

std::runtime_error caught_throw(const std::string& file, int line,
                                const std::string& fn, const std::exception& e)
{
    std::ostringstream oss;
    oss << "File: " << file
        << "\nLine: " << line
        << "\nFunction: " << fn
        << "\nCaught Exception: " << typeid( e ).name()
        << "\nException Message:\n"<< e.what() << '\n';
    return std::runtime_error(oss.str());
}
}
}  // A3
