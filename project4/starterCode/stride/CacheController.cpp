///////////////////////////////////////////////////////////////////////
//
// CacheController.cpp
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Cache controller that instantiates entire cache hierarchy
//
// Revision history
// 8-21-2005, Eric Chung
//
//


#include <iostream>
#include <stdlib.h>
#include <cassert>
#include <deque>

#include "CacheController.h"


namespace nCache {

  using namespace std;

  CacheController::CacheController( int L1Dsize, int L1Dblocksize, int L1Dassoc,
                                    int L1Isize, int L1Iblocksize, int L1Iassoc,
				    int L2Usize, int L2Ublocksize, int L2Uassoc, 
				    int aPrefetchQueueSize, int aPrefetchDelay, int aPrefetchBufferSize )

    : thePrefetchBuffer(aPrefetchBufferSize, L2Ublocksize) 

  {
    theL1D = new CacheArray(L1Dsize, L1Dassoc, L1Dblocksize, L1D, "L1-Data");
    theL1I = new CacheArray(L1Isize, L1Iassoc, L1Iblocksize, L1I, "L1-Inst");
    theL2U = new CacheArray(L2Usize, L2Uassoc, L2Ublocksize, L2U, "L2-Unif");

    #ifdef ENABLE_PF
    thePrefetchQueueMaxSize = aPrefetchQueueSize;
    thePrefetchQueueSize = 0;

    thePrefetchDelay = aPrefetchDelay + 1;
    thePrefetchCountdown = aPrefetchDelay + 1;

    resetAllStats();


    // Internal prefetch event flags and addresses
    resetInternalFlags();

    #endif
  }

  void CacheController::resetInternalFlags() {
    theIsHitL1 = false;
    theIsHitL2 = false;
    theIsHitPrefetch = false;
    theDidNormalEvict = false;
    theDidPrefetchEvict = false;
    aNormalEvictAddress = 0;
    aPrefetchEvictAddress = 0;
  }


  void CacheController::processMemoryMessage(MemoryMessage *aMemoryMessage) {
    //resetInternalFlags();

    #ifdef ENABLE_PF

    //------------------------------------------------------------------------
    // Decrement the counter

    if((aMemoryMessage->theMemoryMessageType == PC) && (thePrefetchCountdown > 0) && (thePrefetchQueueSize > 0)) --thePrefetchCountdown;


    //------------------------------------------------------------------------
    // If the queue isn't empty and we are on a PC and the countdown is zero

    if((aMemoryMessage->theMemoryMessageType == PC) && (thePrefetchCountdown == 0) && (thePrefetchQueueSize > 0)) {
      doPrefetch(thePrefetchQueue.front());
      delete thePrefetchQueue.front();
      thePrefetchQueue.pop_front();
      --thePrefetchQueueSize;
      thePrefetchCountdown = thePrefetchDelay;
    }

    #endif


    switch(aMemoryMessage->theMemoryMessageType) {
      case PC: 
        doMemory(theL1I, aMemoryMessage);
        break;
      case LOAD:
      case STORE:
        doMemory(theL1D, aMemoryMessage);
	break;

      #ifdef ENABLE_PF
      case PREFETCH:
        if(thePrefetchQueueSize < thePrefetchQueueMaxSize) {
	  thePrefetchQueue.push_back(aMemoryMessage);
	  ++thePrefetchQueueSize;
	} else {
          delete aMemoryMessage;
	}

        break;
      #endif

      default:
        assert(false);
    }
  }


  //--------------------------------------------------------
  // Probe L1 caches
  // Side-effects: force evictions, consume and delete
  // the probe message

  void CacheController::checkProbes() {
    if(theL2U->isFrontSideOutReady()) {
      MemoryMessage *aMemoryMessage = theL2U->getFrontSideOut();
      #ifdef ASSERT
      assert(!theL2U->isFrontSideOutReady());
      #endif

      // Handle probes to L1I
      
      #ifdef ENABLE_PF
      if(aMemoryMessage->theSource == SPF) {
	theL1I->processBackSideIn(aMemoryMessage);
        theL1D->processBackSideIn(aMemoryMessage);
	delete aMemoryMessage;
	return ;
      }
      #endif


      if(aMemoryMessage->theSource == L1I) {
	theL1I->processBackSideIn(aMemoryMessage);
	delete aMemoryMessage;
	#ifdef ASSERT
	assert(!theL1I->isBackSideOutReady());
        #endif
	return ;
      }

      // Handle probes to L1D
      if(aMemoryMessage->theSource == L1D) {
        theL1D->processBackSideIn(aMemoryMessage);
	delete aMemoryMessage;
	#ifdef ASSERT
	assert(!theL1D->isBackSideOutReady());
        #endif
	return ;
      }
    }
  } //checkProbes



