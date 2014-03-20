///////////////////////////////////////////////////////////////////////
//
// CacheController.h
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Cache controller header
//
// Revision history
// 8-21-2005, Eric Chung
//
//

#include <iostream>
#include <stdlib.h>
#include <cassert>
#include <deque>


#include "CacheArray.h"
#include "PrefetchBuffer.h"

#ifndef CACHECONTROLLER
#define CACHECONTROLLER

namespace nCache {

  class CacheController {

    public:
      CacheController( int L1Dsize, int L1Dblocksize, int L1Dassoc,
                       int L1Isize, int L1Iblocksize, int L1Iassoc,
	    	       int L2Usize, int L2Ublocksize, int L2Uassoc, 
		       int aPrefetchQueueSize, int aPrefetchDelay, int aBufferSize );

      void processMemoryMessage(MemoryMessage *aMemoryMessage);
      void resetAllStats();
      void dumpAllStats();


      //------------------------------------------------
      // Internal information for a prefetcher

      bool isHitL1();
      bool isHitL2();
      bool isHitPrefetch();
      int getNumPrefetchQueueFree();

      bool didNormalEvict();
      bool didPrefetchEvict();

      MemoryAddress getNormalEvictAddress();
      MemoryAddress getPrefetchEvictAddress();

    private:
      CacheArray *theL1D;
      CacheArray *theL1I;
      CacheArray *theL2U;

      #ifdef EN_DM_AUX
      nPrefetcher::DirectMapPrefetchBuffer thePrefetchBuffer;
      #endif

      #ifdef EN_FIFO_AUX
      nPrefetcher::FifoPrefetchBuffer thePrefetchBuffer;
      #endif

      #ifdef EN_RAND_AUX
      nPrefetcher::RandomPrefetchBuffer thePrefetchBuffer;
      #endif


      #ifdef ENABLE_PF
      std::deque<MemoryMessage *> thePrefetchQueue;

      int thePrefetchQueueMaxSize;
      int thePrefetchQueueSize;

      int thePrefetchDelay;
      int thePrefetchCountdown;
      #endif


      void doMemory(CacheArray *aCache, MemoryMessage *aMemoryMessage);
      void handlePrefetchAux(CacheArray *aL1, CacheArray *aL2, MemoryMessage *aMemoryMessage);

      void doPrefetch(MemoryMessage *aMemoryMessage);
      void checkProbes();

      //-------------------------------------------
      // Prefetch statistics

      uint64_t theNumProcessedPrefetches;
      uint64_t theNumFillsToPB;
      uint64_t theNumFillsToL2;


      // Bandwidth is the total number of fills from memory to L2 due to regular misses and prefetch requests
      // Also includes number of dirty evictions from the L2 cache
 
      uint64_t theReadBandwidth;
      uint64_t theWriteBandwidth;
      uint64_t theBandwidth;

      // Additional stats for breakdown of bandwidth
      uint64_t theNormalEvictionDirtyL1;
      uint64_t theNormalEvictionDirtyL2;
      uint64_t theNormalEvictionDirtyBoth;

      uint64_t thePrefetchEvictionDirtyL1;
      uint64_t thePrefetchEvictionDirtyL2;
      uint64_t thePrefetchEvictionDirtyBoth;

      //-------------------------------------------
      // Internal events exposed to the prefetcher

      void resetInternalFlags();

      bool theIsHitL1;
      bool theIsHitL2;
      bool theIsHitPrefetch;
      bool theDidNormalEvict;
      bool theDidPrefetchEvict;

      int numPrefetchQueueFree;

      MemoryAddress aNormalEvictAddress;
      MemoryAddress aPrefetchEvictAddress;

  };


}

#endif
