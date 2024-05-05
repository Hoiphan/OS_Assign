/*
 * Copyright (C) 2024 pdnguyen of the HCMC University of Technology
 */
/*
 * Source Code License Grant: Authors hereby grants to Licensee 
 * a personal to use and modify the Licensed Source Code for 
 * the sole purpose of studying during attending the course CO2018.
 */
//#ifdef MM_TLB
/*
 * Memory physical based TLB Cache
 * TLB cache module tlb/tlbcache.c
 *
 * TLB cache is physically memory phy
 * supports random access 
 * and runs at high speed
 */


#include "mm.h"
#include <stdlib.h>

#define init_tlbcache(mp,sz,...) init_memphy(mp, sz, (1, ##__VA_ARGS__))

#define TLB_SIZE 0x10000

typedef struct
{
    int pid;
    int pgn;
    int fpn;
} cache_line;
static cache_line tlb_cache[TLB_SIZE];
static int tlb_size = 0;

/*
 *  tlb_cache_read read TLB cache device
 *  @mp: memphy struct
 *  @pid: process id
 *  @pgnum: page number
 *  @value: obtained value
 */
#include"stdio.h"
int tlb_cache_read(struct memphy_struct * mp, int pid, int pgnum, BYTE value)
{
   /* TODO: the identify info is mapped to 
    *      cache line by employing:
    *      direct mapped, associated mapping etc.
    */
   struct mm_struct *mm = mp->used_fp_list->owner;
   int pte = mm->pgd[pgnum];
   for (int i = 0; i < TLB_SIZE; i++)
      {
         printf("1");
        if (tlb_cache[i].pid == pid && tlb_cache[i].pgn == pgnum)
         {
            tlb_cache[i].fpn = PAGING_FPN(pte);
            for (int i = 0; i < mp->maxsz; i++)
            {
               if (PAGING_PGN(i) == pgnum)
               {
                    TLBMEMPHY_read(mp, i, &value);
                    break;
               }
            }
            return 0;
         }
      }
   return -1;
}

/*
 *  tlb_cache_write write TLB cache device
 *  @mp: memphy struct
 *  @pid: process id
 *  @pgnum: page number
 *  @value: obtained value
 */
int tlb_cache_write(struct memphy_struct *mp, int pid, int pgnum, BYTE value)
{
   /* TODO: the identify info is mapped to 
    *      cache line by employing:
    *      direct mapped, associated mapping etc.
    */
   tlb_cache[tlb_size].pid = pid;
   tlb_cache[tlb_size].pgn = pgnum;
   int pte = mp->used_fp_list->owner->pgd[pgnum];
   tlb_cache[tlb_size].fpn = PAGING_FPN(pte);
   tlb_size++;

    // Ghi data vào frame có fpn tương ứng trong tlb MEMPHY
    for (int i = 0; i < mp->maxsz; i++)
    {
        if (PAGING_PGN(i) == pgnum)
        {
            TLBMEMPHY_write(mp, i, value);
            break;
        }
    }
    return 0;
}

/*
 *  TLBMEMPHY_read natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 *  @addr: address
 *  @value: obtained value
 */
int TLBMEMPHY_read(struct memphy_struct * mp, int addr, BYTE *value)
{
   if (mp == NULL)
     return -1;

   /* TLB cached is random access by native */
   *value = mp->storage[addr];

   return 0;
}


/*
 *  TLBMEMPHY_write natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 *  @addr: address
 *  @data: written data
 */
int TLBMEMPHY_write(struct memphy_struct * mp, int addr, BYTE data)
{
   if (mp == NULL)
     return -1;

   /* TLB cached is random access by native */
   mp->storage[addr] = data;

   return 0;
}

/*
 *  TLBMEMPHY_format natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 */


int TLBMEMPHY_dump(struct memphy_struct * mp)
{
   /*TODO dump memphy contnt mp->storage 
    *     for tracing the memory content
    */
   free(mp->storage);
   return 0;
}


/*
 *  Init TLBMEMPHY struct
 */
int init_tlbmemphy(struct memphy_struct *mp, int max_size)
{
   mp->storage = (BYTE *)malloc(max_size*sizeof(BYTE));
   mp->maxsz = max_size;

   mp->rdmflg = 1;

   return 0;
}

//#endif