  //-------------------------------------------------------------------------------------------------
  // This function handles when we have an auxiliary prefetch buffer that is searched in parallel
  // with the L2 cache whenever misses occur in either L1 cache


  void CacheController::handlePrefetchAux(CacheArray *theL1, CacheArray *theL2, MemoryMessage *aMemoryMessage) {

    theL1->processFrontSideIn(aMemoryMessage);

    if(!theL1->isBackSideOutReady()) {
      theIsHitL1 = true;
      theIsHitL2 = true;
      if(thePrefetchBuffer.isHit(aMemoryMessage->theMemoryAddress)) theIsHitPrefetch = true;
      theDidNormalEvict = false;
    } else {
      theIsHitL1 = false;
    }

    while(theL1->isBackSideOutReady()) {

      MemoryMessage *aL1Request = theL1->getBackSideOut();

      //--------------------------------------------------------
      // Process any evictions out of the L1 cache first

      if(aL1Request ->theMemoryMessageType == EVICTDIRTY) {
        theL2U->processFrontSideIn(aL1Request);
        #ifdef ASSERT
	assert(theL1->isBackSideOutReady());
        #endif
      }


      // Handle processing in L2 and the auxiliary cache

      else {

        #ifdef ASSERT
        assert(aL1Request->theMemoryMessageType == LOAD || aL1Request ->theMemoryMessageType == STORE);
	assert(!theL1->isBackSideOutReady());
        #endif

        //--------------------------------------------------------------------
        // Now check if the request is in the auxiliary buffer
        // If so, move the line into the L2 cache, then process the line in L1

        if(thePrefetchBuffer.isHit(aL1Request->theMemoryAddress)) {
	  theIsHitL2 = false;
          theIsHitPrefetch = true;

          #ifdef DEBUG_PF
	  cout.flags(ios::hex);
          cout << "[CC] Line hit in PF buffer, deleting and moving to L2U: " << aL1Request->theMemoryAddress << endl;
          #endif

          #ifdef ASSERT
          // Line should not be in L2
          assert(!theL2U->isHit(aL1Request->theMemoryAddress));
          #endif

          //----------------------------------------------------------------
	  // Remove the entry from the prefetch buffer

          thePrefetchBuffer.deleteBufferEntry(aL1Request->theMemoryAddress);

          //----------------------------------------------------------------
          // Insert the entry into the L2, process any evictions out of the 
	  // L2, process any forced evictions out of the L1

	  bool isDirtyL1 = false;
	  bool isDirtyL2 = false;

          theL2U->insertEntry(aL1Request->theMemoryAddress, SPF, false, true);
    	  while(theL2U->isBackSideOutReady()) {
	    MemoryMessage *aMemoryMessage = theL2U->getBackSideOut(); 
            if(aMemoryMessage->theMemoryMessageType == EVICTDIRTY) {
	      aNormalEvictAddress = aMemoryMessage->theMemoryAddress;
	      theDidNormalEvict = true;
	      isDirtyL2 = true;
	      ++theNormalEvictionDirtyL2;
	    }
	  }

	  checkProbes();

	  if(theL1D->didForcedDirtyWriteback()) {
	    aNormalEvictAddress = theL1D->getForcedDirtyWritebackAddress();
            theDidNormalEvict = true;
	    isDirtyL1 = true;
	    ++theNormalEvictionDirtyL1;
	  }

	  if(isDirtyL1 && isDirtyL2) ++theNormalEvictionDirtyBoth;

	  if(theL2U->didCleanEviction()) {
            theDidNormalEvict = true;
            aNormalEvictAddress = theL2U->getCleanEvictionAddress();
	  }

          //----------------------------------------------------------------
	  // Now process the original request to L2, this should be a hit

          #ifdef ASSERT
	  assert(theL2U->isHit(aL1Request->theMemoryAddress));
	  assert(!theL2U->isBackSideOutReady());                     // No evictions should appear
          #endif

          theL2U->processFrontSideIn(aL1Request);
        }

        //--------------------------------------------------------------------------------
        // Otherwise just process like usual since we didn't hit in the prefetch buffer
	//
	// All the flagging is simply used to monitor events on a per-processing of references
	// This information gets exposed to some external module like a prefetcher
	// 

        else {
	  theIsHitPrefetch = false;
          theL2U->processFrontSideIn(aL1Request);

          if(theL2U->isBackSideOutReady()) theIsHitL2 = false;
          else theIsHitL2 = true;

          bool isDirtyL1 = false;
	  bool isDirtyL2 = false;

          while(theL2U->isBackSideOutReady()) {
	    MemoryMessage *aMemoryMessage = theL2U->getBackSideOut(); 
	    if(aMemoryMessage->theMemoryMessageType == EVICTDIRTY) {
	      aNormalEvictAddress = aMemoryMessage->theMemoryAddress;
	      theDidNormalEvict = true;
	      isDirtyL2 = true;
	      ++theNormalEvictionDirtyL2;
	    }
	  }

          checkProbes();

	  if(theL1D->didForcedDirtyWriteback()) {
            theDidNormalEvict = true;
	    aNormalEvictAddress = theL1D->getForcedDirtyWritebackAddress();
	    isDirtyL1 = true;
	    ++theNormalEvictionDirtyL1;
	  }

	  if(isDirtyL1 && isDirtyL2) ++theNormalEvictionDirtyBoth;

	  if(theL2U->didCleanEviction()) {
            theDidNormalEvict = true;
	    aNormalEvictAddress = theL2U->getCleanEvictionAddress();
	  }

        }
      }
    }

  }


