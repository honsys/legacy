#if !defined(AHTLMSIM_C)
#define AHTLMSIM_C(arg) const char arg##AHTLMSIM_rcsId_svnId[] = "$Id: ahTLMsim.cxx,v 1.8 2012/08/16 19:42:07 dhon Exp $"; 

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

#include "ahtlm/ahBits.h"
AHBITS_H(ahTLMsim)

#include "ahtlm/ahSlice.h"
AHSLICE_H(ahTLMsim)

#include "ahtlm/ahTLM.h"
AHTLM_H(ahTLMsim)

/*
#include "ahhxi/ahHXItyps.h"
#include "ahsgd/ahSGDtyps.h"
*/

namespace { // private local helpers

const string cvsId = "$Name:  $ $Id: ahTLMsim.cxx,v 1.8 2012/08/16 19:42:07 dhon Exp $";
const int _HXInumAsicPerLayer = 8; // duplicated from ahHXItyps.h
const int _SGDnumAsicPerLayer = 208; // duplicated from ahSGDtyps.h
const int _HXImaxAsics = 5*8; // duplicated from ahHXItyps.h
const int _SGDmaxAsics = 3*208; // duplicated from ahSGDtyps.h

} // private local helpers

namespace ahtlm {

/** \addtogroup mod_ahtlm
 *  @{ 
 */

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

  // HXI TLM simulation funcs.
  // header:
  Bits5 HXI_TLM_HDR::setSEUflags(const Bits5 flags) { Bits5 bits; return bits; }
  Bits5 HXI_TLM_HDR::setLCHKflags(const Bits5 flags) { Bits5 bits; return bits; }
  Bits8 HXI_TLM_HDR::setTRGflags(const Bits8 flags) { Bits8 bits; return bits; }
  Bits8 HXI_TLM_HDR::setTRGPATflags(const Bits8 flags) { Bits8 bits; return bits; }
  Bits2 HXI_TLM_HDR::setHPATflags(const Bits2 flags) { Bits2 bits; return bits; }
  Bits2 HXI_TLM_HDR::setFBGflags(const Bits2 flags) { Bits2 bits; return bits; }
  Uint32 HXI_TLM_HDR::setLivetime(Uint32 time) { return 0; }

  // single asic:
  int HXI_1ASIC::setAsicAndLayerId(int aid, int lid) {
    int byte_offset = 1 + offset; // assume struct offset attribute properly initialized.
    Ubyte* val = &rawTLM.bufptr[byte_offset];
    Bits8 bits;
    Bits4 lidb4(lid), aidb4(aid);
    for( int i = 0; i < 4; ++i ) {
      bits[i] = lidb4[i];
      bits[4+i] = aidb4[i];
    }
    *val = bits.to_ulong();
    return *val;
  }

  int HXI_1ASIC::setActiveChans(const Bits32& actvbits) {
    if( 0 == rawTLM.bufptr ) {
      AH_INFO(ahlog::LOW) << " no rawTLM content (yet)? " << endl;
      return -1;
    }
    Uint16 startflag = *((Uint16*) rawTLM.bufptr[0]);
    if( 0x5555 != startflag ) AH_INFO(ahlog::LOW) << " ? rawTLM startflag suspect?: " << hex << startflag << endl;

    // the byte offset to the active chan bits of this single asic buffer:
    int byte_offest = 2 + offset; // assume struct offset attribute properly initialized.
    Uint32* chanflags = (Uint32*) &rawTLM.bufptr[byte_offest];
    *chanflags = (Uint32) actvbits.to_ulong(); // memcpy(chanflags, &bits, sizeof(Uint32));
    // verify bits
    Bits32 verify;
    int actv_cnt = getActiveChans(verify);
    AH_INFO(ahlog::LOW) << " actv_cnt: " << actv_cnt << " -- " << actvbits.to_string() << " :: " << verify.to_string() << endl;
    return actv_cnt;
  }

  Bits32 HXI_1ASIC::setActiveChans(const vector< Uint16 >& actvchans) {
    int actv_cnt = (int) actvchans.size();
    Bits32 bits;
    int bsz = (int) bits.size();
    for( int i = 0; i < actv_cnt; ++i ) { // check bit range: 0 -- 31
      int idx = actvchans[i]; 
      if( idx < 0 ) idx = 0; if( idx >= bsz ) idx = bsz-1;
      bits.set( idx );
    }
    int actv = setActiveChans(bits);
    if( actv != actv_cnt ) AH_INFO(ahlog::LOW) << " actv_cnt discrepency? " << endl;
    return bits;
  }

