#if !defined(AHTLMSIM_H)
#define AHTLMSIM_H(arg) const char arg##AHTLMSIM_rcsId_svnId[] = "$Id$"; 

/** 
 * \file ahTLMsim.h
 * \brief Astro-H HXI-SGD Telemetry (TLM) simulation typedefs, structs, and func. defs.
 * \author David B. Hon
 * \date $Date: 2012/08/16 19:42:07 $
 *
 * Provides a variety of variable length CFITSIO compatible buffer types ("slice" extensions) in support
 * of TLM processing via extensions of ahSlice types and C++ stdlib vector and other types. 
 * 
 */

/// make use of c/c++ stdlib items:
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <bitset>
#include <vector>
#include <sstream>
#include <string>
#include <valarray>

#include "ahtlm/ahBits.h"
#include "ahtlm/ahSlice.h"

#include "ahfits/ahfits.h"
#include "ahgen/ahgen.h"
#include "ahlog/ahlog.h"

using namespace ahlog;
using namespace std;

namespace ahtlm {

/** \addtogroup mod_ahtlm
 *  @{
 */

/// \brief Container for the HXI 112 bit (14 byte) TLM header:
///
/// fancy chart for HXI (and SGD?) TLM indicates this mapping of TLM bits within 16 bit word aligned
/// buffers the bit-field sizes and names derive from the FITS template (of April-May 2012).
/// functions not inlined here are implemented in ahTLM.cxx (gets) or ahTLMsim.cxx (sets).
/// this supports the means for more robust & rigorous self-consistent simulation TLM processing.
struct HXI_TLM_HDR { 
  // 16 bit word aligned TLM
  // should be 7 shorts or 14 bytes (according to hiro's fancy chart):
  Uint16 startflag;
  Uint32 occurId;
  Uint32 localtime; // 0 - 1.273 days -- 25.6? micro-sec / lsb ?
  Uint32 hdrflags; // seu, trg, pat, and other bits
  Ubyte  livetime[3]; 
  Ubyte  nHitAsics; // max of 40 asics
  BSlice rawTLM;

  inline Uint32 getOccurId() { return occurId; }
  inline Uint32 setOccurId(Uint32 id) { Uint32 prev = occurId; occurId = id; return prev; }
  inline Uint32 getLocaltime() { return localtime; }
  inline Uint32 setLocaltime(Uint32 time) { Uint32 prevtime = localtime; localtime = time; return prevtime; }
  Bits5 getSEUflags();
  Bits5 setSEUflags(const Bits5 flags);
  Bits5 getLCHKflags();
  Bits5 setLCHKflags(const Bits5 flags);
  Bits8 getTRGflags();
  Bits8 setTRGflags(const Bits8 flags);
  Bits8 getTRGPATflags();
  Bits8 setTRGPATflags(const Bits8 flags);
  Bits2 getHPATflags();
  Bits2 setHPATflags(const Bits2 flags);
  Bits2 getFBGflags();
  Bits2 setFBGflags(const Bits2 flags);
  Uint32 getLivetime();
  Uint32 setLivetime(Uint32 time);
  inline int getAsicHitCnt() { return (int) nHitAsics; } 
  inline int setAsicHitCnt(int cnt = 0) { int n = (int)nHitAsics; memset(&nHitAsics, cnt, 1); return n; } 
  inline HXI_TLM_HDR() { startflag = 0x5555; localtime = hdrflags = nHitAsics = 0; memset(livetime, 0, sizeof(livetime)); }
  HXI_TLM_HDR(const BSlice& tlm);
};

/// \brief Container for the HXI 16*3 (48) bit single (1) ASIC TLM header + 10*3+ bit adc chans -- 80+ bits
/// 
/// fancy chart for HXI TLM indicates this mapping of TLM bits within 16 bit word aligned
/// buffers the bit-field sizes and names derive from the HXI fancy chart.
/// this supports the means for more robust & rigorous self-consistent simulation TLM processing.
/// functions not inlined here are implemented in ahTLM.cxx (gets) or ahTLMsim.cxx (sets).
struct HXI_1ASIC {
  int offset; // byte offset to start of ASIC raw data block (from rawTLM.bufptr[0])
  Ubyte flags;
  Ubyte asic_and_layer;
  Uint32 chanflags;
  // this "slice" Ubyte* bufptr should be heap or stack space alloc. for entire raw data block,
  // including the HXI_TLM_HDR above -- so (Uint16) rawTLM.bufptr[0] should == 0x5555 (startflag).
  BSlice rawTLM;

  /// \brief extracts (simulated or real) ASIC Id info. from raw TLM "slice".
  /// \return ASIC Id 0 - ?. 
  int getAsicAndLayerId(int& aid, int& lid);

  /// \brief encode (simulated) ASIC Id info. into raw TLM "slice".
  /// \param[in] int id 0 - ?.
  /// \return ASIC Id. 
  int setAsicAndLayerId(int aid, int lid);

  /// \brief extracts (simulated or real) Layer Id info. from raw TLM "slice".
  /// \return Layer Id 0 - ?. 
  int getLayerId();