  void CacheController::doMemory(CacheArray *aCache, MemoryMessage *aMemoryMessage) {

    #ifdef DEBUG
    cout << endl;
    #endif

    handlePrefetchAux(aCache, theL2U, aMemoryMessage);
  }




  void CacheController::doPrefetch(MemoryMessage *aMemoryMessage) {
    ++theNumProcessedPrefetches;

    //---------------------------------
    // If this is a hit, do nothing

    if(thePrefetchBuffer.isHit(aMemoryMessage->theMemoryAddress) || theL2U->isHit(aMemoryMessage->theMemoryAddress)) {
      return ;
    }

    //--------------------------------------------
    // Prefetch directly into the L2 cache

    if(aMemoryMessage->thePrefetchType == L2) {
      #ifdef ASSERT
      assert(!thePrefetchBuffer.isHit(aMemoryMessage->theMemoryAddress) && !theL2U->isHit(aMemoryMessage->theMemoryAddress));
      #endif

      theL2U->insertEntry(aMemoryMessage->theMemoryAddress, SPF, false, true);


      //-------------------------------------------------------------------------------------------------
      // These flags are used to track events when both the L1 and L2 have a dirty copy of the same line
      // and both are evicted at the same time

      bool didL2evict = false;
      bool didL1evict = false;

      while(theL2U->isBackSideOutReady()) {
	MemoryMessage *aMemoryMessage = theL2U->getBackSideOut(); // Dequeue out to main memory
        if(aMemoryMessage->theMemoryMessageType == EVICTDIRTY) {
          aPrefetchEvictAddress = aMemoryMessage->theMemoryAddress;
	  theDidPrefetchEvict = true;
	  didL2evict = true;
	  ++thePrefetchEvictionDirtyL2;
	}
      }

      checkProbes();

      if(theL1D->didForcedDirtyWriteback()) {
        aPrefetchEvictAddress = theL1D->getForcedDirtyWritebackAddress();
	theDidPrefetchEvict = true;
	didL1evict = true;
	++thePrefetchEvictionDirtyL1;
      }

      if(didL2evict && didL1evict) ++thePrefetchEvictionDirtyBoth;

      if(theL2U->didCleanEviction()) {
        aPrefetchEvictAddress = theL2U->getCleanEvictionAddress();
	theDidPrefetchEvict = true;
      }

      ++theNumFillsToL2;  // that are caused by direct prefetching
      return ;
    }

    //--------------------------------------------
    // Otherwise insert into an auxiliary structure

    else if(aMemoryMessage->thePrefetchType == AUX) {
      #ifdef ASSERT
      assert(!thePrefetchBuffer.isHit(aMemoryMessage->theMemoryAddress) && !theL2U->isHit(aMemoryMessage->theMemoryAddress));
      #endif

      #ifdef DEBUG_PF
      cout.flags(ios::hex);
      cout << "[CC] Inserting line into Prefetch Buffer: " << aMemoryMessage->theMemoryAddress << endl;
      #endif

      thePrefetchBuffer.insertBufferEntry(aMemoryMessage->theMemoryAddress);
      ++theNumFillsToPB;
      return ;
    }

    else assert(false);
  }


  void CacheController::resetAllStats() {
    theL1I->resetStats();
    theL1D->resetStats();
    theL2U->resetStats();

    theNumProcessedPrefetches = 0;
    theNumFillsToPB = 0;
    theNumFillsToL2 = 0;

    theReadBandwidth = 0;
    theWriteBandwidth = 0;
    theBandwidth = 0;

    theNormalEvictionDirtyL1 = 0;
    theNormalEvictionDirtyL2 = 0;
    theNormalEvictionDirtyBoth = 0;

    thePrefetchEvictionDirtyL1 = 0;
    thePrefetchEvictionDirtyL2 = 0;
    thePrefetchEvictionDirtyBoth = 0;
  }

