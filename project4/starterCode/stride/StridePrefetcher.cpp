///////////////////////////////////////////////////////////////////////
//
// CompetitionPrefetcher.cpp
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Competition prefetcher
//
// Revision history
// 9-25-2005, Eric Chung
// 3-7-2005 Yu Wang
//

#include <iostream>
#include "CacheArray.h"
#include "StridePrefetcher.h"

using namespace nCache;
using namespace std;

namespace nPrefetcher {


  Prefetcher::Prefetcher( uint16_t aBlockSize ) {
    theBlockSize = aBlockSize;
    theBlockBits = nCache::log_base2(theBlockSize);

    theIsPrefetchReady = false;
    resetStats();

    isEven = true;
  }


  MemoryAddress Prefetcher::getBlockAddress(MemoryAddress aMemoryAddress) {
    return aMemoryAddress >> theBlockBits;
  }


  bool Prefetcher::isPrefetchReady() {
    return theIsPrefetchReady;
  }


  MemoryMessage Prefetcher::getPrefetch() {
    #ifdef ASSERT
    assert(theIsPrefetchReady);
    #endif

    theIsPrefetchReady = false;

    ++thePrefetchesRequested;
    return thePrefetchOutputEntry;
  }


  void Prefetcher::resetStats() {
    thePrefetchesRequested = 0;
  }


  void Prefetcher::dumpStats() {
    cout << "[EX-PF] Number of requested prefetches: " << thePrefetchesRequested << endl;
  }


  void Prefetcher::updatePrefetcher(MemoryMessageType aMemoryMessageType,
   				    MemoryAddress aAddress, 
  				    bool isHitL1, 
				    bool isHitL2, 
				    bool isHitPrefetch, 
				    int numPrefetchQueueFree) {

    if(isEven) sendPrefetch((getBlockAddress(aAddress) + 1) << theBlockBits, L2);
    else sendPrefetch((getBlockAddress(aAddress) + 1) << theBlockBits, AUX);
    
    isEven = !isEven;
  }


  void Prefetcher::sendPrefetch(MemoryAddress aMemoryAddress, PrefetchType aPrefetchType) {
    assert(!theIsPrefetchReady);
    thePrefetchOutputEntry.theMemoryAddress = aMemoryAddress;
    thePrefetchOutputEntry.theMemoryMessageType = PREFETCH;
    thePrefetchOutputEntry.theSource = SPF;
    thePrefetchOutputEntry.thePrefetchType = aPrefetchType;
    theIsPrefetchReady = true;
  }

  void Prefetcher::evictionL2(MemoryAddress aMemoryAddress, EvictType aEvictType) {
  }

}
