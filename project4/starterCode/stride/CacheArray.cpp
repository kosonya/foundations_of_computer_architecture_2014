///////////////////////////////////////////////////////////////////////
//
// CacheArray.cpp
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Cache array 
//
// Revision history
// 8-21-2005, Eric Chung
//
//

#include <iostream>
#include <stdlib.h>
#include <cassert>
#include <string>

#include "CacheArray.h"

namespace nCache {

  using namespace std;

  unsigned int log_base2(unsigned int num) {
    unsigned int i = 0;
    while(num > 1) {
      ++i;
      num >>= 1;
    }
    return i;
  }

  void CacheArray::dbg_PrintSource(Source aSource) {
    switch(aSource) {
      case(NIL):
        cout << "No source initialized" << endl;
	break;
      case(CPU):
        cout << "CPU" << endl;
        break;
      case(L1D):
        cout << "L1D" << endl;
        break;
      case(L1I):
        cout << "L1I" << endl;
	break;
      case(L2U):
        cout << "L2U" << endl;
	break;
      case(SPF):
        cout << "SPF" << endl;
	break;
      default:
        assert(false);
        break;
    }
  }

  void CacheArray::dbg_PrintMemoryMessage(MemoryMessage *aMemoryMessage) {
    cout.flags(ios::hex); 
    switch(aMemoryMessage->theMemoryMessageType) {
      case(PC):
        cout << "PC Fetch, A:[0x" << aMemoryMessage->theMemoryAddress << "]" << endl;
        break;
      case(LOAD):
        cout << "LOAD, A:[0x" << aMemoryMessage->theMemoryAddress << "]" << endl;
        break;
      case(STORE):
        cout << "STORE, A:[0x" << aMemoryMessage->theMemoryAddress << "]" << endl;
        break;
      case(EVICTDIRTY):
        cout << "EVICTDIRTY, A:[0x" << aMemoryMessage->theMemoryAddress << "]" << endl;
        break;
      case(PROBE):
        cout << "PROBE, A:[0x" << aMemoryMessage->theMemoryAddress << "]" << endl;
        break;
      case(PROBEREPLY):
        cout << "PROBEREPLY, A:[0x" << aMemoryMessage->theMemoryAddress << "]" << endl;
        break;
      case(PREFETCH):
        cout << "PREFETCH, A:[0x" << aMemoryMessage->theMemoryAddress << "]" << endl;
        break;
      default: 
        assert(false);
	break;
    }
  }

  int CacheSet::getTail() {
    return theMRUOrder[0];
  }

  MemoryAddress CacheSet::getMemoryAddress(int aBlock) {
    return theBlocks[aBlock].getMemoryAddress();
  }        

  void CacheSet::moveToHead(int aBlock) {

    int i = 0;
    #ifdef ASSERT
    assert((aBlock >= 0) && (aBlock < theAssociativity));
    #endif

    // Identify the block in the MRUOrder
    while(theMRUOrder[i] != aBlock) i++; 

    while(i < theAssociativity - 1) {
      theMRUOrder[i] = theMRUOrder[i+1];
      i++;
    }

    theMRUOrder[theAssociativity-1] = aBlock;

/*
    #ifdef DEBUG
    cout << "Moving block " << aBlock << " to head" << endl;
    cout << "New MRU order: " << endl;
    for(int i = theAssociativity-1; i >= 0; i--) {
      cout << "i: " << i << " " << theMRUOrder[i] << endl;
    }
    #endif
*/

  }