  void CacheController::dumpAllStats() {
    theL1I->dumpStats();
    theL1D->dumpStats();
    theL2U->dumpStats();
    
    cout.flags(ios::dec);
    cout << "-------------------------------------------------------------------------------" << endl;
    if(theNumProcessedPrefetches > 0) cout << "[CC] Number of prefetches processed, searched in L2 and PB: " << theNumProcessedPrefetches << endl;
    if((theNumProcessedPrefetches - theNumFillsToPB) > 0) 
      cout << "[CC] Number of prefetches already in PB or L2: " << theNumProcessedPrefetches - theNumFillsToPB - theNumFillsToL2 << endl;
    cout << endl;



    //--------------------
    // Calculate bandwidth

    // Read bandwidth from memory

    theReadBandwidth += theNumFillsToPB;
    theReadBandwidth += theNumFillsToL2;
    theReadBandwidth += theL2U->theNumReadMisses;
    theReadBandwidth += theL2U->theNumWriteMisses;

    // Write bandwidth to memory

    theWriteBandwidth += theNormalEvictionDirtyL1;
    theWriteBandwidth += theNormalEvictionDirtyL2;
    theWriteBandwidth -= theNormalEvictionDirtyBoth;

    theWriteBandwidth += thePrefetchEvictionDirtyL1;
    theWriteBandwidth += thePrefetchEvictionDirtyL2;
    theWriteBandwidth -= thePrefetchEvictionDirtyBoth;

    theBandwidth = theReadBandwidth + theWriteBandwidth;

    if(theBandwidth > 0) {
      cout << "[CC] Read bandwidth: " << theReadBandwidth << endl;
      cout << endl;
      cout << "     Number of fills to PB: " << theNumFillsToPB << endl;
      cout << "     Number of direct prefetches to L2: " << theNumFillsToL2 << endl;
      cout << "     Number of normal read fills to L2: " << theL2U->theNumReadMisses << endl;
      cout << "     Number of normal write fills to L2: " << theL2U->theNumWriteMisses << endl;
      cout << endl << endl;

      cout << "[CC] Write bandwidth: " << theWriteBandwidth << endl;
      cout << endl;
      cout << "     Number of normal L1 dirty evictions: " << theNormalEvictionDirtyL1 << endl;
      cout << "     Number of normal L2 dirty evictions: " << theNormalEvictionDirtyL2 << endl;
      cout << "     Number of normal combined L1+L2 dirty evictions: " << theNormalEvictionDirtyBoth << endl;
      cout << endl;
      cout << "     Number of prefetch L1 dirty evictions: " << thePrefetchEvictionDirtyL1 << endl;
      cout << "     Number of prefetch L2 dirty evictions: " << thePrefetchEvictionDirtyL2 << endl;
      cout << "     Number of prefetch combined L1+L2 dirty evictions: " << thePrefetchEvictionDirtyBoth << endl;
      cout << endl << endl;

      cout << "[CC] Total bandwidth consumed for L2: " << theBandwidth << endl;
    }
  }


  //----------------------------------------------------
  // Internal event flags for an external prefetcher

  bool CacheController::isHitL1() {
    bool original = theIsHitL1;
    theIsHitL1 = false;
    return original;
  }

  bool CacheController::isHitL2() {
    bool original = theIsHitL2;
    theIsHitL2 = false;
    return original;
  }

  bool CacheController::isHitPrefetch() {
    bool original = theIsHitPrefetch;
    theIsHitPrefetch = false;
    return original;
  }

  int CacheController::getNumPrefetchQueueFree() {
    return thePrefetchQueueMaxSize - thePrefetchQueueSize;
  }

  bool CacheController::didNormalEvict() {
    #ifdef ASSERT
    if(theDidNormalEvict) assert(aNormalEvictAddress != 0);
    #endif
    return theDidNormalEvict;
  }

  bool CacheController::didPrefetchEvict() {
    #ifdef ASSERT
    if(theDidPrefetchEvict) assert(aPrefetchEvictAddress != 0);
    #endif
    return theDidPrefetchEvict;
  }

  MemoryAddress CacheController::getNormalEvictAddress() {
    #ifdef ASSERT
    assert(aNormalEvictAddress != 0);      
    assert(theDidNormalEvict);
    #endif
    MemoryAddress theAddress = aNormalEvictAddress;
    aNormalEvictAddress = 0;
    theDidNormalEvict = false;
    return theAddress;
  }

  MemoryAddress CacheController::getPrefetchEvictAddress() {
    #ifdef ASSERT
    assert(aPrefetchEvictAddress != 0);      
    assert(theDidPrefetchEvict);      
    #endif
    theDidPrefetchEvict = false;
    MemoryAddress theAddress = aPrefetchEvictAddress;
    aPrefetchEvictAddress = 0;
    return theAddress;
  }

}
