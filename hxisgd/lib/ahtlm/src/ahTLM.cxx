#if !defined(AHTLM_C)
#define AHTLM_C(arg) const char arg##AHTLM_rcsId_svnId[] = "$Id: ahTLM.cxx,v 1.19 2012/08/16 19:42:07 dhon Exp $"; 

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
#include <cstring>
#include <cmath>
#include <bitset>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>

#include "ahlog/ahlog.h"

#include "ahtlm/ahSlice.h"
AHSLICE_H(ahTLM)

#include "ahtlm/ahBits.h"
AHBITS_H(ahTLM)

#include "ahtlm/ahTLM.h"
AHTLM_H(ahTLM)

/*
#include "ahhxi/ahHXItyps.h"
#include "ahsgd/ahSGDtyps.h"
*/

using namespace ahlog;
using namespace std;

namespace { // private local helpers

  const string cvsId = "$Name:  $ $Id: ahTLM.cxx,v 1.19 2012/08/16 19:42:07 dhon Exp $";

  const int _HXInumAsicPerLayer = 8; // duplicated from ahHXItyps.h
  const int _SGDnumAsicPerLayer = 208; // duplicated from ahSGDtyps.h

  // according to a comment in getcols.c (~line 700):
  // "if string length is greater than a FITS block (2880 char) then must only read 1 string at a time..."
  // so let's force the issue 
  char _cfitstext_buff[2882];

  int clearFITSstr(string& cfstr) {
    //memset(_cfitstext_buff, 'F', sizeof(_cfitstext_buff)); _cfitstext_buff[2881] = '\0';
    memset(_cfitstext_buff, 0, sizeof(_cfitstext_buff));
    cfstr = _cfitstext_buff;
    return (int) cfstr.length();
  }
 
  // assume offsets were sanity checked with buffer length by TLMbyteVec or ByteTLM
  // invokers of these helpers:

  int extractTLMbits(ahtlm::Bits10& bitflags, ahtlm::Ubyte* rawdata, int byt_offset, int bit_offset) {
    union { ahtlm::Uint16 u16tlm; unsigned char bytlm[2]; };
    bitflags.reset(); memset(&u16tlm, 0, sizeof(u16tlm));
    if( bitflags.count() <= 0 ) return 0; // something is amiss -- bad sim/test telemetry -- do nothing now
    for( int i = 0; i < 2; ++i ) { bytlm[i] = rawdata[byt_offset + i]; }
    // must re-align the returned bitflag item based on any bit offset:
    ahtlm::Bits16 alignme = ahtlm::Bits16(u16tlm);
    // shift the 10 bits from the bit-offset down to the lsb at 0
    for( int i = 0; i < 10; ++i ) {
      bitflags[i] = alignme[bit_offset + i];
    }
    return bitflags.count();
  }

  int extractTLMbits(ahtlm::Bits16& bitflags, ahtlm::Ubyte* rawdata, int byt_offset) {
    union { ahtlm::Uint16 u16tlm; unsigned char bytlm[2]; };
    bitflags.reset(); memset(&u16tlm, 0, sizeof(u16tlm));
    if( bitflags.count() <= 0 ) return 0; // something is amiss -- bad sim/test telemetry -- do nothing now
    for( int i = 0; i < 2; ++i ) { bytlm[i] = rawdata[byt_offset + i]; }
    bitflags = ahtlm::Bits16(u16tlm);
    return (int)bitflags.count();
  }
  
  int extractTLMbits(ahtlm::Bits32& bitflags, ahtlm::Ubyte* rawdata, int byt_offset) {
    union { ahtlm::Uint32 u32tlm; unsigned char bytlm[4]; };
    bitflags.reset(); memset(&u32tlm, 0, sizeof(u32tlm));
    if( bitflags.count() <= 0 ) return 0; // something is amiss -- bad sim/test telemetry -- do nothing now
    for( int i = 0; i < 4; ++i ) { bytlm[i] = rawdata[byt_offset + i]; }
    bitflags = ahtlm::Bits32(u32tlm);
    return (int)bitflags.count();
  }

