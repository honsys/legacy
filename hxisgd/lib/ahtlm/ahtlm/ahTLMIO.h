#if !defined(AHTLMIO_H)
#define AHTLMIO_H(arg) const char arg##AHTLMIO_rcsId_svnId[] = "$Id: ahTLMIO.h,v 1.4 2012/08/13 21:12:27 dhon Exp $"; 

/** 
 * \file ahTLMIO.h
 * \brief Astro-H HXI-SGD Telemetry (TLM) FITS file I/O typedefs, structs, and func. defs.
 * \author David B. Hon
 * \date $Date: 2012/08/13 21:12:27 $
 *
 * Provides a variety of variable length CFITSIO compatible buffer types ("TLM slice" extensions) in support
 * of FITS file I/O (read/write) of TLM slices types, along with C++ stdlib vector and other types. 
 * 
 */

#include "ahtlm/ahBits.h"
#include "ahtlm/ahSlice.h"
#include "ahtlm/ahTLM.h"
#include "ahtlm/ahTLMinfoCache.h"

#include "ahfits/ahfits.h"
#include "ahgen/ahgen.h"
#include "ahlog/ahlog.h"

#include <map>
#include <string>
#include <vector>

using namespace ahfits;
using namespace ahgen;
using namespace ahlog;
using namespace std;

namespace ahtlm {

/** \addtogroup mod_ahtlm
 *  @{
 */
 
/// \brief Initializes a hash map of CFITSIO error status (int keys) and text (string values).
/// \param[out] std::map< int, string >& errhash
/// \return number of elements in the hash-map. 
/// \remarks While this is based on the web-site page, all error text strings should be directly
/// available from the CFITSIO API via:
/// void fits_get_errstatus(int status, > char *err_text)
/// see http://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node34.html#ffgerr).
/// \attention Should be moved to ahfits, but are currently implemented in ahTLMIO.cxx
/// to facilitate error message formulation during FITS I/O opps.
int setErrHashFITS(std::map< int, string >& errhash);

/// \brief Check CFITSIO API for current release version Id.
/// \return formatted std::string version text.
/// \attention Should be moved to ahfits, but are currently implemented in ahTLMIO.cxx
/// to facilitate error message formulation during FITS I/O opps.
string getCFITSIOversion();

/// \brief Initializes a hash map of CFITSIO error status (int keys) and text (string values).
/// \param[in] CFITSIO integer status code.
/// \return text string that describes the associated CFITSIO error, suitable for printout. 
string textOfErr(int statcode);

/// candidates for ahfit lib, implemented in ahTLMIO.cxx
int alloc1ColDataOfRow(AhFitsFilePtr ahf, string& col_name, TLMbyteVec& celldata, int colIdx= 0, int rowIdx= 0, int hduIdx= 1);

int allocAllColDataOfRow(AhFitsFilePtr ahf, vector<string>& col_names, vector<int>& cell_sizes, HashNamedTLMbyteVec& datacell_hash, int rowIdx= 0, int hduIdx= 1);

int readSlice(AhFitsFilePtr ahf, FFSliceIO& slice, int colIdx= 0, int rowIdx= 0, int hduIdx= 1);

int readColumn(AhFitsFilePtr ahf, TLMbyteVec& vals, string& col_name, int colIdx= 0, int rowIdx= 0, int hduIdx= 1);

int readBlob(AhFitsFilePtr ahf, FFBlob& blob, int colIdx= 0, int rowIdx= 0, int hduIdx= 1);

int readBlobGroup(AhFitsFilePtr ahf, FFBlobGroup& blobs, vector<int>& colIdxVec, int rowIdx= 0, int hduIdx= 1);

int writeSlice(AhFitsFilePtr ahf, FFSliceIO& slice, int colIdx= 0, int rowIdx= 0, int hduIdx= 1);

int writeColumn(AhFitsFilePtr ahf, TLMbyteVec& vals, string& col_name, int colIdx= 0, int rowId= 0, int hduIdx= 1);

int writeBlob(AhFitsFilePtr ahf, FFBlob& blob, int colIdx= 0, int rowIdx= 0, int hduIdx= 1);

int writeBlobGroup(AhFitsFilePtr ahf, FFBlobGroup& blobs, vector<int>& colIdxVec, int rowIdx= 0, int hduIdx= 1);

int writeBlobGroup(AhFitsFilePtr ahf, FFBlobGroup& blobs, int rowIdx= 0, int hduIdx= 1);

int readFITS(AhFitsFilePtr ahf, int native_typecode, void* pdata, int bufsize, string& colname, int rowIdx= 0, int hduIdx= 1);

int writeFITS(AhFitsFilePtr ahf, int native_typecode, void* pdata, long long length, string& colname, string& format, int colIdx, int rowIdx= 0, int hduIdx= 1);

  /** @} */

} // namespace ahtlm

#endif