  void CacheSet::moveToTail(int aBlock) {
    int i = 0;
    #ifdef ASSERT
    assert((aBlock >= 0) && (aBlock < theAssociativity));
    #endif

/*
    #ifdef DEBUG
    cout << "Old MRU order: " << endl;
    for(int i = theAssociativity-1; i >= 0; i--) {
      cout << "i: " << i << " " << theMRUOrder[i] << endl;
    }
    #endif
*/

    // Identify the block in the MRUOrder
    while(theMRUOrder[i] != aBlock) i++; 

    while(i > 0) {
      theMRUOrder[i] = theMRUOrder[i-1];
      i--;
    }

    theMRUOrder[0] = aBlock;

/*
    #ifdef DEBUG
    cout << "Moving block " << aBlock << " to tail" << endl;
    cout << "New MRU order: " << endl;
    for(int i = theAssociativity-1; i >= 0; i--) {
      cout << "i: " << i << " " << theMRUOrder[i] << endl;
    }
    #endif
*/
  }

  CacheSet::CacheSet( const uint64_t aAssociativity ) {
    theAssociativity = aAssociativity;
    theBlocks = new CacheBlock[theAssociativity];
    theMRUOrder = new int[theAssociativity];

    for(int i = 0; i < theAssociativity; i++) 
      theMRUOrder[i] = i;
  }

  LookupResult CacheSet::lookupBlock( const Tag aTag ) {
    LookupResult aResult;
    aResult.isHit = false;
    for(int i = 0; i < theAssociativity; i++ ) {
      if(theBlocks[i].getTag() == aTag && theBlocks[i].getValidBit() == VALID) {
        aResult.isHit = true;
	aResult.theIndex = i;
	return aResult;
      }
    }
    return aResult;
  }

  DirtyBit CacheSet::getDirtyBit(int i) {
    return theBlocks[i].getDirtyBit();
  }

  ValidBit CacheSet::getValidBit(int i) {
    return theBlocks[i].getValidBit();       
  }

  void CacheSet::updateDirtyBit(DirtyBit aDirtyBit, int i) {
    theBlocks[i].updateDirtyBit(aDirtyBit);
  }

  void CacheSet::updateValidBit(ValidBit aValidBit, int i) {
    theBlocks[i].updateValidBit(aValidBit);
  }

  void CacheSet::updateTag(Tag aTag, MemoryAddress aMemoryAddress, int i) {
    theBlocks[i].updateTag(aTag);
    //This is purely for simulation purposes to simplify our lives
    theBlocks[i].updateMemoryAddress(aMemoryAddress);
  }

  void CacheSet::updateSource(Source aSource, int aBlock) {
    theBlocks[aBlock].updateSource(aSource);
  }
  
  Source CacheSet::getSource(int aBlock) {
    return theBlocks[aBlock].getSource();
  }


  //-----------------------------------------------------------------
  // The CacheArray constructor. Implements the cache array storage and
  // lookup functions.

  CacheArray::CacheArray ( uint64_t		aCacheSize,
   			   uint64_t		aAssociativity,
			   uint16_t		aBlockSize,
			   Source		aSource,
			   string		aCacheName ) {


    theCacheSize = aCacheSize;
    theAssociativity = aAssociativity;
    theBlockSize = aBlockSize;
    theCacheName = aCacheName;
    theSource = aSource;

    cout.flags(ios::dec); 

    cout << "-------------------------------------------------------------------------------" << endl;
    cout << "[" << theCacheName << "] Cache Size: " << (float)theCacheSize/1024 << "k" << endl;
    cout << "[" << theCacheName << "] Cache Associativity: " << theAssociativity << endl;
    cout << "[" << theCacheName << "] Cache Block Size: " << theBlockSize << endl;


    assert( aCacheSize > 0 );
    assert( aAssociativity > 0 );
    assert( aBlockSize > 0 );

    theSetCount = theCacheSize / theAssociativity / theBlockSize;
    assert(theSetCount > 0);

    #ifdef DEBUG
    cout.flags(ios::hex); 
    #endif

    //-------------------------------------------------------------
    // Compute Block Offset Bits and Mask

    theBlockOffsetNumBits = log_base2(theBlockSize);
    theBlockOffsetMask = (1LL << theBlockOffsetNumBits) - 1;

    #ifdef DEBUG
    cout << "[" << theCacheName << "] " << "Initializing BlockOffsetMask: " << theBlockOffsetMask << endl;
    #endif


    //-------------------------------------------------------------
    // Compute the Mask and Number of bits for Set Index 

    theSetIndexNumBits = log_base2(theSetCount);
    theSetIndexMask = ((1 << theSetIndexNumBits) - 1) << theBlockOffsetNumBits;

    #ifdef DEBUG
    cout << "[" << theCacheName << "] " << "Initializing SetMask: " << theSetIndexMask << endl;
    #endif

    //-------------------------------------------------------------
    // Compute the Tag Shift and the Tag Mask

    theTagShift = theBlockOffsetNumBits + theSetIndexNumBits;
    theTagMask = (-1LL) - ((1 << theTagShift) - 1); 

    #ifdef DEBUG
    cout << "[" << theCacheName << "] " << "Initializing TagMask: " << theTagMask << endl;
    #endif


    theSets = new CacheSet*[theSetCount];

    for(int i = 0; i < theSetCount; i++) 
      theSets[i] = new CacheSet(theAssociativity);


    //-------------------------------------------------------------
    // Initialize statistic counters

    resetStats();


    //-------------------------------------------------------------
    // Internal information exposed to the outside world

    resetInternalFlags();

  };