  int extractTLMbits(ahtlm::Bits64& bitflags, ahtlm::Ubyte* rawdata, int byt_offset) {
    union { ahtlm::Uint64 u64tlm; ahtlm::Uint32 u32tlm[2]; unsigned char bytlm[8]; };
    bitflags.reset(); memset(&u64tlm, 0, sizeof(u32tlm));
    if( bitflags.count() <= 0 ) return 0; // something is amiss -- bad sim/test telemetry -- do nothing now
    for( int i = 0; i < 8; ++i ) { bytlm[i] = rawdata[byt_offset + i]; }
    // need a Bits64(Unit64 and/or Bits64(Uint32[2]) initializor
    // bitflags = ahtlm::Bits64(u64tlm); // later
    ahtlm::Bits32 lsb = ahtlm::Bits32(u32tlm[0]);
    ahtlm::Bits32 msb = ahtlm::Bits32(u32tlm[1]);
    for( int i = 0; i < 8; ++i ) {
      bitflags[i] = lsb[i]; bitflags[8+i] = msb[i]; 
    }
    return (int)bitflags.count();
  }

  int printChanFlags(map<int, ahtlm::Bits32>& chanFlagHash) {
    map< int, ahtlm::Bits32 >::iterator it;
    AH_INFO(ahlog::LOW) << "ASIC Id -- Bit 00 . 01 .02 .03 . 04 . 05 . 06 . 07 . 08 . 09 . 10 . 11 . 12 . 13 . 14 . 15 .";   
    AH_INFO(ahlog::LOW) << " 16 . 17 . 18 . 19 . 20 . 21 . 22 . 23 . 24 . 25 . 26 . 27 . 28 . 29 . 30 . 31." << endl;   
    for( it = chanFlagHash.begin(); it != chanFlagHash.end(); ++it ) {
      int actvchans = 0, asic = it->first;
      ahtlm::Bits32& flags = it->second;
      AH_INFO(ahlog::LOW) << "ASIC Id: " << asic << " --------------------------------------------------------------------------------- " << endl;
      for( int i = 0; i < 32; ++i ) {
        flags[i] ? AH_INFO(ahlog::LOW) << " 1 ." : AH_INFO(ahlog::LOW) << " 0 .";
        if( flags[i] ) ++actvchans;
      }
      AH_INFO(ahlog::LOW) << endl;
      AH_INFO(ahlog::LOW) << "ASIC Id: " << asic << " & active channels of ASIC: " << actvchans << endl;
    }
    return 0;
  }
 
  int printChanFlags(map<int, ahtlm::Bits64>& chanFlagHash) {
    map< int, ahtlm::Bits64 >::iterator it;
    AH_INFO(ahlog::LOW) << "ASIC Id -- Bit 00 . 01 . 02 . 03 . 04 . 05 . 06 . 07 . 08 . 09 . 10 . 11 . 12 . 13 . 14 . 15 .";   
    AH_INFO(ahlog::LOW) << " 16 . 17 . 18 . 19 . 20 . 21 . 22 . 23 . 24 . 25 . 26 . 27 . 28 . 29 . 30 . 31." << endl;   
    AH_INFO(ahlog::LOW) << "ASIC Id -- Bit 032 . 33 . 34 . 35 . 36 . 37 . 38 . 39 . 40 . 41 . 42 . 43 . 44 . 45 . 46 . 47 .";   
    AH_INFO(ahlog::LOW) << " 48 . 49 . 50 . 51 . 52 . 53 . 54 . 55 . 56 . 57 . 58 . 59 . 60 . 61 . 62 . 63." << endl;   
    for( it = chanFlagHash.begin(); it != chanFlagHash.end(); ++it ) {
      int actvchans = 0, asic = it->first;
      ahtlm::Bits64& flags = it->second;
      AH_INFO(ahlog::LOW) << "ASIC Id: " << asic << " --------------------------------------------------------------------------------- " << endl;
      for( int i = 0; i < 32; ++i ) {
        flags[i] ? AH_INFO(ahlog::LOW) << " 1 ." : AH_INFO(ahlog::LOW) << " 0 .";
        if( flags[i] ) ++actvchans;
      }
      AH_INFO(ahlog::LOW) << endl;
      for( int i = 32; i < 64; ++i ) {
        flags[i] ? AH_INFO(ahlog::LOW) << " 1 ." : AH_INFO(ahlog::LOW) << " 0 .";
        if( flags[i] ) ++actvchans;
      }
      AH_INFO(ahlog::LOW) << endl;
      AH_INFO(ahlog::LOW) << "ASIC Id: " << asic << " & active channels of ASIC: " << actvchans << endl;
    }
    return 0;
  }

} // private local helpers