  Uint16 HXI_1ASIC::setRefChan(const Bits10& bits) { return (Uint16)bits.to_ulong(); }
  Uint16 HXI_1ASIC::setNoiseChan(const Bits10& bits) { return (Uint16)bits.to_ulong();  }
  int HXI_1ASIC::setADCvals(vector< Bits10 >& adcvec) { return 0; }

  // SGD TLM simulation funcs.
  // header:
  Bits1 SGD_TLM_HDR::setSEUflag(const Bits1 flags) { Bits1 bits; return bits; }
  Bits1 SGD_TLM_HDR::setLCHKflag(const Bits1 flags) { Bits1 bits; return bits; }
  Bits1 SGD_TLM_HDR::setCalflag(const Bits1 flags) { Bits1 bits; return bits; }
  Bits31 SGD_TLM_HDR::setTRGPATflags(const Bits31 flags) { Bits31 bits; return bits; }
  Bits6 SGD_TLM_HDR::setTRGflags(const Bits6 flags) { Bits6 bits; return bits; }
  Bits3 SGD_TLM_HDR::setCCbusyflags(const Bits3 flags) { Bits3 bits; return bits; }
  Bits3 SGD_TLM_HDR::setHitPatCCflags(const Bits3 flags) { Bits3 bits; return bits; }
  Bits4 SGD_TLM_HDR::setHitPatflags(const Bits4 flags) { Bits4 bits; return bits; }
  Bits4 SGD_TLM_HDR::setFastBGOflags(const Bits4 flags) { Bits4 bits; return bits; }
  Uint32 SGD_TLM_HDR::setLivetime(Uint32 time) { return 0; }

  // single asic:
  int SGD_1ASIC::setAsicAndLayerId(int aid, int lid) {
    int byte_offset = 2 + offset; // assume struct offset attribute properly initialized.
    Uint16* val = (Uint16* ) &rawTLM.bufptr[byte_offset];
    Bits16 bits; // the real SGD TLM may different bit lengths ...
    Bits8 lidb8(lid), aidb8(aid);
    for( int i = 0; i < 8; ++i ) {
      bits[i] = lidb8[i];
      bits[8+i] = aidb8[i];
    }
    *val = bits.to_ulong();
    return *val;
  }

  int SGD_1ASIC::setActiveChans(const Bits64& actvbits) {
    if( 0 == rawTLM.bufptr ) {
      AH_INFO(ahlog::LOW) << " no rawTLM content (yet)? " << endl;
      return -1;
    }
    Uint16 startflag = *((Uint16*) &rawTLM.bufptr[0]);
    if( 0x5555 != startflag ) AH_INFO(ahlog::LOW) << " ? rawTLM startflag suspect?: " << hex << startflag << endl;
    //
    // use 2 Bits32 intermediaries:
    Bits32 lsb, msb;
    int bsz = (int) lsb.size();
    for( int i = 0; i < bsz; ++i ) {
      lsb[i] = actvbits[i];
      msb[i] = actvbits[i+bsz];
    }

    // the byte offset to the active chan bits of this single asic buffer:
    int byte_offest = 4 + offset;  // assume struct offset attribute properly initialized.
    Uint32* lsbchanflags = (Uint32*) &rawTLM.bufptr[byte_offest];
    Uint32* msbchanflags = (Uint32*) &rawTLM.bufptr[2+byte_offest];
    *lsbchanflags = (Uint32) lsb.to_ulong();
    *msbchanflags = (Uint32) msb.to_ulong();
    // verify bits
    Bits64 verify;
    int actv_cnt = getActiveChans(verify);
    AH_INFO(ahlog::LOW) << " actv_cnt: " << actv_cnt << " -- " << actvbits.to_string() << " :: " << verify.to_string() << endl;
    return actv_cnt;
  }

  Bits64 SGD_1ASIC::setActiveChans(const vector< Uint16 >& actvchans) {
    int actv_cnt = (int) actvchans.size();
    Bits64 bits;
    int bsz = (int) bits.size();
    for( int i = 0; i < actv_cnt; ++i ) { // check bit range: 0 -- 63
      int idx = actvchans[i]; 
      if( idx < 0 ) idx = 0; if( idx >= bsz ) idx = bsz-1;
      bits.set( idx );
    }
    int actv = setActiveChans(bits);
    if( actv != actv_cnt ) AH_INFO(ahlog::LOW) << " actv_cnt discrepency? " << endl;
    return bits;
   
  }

  Uint16 SGD_1ASIC::setRefChan(const Bits10& bits) { return (Uint16)bits.to_ulong(); }
  Uint16 SGD_1ASIC::setNoiseChan(const Bits10& bits) { return (Uint16)bits.to_ulong(); }
  int SGD_1ASIC::setADCvals(vector< Bits10 >& adcvec) { return 0; }
 
