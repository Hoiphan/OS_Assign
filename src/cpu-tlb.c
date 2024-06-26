/*
 * Copyright (C) 2024 pdnguyen of the HCMC University of Technology
 */
/*
 * Source Code License Grant: Authors hereby grants to Licensee 
 * a personal to use and modify the Licensed Source Code for 
 * the sole purpose of studying during attending the course CO2018.
 */
//#ifdef CPU_TLB
/*
 * CPU TLB
 * TLB module cpu/cpu-tlb.c
 */
 
#include "mm.h"
#include <stdlib.h>
#include <stdio.h>
#define TLB_SIZE 0x10000

// int tlb_change_all_page_tables_of(struct pcb_t *proc,  struct memphy_struct * mp)
// {
//   /* TODO update all page table directory info 
//    *      in flush or wipe TLB (if needed)
//    */

//   return 0;
// }

// int tlb_flush_tlb_of(struct pcb_t *proc, struct memphy_struct * mp)
// {
//   /* TODO flush tlb cached*/
//   for(int i = 0; i < proc->page_table->size; i++) {
//         if(mp->used_fp_list->fpn == proc->pid) {
//             mp->tlb[i].pid = -1; // Invalid pid
//             mp->tlb[i].pgnum = -1; // Invalid pgnum
//         }
//     }
//   return 0;
// }

/*tlballoc - CPU TLB-based allocate a region memory
 *@proc:  Process executing the instruction
 *@size: allocated size 
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */
int tlballoc(struct pcb_t *proc, uint32_t size, uint32_t reg_index)
{
  int addr, val;

  /* By default using vmaid = 0 */
  val = __alloc(proc, 0, reg_index, size, &addr);

  /* TODO update TLB CACHED frame num of the new allocated page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/
  if(proc->tlb == NULL){
    proc->tlb = (struct memphy_struct *)malloc(sizeof(struct memphy_struct));
    init_tlbmemphy(proc->tlb,TLB_SIZE);
    // struct framephy_struct *newft = malloc(sizeof(struct framephy_struct)); 
    // newft->fp_next=NULL;
    // proc->tlb->used_fp_list->fp_next = newft;
    // proc->tlb->used_fp_list = newft;
  }

  tlb_cache_write(proc->tlb, proc->pid, reg_index, addr/PAGE_SIZE);
  
  return val;
}

/*pgfree - CPU TLB-based free a region memory
 *@proc: Process executing the instruction
 *@size: allocated size 
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */
int tlbfree_data(struct pcb_t *proc, uint32_t reg_index)
{
  __free(proc, 0, reg_index);

  /* TODO update TLB CACHED frame num of freed page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/
  tlb_cache_write(proc->tlb, proc->pid, reg_index, 0);
  

  return 0;
}


/*tlbread - CPU TLB-based read a region memory
 *@proc: Process executing the instruction
 *@source: index of source register
 *@offset: source address = [source] + [offset]
 *@destination: destination storage
 */
int tlbread(struct pcb_t * proc, uint32_t source,
            uint32_t offset, 	uint32_t destination) 
{
  BYTE data = 0, frmnum = -1;

    /* TODO retrieve TLB CACHED frame num of accessing page(s)*/
    /* by using tlb_cache_read()/tlb_cache_write()*/
    /* frmnum is return value of tlb_cache_read/write value*/
    addr_t addr = proc->regs[source] + offset;
    int pid = proc->pid;

    int pgnum = PAGING_PGN(addr);
    frmnum = tlb_cache_read(proc->tlb, pid, pgnum, data);
#ifdef IODUMP
    if (frmnum >= 0)
        printf("TLB hit at read region=%d offset=%d\n", source, offset);
    else
        printf("TLB miss at read region=%d offset=%d\n", source, offset);
#ifdef PAGETBL_DUMP
    print_pgtbl(proc, 0, -1); // print max TBL
#endif
    MEMPHY_dump(proc->mram);
#endif

    if (frmnum >= 0)
        return 0;

    int val = __read(proc, 0, source, offset, &data);

    destination = (uint32_t)data;

    /* TODO update TLB CACHED with frame num of recent accessing page(s)*/
    /* by using tlb_cache_read()/tlb_cache_write()*/
    tlb_cache_write(proc->tlb, pid, pgnum, data);
    return val;
}

/*tlbwrite - CPU TLB-based write a region memory
 *@proc: Process executing the instruction
 *@data: data to be wrttien into memory
 *@destination: index of destination register
 *@offset: destination address = [destination] + [offset]
 */
int tlbwrite(struct pcb_t * proc, BYTE data,
             uint32_t destination, uint32_t offset)
{
  int val;
    BYTE frmnum = -1;

    /* TODO retrieve TLB CACHED frame num of accessing page(s))*/
    /* by using tlb_cache_read()/tlb_cache_write()
    frmnum is return value of tlb_cache_read/write value*/
    addr_t addr = proc->regs[destination] + offset;
    int pid = proc->pid;

    int pgnum = PAGING_PGN(addr);
    frmnum = tlb_cache_read(proc->tlb, pid, pgnum, data);
    if (frmnum >= 0)
    {
        TLBMEMPHY_write(proc->tlb, addr, data);  
    }
    else
    {
        tlb_cache_write(proc->tlb, pid, pgnum, data);
    }
#ifdef IODUMP
    if (frmnum >= 0)
        printf("TLB hit at write region=%d offset=%d value=%d\n",
               destination, offset, data);
    else
        printf("TLB miss at write region=%d offset=%d value=%d\n",
               destination, offset, data);
#ifdef PAGETBL_DUMP
    print_pgtbl(proc, 0, -1); // print max TBL
#endif
    MEMPHY_dump(proc->mram);
#endif

    val = __write(proc, 0, destination, offset, data);

    /* TODO update TLB CACHED with frame num of recent accessing page(s)*/
    /* by using tlb_cache_read()/tlb_cache_write()*/

    tlb_cache_write(proc->tlb, pid, pgnum, data);
    return val;
}

//#endif
