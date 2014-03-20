///////////////////////////////////////////////////////////////////////
//
// PrefetchBuffer.cpp
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Prefetch Buffer
//
// Revision history
// 9-20-2005, Eric Chung
//
//

#include <iostream>
#include <stdio.h>
#include "CacheArray.h"
#include "PrefetchBuffer.h"
#include <math.h>

//-----------------------------------------
// LFSR-specific constants 

#define LSFR_LENGTH 21
#define POLY "010000000000000000001" 


using namespace nCache;
using namespace std;

namespace nPrefetcher {

  DirectMapPrefetchBuffer::DirectMapPrefetchBuffer( uint32_t aPrefetchBufferSize, uint16_t aBlockSize ) {
    theBufferSize = aPrefetchBufferSize;
    theBlockSize = aBlockSize;
    theBlockBits = nCache::log_base2(theBlockSize);

    cout << "-------------------------------------------------------------------------------" << endl;
    cout << "[PFB-DM] Number of prefetch buffer entries: " << theBufferSize << endl;

    theBuffer = new PrefetchBufferEntry*[theBufferSize];

    for(int i = 0; i < theBufferSize; i++) {
      theBuffer[i] = new PrefetchBufferEntry;
      theBuffer[i]->theValid = false;
      theBuffer[i]->theAddress = 0;
    }
  }

  uint32_t DirectMapPrefetchBuffer::getBufferIndex(MemoryAddress aMemoryAddress) {
    return (aMemoryAddress >> theBlockBits) % theBufferSize;
  }

  void DirectMapPrefetchBuffer::insertBufferEntry(MemoryAddress aMemoryAddress) {
    int bufferIdx = getBufferIndex(aMemoryAddress);
    #ifdef DEBUG_PF
    cout << "[PFB-DM] Inserting buffer entry: " << aMemoryAddress << endl;
    if(theBuffer[bufferIdx]->theValid == true)
      cout << "[PFB-DM] Replacing old entry: " << theBuffer[bufferIdx]->theAddress << endl;
    #endif

    theBuffer[bufferIdx]->theValid = true;
    theBuffer[bufferIdx]->theAddress = aMemoryAddress;
  }

  void DirectMapPrefetchBuffer::deleteBufferEntry(MemoryAddress aMemoryAddress) {
    #ifdef DEBUG_PF
    cout << "[PFB] Deleting buffer entry: " << aMemoryAddress << endl;
    #endif
    int bufferIdx = getBufferIndex(aMemoryAddress);
    theBuffer[bufferIdx]->theValid = false;
    theBuffer[bufferIdx]->theAddress = 0;
  }


  MemoryAddress DirectMapPrefetchBuffer::getBlockAddress(MemoryAddress aMemoryAddress) {
    return aMemoryAddress >> theBlockBits;
  }

  bool DirectMapPrefetchBuffer::isHit(MemoryAddress aMemoryAddress) {
    int bufferIdx = getBufferIndex(aMemoryAddress);
    return (theBuffer[bufferIdx]->theValid == true) && (getBlockAddress(theBuffer[bufferIdx]->theAddress) == getBlockAddress(aMemoryAddress));
  }


  //----------------------------------------------------
  // FIFO prefetch buffer

  FifoPrefetchBuffer::FifoPrefetchBuffer( uint32_t aPrefetchBufferSize, uint16_t aBlockSize ) {
    theBufferSize = aPrefetchBufferSize;
    theBlockSize = aBlockSize;
    theBlockBits = nCache::log_base2(theBlockSize);

    cout << "-------------------------------------------------------------------------------" << endl;
    cout << "[PFB-FIFO] Number of prefetch buffer entries: " << theBufferSize << endl;

  }

  void FifoPrefetchBuffer::insertBufferEntry(MemoryAddress aMemoryAddress) {
    #ifdef DEBUG_PF
    cout << "[PFB-FIFO] Inserting buffer entry: " << aMemoryAddress << endl;
    #endif

    if(theBuffer.size() < theBufferSize) theBuffer.push_back(aMemoryAddress);
    else {
      // Delete the tail
      std::vector<MemoryAddress>::iterator iter = theBuffer.begin();
      theBuffer.erase(iter);

      #ifdef ASSERT
      assert(theBuffer.size() < theBufferSize);
      #endif
      theBuffer.push_back(aMemoryAddress);
    }
  }

  void FifoPrefetchBuffer::deleteBufferEntry(MemoryAddress aMemoryAddress) {
    for(int i=0; i < theBuffer.size(); i++) {
      if(getBlockAddress(theBuffer[i]) == getBlockAddress(aMemoryAddress)) {
        std::vector<MemoryAddress>::iterator iter = theBuffer.begin() + i;
	theBuffer.erase(iter);
        #ifdef ASSERT
	assert(!isHit(aMemoryAddress));
        #endif
        return ;
      }
    }

    #ifdef ASSERT
    assert(false);
    #endif
  }