  /// \brief encode (simulated) Layer Id info. into raw TLM "slice".
  /// \param[in] int id 0 - ?.
  /// \return ASIC Id. 
  int setLayerId(int id);

  /// \brief extracts (simulated or real) active channel info. from raw TLM "slice".
  /// \param[out] 32 bit flag of active chans.
  /// \return total number of active chans. 
  int getActiveChans(Bits32& actvbits);

  /// \brief extracts (simulated or real) active channel info. from raw TLM "slice".
  /// \param[out] list of (remapped) active chan. Ids.
  /// \return active chans. 32 bit bitfield active chan. flags.  
  Bits32 getActiveChans(vector< Uint16 >& actvchans);

  /// \brief supports simulation of active channel info. from raw TLM "slice".
  /// \param[in] 32 bit bitfield of active chan. flags.
  /// \return total number of active chans.  
  int setActiveChans(const Bits32& actvbits);

  /// \brief supports simulation of active channel info. from raw TLM "slice".
  /// \param[in] list of (remapped) active chan. Ids.
  /// \return 64 bit bit-field of active chan. flags.  
  Bits32 setActiveChans(const vector< Uint16 >& actvchans);

  Uint16 getRefChan(Bits10& bits);
  Uint16 setRefChan(const Bits10& bits);
  Uint16 getNoiseChan(Bits10& bits);
  Uint16 setNoiseChan(const Bits10& bits);
  int getADCvals(vector< Bits10 >& adcvec);
  int setADCvals(vector< Bits10 >& adcvec);

  /// \brief provide default initialization with optional/default offset val.
  /// \param[in] integer byte offset to start of single/individual ASIC TLM buffer.
  /// \return n/a.
  inline HXI_1ASIC(int byte_offset = 14) { chanflags = flags = asic_and_layer = 0; offset = byte_offset; }
 
  /// \brief provide raw TLM initialization with optional/default offset val.
  /// \param[in] integer byte offset to start of single/individual ASIC TLM buffer.
  /// \return n/a.
  inline HXI_1ASIC(const BSlice& tlm, int byte_offset = 14) { chanflags = flags = asic_and_layer = 0; offset = byte_offset; rawTLM = tlm;  }
};

/// \brief Container for the SGD 144 bit (9 shorts or 18 bytes?) TLM header:
///
/// Assuming SGD TLM is similar to HXI TLM -- mapping of TLM bits within 16 bit word aligned
/// buffers the bit-field sizes and names derive from the FITS template (of April-May 2012).
/// this supports the means for more robust & rigorous self-consistent simulation TLM processing.
/// functions not inlined here are implemented in ahTLM.cxx (gets) or ahTLMsim.cxx (sets).
struct SGD_TLM_HDR {
  // 16 bit word aligned TLM
  // guessing thos should be 9 shorts or 18 bytes (according to non-existant fancy chart):
  Uint16 startflag;
  Uint32 occurId;
  Uint32 localtime; // 0 - 1.273 days -- 25.6? micro-sec / lsb ?
  Ubyte  hdrflags[7]; // cal, cc, seu, trg, pat, and other bits
  Ubyte  livetime[3]; 
  Uint16 nHitAsics; // max of 624 asics
  BSlice rawTLM;
  
  inline Uint32 getOccurId() { return occurId; }
  inline Uint32 setOccurId(Uint32 id) { Uint32 prev = occurId; occurId = id; return prev; }
  inline Uint32 getLocaltime() { return localtime; }
  inline Uint32 setLocaltime(Uint32 time) { Uint32 prev = localtime; localtime = time; return prev; }
  Bits1 getSEUflag();
  Bits1 setSEUflag(const Bits1 flags);
  Bits1 getLCHKflag();
  Bits1 setLCHKflag(const Bits1 flags);
  Bits1 getCalflag();
  Bits1 setCalflag(const Bits1 flags);
  Bits31 getTRGPATflags();
  Bits31 setTRGPATflags(const Bits31 flags);
  Bits6 getTRGflags();
  Bits6 setTRGflags(const Bits6 flags);
  Bits3 getCCbusyflags();
  Bits3 setCCbusyflags(const Bits3 flags);
  Bits3 getHitPatCCflags();
  Bits3 setHitPatCCflags(const Bits3 flags);
  Bits4 getHitPatflags();
  Bits4 setHitPatflags(const Bits4 flags);
  Bits4 getFastBGOflags();
  Bits4 setFastBGOflags(const Bits4 flags);
  Uint32 getLivetime();
  Uint32 setLivetime(Uint32 time);
  inline int getAsicHitCnt() { return (int) nHitAsics; } 
  inline int setAsicHitCnt(Uint16 cnt = 0) { int n = (int)nHitAsics; nHitAsics = cnt; return n; } 
  inline SGD_TLM_HDR() { startflag = 0x5555; localtime = nHitAsics = 0; memset(hdrflags, 0, sizeof(hdrflags)); memset(livetime, 0, sizeof(livetime)); }
  SGD_TLM_HDR(const BSlice& tlm);
}; 

/// \brief Container for the SGD 16*6 (96) bit single (1) ASIC TLM header + 10*3+ bit adc chans -- 128+ bits
/// 
/// assume SGD is ismilar to HXI TLM --  mapping of TLM bits within 16 bit word aligned
/// buffers the bit-field sizes and names derive from the HXI fancy chart, but more bits
/// are likely required for SGD (more flags, more asics, more chans).
/// this supports the means for more robust & rigorous self-consistent simulation TLM processing.
/// functions not inlined here are implemented in ahTLM.cxx (gets) or ahTLMsim.cxx (sets).
struct SGD_1ASIC {
  int offset; // byte offset to start of ASIC raw data block (from rawTLM.bufptr[0])
  Uint16 flags;
  Uint16 asic_and_layer;
  Uint64 chanflags;
  // this "slice" Ubyte* bufptr should be heap or stack space alloc. for entire raw data block,
  // including the SGD_TLM_HDR above -- so (Uint16) rawTLM.bufptr[0] should == 0x5555 (startflag).
  BSlice rawTLM;