  // Die, die, so ugly that we're exposing internal state to the outside world
  // Stop exposing yourself

  void CacheArray::resetInternalFlags() {
    forcedDirtyWritebackAddress = 0;
    theDidForcedDirtyWriteback = false;

    cleanEvictionAddress = 0;
    theDidCleanEviction = false;
  }


  Set CacheArray::getSetIndex(MemoryAddress aMemoryAddress) {
    return (theSetIndexMask & aMemoryAddress) >> theBlockOffsetNumBits;
  }

  Tag CacheArray::getTag(MemoryAddress aMemoryAddress) {
    return (theTagMask & aMemoryAddress) >> (theBlockOffsetNumBits + theSetIndexNumBits);
  }



  void CacheArray::processFrontSideIn(MemoryMessage *aMemoryMessage) {

    //resetInternalFlags();

    MemoryAddress aMemoryAddress = aMemoryMessage->theMemoryAddress;
    if(aMemoryMessage->theMemoryMessageType == PC) aMemoryMessage->theMemoryMessageType = LOAD;
    MemoryMessageType aMemoryMessageType = aMemoryMessage->theMemoryMessageType;
    Set aSetIndex = getSetIndex(aMemoryAddress);
    Tag aTag = getTag(aMemoryAddress);


    #ifdef DEBUG
    cout.flags(ios::hex); 
    cout << "[" << theCacheName << "] " << "Processing on FrontSideIn: "; dbg_PrintMemoryMessage(aMemoryMessage);
    cout << "[" << theCacheName << "] " << "Cache Set Index: " << aSetIndex << endl;
    cout << "[" << theCacheName << "] " << "Cache Tag: " << aTag << endl;
    #endif

    #ifdef ASSERT
    assert(aMemoryMessageType == LOAD || aMemoryMessageType == STORE || aMemoryMessageType == EVICTDIRTY);
    #endif


    //-------------------------------------
    // Handle Loads and Stores

    if(aMemoryMessageType == LOAD || aMemoryMessageType == STORE) {

      if(aMemoryMessageType == LOAD) theNumReads++;
      if(aMemoryMessageType == STORE) theNumWrites++;

      LookupResult aResult = theSets[aSetIndex]->lookupBlock(aTag);

      //---------------------------------------------------
      // Cache Hits

      if(aResult.isHit) {
        theSets[aSetIndex]->moveToHead(aResult.theIndex);
	if((aMemoryMessageType == STORE) && (theSource == L1D)) theSets[aSetIndex]->updateDirtyBit(DIRTY, aResult.theIndex);

        #ifdef DEBUG
        cout << "[" << theCacheName << "] " << "Cache hit!" << endl;
	cout << "[" << theCacheName << "] " << "Block index: " << aResult.theIndex << endl; 
        #endif

	return ;
      } 
      
      //---------------------------------------------------
      // Cache Misses 

      else {
        #ifdef ASSERT
        assert(theBackSideOutputQueue.empty()); // We shouldn't be processing this message until all backside out messages have been processed
        #endif

        Source theMessageOwner = aMemoryMessage->theSource;

	#ifdef DEBUG
        cout << "[" << theCacheName << "] " << "Cache miss! Sending request to lower-level cache" << endl;
	cout << "[" << theCacheName << "] " << "Message owner: "; 
	dbg_PrintSource(theMessageOwner);
        #endif

        if(aMemoryMessageType == LOAD) theNumReadMisses++;
        else theNumWriteMisses++;

        // Notifies lower-level cache who sent this message since a probe can be caused by lower level miss 
        aMemoryMessage->theSource = theSource;     
        int victimBlock = theSets[aSetIndex]->getTail();

        //----------------------------------------------------------------
        // Process the victim block

        bool isPrefetch = false;
        if(theSets[aSetIndex]->getValidBit(victimBlock) == VALID) handleVictimBlock(victimBlock, aSetIndex, isPrefetch);

        //------------------------------------------------------------------------
        // Send the miss request to lower cache level after an eviction request

        theBackSideOutputQueue.push_back(aMemoryMessage);

	if((aMemoryMessageType == STORE) && (theSource == L2U)) {
          theSets[aSetIndex]->updateDirtyBit(NOTDIRTY, victimBlock);
	}

        if((aMemoryMessageType == STORE) && (theSource == L1D)) {
	  theSets[aSetIndex]->updateDirtyBit(DIRTY, victimBlock);
          #ifdef DEBUG
	  cout << "[" << theCacheName << "] Setting dirty bit" << endl;
          #endif
	}

	if(aMemoryMessageType == LOAD) {
	  theSets[aSetIndex]->updateDirtyBit(NOTDIRTY, victimBlock);
          #ifdef DEBUG
	  cout << "[" << theCacheName << "] Unsetting dirty bit" << endl;
          #endif
	}

	theSets[aSetIndex]->updateValidBit(VALID, victimBlock);
	theSets[aSetIndex]->updateSource(theMessageOwner, victimBlock);
        #ifdef DEBUG
	cout << "[" << theCacheName << "] Setting Valid bit" << endl;
	cout << "[" << theCacheName << "] Updating source for addr: " << aMemoryAddress << " to ";
	dbg_PrintSource(theMessageOwner);
        #endif
        theSets[aSetIndex]->moveToHead(victimBlock);
        theSets[aSetIndex]->updateTag(aTag, aMemoryAddress, victimBlock);

        return ;
      }
    }

    //---------------------------------------------------------------------------------------
    // Process Evict Dirty Requests from caches above

    if(aMemoryMessageType == EVICTDIRTY) {
      LookupResult aResult = theSets[aSetIndex]->lookupBlock(aTag);

      // highest level of the cache hierarchy should not be processing any eviction messages     
      #ifdef ASSERT
      assert(aResult.isHit); // This line better exist here or else our inclusion property is violated
      #endif

      #ifdef DEBUG
      if(!aResult.isHit) 
        cout << "[" << theCacheName << "] " << "Dirty eviction received has no match, must have been evicted due to miss from above" << endl;
      #endif
      if(aResult.isHit) {
        #ifdef ASSERT
        assert(theSets[aSetIndex]->getValidBit(aResult.theIndex) == VALID);
	#endif
        if(theSets[aSetIndex]->getDirtyBit(aResult.theIndex) == DIRTY) {
          theNumDirtyEvictionsReceivedDropped++;
	  #ifdef DEBUG
          cout << "[" << theCacheName << "] Dirty eviction was dropped" << endl;
	  #endif
	} else {
          theNumDirtyEvictionsReceivedProcessed++;
	  #ifdef DEBUG
          cout << "[" << theCacheName << "] Dirty eviction was processed" << endl;
	  #endif
	}

        theSets[aSetIndex]->updateDirtyBit(DIRTY, aResult.theIndex);

        #ifdef DEBUG
        cout << "[" << theCacheName << "] " << "Evict Dirty received, updating line state to DIRTY";
        dbg_PrintMemoryMessage(aMemoryMessage);
        #endif
      }

      delete aMemoryMessage;
      return ;
    }


    #ifdef ASSERT
    assert(false); // Shouldn't reach this point
    #endif
  }


