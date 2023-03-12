#include "os.h"
#define MAXDEPTH 5
int getIndex(uint64_t vpn,int part){
    uint64_t shift = 36-9*(part-1);
    return (vpn >> shift)& (0x01FF);
}


void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn){
    uint64_t * pt_data = phys_to_virt(pt<<12);
    uint64_t PTE;
    uint64_t phys;
    int index;
        for(int i = 1;i < MAXDEPTH;i++){
        index= getIndex(vpn,i);
        PTE = pt_data[index];
        if ((PTE & (0x1)) == 1){ //then valid bit =1 so keep walking
        pt_data = phys_to_virt(PTE-1);
        }
        else{ //so valid bit=0
            if(ppn == NO_MAPPING){ // the map doesnt exist anyway so i finished
                return;
            }
            // create new node and walk
            phys = alloc_page_frame()<<12;
            pt_data[index]= phys+1;
            pt_data = phys_to_virt(phys);
        }  
    }
    // the last level
    index= getIndex(vpn,MAXDEPTH);
    if (ppn == NO_MAPPING){
        pt_data[index] = 0;
    }
    else{
        pt_data[index]= (ppn<<12)+1;
    }
}

uint64_t page_table_query(uint64_t pt, uint64_t vpn){
    uint64_t * pt_data = phys_to_virt(pt<<12);
    uint64_t PTE;
    int index;
    for(int i = 1;i < MAXDEPTH;i++){
        index= getIndex(vpn,i);
        PTE = pt_data[index];
        if ((PTE & (0x1)) == 1){ //then valid bit =1 so keep walking
        pt_data = phys_to_virt(PTE-1);
        }
        else{ // then the next node doesnt exist
        return NO_MAPPING;
        }
    }
        // the last level
    index= getIndex(vpn,MAXDEPTH);
    PTE = pt_data[index];
    if ((PTE & (0x1)) == 1){
        return PTE>>12;
    }
    else{
        return NO_MAPPING;
    }
}


