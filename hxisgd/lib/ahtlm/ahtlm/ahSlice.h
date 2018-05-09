#if !defined(AHSLICE_H)
#define AHSLICE_H(arg) const char arg##AHSLICE_rcsId_svnId[] = "$Id: ahSlice.h,v 1.9 2012/08/16 19:42:07 dhon Exp $"; 

/** 
 * \file ahSlice.h
 * \brief Astro-H HXI-SGD Telemetry (TLM) slice typedefs, structs, and func. defs.
 * \author David B. Hon
 * \date $Date: 2012/08/16 19:42:07 $
 *
 * Provides a variety of variable length buffer types (called "slices") in support of TLM processing
 * via extensions of ahBits types and the C++ stdlib vector and other types. 
 * 
 */

// according to the HXI and SGD SFF templates, we need these

/// using CFITSIO conventions:
/// X (bits), B (bytes), I (shorts), J (ints), K (longlongs), D (double)

/// note that according to page 181-2 of the CFITSIO doc:
/// http://heasarc.gsfc.nasa.gov/FTP/software/fitsio/c/cfitsio.pdf
/// on-line:
/// http://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node123.html
/// 'X' indicate a datatype of bits.

/// but according to the on-line quickfits guide:
/// http://heasarc.gsfc.nasa.gov/FTP/software/fitsio/c/quick.pdf
/// on-line:
/// http://heasarc.gsfc.nasa.gov/docs/software/fitsio/quick/node12.html
/// 'X' indicates a datatype of complex.

/// make use of c/c++ stdlib items:
#include <cstdlib>
#include <cmath>
#include <bitset>
#include <vector>
#include <sstream>
#include <string>

#include "ahlog/ahlog.h"
#include "ahtlm/ahBits.h"

using namespace ahlog;
using namespace std;

namespace ahtlm {