  void CacheArray::handleVictimBlock(int victimBlock, Set aSetIndex, bool isPrefetch) {

    #ifdef ASSERT
    assert(theSets[aSetIndex]->getValidBit(victimBlock) == true);
    #endif

    #ifdef DEBUG
    cout << "[" << theCacheName << "] " << "Replacing victim block addr: " << theSets[aSetIndex]->getMemoryAddress(victimBlock) << " that belongs to ";
    dbg_PrintSource(theSets[aSetIndex]->getSource(victimBlock));
    cout << "[" << theCacheName << "] " << "Replacing victim block addr (tag+idx): " << (theSets[aSetIndex]->getMemoryAddress(victimBlock) >> (theBlockOffsetNumBits)) << endl;
    #endif

    if(theSets[aSetIndex]->getDirtyBit(victimBlock) == DIRTY) {

      #ifdef DEBUG
      cout << "[" << theCacheName << "] " << "Victim block is dirty, sending to lower-level cache" << endl;
      #endif

      MemoryMessage *aEviction = new MemoryMessage;
      aEviction->theMemoryMessageType = EVICTDIRTY;
      aEviction->theMemoryAddress = theSets[aSetIndex]->getMemoryAddress(victimBlock);
      aEviction->theSource = theSource;
      theBackSideOutputQueue.push_back(aEviction);

      theNumDirtyWritebacks++;
    } else {
      theNumCleanEvictions++;
      #ifdef DEBUG
      cout << "[" << theCacheName << "] " << "Clean eviction of victim block" << endl;
      #endif

      theDidCleanEviction = true;
      cleanEvictionAddress = theSets[aSetIndex]->getMemoryAddress(victimBlock);
    }

    //-----------------------------------------------------------------
    // If I am L2 send a probe upwards due to inclusion property

    if(theSource == L2U) {
      MemoryMessage *aProbe = new MemoryMessage;
      aProbe->theMemoryMessageType = PROBE;
      aProbe->theMemoryAddress = theSets[aSetIndex]->getMemoryAddress(victimBlock);

      #ifdef ENABLE_PF
      // If this probe was caused by a prefetch, notify the Cache Controller
      if(isPrefetch) {
	aProbe->theSource = SPF;
        #ifdef DEBUG_PF
	cout << "[" << theCacheName << "] Probe source is set to SPF" << endl;
        #endif
      }
      else aProbe->theSource = theSets[aSetIndex]->getSource(victimBlock);
      #else
      aProbe->theSource = theSets[aSetIndex]->getSource(victimBlock);
      #endif

      #ifdef DEBUG
      cout << "[" << theCacheName << "] Sending probe on addr: " << aProbe->theMemoryAddress << " to "; 
      dbg_PrintSource(aProbe->theSource);
      #endif
      theFrontSideOutputQueue.push_back(aProbe);
      theNumProbesSent++;
    }
  }