namespace ahtlm {

  /** \addtogroup mod_ahtlm
   *  @{
   */

  int checkAsicCount(const string& testInfo) {
    // testInfo should look something like:
    // "Default Occurrence test values SGD -- Id: 1, asics: 10, chans: 120"
    string searchme = "asics:";
    size_t slen = searchme.length();
    size_t colon = testInfo.find(searchme);
    if( colon == string::npos ) {
      AH_INFO(ahlog::LOW) << " ? No Asic count in Occurrence (test gen.) Info: " << testInfo << endl;
      return -1;
    }
    colon += slen;
    size_t comma = testInfo.find_last_of(',');
    string s = testInfo.substr(colon, comma - colon);
    int cnt = atoi(s.c_str());
    return cnt;
  }

  int checkChanCount(const string& testInfo) {
    // testInfo should look something like:
    // "Default Occurrence test values SGD -- Id: 1, asics: 10, chans: 120"
    string searchme = "chans:";
    size_t colon = testInfo.find_last_of(searchme);
    if( colon == string::npos ) {
      AH_INFO(ahlog::LOW) << " ? No Channel count in Occurrence (test gen.) Info: " << testInfo << endl;
      return -1;
    }
    string s = testInfo.substr(1+colon);
    int cnt = atoi(s.c_str());
    return cnt;
  }

void printRawTLM(const ByteTLM& tlm) {
  AH_INFO(ahlog::LOW) << " length of raw telemetry buffer: " << tlm.length << endl; 
  char chbuff[BUFSIZ]; memset(chbuff, 0, sizeof(chbuff));
  memcpy(chbuff, tlm.bufptr, BUFSIZ); // tlm.length);
  string tlmtxt = chbuff;
  if( tlmtxt.length() <=  0 ) return;
  AH_INFO(ahlog::LOW) << tlmtxt << endl;
  /*
  int offset = 0, cnt = tlm.length / 16;
  Bits16 bitflags;
  vector<string> textbuff;
  BitsAny bv;
  for( int excnt = 0; excnt < cnt; ++excnt ) {
    extractTLMbits(bitflags, rawtlm, offset);
    setBitsAny(bv, bitflags);
    bitText(textbuff, bv, (int)bitflags.size(), " raw TLM as 16 bit bitsets: ");
    AH_INFO(ahlog::LOW) << textbuff[0];
  }
  */
  return;
}

int findStopBit(const BitsHXIMaxRaw& bits) {
  int stopIdx = hxi_maxbits - 1;
  do {
    if( bits[stopIdx] ) break;
  } while ( --stopIdx > 0 );
  return stopIdx;
}

int findStopBit(const BitsSGDMaxRaw& bits) {
  int stopIdx = sgd_maxbits - 1;
  do {
    if( bits[stopIdx] ) break;
  } while ( --stopIdx > 0 );
  return stopIdx;
}

// HXI TLM header init from raw tlm data:
HXI_TLM_HDR::HXI_TLM_HDR(const BSlice& tlm) { 
  localtime = hdrflags = startflag = nHitAsics = 0; memset(livetime, 0, sizeof(livetime));
  if( 0 == tlm.bufptr ) {
    AH_INFO(ahlog::LOW) << " ? tlm bufptr uninitialized? aborting..." << endl;
    return;
  }
  rawTLM = tlm;
  int offset = 0;
  startflag = *((Uint16*) &rawTLM.bufptr[offset]);
  if( 0x5555 != startflag ) AH_INFO(ahlog::LOW) << " ? rawTLM startflag suspect?: " << hex << startflag << endl;
  offset += 2;
  localtime = *((Uint32*) &rawTLM.bufptr[offset]);
  offset += sizeof(localtime);
  hdrflags = *((Uint32*) &rawTLM.bufptr[offset]); // memcpy(&hdrflags, &rawTLM.bufptr[offset], sizeof(hdrflags));
  offset += sizeof(hdrflags);
  memcpy(livetime, &rawTLM.bufptr[offset], sizeof(livetime));
  offset += sizeof(livetime);
  nHitAsics = *((Ubyte*) &rawTLM.bufptr[offset]);
}

// HXI TLM header access funcs:
Bits5 HXI_TLM_HDR::getSEUflags() { Bits5 bits; return bits; }
Bits5 HXI_TLM_HDR::getLCHKflags() { Bits5 bits; return bits; }
Bits8 HXI_TLM_HDR::getTRGflags() { Bits8 bits; return bits; }
Bits8 HXI_TLM_HDR::getTRGPATflags() { Bits8 bits; return bits; }
Bits2 HXI_TLM_HDR::getHPATflags() { Bits2 bits; return bits; }
Bits2 HXI_TLM_HDR::getFBGflags() { Bits2 bits; return bits; }
Uint32 HXI_TLM_HDR::getLivetime() { return 0; }

int HXI_1ASIC::getAsicAndLayerId(int& aid, int& lid) {
  int byte_offset = 1 + offset; // assume struct offset attribute properly initialized.
  Ubyte val = rawTLM.bufptr[byte_offset];
  Bits8 b8(val);
  Bits4 lidb4, aidb4;
  for( int i = 0; i < 4; ++i ) {
    lidb4[i] = b8[i]; // layer Id is lsb
    aidb4[i] = b8[4+i];
  }
  aid = aidb4.to_ulong();
  lid = lidb4.to_ulong();
  return val;
}

int HXI_1ASIC::getActiveChans(Bits32& bits) {
  int bsz = (int) bits.size();
  // the byte offset to the active chan bits of this single asic buffer:
  int byte_offest = 2 + offset;
  Uint32* chanflags = (Uint32*) &rawTLM.bufptr[byte_offest];
  bits = Bits32(*chanflags);
  int actv_cnt = 0;
  for( int i = 0; i < bsz; ++i ) if( bits[i] ) ++actv_cnt;
  // AH_INFO(ahlog::LOW) << " actv_cnt: " << actv_cnt << " -- " << bits << " :: " << bits.to_string() << endl;
  clog << __func__ << " actv_cnt: " << actv_cnt << " -- " << bits << " :: " << bits.to_string() << endl;
  return actv_cnt;
}

Bits32 HXI_1ASIC::getActiveChans(vector< Uint16 >& actvchans) {
  actvchans.clear();
  Bits32 bits;
  Uint16 bsz = (Uint16) bits.size();
  int actv_cnt = getActiveChans(bits);
  // AH_INFO(ahlog::LOW) << " actv_cnt: " << actv_cnt << " -- " << bits << " :: " << bits.to_string() << endl;
  clog << __func__ << " actv_cnt: " << actv_cnt << " -- " << bits << " :: " << bits.to_string() << endl;
  for( Uint16 i = 0; i < bsz; ++i ) {
    if( bits[i] ) actvchans.push_back(i);
  }
  return bits; 
}

Uint16 HXI_1ASIC::getRefChan(Bits10& bits) { return (Uint16)bits.to_ulong(); }
Uint16 HXI_1ASIC::getNoiseChan(Bits10& bits) { return (Uint16)bits.to_ulong(); }
int HXI_1ASIC::getADCvals(vector< Bits10 >& adcvec) { return 0; }

// SGD TLM header init from raw tlm data:
SGD_TLM_HDR::SGD_TLM_HDR(const BSlice& tlm) { 
  localtime = startflag = nHitAsics = 0; memset(hdrflags, 0, sizeof(hdrflags)); memset(livetime, 0, sizeof(livetime));
  if( 0 == tlm.bufptr ) {
    AH_INFO(ahlog::LOW) << " ? tlm bufptr uninitialized? aborting..." << endl;
    return;
  }
  rawTLM = tlm;
  int offset = 0;
  startflag = *((Uint16*) &rawTLM.bufptr[offset]);
  if( 0x5555 != startflag ) AH_INFO(ahlog::LOW) << " ? rawTLM startflag suspect?: " << hex << startflag << endl;
  offset += 2;
  localtime = *((Uint32*) &rawTLM.bufptr[offset]);
  offset += sizeof(localtime);
  memcpy(&hdrflags, &rawTLM.bufptr[offset], sizeof(hdrflags));
  offset += sizeof(hdrflags);
  memcpy(livetime, &rawTLM.bufptr[offset], sizeof(livetime));
  offset += sizeof(livetime);
  nHitAsics = *((Uint16*) &rawTLM.bufptr[offset]);
}

// SGD TLM header access funcs:
Bits1 SGD_TLM_HDR::getSEUflag() { Bits1 bits; return bits; }
Bits1 SGD_TLM_HDR::getLCHKflag() { Bits1 bits; return bits; }
Bits1 SGD_TLM_HDR::getCalflag() { Bits1 bits; return bits; }
Bits31 SGD_TLM_HDR::getTRGPATflags() { Bits31 bits; return bits; }
Bits6 SGD_TLM_HDR::getTRGflags() { Bits6 bits; return bits; }
Bits3 SGD_TLM_HDR::getCCbusyflags() { Bits3 bits; return bits; }
Bits3 SGD_TLM_HDR::getHitPatCCflags() { Bits3 bits; return bits; }
Bits4 SGD_TLM_HDR::getHitPatflags() { Bits4 bits; return bits; }
Bits4 SGD_TLM_HDR::getFastBGOflags() { Bits4 bits; return bits; }
Uint32 SGD_TLM_HDR::getLivetime() { return 0; }

int SGD_1ASIC::getAsicAndLayerId(int& aid, int& lid) {
  int byte_offset = 2 + offset; // assume struct offset attribute properly initialized.
  Uint16 val = *((Uint16*) &rawTLM.bufptr[byte_offset]);
  Bits16 b16(val);
  Bits8 lidb8, aidb8;
  for( int i = 0; i < 8; ++i ) {
    lidb8[i] = b16[i]; // layer Id is lsb
    aidb8[i] = b16[8+i];
  }
  aid = aidb8.to_ulong();
  lid = lidb8.to_ulong();
  return val;
}

int SGD_1ASIC::getActiveChans(Bits64& bits) {
  bits.reset();
  // the byte offset to the active chan bits of this single asic buffer:
  int byte_offest = 4 + offset;
  Bits32 lsbchanflags = Bits32(*((Uint32*) &rawTLM.bufptr[byte_offest]));
  Bits32 msbchanflags = Bits32(*((Uint32*) &rawTLM.bufptr[4+byte_offest]));
  int actv_cnt = 0;
  for( int i = 0; i < 32; ++i ) {
    bits[i] = lsbchanflags[i];
    if( bits[i] ) ++actv_cnt;
    bits[32+i] = msbchanflags[i];
    if( bits[32+i] ) ++actv_cnt;
  }
  // AH_INFO(ahlog::LOW) << " actv_cnt: " << actv_cnt << " -- " << bits << " :: " << bits.to_string() << endl;
  clog << " actv_cnt: " << actv_cnt << " -- " << bits << " :: " << bits.to_string() << endl;
  return actv_cnt;
}

Bits64 SGD_1ASIC::getActiveChans(vector< Uint16 >& actvchans) {
  actvchans.clear();
  Bits64 bits;
  Uint16 bsz = (Uint16) bits.size();
  int actv_cnt = getActiveChans(bits);
  // AH_INFO(ahlog::LOW) << " actv_cnt: " << actv_cnt << " -- " << bits << " :: " << bits.to_string() << endl;
  clog << " actv_cnt: " << actv_cnt << " -- " << bits << " :: " << bits.to_string() << endl;
  for( Uint16 i = 0; i < bsz; ++i ) {
    if( bits[i] ) actvchans.push_back(i);
  }
  return bits;
}

Uint16 SGD_1ASIC::getRefChan(Bits10& bits) { return (Uint16)bits.to_ulong(); }
Uint16 SGD_1ASIC::getNoiseChan(Bits10& bits) { return (Uint16)bits.to_ulong(); }
int SGD_1ASIC::getADCvals(vector< Bits10 >& adcvec) { return 0; }

/// use private local helper func., but it does not perform sanity check of offset vs buff size ...
int extractTLMbits(Bits10& bitflags, const ByteTLM& raw_asic_data, int byt_offset, int bit_offset) {
  if( ! raw_asic_data.bufptr ) return 0;
  if( byt_offset >= raw_asic_data.length - 2 ) return 0;
  return extractTLMbits(bitflags, raw_asic_data.bufptr, byt_offset, bit_offset);
}

int extractTLMbits(Bits8& bitflags, const ByteTLM& raw_asic_data, int offset) {
  if( ! raw_asic_data.bufptr ) return 0;
  if( offset >= raw_asic_data.length - 1 ) return 0;
  return extractTLMbits(bitflags, raw_asic_data.bufptr, offset);
}

int extractTLMbits(Bits16& bitflags, const ByteTLM& raw_asic_data, int offset) {
  if( ! raw_asic_data.bufptr ) return 0;
  if( offset >= raw_asic_data.length - 2 ) return 0;
  return extractTLMbits(bitflags, raw_asic_data.bufptr, offset);
}

int extractTLMbits(Bits32& bitflags, const ByteTLM& raw_asic_data, int offset) {
  if( ! raw_asic_data.bufptr ) return 0;
  if( offset >= raw_asic_data.length - 4 ) return 0;
  return extractTLMbits(bitflags, raw_asic_data.bufptr, offset);
}

int extractTLMbits(Bits64& bitflags, const ByteTLM& raw_asic_data, int offset) {
  if( ! raw_asic_data.bufptr ) return 0;
  if( offset >= raw_asic_data.length - 8 ) return 0;
  return extractTLMbits(bitflags, raw_asic_data.bufptr, offset);
}

int extractTLMbits(Bits8& bitflags, const TLMbyteVec& raw_asic_data, int offset) {
  if( ! raw_asic_data.data() ) return 0;
  if( offset >= (int)raw_asic_data.size() - 1 ) return 0;
  return extractTLMbits(bitflags, (Ubyte*)raw_asic_data.data(), offset);
}

int extractTLMbits(Bits10& bitflags, const TLMbyteVec& raw_asic_data, int byt_offset, int bit_offset) {
  if( ! raw_asic_data.data() ) return 0;
  if( byt_offset >= (int)raw_asic_data.size() - 2 ) return 0;
  return extractTLMbits(bitflags, (Ubyte*)raw_asic_data.data(), byt_offset, bit_offset);
}

int extractTLMbits(Bits16& bitflags, const TLMbyteVec& raw_asic_data, int offset) {
  if( ! raw_asic_data.data() ) return 0;
  if( offset >= (int)raw_asic_data.size() - 2 ) return 0;
  return extractTLMbits(bitflags, (Ubyte*)raw_asic_data.data(), offset);
}

int extractTLMbits(Bits32& bitflags, const TLMbyteVec& raw_asic_data, int offset) {
  if( ! raw_asic_data.data() ) return 0;
  if( offset >= (int)raw_asic_data.size() - 4 ) return 0;
  return extractTLMbits(bitflags, (Ubyte*)raw_asic_data.data(), offset);
}

int extractTLMbits(Bits24& bitflags, const TLMbyteVec& raw_asic_data, int offset) {
  if( ! raw_asic_data.data() ) return 0;
  if( offset >= (int)raw_asic_data.size() - 8 ) return 0;
  return extractTLMbits(bitflags, (Ubyte*)raw_asic_data.data(), offset);
}

/// this demonstrates how to deal with bitsets > 32
int extractTLMbits(Bits64& bitflags, const TLMbyteVec& raw_asic_data, int offset) {
  Bits32 lsb, msb;
  extractTLMbits(lsb, raw_asic_data, offset);
  extractTLMbits(msb, raw_asic_data, offset);
  for( int i = 0; i < 32; ++i ) {
    bitflags[i] = lsb[i];
    bitflags[32+i] = msb[i];
  }
  return (int)bitflags.size(); 
}

int getActiveChanIds(vector<int>& actv_chanIds, Bits32& bitflags, int rowIdx, int hduIdx) {
  actv_chanIds.clear();
  int sz = (int)bitflags.size();
  for( int i = 0; i <  sz; ++i ) {
    if( bitflags.test(i) ) actv_chanIds.push_back(i + 1); // channel Ids range from 1-32 (rampped from 1-32*40)
  }
  return (int) actv_chanIds.size();
}

/// needs pre-allocated hashmap of (empty) vecs and accociated vec of bitflags
int getAllActiveChans(map<int, vector<int> >& actv_chanIds, vector<Bits32>& bitflags, int rowIdx, int hduIdx) {
  if( actv_chanIds.empty() ) {
    AH_INFO(ahlog::LOW)<< " please pre-allocate the hash map with empty vecs ... " << endl;
    return -1;
  }
  int i= 0, cnt= 0;
  map< int, vector<int> >::iterator it;
  for( it = actv_chanIds.begin(); it != actv_chanIds.end(); ++it ) {
    int asic = it->first;
    AH_INFO(ahlog::LOW)<< " fetching active chanIds for (remapped) ASIC # " << asic << endl;
    vector<int>& chanIds = it->second;
    Bits32& bits = bitflags[i++];
    cnt += getActiveChanIds(chanIds, bits, rowIdx, hduIdx);
  }
  return cnt;
}

int getActiveChanIds(vector<int>& actv_chanIds, Bits64& bitflags, int rowIdx, int hduIdx) {
  actv_chanIds.clear();
  int sz = (int)bitflags.size();
  for( int i = 0; i <  sz; ++i ) {
    if( bitflags.test(i) ) actv_chanIds.push_back(i + 1);
  }
  return (int) actv_chanIds.size();
}

/// needs pre-allocated hashmap of (empty) vecs and accociated vec of bitflags
int getAllActiveChans(map< int, vector<int> >& actv_chanIds, vector<Bits64>& bitflags, int rowIdx, int hduIdx) {
  if( actv_chanIds.empty() ) {
    AH_INFO(ahlog::LOW)<< " please pre-allocate the hash map with empty vecs ... " << endl;
    return -1;
  }
  int i= 0, cnt= 0;
  map< int, vector<int> >::iterator it;
  for( it = actv_chanIds.begin(); it != actv_chanIds.end(); ++it ) {
    int asic = it->first;
    AH_INFO(ahlog::LOW)<< " fetching active chanIds for (remapped) ASIC # " << asic << endl;
    vector<int>& chanIds = it->second;
    Bits64& bits = bitflags[i++];
    cnt += getActiveChanIds(chanIds, bits, rowIdx, hduIdx);
  }
  return cnt;
}

//int BitTLM::capacity() { return (int) size(); }
void BitTLM::init(int maxlen, string nam, string fm) { name= nam; format= fm; length= 0; typecode= TBIT; resize(maxlen, 0); }
void BitTLM::setSlice(BitsAny& bv, string nam) { initSlice(bv); length = (int)bv.size(); if( nam.length() > 0 ) name = nam; }
BitTLM::BitTLM(int maxlen, string nam, string fm) : XSlice(maxlen) { init(maxlen, nam, fm); }

//int ByteTLM::capacity() { return (int) size(); }
void ByteTLM::init(int maxlen, string nam, string fm) { name= nam; format= fm; length= 0; typecode= TBYTE; initSlice(0, maxlen); }
void ByteTLM::setSlice(TLMbyteVec& bv, string nam) { initSlice(bv); length = (int)bv.size(); if( nam.length() > 0 ) name = nam; }
ByteTLM::ByteTLM(int maxlen, string nam, string fm) : BSlice(maxlen) { init(maxlen, nam, fm); }
ByteTLM::~ByteTLM() { if( length && maxlength ) clear(); }

//int Int16TLM::capacity() { return (int) size(); }
  void Int16TLM::init(int maxlen, string nam, string fm) { name= nam; format= fm; length= 0; typecode= TSHORT; initSlice(0, maxlen); }
void Int16TLM::setSlice(vector<Uint16>& iv, string nam) { initSlice(iv); length = (int)iv.size(); if( nam.length() > 0 ) name = nam; }
Int16TLM::Int16TLM(int maxlen, string nam, string fm) : ISlice(maxlen) { init(maxlen, nam, fm); }
Int16TLM::~Int16TLM() { if( length && maxlength ) clear(); }

//int Int32TLM::capacity() { return (int) size(); }
//void Int32TLM::init(int maxlen, string nam, string fm) { name= nam; format= fm; length= 0; typecode= TINT; initSlice(0, maxlen); }
void Int32TLM::init(int maxlen, string nam, string fm) { name= nam; format= fm; length= 0; typecode= TINT32BIT; initSlice(0, maxlen); }
void Int32TLM::setSlice(vector<Uint32>& iv, string nam) { initSlice(iv); length = (int)iv.size(); if( nam.length() > 0 ) name = nam; }
Int32TLM::Int32TLM(int maxlen, string nam, string fm) : JSlice(maxlen) { init(maxlen, nam, fm); }
Int32TLM::~Int32TLM() { if( length && maxlength ) clear(); }

//int Int64TLM::capacity() { return (int) size(); }
void Int64TLM::init(int maxlen, string nam, string fm) { name= nam; format= fm; length= 0; typecode= TLONGLONG; initSlice(0, maxlen); }
void Int64TLM::setSlice(vector<Uint64>& iv, string nam) { initSlice(iv); length = (int)iv.size(); if( nam.length() > 0 ) name = nam; }
Int64TLM::Int64TLM(int maxlen, string nam, string fm) : KSlice(maxlen) { init(maxlen, nam, fm); }
Int64TLM::~Int64TLM() { if( length && maxlength ) clear(); }

//int Real64TLM::capacity() { return (int) size(); }
void Real64TLM::init(int maxlen, string nam, string fm) { name= nam; format= fm; length= 0; typecode= TDOUBLE; initSlice(0, maxlen); }
void Real64TLM::setSlice(vector<double>& rv, string nam) { initSlice(rv); length = (int)rv.size(); if( nam.length() > 0 ) name = nam; }
Real64TLM::Real64TLM(int maxlen, string nam, string fm) : DSlice(maxlen) { init(maxlen, nam, fm); }
Real64TLM::~Real64TLM() { if( length && maxlength ) clear(); }

//int TextTLM::capacity() { return 1; }
void TextTLM::init(int maxlen, string nam, string fm) { name= nam; format= fm; length= 0; typecode= TSTRING; resize(maxlen, 'a'); }
void TextTLM::setSlice(string& text, string nam) { 
  string maxtext; clearFITSstr(maxtext); // ensure a viable (fits text) max capacity
  maxtext = text; // should not resize capacity
  initSlice(maxtext); 
  length = 1;
  if( nam.length() > 0 ) name = nam;
}
TextTLM::TextTLM(int maxlen, string nm, string fm) : ASlice(maxlen) { init(maxlen, nm, fm); }

  /** @} */

} // namespace ahtlm

#endif // AHTLM_C