  /// \brief extracts (simulated or real) ASIC Id info. from raw TLM "slice".
  /// \return ASIC Id 0 - ?. 
  int getAsicAndLayerId(int& aid, int& lid);

  /// \brief encode (simulated) ASIC Id info. into raw TLM "slice".
  /// \param[in] int id 0 - ?.
  /// \return ASIC Id. 
  int setAsicAndLayerId(int aid, int lid);

  /// \brief extracts (simulated or real) active channel info. from raw TLM "slice".
  /// \param[out] 64 bit flag of active chans.
  /// \return total number of active chans. 
  int getActiveChans(Bits64& actvbits);

  /// \brief extracts (simulated or real) active channel info. from raw TLM "slice".
  /// \param[out] list of (remapped) active chan. Ids.
  /// \return active chans. 64 bit bitfield active chan. flags.  
  Bits64 getActiveChans(vector< Uint16 >& actvchans);

  /// \brief supports simulation of active channel info. from raw TLM "slice".
  /// \param[in] 64 bit bitfield of active chan. flags.
  /// \return total number of active chans.  
  int setActiveChans(const Bits64& actvbits);

  /// \brief supports simulation of active channel info. from raw TLM "slice".
  /// \param[in] list of (remapped) active chan. Ids.
  /// \return 64 bit bit-field of active chan. flags.  
  Bits64 setActiveChans(const vector< Uint16 >& actvchans);

  Uint16 getRefChan(Bits10& bits);
  Uint16 setRefChan(const Bits10& bits);
  Uint16 getNoiseChan(Bits10& bits);
  Uint16 setNoiseChan(const Bits10& bits);
  int getADCvals(vector< Bits10 >& adcvec);
  int setADCvals(vector< Bits10 >& adcvec);

  /// \brief provide default initialization with optional/default offset val.
  /// \param[in] integer byte offset to start of single/individual ASIC TLM buffer.
  /// \return n/a.
  inline SGD_1ASIC(int byte_offset = 18) { chanflags = flags = asic_and_layer = 0; offset = byte_offset; }
 
  /// \brief provide raw TLM initialization with optional/default offset val.
  /// \param[in] integer byte offset to start of single/individual ASIC TLM buffer.
  /// \return n/a.
  inline SGD_1ASIC(const BSlice& tlm, int byte_offset = 18) { chanflags = flags = asic_and_layer = 0; offset = byte_offset; rawTLM = tlm;  }
};

//
///////////////////////// simple test telemetry pattern
//
// these assume each asic has exactly the same number of active adc-chans per asic
//
int genTLM4Test(string& tlm_astext, int hitcnt, int actvchan, int tlmIdx);
// max for HXI: 40 asics, 32 adc-chans
int genTLM4Test(string& tlm_astext, int hitcnt, int actvchan, Bits32& chanflags);
// max for SGD: 624, 64
int genTLM4Test(string& tlm_astext, int hitcnt, int actvchan, Bits64& chanflags);
//
//
//////////////////////// more realistic TLM patterns
// each asic has a different number of active adc-chans. and noise and ref. chans
// hitcnt == asicIdVec.size() and hash-map key == asicId with hash-map value == chan-bit-flag
//
int genTLM4Test(string& tlm_astext, vector<int>& asicIdVec, map<int, Bits32>& chanFlagVec);
//
int genTLM4Test(string& tlm_astext, vector<int>& asicIdVec, map<int, vector<int> >& chanIdHash, map<int, Bits32>& chanFlagHash);
//
int genTLM4Test(string& tlm_astext, vector<int>& asicIdVec, map<int, vector<int> >& chanIdHash, map<int, Bits64>& chanFlagHash);

int genOcccurrence(SGD_TLM_HDR& sgd, vector<SGD_1ASIC>& asicvec, vectorint nhits= 1, int occurId = 1);

/** @} */

} // namespace ahtlm

#endif