  //-------------------------------------------------------------------------
  // Process probe (search cache for any matching lines and evict)

  void CacheArray::processBackSideIn(MemoryMessage *aMemoryMessage) {
    MemoryMessageType aMemoryMessageType = aMemoryMessage->theMemoryMessageType;

    #ifdef ASSERT
    assert(aMemoryMessageType == PROBE);
    #endif

    #ifdef DEBUG
    cout << "[" << theCacheName << "] " << "Processing on BackSideIn: ";
    dbg_PrintMemoryMessage(aMemoryMessage);
    #endif

    if(aMemoryMessageType == PROBE) {
      processProbe(aMemoryMessage);
      return ;
    }

    assert(false);
  }



  void CacheArray::processProbe(MemoryMessage *aProbe) {
    MemoryAddress aMemoryAddress = aProbe->theMemoryAddress;
    MemoryMessageType aMemoryMessageType = aProbe->theMemoryMessageType;

    #ifdef ASSERT
    assert(aMemoryMessageType == PROBE);
    #endif

    Set aSetIndex = getSetIndex(aMemoryAddress);
    Tag aTag = getTag(aMemoryAddress);
    LookupResult aResult = theSets[aSetIndex]->lookupBlock(aTag);

    theNumProbesReceived++;

    if(aResult.isHit) {

      //-------------------------------------------------------------------------
      // If line is Dirty, count as a forced dirty eviction

      if(theSets[aSetIndex]->getDirtyBit(aResult.theIndex) == DIRTY) {
        theNumForcedDirtyEvictions++;

        #ifdef ENABLE_PF
        if(aProbe->theSource == SPF) theNumForcedDirtyEvictionsByPrefetch++;
        #endif

        #ifdef DEBUG_PF
	if(aProbe->theSource == SPF) cout << "[" << theCacheName << "] " << "... was caused by a prefetch to lower cache" << endl;
        #endif

        #ifdef DEBUG
        cout << "[" << theCacheName << "] " << "Forced dirty writeback" << endl;
	cout << "[" << theCacheName << "] " << "Tag addr: " << aTag << endl;
        #endif

        theDidForcedDirtyWriteback = true;
        forcedDirtyWritebackAddress = aMemoryAddress;
      } 
      #ifdef DEBUG
      else {
        cout << "[" << theCacheName << "] " << "Forced clean eviction" << endl;
	cout << "[" << theCacheName << "] " << "Tag addr: " << aTag << endl;
        #ifdef ENABLE_PF
        if(aProbe->theSource == SPF) theNumForcedCleanEvictionsByPrefetch++;
        #endif

        #ifdef DEBUG_PF
	if(aProbe->theSource == SPF) cout << "[" << theCacheName << "] " << "... was caused by prefetch due to lower cache" << endl;
        #endif
      }
      #endif

      //-----------------------------------------------------------------------
      // Invalidate the line

      #ifdef DEBUG
      cout << "[" << theCacheName << "] " << "Invalidating cache line and moving to tail on MRU List" << endl;
      #endif

      theNumForcedEvictions++;
      theSets[aSetIndex]->updateValidBit(INVALID, aResult.theIndex);
      theSets[aSetIndex]->updateSource(NIL, aResult.theIndex);
      theSets[aSetIndex]->moveToTail(aResult.theIndex);
    }
  }

