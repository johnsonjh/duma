
/*
 * Electric Fence - Red-Zone memory allocator.
 * Copyright (C) 2002-2004 Hayati Ayguen <hayati.ayguen@epost.de>, Procitec GmbH
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
  struct _EF_Slot * slot            = allocationList;
  size_t            count           = slotCount;
  long              alreadyReducekB = 0;

  for ( ; count > 0  &&  alreadyReducekB < reductionSizekB; --count, ++slot )
    if (EFST_PROTECTED == slot->mode)
    {
      Page_Delete(slot->internalAddress, slot->internalSize);
      alreadyReducekB += (slot->internalSize+1023) >>10;
      slot->mode       = EFST_NOT_IN_USE;
    }
  sumProtectedMem -= alreadyReducekB;
  sumAllocatedMem -= alreadyReducekB;
}


/*
 * Find the slot structure for a user address.
 */
static struct _EF_Slot *
slotForUserAddress(void * address)
{
  struct _EF_Slot * slot  = allocationList;
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
  struct _EF_Slot * slot  = allocationList;
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
  struct _EF_Slot * slot  = allocationList;
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
  struct _EF_Slot * slot  = allocationList;
  size_t            count = slotCount;

  for ( ; count > 0; --count, ++slot )
    if ( (char*)slot->internalAddress + slot->internalSize == address )
      return slot;
  return 0;
}


/*
 * Checks the integrity of no mans land, for a given slot
 */
static
void _eff_check_slot( struct _EF_Slot * slot )
{
  char    * tmpBegAddr, * tmpEndAddr;
  size_t    userSlack,    pageSlack;

  /* calculate userSlack */
  if ( !EF_PROTECT_BELOW && EF_ALIGNMENT > 1 )
  {
    userSlack = slot->userSize % EF_ALIGNMENT;
    if ( userSlack )
      userSlack = EF_ALIGNMENT - userSlack;
  }
  else
    userSlack = 0;

  /* calculate pageSlack */
  pageSlack = slot->internalSize - slot->userSize - userSlack - EF_PAGE_SIZE;

  /* check the no man's land; use internal knowledge to detect the EF_PROTECT_BELOW on allocation */
  if ( (char*)slot->userAddress != (char*)slot->internalAddress + EF_PAGE_SIZE )
  {
    /* EF_PROTECT_BELOW was 0 when allocating this piece of memory */
    tmpBegAddr = (char*)slot->internalAddress;
    tmpEndAddr = tmpBegAddr + pageSlack;
    while (tmpBegAddr < tmpEndAddr)
      if (*tmpBegAddr++ != (char)EF_FILL)
        #ifndef EF_NO_LEAKDETECTION
          EF_Abort("\nElectric Fence: ptr=%a: free() detected overwrite of ptrs no mans land, size=%d alloced from %s(%d)",
            slot->userAddress, (int)slot->userSize, slot->filename, slot->lineno);
        #else
          EF_Abort("\nElectric Fence: ptr=%a: free() detected overwrite of ptrs no mans land", slot->userAddress);
        #endif

    tmpBegAddr = (char*)slot->userAddress + slot->userSize;
    tmpEndAddr = tmpBegAddr + userSlack;
    while (tmpBegAddr < tmpEndAddr)
      if (*tmpBegAddr++ != (char)EF_FILL)
        #ifndef EF_NO_LEAKDETECTION
          EF_Abort("\nfree() detected overwrite of no mans land: ptr=%a, size=%d\nalloced from %s(%d)",
            slot->userAddress, (int)slot->userSize, slot->filename, slot->lineno);
        #else
          EF_Abort("\nfree() detected overwrite of no mans land: ptr=%a", slot->userAddress);
        #endif
  }
  else
  {
    /* EF_PROTECT_BELOW was 1 when allocating this piece of memory */
    tmpBegAddr = (char*)slot->userAddress + slot->userSize;
    tmpEndAddr = tmpBegAddr + pageSlack;
    while (tmpBegAddr < tmpEndAddr)
      if (*tmpBegAddr++ != (char)EF_FILL)
        #ifndef EF_NO_LEAKDETECTION
          EF_Abort("\nElectric Fence: ptr=%a: free() detected overwrite of ptrs no mans land, size=%d alloced from %s(%d)",
            slot->userAddress, (int)slot->userSize, slot->filename, slot->lineno);
        #else
          EF_Abort("\nElectric Fence: ptr=%a: free() detected overwrite of ptrs no mans land", slot->userAddress);
        #endif
  }

}