  // placeholder/pretend TLM header that places an HXI or SGD nHitAsics count (1-40 or 1-624)
  // into the header at byte offset == 12 (i.e byte idx 12 and 13), and asics id in the
  // next two bytes (offset = 14 -- idx 14 and 15), and then the 32 or 64 bit chan flags

  // append raw data to occurrence buffer
  int genTLM4Test(string& tlm_astext, int hitcnt, int actvchan, int tlmIdx) {
    //tlm_astext = "aabbccddeeff"; // example: 6 asics, 2 chan each
    string ascii = "abcdefghigklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"; // our TLM raw data range! :)
    for( int asicIdx = 0; asicIdx < hitcnt; ++asicIdx ) {
      char tlm = ascii[asicIdx];
      for( int chanIdx = 0; chanIdx < actvchan; ++chanIdx ) tlm_astext[tlmIdx++] = tlm;
    }
    return (int)tlm_astext.length();
  }
  
  int genTLM4Test(string& tlm_astext, int hitcnt, int actvchan, Bits32& chanbits) {
    int occurrence_data_offset = 16 + 4;
    tlm_astext.clear(); tlm_astext.resize(occurrence_data_offset + hitcnt*actvchan);
    // the first 12 bytes should contain flags and local-time, but just put text:
    string byte_hdr = "HXI_HXI_HXI\n";
    int offset = 12; 
    for( int i = 0; i < offset; ++i ) tlm_astext[i] = byte_hdr[i];
    // set the asic hit count, asic ids, and the active channel flags:
    union { unsigned short asic; unsigned char abuf[2]; };
    asic = (unsigned short)hitcnt; 
    tlm_astext[offset] = abuf[0]; tlm_astext[1+offset] = abuf[1];

    offset = 14;
    Bits16 asic_ids;
    for( int i = 0; i < hitcnt % _HXInumAsicPerLayer; ++i ) asic_ids.set(i);
    asic = (unsigned short) asic_ids.to_ulong();
    tlm_astext[offset] = abuf[0]; tlm_astext[1+offset] = abuf[1];

    offset = 16;
    chanbits.reset();
    //for( int i = 0; i < actvchan % 32; ++i ) chanbits.set(i);
    for( int i = 0; i < actvchan % 16; ++i ) chanbits.set(i);
    union { unsigned int chanflags ; unsigned char cbuf[4]; };
    chanflags = (unsigned int) chanbits.to_ulong();
    for( int i = 0; i < 4; ++i ) tlm_astext[i+offset] = cbuf[i];
   
    // and append some "raw" data
    return genTLM4Test(tlm_astext, hitcnt, actvchan, occurrence_data_offset);
  }

  // this placeholder is less than bogus, due to lack of details about the bit lengths
  // and organization of SGD nhit-asics, and asic Id telemetry elements -- need SGD
  // fancy chart event telemtry schematic comparable to HXI's.
  int genTLM4Test(string& tlm_astext, int hitcnt, int actvchan, Bits64& chanbits) {
    int occurrence_data_offset = 16 + 8;
    tlm_astext.clear(); tlm_astext.resize(occurrence_data_offset + hitcnt*actvchan);
    string byte_hdr = "SGD_SGD_SGD\n";
    // ? bogus SGD offsets...
    int offset = 12; 
    for( int i = 0; i < offset; ++i ) tlm_astext[i] = byte_hdr[i];
    // set the asic hit count, asic ids, and the active channel flags:
    union { unsigned short asic; char abuf[2]; };
    asic = (unsigned short)hitcnt; 
    tlm_astext[offset] = abuf[0]; tlm_astext[1+offset] = abuf[1];

    offset = 14;
    Bits16 asic_ids; // ? bogus _SGDnumAsicPerLayer value
    //for( int i = 0; i < hitcnt % _SGDnumAsicPerLayer; ++i ) asic_ids.set(i);
    for( int i = 0; i < hitcnt % 16; ++i ) asic_ids.set(i);
    asic = (unsigned short) asic_ids.to_ulong();
    tlm_astext[offset] = abuf[0]; tlm_astext[1+offset] = abuf[1];

    offset = 16;
    chanbits.reset();
    for( int i = 0; i < actvchan % 64; ++i ) chanbits.set(i);
    union { unsigned long long chanflags ; unsigned char cbuf[8]; };
    chanflags = (unsigned int) chanbits.to_ulong();
    for( int i = 0; i < 8; ++i ) tlm_astext[i+offset] = cbuf[i];

    // and append some "raw" data
    return genTLM4Test(tlm_astext, hitcnt, actvchan, occurrence_data_offset);
  }