  MemoryMessage* CacheArray::getBackSideOut() {
    #ifdef ASSERT
    assert(!theBackSideOutputQueue.empty());
    #endif
    MemoryMessage *aMemoryMessage = theBackSideOutputQueue.front();
    theBackSideOutputQueue.pop_front();
    return aMemoryMessage;
  }

  MemoryMessage* CacheArray::getFrontSideOut() {
    #ifdef ASSERT
    assert(!theFrontSideOutputQueue.empty());
    #endif
    MemoryMessage *aMemoryMessage = theFrontSideOutputQueue.front();
    theFrontSideOutputQueue.pop_front();
    return aMemoryMessage;
  }

  bool CacheArray::isFrontSideOutReady() {
    return !theFrontSideOutputQueue.empty();
  }

  bool CacheArray::isBackSideOutReady() {
    return !theBackSideOutputQueue.empty();
  }

  //---------------------------------------------------------------------------------------------------------------
  // Presence check for any address inside this cache array
  // Side-effects: none

  bool CacheArray::isHit(MemoryAddress aMemoryAddress) {
    Set aSetIndex = getSetIndex(aMemoryAddress);
    Tag aTag = getTag(aMemoryAddress);
    LookupResult aResult = theSets[aSetIndex]->lookupBlock(aTag);
    #ifdef DEBUG
    if(aResult.isHit) cout << "[" << theCacheName << "] Found hit in cache for: " << aMemoryAddress << endl;
    else cout << "[" << theCacheName << "] Did not hit for: " << aMemoryAddress << endl;
    cout << "[" << theCacheName << "] Tag: " << aTag << " Set: " << aSetIndex << endl;
    #endif
    return aResult.isHit;
  }

