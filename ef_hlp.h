
/*
 * Electric Fence - Red-Zone memory allocator.
 * Copyright (C) 2002-2005 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
 * License: GNU LGPL (GNU Lesser General Public License, see COPYING-GPL)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * FILE CONTENTS:
 * internal implementation file
 * contains helper functions for electric fence
 */


/*
 * delete reductionSizekB amount of memory, which has already
 * been freed but got protected
 */
static void
reduceProtectedMemory( long reductionSizekB )
{
  struct _EF_Slot * slot            = _ef_allocList;
  size_t            count           = slotCount;
  long              alreadyReducekB = 0;
  size_t            delSize, newSize;

  /* 1- try reducing memory to just keep page(s) with userAddress */
  for ( ; count > 0  &&  alreadyReducekB < reductionSizekB; --count, ++slot )
    if ( EFST_PROTECTED == slot->state )
    {
      /* free memory above userAddr; keep userAddr protected  */
      newSize = (char*)slot->userAddress - (char*)slot->internalAddress;
      newSize = (newSize + EF_PAGE_SIZE) & ~(EF_PAGE_SIZE -1);
      delSize = slot->internalSize - newSize;
      Page_Delete( (char*)slot->internalAddress + newSize, delSize );
      alreadyReducekB += (delSize+1023) >>10;
      /* but keep the slot and userAddr */
      slot->internalSize    = newSize;

      if ( alreadyReducekB >= reductionSizekB )
      {
        sumProtectedMem -= alreadyReducekB;
        sumAllocatedMem -= alreadyReducekB;
        return;
      }
    }
  /* 2- deallocated all page(s) with userAddress, empty whole slot */
  slot  = _ef_allocList;
  count = slotCount;
  for ( ; count > 0  &&  alreadyReducekB < reductionSizekB; --count, ++slot )
    if ( EFST_DEALLOCATED == slot->state )
    {
      /* free all the memory */
      Page_Delete(slot->internalAddress, slot->internalSize);
      alreadyReducekB += (slot->internalSize+1023) >>10;
      /* but keep the slot and userAddr */
      slot->internalAddress = slot->userAddress = 0;
      slot->internalSize    = slot->userSize    = 0;
      slot->state           = EFST_EMPTY;
      slot->allocator       = EFA_INT_ALLOC;
      #ifndef EF_NO_LEAKDETECTION
      #ifdef EF_USE_FRAMENO
        slot->frame         = 0;
      #endif
        slot->filename      = 0;
        slot->lineno        = 0;
      #endif

      if ( alreadyReducekB >= reductionSizekB )
      {
        sumProtectedMem -= alreadyReducekB;
        sumAllocatedMem -= alreadyReducekB;
        return;
      }
    }
}


/*
 * Find the slot structure for a user address.
 */
static struct _EF_Slot *
slotForUserAddress(void * address)
{
  struct _EF_Slot * slot  = _ef_allocList;
  size_t            count = slotCount;

  for ( ; count > 0; --count, ++slot )
    if ( slot->userAddress == address )
      return slot;
  return 0;
}


/*
 * Find the nearest slot structure for a user address.
 */
static struct _EF_Slot *
nearestSlotForUserAddress(void * userAddress)
{
  struct _EF_Slot * slot  = _ef_allocList;
  size_t            count = slotCount;

  for ( ; count > 0; --count, ++slot )
    if (   (char*)slot->internalAddress <= (char*)userAddress
        && (char*)userAddress           <= (char*)slot->internalAddress + slot->internalSize
       )
      return slot;
  return 0;
}


/*
 * Find the slot structure for an internal address.
 */
static struct _EF_Slot *
slotForInternalAddrNextTo(void * address)
{
  struct _EF_Slot * slot  = _ef_allocList;
  size_t            count = slotCount;

  for ( ; count > 0; --count, ++slot )
    if ( slot->internalAddress == address )
      return slot;
  return 0;
}


/*
 * Given the internal address of a buffer, find the buffer immediately
 * before that buffer in the address space. This is used by free() to
 * coalesce two free buffers into one.
 */
