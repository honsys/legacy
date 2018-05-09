#if !defined(AHTLMINFOCACHE_H)
#define AHTLMINFOCACHE_H(arg) const char arg##AHTLMINFOCACHE_rcsId_svnId[] = "$Id: ahTLMinfoCache.h,v 1.5 2012/08/13 21:12:27 dhon Exp $"; 

/** 
 * \file ahTLMinfoCache.h
 * \brief Astro-H HXI-SGD Telemetry (TLM) FITS binary column related typedefs, structs, and func. defs.
 * \author David B. Hon
 * \date $Date: 2012/08/13 21:12:27 $
 *
 * Provides a variety of CFITSIO compatible binary table HDU and column information cacheing in support 
 * of TLM processing via extensions of ahTLM and ahTLMIO types, along with C++ stdlib vector and other types. 
 * 
 */

#include "ahtlm/ahBits.h"
#include "ahtlm/ahSlice.h"
#include "ahtlm/ahTLM.h"
#include "ahtlm/ahTLMIO.h"

#include "ahfits/ahfits.h"
#include "ahgen/ahgen.h"
#include "ahlog/ahlog.h"

#include <stdint.h> // int limits

using namespace ahfits;
using namespace ahgen;
using namespace ahlog;
using namespace std;

namespace ahtlm {

/** \addtogroup mod_ahtlm
  *  @{
  */

typedef struct { int index; int count; int type; int version; int rows; string typname; string extname; string instrum; } ahHDUinfo;

/// \brief Inline that returns the values of the CFIOSTIO FITS file structure pointer, if non-NULL.
/// \param[in] AhFitsFilePtr -- open FITS file info. Astro-H struct pointer.
/// \return fitsfile* CFIOSTIO FITS file structure pointer or NULL.
inline fitsfile* fitsFileStructPtr(AhFitsFilePtr ahf) { if( 0 != ahf ) { return ahf->m_cfitsfp; } return 0; }

/// \brief Returns a single text string description of a simulated "occurrence".
/// \param[in] const string& instrum -- name of the instrument being "simulated".
/// \param[in] int asic_hits -- the count of active ASICs in the occurrence being "simulated".
/// \param[in] int actv_chans -- the total number of active channels (over all active ASICs).
/// \return single string that holds the description text. 
/// \remarks This is used to make note of specific test data (simulated/fake TLM) attributes via 
/// a FITS binary-table ascii column. The column should only be present when simulated/fake data
/// is being used to test the software.
/// \attention This is only of use for testing with simulated/test/fake data and its FITS column 
/// will eventually be removed from the official FITS template.
string describeOccurrence(const string& instrum, int asic_hits, int actv_chans, int occurId);

/// \brief Ensures all expected runtime paths are present on the host file-system.
/// \return integer status of path check and/or initialization.
/// \attention This is meant to be congruent with the "aht" unit test setup and should be 
/// refactored to libahgen.
int creatIOpaths();

/// \brief Ensures indicated list of runtime paths are present on the host file-system.
/// \param[in] vector< string >& paths -- list of path directory names.
/// \return integer status of path check and/or initialization 
/// \attention This is meant to be congruent with the "aht" unit test setup and should be 
/// refactored to libahgen.
int creatIOpaths(vector< string >& paths);

/// \brief Opens or creates indicated file and appends text.
/// \param[in] string& text -- text string that may contain multiple lines.
/// \param[in] string& filname -- full path of filename.
/// \return integer status of path check and/or initialization 
/// \attention This should be refactored to libahgen.
int writeTextFile(const string& content, const string& filename);

/// \brief Fetches one or some or all HDU extension names from the currently opened FITS file.
/// \param[in] AhFitsFilePtr -- Struct (typedef declared in ahfits.h) pointer to de-reference for CFITSIO FITS filefile*.
/// \param[out] vector<string>& extnames -- list of one or more HDU extensnion names found, starting from #1.
/// \return integer status of CFITSIO API or ?
/// \attention This should be refactored to libahfits.
int getHDUnames(AhFitsFilePtr ahf, vector<string>& extnames, int count= 1);

/// \brief Fetches some or all HDU key/value content names from all the HDUs of the currently opened FITS file.
/// \param[in] AhFitsFilePtr -- Struct (typedef declared in ahfits.h) pointer to de-reference for CFITSIO FITS filefile*.
/// \param[out] vector<ahHDUinfo>& hdu_info -- list of one or more HDU information structs.
/// \return integer status of CFITSIO API or count of returned list elements?
/// \attention This should be refactored to libahfits.
int getHDUinfo(AhFitsFilePtr ahf, vector<ahHDUinfo>& hdu_info);

/// \brief Fetches some or all HDU key/value content from a single the HDUs of the currently opened FITS file.
/// \param[in] AhFitsFilePtr -- Struct (typedef declared in ahfits.h) pointer to de-reference for CFITSIO FITS filefile*.
/// \param[out] ahHDUinfo& hdu_info -- an HDU information struct.
/// \param[in] int hduIdx -- an HDU index == CFITSIO hdu number - 1, so the default is the first extension after the primary HDU.
/// \param[in] bool move -- move to the indicated HDU (if needed) and set / establish that as the current HDU.
/// \return integer status of CFITSIO API or ?
/// \attention This should be refactored to libahfits
int getHDUinfo(AhFitsFilePtr ahf, ahHDUinfo& hdu_info, int hduIdx= 1, bool move= false);

/// \brief Fetches some or all HDU key/value content from a single the HDUs of the currently opened FITS file.
/// \param[in] AhFitsFilePtr -- Struct (typedef declared in ahfits.h) pointer to de-reference for CFITSIO FITS filefile*.
/// \param[out] ahHDUinfo& hdu_info -- an HDU information struct.
/// \param[in] string hdu_name -- an HDU extension name.
/// \param[in] int type -- an HDU extension type.
/// \param[in] int version -- an HDU extension version number.
/// \param[in] bool move -- move to the indicated HDU (if needed) and set / establish that as the current HDU.
/// \return integer status of CFITSIO API or ?
/// \attention This should be refactored to libahfits
int getHDUinfo(AhFitsFilePtr ahf, ahHDUinfo& hdu_info, string hdu_name= "events", int hdutype= BINARY_TBL, int versnum= 0, bool move= true);

/// \brief Fetches all HDU key/value instrument names (key == "INSTRUME") from the indicated HDU of all the currently opened FITS files.
/// \param[in] const vector<AhFitsFilePtr>& -- list of Struct (typedef declared in ahfits.h) pointers to de-reference for CFITSIO FITS filefile*.
/// \param[out] vector<string>& instrum_names -- list of intrument names found.i
/// \return integer status of CFITSIO API or count of names in list ?
/// \attention This should be refactored to libahfits.
int getInstrumNames(vector<string>& instrum_names, const vector<AhFitsFilePtr>& filelist, int hduIdx= 1);

/// \brief Fetches the table row count (from the NAXIS2 keyword?) of the indicated HDU of the currently opened FITS file.
/// \param[in] AhFitsFilePtr -- Struct (typedef declared in ahfits.h) pointer to de-reference for CFITSIO FITS filefile*.
/// \param[in] int hduIdx -- HDU index == CFITSIO HDU number - 1, so the default is the first extension after the primary HDU.
/// \param[in] bool move -- move to the indicated HDU (if needed) and set / establish that as the current HDU.
/// \return integer row count
/// \attention This should be refactored to libahfits.
int getRowCountFromHeader(AhFitsFilePtr ahf, int hduIdx= 1, bool move= true);

/// \brief Fetches the table row count of the indicated HDU of the currently opened FITS file.
/// \param[in] AhFitsFilePtr -- Struct (typedef declared in ahfits.h) pointer to de-reference for CFITSIO FITS filefile*.
/// \param[in] int hduIdx -- HDU index == CFITSIO HDU number - 1, so the default is the first extension after the primary HDU.
/// \return integer row count
/// \attention This should be refactored to libahfits.
long getRowCount(AhFitsFilePtr ahf, int hduIdx= 1);

/// \brief Fetches the table column count of the indicated HDU of the currently opened FITS file. rowIdx is required by CFITSIO?
/// \param[in] AhFitsFilePtr -- Struct (typedef declared in ahfits.h) pointer to de-reference for CFITSIO FITS filefile*.
/// \param[in] int rowIdx -- table row index == CFITSIO row number - 1, so the default is the first row.
/// \param[in] int hduIdx -- HDU index == CFITSIO HDU number - 1, so the default is the first extension after the primary HDU.
/// \return integer column count
/// \attention This should be refactored to libahfits.
int getColCount(AhFitsFilePtr ahf, int rowIdx= 0, int hduIdx= 1);

/// \brief Fetches the table column Id (not index) of the indicated column name from the indicated HDU of the currently 
/// opened FITS file. rowIdx is required by CFITSIO?
/// \param[in] AhFitsFilePtr -- Struct (typedef declared in ahfits.h) pointer to de-reference for CFITSIO FITS filefile*.
/// \param[in] string& col_name -- table column name.
/// \param[in] int rowIdx -- table row index == CFITSIO row number - 1, so the default is the first row.
/// \param[in] int hduIdx -- HDU index == CFITSIO HDU number - 1, so the default is the first extension after the primary HDU.
/// \return integer column number (not index?)
/// \attention This should be refactored to libahfits.
int getColNumberOfName(AhFitsFilePtr ahf, const string& col_name, int rowIdx= 0, int hduIdx= 1);

/// \brief Fetches the table column length and (native) data type of the indicated column index from the indicated HDU of the 
/// currently opened FITS file. Note that rowIdx is required by CFITSIO?
/// \param[in] AhFitsFilePtr -- Struct (typedef declared in ahfits.h) pointer to de-reference for CFITSIO FITS filefile*.
/// \param[out] int& typecode -- returned CFITSIO native type.
/// \param[in] int colIdx -- table column index == CFITSIO column number - 1, so the default is the first column.
/// \param[in] int rowIdx -- table row index == CFITSIO row number - 1, so the default is the first row.
/// \param[in] int hduIdx -- HDU index == CFITSIO HDU number - 1, so the default is the first extension after the primary HDU.
/// \return integer column length.
/// \attention This should be refactored to libahfits.
int getColLengthAndType(AhFitsFilePtr ahf, int& typecode, int colIdx= 0, int rowIdx= 0, int hduIdx= 1);

/// \brief Fetches all column Ids (not indices) and all column names from the indicated HDU of the currently opened FITS 
/// file. rowIdx is required by CFITSIO?
/// \param[in] AhFitsFilePtr -- Struct (typedef declared in ahfits.h) pointer to de-reference for CFITSIO FITS filefile*.
/// \param[out] vector<string>& col_names -- all table column names.
/// \param[out] map<string, int>& colnamenum_hash -- hash map of all column names (as keys) and column numbers (as values).
/// \param[in] int hduIdx -- HDU index == CFITSIO HDU number - 1, so the default is the first extension after the primary HDU.
/// \return integer total number of columns.
/// \attention This should be refactored to libahfits.
int getAllColNamesAndNums(AhFitsFilePtr ahf, vector<string>& col_names, map<string, int>& colnamenum_hash, int hduIdx= 1);

/// \brief Fetches all column Ids (not indices) and all column names from the indicated HDU of the currently opened FITS 
/// file. rowIdx is required by CFITSIO?
/// \param[in] AhFitsFilePtr -- Struct (typedef declared in ahfits.h) pointer to de-reference for CFITSIO FITS filefile*.
/// \param[out] vector<string>& col_names -- all table column names.
/// \param[out] map<string, int>& colnamesize_hash -- hash map of all column names (as keys) and column lengths (as values).
/// \param[in] int rowIdx -- table row index == CFITSIO row number - 1, so the default is the first extension after the primary HDU.
/// \param[in] int hduIdx -- HDU index == CFITSIO HDU number - 1, so the default is the first extension after the primary HDU.
/// \return integer total number of columns.
/// \attention This should be refactored to libahfits.
int getDataColNamesAndSizes(AhFitsFilePtr ahf, vector<string>& col_names, map<string, int>& colnamesize_hash, int rowIdx= 0, int hduIdx= 1);

/// \brief Sets all column names and numbers and types of the indicated HDU binary table of the currently opened FITS 
/// file. rowIdx is required by CFITSIO?
/// \param[in] AhFitsFilePtr -- Struct (typedef declared in ahfits.h) pointer to de-reference for CFITSIO FITS filefile*.
/// \param[out] vector<string>& col_names -- all table column names.
/// \param[out] map<string, int>& colnum_hash -- hash map of all column names (as keys) and column numbers (as values).
/// \param[out] map<string, int>& coltype_hash -- hash map of all column names (as keys) and column types (as values).
/// \param[in] int rowIdx -- table row index == CFITSIO row number - 1, so the default is the first extension after the primary HDU.
/// \param[in] int hduIdx -- HDU index == CFITSIO HDU number - 1, so the default is the first extension after the primary HDU.
/// \return integer total number of columns.
/// \attention This should be refactored to libahfits.
int setAllColInfo(AhFitsFilePtr ahf, vector<string>& col_names, map<string, int>& colnum_hash, map<string, string>& coltype_hash, int hduIdx= 1);

/** @} */

} // namespace ahtlm

#endif