  //---------------------------------------------------------------------------------------------------------------
  // Low-level function for directly inserting cache entries 
  // Side-effects: may generate an eviction out of the back-side of the cache

  void CacheArray::insertEntry(MemoryAddress aMemoryAddress, Source aSource, bool isDirty, bool isPrefetch) {
    #ifdef ASSERT
    assert(!isHit(aMemoryAddress));
    #endif


    #ifdef DEBUG
    cout << "[" << theCacheName << "] Inserting entry: " << aMemoryAddress << endl;
    #endif

    Set aSetIndex = getSetIndex(aMemoryAddress);
    Tag aTag = getTag(aMemoryAddress);
    int victimBlock = theSets[aSetIndex]->getTail();

    //---------------------------------------------------------------------
    // Generate an eviction out of the back end of the cache if necessary

    if(theSets[aSetIndex]->getValidBit(victimBlock) == VALID) handleVictimBlock(victimBlock, aSetIndex, isPrefetch);

    //---------------------------------------------------------------------
    // Update the state of the cache

    theSets[aSetIndex]->updateDirtyBit(isDirty ? DIRTY : NOTDIRTY, victimBlock);
    theSets[aSetIndex]->updateValidBit(VALID, victimBlock);
    theSets[aSetIndex]->updateTag(aTag, aMemoryAddress, victimBlock);
    theSets[aSetIndex]->updateSource(aSource, victimBlock);
    theSets[aSetIndex]->moveToHead(victimBlock);
  }


  //-----------------------------------------------------
  // Methods for exposing internal events

  MemoryAddress CacheArray::getForcedDirtyWritebackAddress() {
    #ifdef ASSERT
    assert(theDidForcedDirtyWriteback);
    assert(forcedDirtyWritebackAddress != 0);
    #endif

    theDidForcedDirtyWriteback = false;
    MemoryAddress theAddress = forcedDirtyWritebackAddress;
    forcedDirtyWritebackAddress = 0;
    return theAddress;
  }


  bool CacheArray::didForcedDirtyWriteback() {
    #ifdef ASSERT
    if(theDidForcedDirtyWriteback) assert(forcedDirtyWritebackAddress != 0);
    #endif
    return theDidForcedDirtyWriteback;
  }


  MemoryAddress CacheArray::getCleanEvictionAddress() {
    #ifdef ASSERT
    assert(theDidCleanEviction);
    assert(cleanEvictionAddress != 0);
    #endif

    theDidCleanEviction = false;
    MemoryAddress theAddress = cleanEvictionAddress;
    cleanEvictionAddress = 0;
    return theAddress;
  }


  bool CacheArray::didCleanEviction() {
    #ifdef ASSERT
    if(theDidCleanEviction) assert(cleanEvictionAddress != 0);
    #endif
    return theDidCleanEviction;
  }



  void CacheArray::resetStats() {
    theNumReads = 0;
    theNumReadMisses = 0;
    theNumWrites = 0;
    theNumWriteMisses = 0;

    theNumCleanEvictions = 0;
    theNumDirtyWritebacks = 0;

    theNumProbesSent = 0;
    theNumProbesReceived = 0;
    theNumForcedEvictions = 0;
    theNumForcedCleanEvictions = 0;
    theNumForcedDirtyEvictions = 0;

    theNumDirtyEvictionsReceivedProcessed = 0;
    theNumDirtyEvictionsReceivedDropped = 0;

    #ifdef ENABLE_PF
    theNumForcedCleanEvictionsByPrefetch = 0;
    theNumForcedDirtyEvictionsByPrefetch = 0;
    #endif
  }