static struct _EF_Slot *
slotForInternalAddrPrevTo(void * address)
{
  struct _EF_Slot * slot  = _ef_allocList;
  size_t            count = slotCount;

  for ( ; count > 0; --count, ++slot )
    if ( (char*)slot->internalAddress + slot->internalSize == address )
      return slot;
  return 0;
}


/*
 * Initialise the no mans land, for a given slot
 */
static
void _eff_init_slack( struct _EF_Slot * slot )
{
  char * accBegAddr, * accEndAddr;
  char * tmpBegAddr, * tmpEndAddr;

  /* calculate accessible non-protectable address area */
  /* check the no man's land; use internal knowledge to detect the EF_PROTECT_BELOW on allocation */
  if ( (char*)slot->internalAddress + EF_PAGE_SIZE == (char*)slot->userAddress )
  {
    /* EF_PROTECT_BELOW was 1 when allocating this piece of memory */
    accBegAddr = (char*)slot->internalAddress + EF_PAGE_SIZE;
    accEndAddr = (char*)slot->internalAddress + slot->internalSize;
  }
  else
  {
    /* EF_PROTECT_BELOW was 0 when allocating this piece of memory */
    accBegAddr = (char*)slot->internalAddress;
    accEndAddr = (char*)slot->internalAddress + slot->internalSize - EF_PAGE_SIZE;
  }

  tmpBegAddr = accBegAddr;
  tmpEndAddr = (char*)slot->userAddress;
  while (tmpBegAddr < tmpEndAddr)
    *tmpBegAddr++ = (char)EF_SLACKFILL;

  tmpBegAddr = (char*)slot->userAddress + slot->userSize;
  tmpEndAddr = accEndAddr;
  while (tmpBegAddr < tmpEndAddr)
    *tmpBegAddr++ = (char)EF_SLACKFILL;
}


/*
 * Checks the integrity of no mans land, for a given slot
 */
static
void _eff_check_slack( struct _EF_Slot * slot )
{
  char    * accBegAddr, * accEndAddr;
  char    * tmpBegAddr, * tmpEndAddr;

  /* calculate accessible non-protectable address area */
  /* check the no man's land; use internal knowledge to detect the EF_PROTECT_BELOW on allocation */
  if ( (char*)slot->internalAddress + EF_PAGE_SIZE == (char*)slot->userAddress )
  {
    /* EF_PROTECT_BELOW was 1 when allocating this piece of memory */
    accBegAddr = (char*)slot->internalAddress + EF_PAGE_SIZE;
    accEndAddr = (char*)slot->internalAddress + slot->internalSize;
  }
  else
  {
    /* EF_PROTECT_BELOW was 0 when allocating this piece of memory */
    accBegAddr = (char*)slot->internalAddress;
    accEndAddr = (char*)slot->internalAddress + slot->internalSize - EF_PAGE_SIZE;
  }

  tmpBegAddr = accBegAddr;
  tmpEndAddr = (char*)slot->userAddress;
  while (tmpBegAddr < tmpEndAddr)
  {
    if ( (char)EF_SLACKFILL != *tmpBegAddr++ )
    {
      #ifndef EF_NO_LEAKDETECTION
        EF_Abort("\nElectric Fence: ptr=%a: free() detected overwrite of ptrs no mans land, size=%d alloced from %s(%d)",
          slot->userAddress, (int)slot->userSize, slot->filename, slot->lineno);
      #else
        EF_Abort("\nElectric Fence: ptr=%a: free() detected overwrite of ptrs no mans land", slot->userAddress);
      #endif
    }
  }

  tmpBegAddr = (char*)slot->userAddress + slot->userSize;
  tmpEndAddr = accEndAddr;
  while (tmpBegAddr < tmpEndAddr)
  {
    if ( (char)EF_SLACKFILL != *tmpBegAddr++ )
    {
      #ifndef EF_NO_LEAKDETECTION
        EF_Abort("\nfree() detected overwrite of no mans land: ptr=%a, size=%d\nalloced from %s(%d)",
          slot->userAddress, (int)slot->userSize, slot->filename, slot->lineno);
      #else
        EF_Abort("\nfree() detected overwrite of no mans land: ptr=%a", slot->userAddress);
      #endif
    }
  }
}