  bool FifoPrefetchBuffer::isHit(MemoryAddress aMemoryAddress) {
    for(int i=0; i < theBuffer.size(); i++) {
      if(getBlockAddress(theBuffer[i]) == getBlockAddress(aMemoryAddress)) return true;
    }

    return false;
  }

  MemoryAddress FifoPrefetchBuffer::getBlockAddress(MemoryAddress aMemoryAddress) {
    return aMemoryAddress >> theBlockBits;
  }


  //----------------------------------------------------
  // Random prefetch buffer

  RandomPrefetchBuffer::RandomPrefetchBuffer( uint32_t aPrefetchBufferSize, uint16_t aBlockSize ) {
    theBufferSize = aPrefetchBufferSize;
    theCurrentBufferSize = 0;

    theBlockSize = aBlockSize;
    theBlockBits = nCache::log_base2(theBlockSize);

    theBuffer = new PrefetchBufferEntry*[theBufferSize];

    for(int i=0; i < aPrefetchBufferSize; i++) {
      theBuffer[i] = new PrefetchBufferEntry;
      theBuffer[i]->theValid = false;
      theBuffer[i]->theAddress = 0;
    }

    //-------------------------------
    // LFSR initialization

    theCurrRand = 1;
    theMaxRand = (int) pow((double)2, (double)LSFR_LENGTH);
    thePolyStr = POLY;
    thePoly = 0;

    for(int i=0; i < LSFR_LENGTH; i++) {
      if(thePolyStr[LSFR_LENGTH-i-1] == '1') {
        thePoly = thePoly + (uint32_t)pow((double)2, (double)i);
      }
    }

    cout << "-------------------------------------------------------------------------------" << endl;
    cout << "[PFB-RAND] Number of prefetch buffer entries: " << theBufferSize << endl;

    theRandCalls = 0;
    theRandSum = 0;

  }



  void RandomPrefetchBuffer::insertBufferEntry(MemoryAddress aMemoryAddress) {
    #ifdef DEBUG_PF
    cout << "[PFB-RAND] Inserting buffer entry: " << aMemoryAddress << endl;
    #endif

    #ifdef ASSERT
    assert((theCurrentBufferSize >= 0) && (theCurrentBufferSize <= theBufferSize));
    assert(!isHit(aMemoryAddress));
    #endif

    if(theCurrentBufferSize < theBufferSize) {
      int foundFreeEntry = false;
      for(int i=0; i < theBufferSize; i++) {
        if(!theBuffer[i]->theValid) {
          theBuffer[i]->theValid = true;
	  theBuffer[i]->theAddress = aMemoryAddress;
	  /*
	  cout.flags(ios::dec);
	  cout << i << " ";
	  cout.flags(ios::hex);
	  cout << getBlockAddress(aMemoryAddress) << endl;
	  */
	  foundFreeEntry = true;
          ++theCurrentBufferSize;
	  return ;
	}
      }
    }

    else {
      uint32_t tRand = theCurrRand;
      tRand = tRand >> 1;
      tRand = tRand % (theBufferSize);
      #ifdef ASSERT
      assert(tRand <= theBufferSize - 1);
      #endif

      theCurrRand = theCurrRand << 1;
      if(theCurrRand >= theMaxRand) {
        theCurrRand = theCurrRand - theMaxRand;
	theCurrRand = theCurrRand ^ thePoly;
      }

      ++theRandCalls;
      theRandSum += tRand;

/*
      cout.flags(ios::dec);
      cout << tRand;
      cout.flags(ios::hex);
      cout << " " << getBlockAddress(theBuffer[tRand]->theAddress) << " " << getBlockAddress(aMemoryAddress) << endl;
*/

      theBuffer[tRand]->theValid = true; theBuffer[tRand]->theAddress = aMemoryAddress;

    }
  }

                        

  void RandomPrefetchBuffer::deleteBufferEntry(MemoryAddress aMemoryAddress) {

    #ifdef ASSERT
    assert(isHit(aMemoryAddress));
    #endif

    for(int i=0; i < theBufferSize; i++) {
      if((theBuffer[i]->theValid == true) && (getBlockAddress(theBuffer[i]->theAddress) == getBlockAddress(aMemoryAddress))) {
        theBuffer[i]->theValid = false;
	theBuffer[i]->theAddress = 0;
	--theCurrentBufferSize;
        #ifdef ASSERT
	assert(!isHit(aMemoryAddress));
        #endif
	return ;
      }
    }

    #ifdef ASSERT
    assert(false);
    #endif
  }


  bool RandomPrefetchBuffer::isHit(MemoryAddress aMemoryAddress) {
    for(int i=0; i < theBufferSize; i++) {
      if((theBuffer[i]->theValid == true) && (getBlockAddress(theBuffer[i]->theAddress) == getBlockAddress(aMemoryAddress))) return true;
    }        
    return false;
  }

  MemoryAddress RandomPrefetchBuffer::getBlockAddress(MemoryAddress aMemoryAddress) {
    return aMemoryAddress >> theBlockBits;
  }
}

