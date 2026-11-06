 /*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _CPS_V3_H_
#define _CPS_V3_H_

#include "cdn_stdtypes.h"

/****************************************************************************
 * Types
 ***************************************************************************/

/** A lock handle */
typedef void* CPS_LockHandle;

/****************************************************************************
 * Prototypes
 ***************************************************************************/

/**
 * Check that sufficient locks are available
 * @param[in] lockCount number of locks requested
 * @return 0 on success (locks available)
 * @return ENOENT if insufficient locks are available
 */
extern uint32_t CPS_ProbeLocks(uint32_t lockCount);

/**
 * Initialize a lock
 * @param[out] lock where to store the allocated, initialized lock
 * @return 0 on success (lock is allocated and initialized)
 * @return ENOENT if insufficient locks are available
 */
extern uint32_t CPS_InitLock(CPS_LockHandle* lock);

/**
 * Free a lock
 * @param[in] lock the lock
 */
extern void CPS_FreeLock(CPS_LockHandle lock);

/**
 * Lock a lock, pending the current thread/task if necessary until the lock is available
 * @param[in] lock the lock
 */
extern uint32_t CPS_Lock(CPS_LockHandle lock);

/**
 * Unlock a lock, readying the next highest-priority thread/task pended on it if any
 * @param[in] lock the lock
 */
extern uint32_t CPS_Unlock(CPS_LockHandle lock);

/**
 * Read a byte, bypassing the cache
 * @param[in] address the address
 * @return the byte at the given address
 */
extern uint8_t CPS_UncachedRead8(volatile uint8_t* address);

/**
 * Read a short, bypassing the cache
 * @param[in] address the address
 * @return the short at the given address
 */
extern uint16_t CPS_UncachedRead16(volatile uint16_t* address);

/**
 * Read a (32-bit) word, bypassing the cache
 * @param[in] address the address
 * @return the word at the given address
 */
//extern uint32_t CPS_UncachedRead32(volatile uint32_t* address);

/**
 * Write a byte to memory, bypassing the cache
 * @param[in] address the address
 * @param[in] value the byte to write
 */
extern void CPS_UncachedWrite8(volatile uint8_t* address, uint8_t value);

/**
 * Write a short to memory, bypassing the cache
 * @param[in] address the address
 * @param[in] value the short to write
 */
extern void CPS_UncachedWrite16(volatile uint16_t* address, uint16_t value);

/**
 * Write a (32-bit) word to memory, bypassing the cache
 * @param[in] address the address
 * @param[in] value the word to write
 */
//extern void CPS_UncachedWrite32(volatile uint32_t* address, uint32_t value);

/**
 * Write a (32-bit) address value to memory, bypassing the cache.
 * This function is for writing an address value, i.e. something that
 * will be treated as an address by hardware, and therefore might need
 * to be translated to a physical bus address.
 * @param[in] location the (CPU) location where to write the address value
 * @param[in] addrValue the address value to write
 */
extern void CPS_WritePhysAddress32(volatile uint32_t* location, uint32_t addrValue);

/**
 * Hardware specific memcpy.
 * @param[in] src  src address
 * @param[in] dst  destination address
 * @param[in] size size of the copy
 */
extern void CPS_BufferCopy(volatile uint8_t *dst, volatile uint8_t *src, uint32_t size);

/**
* Invalidate the cache for the specified memory region.
* This function may be stubbed out if caching is disabled for memory regions
* as described in the driver documentation, or if the driver configuration does
* not require this function.
* @param[in] address Virtual address of memory region. (If an MMU is not in use,
* this will be equivalent to the physical address.) This address should be
* rounded down to the nearest cache line boundary.
* @param[in] size  size of memory in bytes.  This size should be rounded up to
* the nearest cache line boundary.  Use size UINTPTR_MAX to invalidate all
* memory cache.  A size of 0 should be ignored and the function should return
* immediately with no effect.
* @param[in] devInfo   This parameter can be used to pass implementation specific
* data to this function.  The content and use of this parameter is up to the
* implementor of this function to determine, and if not required it may be ignored.
*  For example, under Linux it can be used to pass a pointer to
* the device struct to be used in a call to dma_sync_single_for_device().  If
* used, the parameter should be passed to the core driver at initialisation as
* part of the configurationInfo struct.  Please
* see the core driver documentation for details of how to do this.
*/
extern void CPS_CacheInvalidate(uintptr_t address, size_t size, uintptr_t devInfo);

/**
* Flush the cache for the specified memory region
* This function may be stubbed out if caching is disabled for memory regions
* as described in the driver documentation, or if the driver configuration does
* not require this function.
* @param[in] address Virtual address of memory region. (If an MMU is not in use,
* this will be equivalent to the physical address.) This address should be
* rounded down to the nearest cache line boundary.
* @param[in] size  size of memory in bytes.  This size should be rounded up to
* the nearest cache line boundary.  Use size UINTPTR_MAX to flush all
* memory cache.  A size of 0 should be ignored and the function should return
* immediately with no effect.
* @param[in] devInfo   This parameter can be used to pass implementation specific
* data to this function.  The content and use of this parameter is up to the
* implementor of this function to determine, and if not required it may be ignored.
*  For example, under Linux it can be used to pass a pointer to
* the device struct to be used in a call to dma_sync_single_for_device().  If
* used, the parameter should be passed to the core driver at initialisation as
* part of the configurationInfo struct.  Please
* see the core driver documentation for details of how to do this.
*/
extern void CPS_CacheFlush(uintptr_t address, size_t size, uintptr_t devInfo);

#endif /* multiple inclusion protection */