  // each asic has a different number of active adc-chans. and noise and ref. chans
  // hitcnt == asicIdVec.size() and hash-map key == asicId with hash-map value == chan-bit-flag
  int genTLM4Test(string& tlm_astext, vector<int>& asicIdVec, map<int, Bits32>& chanFlagHash) {
    chanFlagHash.clear(); 
    int asic_cnt = (int) asicIdVec.size();
    int asic_id = 1, tot_actvchans = 0;
    const int max_asics = 40;
    if( asic_cnt <= 0 || asic_cnt > max_asics ) {
      // assume asic Ids are not provided
      asicIdVec.clear();
      char* txtid = getenv("AHTEST_ASICID"); 
      // start sim with this asic id and increment by hitcnt
      if( txtid ) {
        asic_id = atoi(txtid);
      }
      // presumably all asic ids should be unique within occurrence
      char* txtcnt = getenv("AHTEST_HITASICS");
      if( txtcnt ) {
        asic_cnt = atoi(txtcnt);
      }
      else {
        txtcnt = getenv("AHTEST_ROWS");
        if( txtcnt ) {
          asic_cnt = atoi(txtcnt);
        }
        else {
          asic_cnt = max_asics; // max: stress-test !
        }
      }
    }
    // make sure asic_id + asic_cnt <= 40!
    if( asic_cnt >= max_asics ) {
      asic_cnt = max_asics;
      asic_id = 1;
    }
    if( asic_cnt - asic_id  >= max_asics ) {
      asic_id = 1 + (max_asics - asic_cnt);
    }
    if( asicIdVec.empty() ) {
      for( int i = asic_id; i <= asic_cnt; ++i ) asicIdVec.push_back(i);
    }
    for( int ia = 1; ia <= asic_cnt; ++ia ) {
      int actvchans = 2 + ia / 2; // 2 <= chans <= 22
      tot_actvchans += actvchans;
      Bits32 actvbits;
      for( int ic = 1; ic <= actvchans; ++ic ) actvbits[ic] = 1;
      chanFlagHash[ia] = actvbits;
    }
    AH_INFO(ahlog::LOW) << " asic hit cnt: " <<  asicIdVec.size() << ", total active chan. count: " << tot_actvchans << endl;
    printChanFlags(chanFlagHash);
    return tot_actvchans;
  }  // genTLM4Test 32bit channel flags

  // each asic has a different number of active adc-chans. and noise and ref. chans
  // hitcnt == asicIdVec.size() and hash-map key == asicId with hash-map value == chan-bit-flag
  int genTLM4Test(string& tlm_astext, vector<int>& asicIdVec, map<int, Bits64>& chanFlagHash) {
    chanFlagHash.clear(); 
    int asic_cnt = (int) asicIdVec.size();
    int asic_id = 1, tot_actvchans = 0;
    const int max_asics = 624;
    if( asic_cnt <= 0 || asic_cnt > max_asics ) {
      // assume asic Ids are not provided
      asicIdVec.clear();
      char* txtid = getenv("AHTEST_ASICID"); 
      // start sim with this asic id and increment by hitcnt
      if( txtid ) {
        asic_id = atoi(txtid);
      }
      // presumably all asic ids should be unique within occurrence
      char* txtcnt = getenv("AHTEST_HITASICS");
      if( txtcnt ) {
        asic_cnt = atoi(txtcnt);
      }
      else {
        txtcnt = getenv("AHTEST_ROWS");
        if( txtcnt ) {
          asic_cnt = atoi(txtcnt);
        }
        else {
          asic_cnt = max_asics; // max: stress-test !
        }
      }
    }
    // make sure asic_id + asic_cnt <= 624!
    if( asic_cnt >= max_asics ) {
      asic_cnt = max_asics;
      asic_id = 1;
    }
    if( asic_cnt - asic_id  >= max_asics ) {
      asic_id = 1 + (max_asics - asic_cnt);
    }
    if( asicIdVec.empty() ) {
      for( int i = asic_id; i <= asic_cnt; ++i ) asicIdVec.push_back(i);
    }
    for( int ia = 1; ia <= asic_cnt; ++ia ) {
      int actvchans = 1 + ia / 10; // 1 <= chans <= 63
      tot_actvchans += actvchans;
      Bits64 actvbits;
      for( int ic = 1; ic <= actvchans; ++ic ) actvbits[ic] = 1;
      chanFlagHash[ia] = actvbits;
    }
    AH_INFO(ahlog::LOW) << " asic hit cnt: " <<  asicIdVec.size() << ", total active chan. count: " << tot_actvchans << endl;
    printChanFlags(chanFlagHash);
    return tot_actvchans;
  }  // genTLM4Test 64bit channel flags

  /** @} */

} // namespace ahtlm

#endif // AHTLMSIM_C