  void CacheArray::dumpStats() {
    cout.flags(ios::dec); 

    cout << "-------------------------------------------------------------------------------" << endl;
    cout << "[" << theCacheName << "] Accesses: " << theNumReads + theNumWrites << endl;
    cout << "[" << theCacheName << "] Misses: " << theNumReadMisses + theNumWriteMisses << endl;
    cout << endl;
    cout << "[" << theCacheName << "] Reads: " << theNumReads << endl;
    cout << "[" << theCacheName << "] Read misses: " << theNumReadMisses << endl;
    if(theNumReads != 0) cout << "[" << theCacheName << "] Read miss rate: " << (float)theNumReadMisses/(float)theNumReads << endl;
    cout << "[" << theCacheName << "] Writes: " << theNumWrites << endl;
    cout << "[" << theCacheName << "] Write misses: " << theNumWriteMisses << endl;
    if(theNumWrites != 0) cout << "[" << theCacheName << "] Write miss rate: " << (float)theNumWriteMisses/(float)theNumWrites << endl;
    cout << endl;
    cout << "[" << theCacheName << "] Miss rate: " << (float)(theNumReadMisses+theNumWriteMisses)/(float)(theNumReads + theNumWrites);
    cout << endl;

    theNumForcedCleanEvictions = theNumForcedEvictions - theNumForcedDirtyEvictions;

    cout << "[" << theCacheName << "] Clean evictions: " << theNumCleanEvictions << endl;
    cout << "[" << theCacheName << "] Dirty writebacks: " << theNumDirtyWritebacks << endl;
    cout << endl;
    if(theNumProbesSent != 0) cout << "[" << theCacheName << "] Probes sent: " << theNumProbesSent << endl;
    if(theNumProbesReceived != 0) cout << "[" << theCacheName << "] Probes received: " << theNumProbesReceived << endl;
    if(theNumForcedCleanEvictions != 0) cout << "[" << theCacheName << "] Forced clean evictions: " << theNumForcedCleanEvictions << endl;
    if(theNumForcedDirtyEvictions != 0) cout << "[" << theCacheName << "] Forced dirty evictions: " << theNumForcedDirtyEvictions << endl;

    if(theNumDirtyEvictionsReceivedProcessed != 0) cout << "[" << theCacheName << "] Dirty evictions processed: " << theNumDirtyEvictionsReceivedProcessed << endl;
    if(theNumDirtyEvictionsReceivedDropped != 0) cout << "[" << theCacheName << "] Dirty evictions dropped: " << theNumDirtyEvictionsReceivedDropped << endl;


    #ifdef ENABLE_PF
    if(theNumForcedCleanEvictionsByPrefetch != 0) 
      cout << "[" << theCacheName << "] Forced clean evictions due to prefetch: " << theNumForcedCleanEvictionsByPrefetch << endl;
    if(theNumForcedDirtyEvictionsByPrefetch != 0)
      cout << "[" << theCacheName << "] Forced dirty evictions due to prefetch: " << theNumForcedDirtyEvictionsByPrefetch << endl;
    #endif

    // Need to compute the CHECKSUM
    uint64_t theCheckSum = 0;
    for(int i = 0 ; i < theSetCount; i++) {
      for(int j = 0; j < theAssociativity; j++) {
	ValidBit theValidBit = theSets[i]->getValidBit(j);

	if(theValidBit == VALID) {
	  DirtyBit theDirtyBit = theSets[i]->getDirtyBit(j);
	  Tag theTag = getTag(theSets[i]->getMemoryAddress(j));
	  theCheckSum = theCheckSum ^ ((theTag << 2) | (theValidBit << 1) | (theDirtyBit));
	}
      }
    }

    cout.flags(ios::hex); 
    cout << "[" << theCacheName << "] Checksum: " << uppercase << theCheckSum << endl;
  }

}