  /** \addtogroup mod_ahtlm
   *  @{
   */

//typedef struct { Bits16 msb; Bits16 lsb; } StartTimeFlag;
//alt. live-time nhitasic buff -- a 24 bit time int and an 8 bit int count val: 
//typedef struct { char livetime[3] ; char nHitASICs; } LiveTimeAndNHitASICs; 
typedef std::map< int, Bits16 > Bit16Hash;
//typedef std::vector<bool> BitVec;
typedef std::map< int, BitsAny > BitHash;

//
// use CFITSIO "tform" conventions X, B, I, J, K, D, A:
// CFITSIO negative typecodes indicate variable-length arays ...
// bufptrs should always be checked for null -- and may point to heap or stack space...
// if calloc or new used, please set heap bool true ... and/or to avoid the need for the heap bool,
// consider negative length args as indicators of heap vs. stack alloc?
// that is, always use heap for variable length arrays (indicated via alloc arg or neg. len)

/// \brief Container to manage a byte buffer data "slice".
///
/// This struct stores a "slice" of bytes in a contiguous memory buffer and 
/// provides information about the nature of the buffer. It also provides built-in 
/// functions that help initialize the buffer. Access to buffer elements is via
/// standard C 1-dimensional array index or pointer de-reference..
struct BSlice {
  Ubyte* bufptr; int maxlength; bool heap;
  inline size_t size() { return abs(maxlength); }
  void clear();
  void initSlice(Ubyte* ptr= 0, int len= 1, bool alloc= false);
  void initSlice(const basic_string<Ubyte>& data);
  BSlice(int len, bool alloc= false);
  BSlice(Ubyte* ptr= 0, int len= 1, bool alloc= false);
  ~BSlice();
};

/// \brief Container to manage a 16 bit integer buffer data "slice".
///
/// This struct stores a "slice" of bytes in a contiguous memory buffer and 
/// provides information about the nature of the buffer. It also provides built-in 
/// functions that help initialize the buffer. Access to buffer elements is via
/// standard C 1-dimensional array index or pointer de-reference..
struct ISlice { 
  Uint16* bufptr; int maxlength; bool heap; 
  inline size_t size() { return abs(maxlength); }
  void clear();
  void initSlice(Uint16* ptr= 0, int len= 1, bool alloc= false);
  void initSlice(const vector<Uint16>& data);
  ISlice(int len, bool alloc= false);
  ISlice(Uint16* ptr= 0, int len= 1, bool alloc= false);
  ~ISlice();
};

/// \brief Container to manage a 32 bit integer buffer data "slice".
///
/// This struct stores a "slice" of bytes in a contiguous memory buffer and 
/// provides information about the nature of the buffer. It also provides built-in 
/// functions that help initialize the buffer. Access to buffer elements is via
/// standard C 1-dimensional array index or pointer de-reference..
struct JSlice { 
  Uint32* bufptr; int maxlength; bool heap;
  inline size_t size() { return abs(maxlength); }
  void clear();
  void initSlice(Uint32* ptr= 0, int len= 1, bool alloc= false);
  void initSlice(const vector<Uint32>& data);
  JSlice(int len, bool alloc= false);
  JSlice(Uint32* ptr= 0, int len= 1, bool alloc= false);
  ~JSlice();
};


/// \brief Container to manage a 64 bit integer buffer data "slice".
///
/// This struct stores a "slice" of bytes in a contiguous memory buffer and 
/// provides information about the nature of the buffer. It also provides built-in 
/// functions that help initialize the buffer. Access to buffer elements is via
/// standard C 1-dimensional array index or pointer de-reference..
struct KSlice {
  Uint64* bufptr; int maxlength; bool heap; 
  inline size_t size() { return abs(maxlength); }
  void clear();
  void initSlice(Uint64* ptr= 0, int len= 1, bool alloc= false);
  void initSlice(const vector<Uint64>& data);
  KSlice(int len, bool alloc= false);
  KSlice(Uint64* ptr= 0, int len= 1, bool alloc= false);
  ~KSlice();
};


/// \brief Container to manage a 64 bit floating-point buffer data "slice".
///
/// This struct stores a "slice" of bytes in a contiguous memory buffer and 
/// provides information about the nature of the buffer. It also provides built-in 
/// functions that help initialize the buffer. Access to buffer elements is via
/// standard C 1-dimensional array index or pointer de-reference..
struct DSlice {
  double* bufptr; int maxlength; bool heap;
  inline size_t size() { return abs(maxlength); }
  void clear();
  void initSlice(double* ptr= 0, int len= 1, bool alloc= false);
  void initSlice(const vector<double>& data);
  DSlice(int len, bool alloc= true);
  DSlice(double* ptr= 0, int len= 1, bool alloc= false);
  ~DSlice();
};


/// \brief Container to manage a character buffer "slice".
///
/// This issimply be a std::string, which provides a length() func. and a
/// const char* c_str() func. and standard C 1-dimensional array index access.
struct ASlice : std::string {
  const char* bufptr; int maxlength; bool heap;
  void initSlice(const string& ascitext);
  ASlice(const char* c= 0);
  ASlice(int len);
};

/// \brief Alternate container to manage a character buffer "slice".
///
/// This struct stores a "slice" of byte characters in a contiguous memory buffer and 
/// provides information about the nature of the buffer. It also provides built-in 
/// functions that help initialize the buffer. Access to buffer elements is via
/// standard C 1-dimensional array index or pointer de-reference..
struct CSlice {
  char* bufptr; int maxlength; bool heap;
  inline size_t size() { return abs(maxlength); }
  void clear();
  void initSlice(char* ptr= 0, int len= 1, bool alloc= false);
  void initSlice(const vector<char>& data);
  CSlice(int len, bool alloc= false);
  CSlice(char* ptr= 0, int len= 1, bool alloc= false);
  ~CSlice(); // forever alloc may reset heap to false?
};

/// \brief Container to manage a variable length bit buffer "slice".
///
/// This is simply a std::vector<bool>, which provides a length() func. and
/// direct access of individual bits via standard C 1-dimensional array index.
struct XSlice : BitsAny { 
 inline int length() { return (int) size(); } 
 void initSlice(const BitsAny& bv);
 XSlice(int len= 16);
};


  /** @} */

} // namespace ahtlm

#endif

