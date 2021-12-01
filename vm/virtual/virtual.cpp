/************************
*Rowan Antonuccio
*A02184507
************************/
#include <stdio.h> 
#include <math.h> 
#include <stdlib.h>
#include <iostream>
#include "constants.h"

void printTLB(void);
void printPageTable(void);
int translate(void);
unsigned help_bits(void);

using namespace std;
unsigned help_bits(unsigned In_Var, unsigned TOP, unsigned BOT) {//helper func for breaking into top and bottom
  unsigned TEMP = 0, Ret_Var = 0;
  for (unsigned i = BOT; i <= TOP; i++) {
    TEMP |= 1 << i;
  }

  Ret_Var = (In_Var & TEMP) >> BOT;
  return Ret_Var;
}

int translate(int virtualAddress) {
  printTLB();
  int VPO_Bit, VPN, VPO, TLBI_Bit, TLBT, TLBI; //chunks to break address into
  int Address = 0;

  VPO_Bit = log2(PAGE_SIZE); //examine address and size
  //now configure VPN and VPO
  VPN = help_bits(virtualAddress, VIRTUAL_WIDTH - 1, VPO_Bit); //configure top to VPN
  VPO = help_bits(virtualAddress, VPO_Bit - 1, 0); //configure bottom to VPO
  //move to TLBI
  TLBI_Bit = log2(TLB_SIZE / TLB_ASSOC); //Calculate TLBI size
  //now configure TLBT and TLBI
  TLBT = help_bits(virtualAddress, VIRTUAL_WIDTH - 1, (VPO_Bit + TLBI_Bit)); //TLBT as top
  TLBI = help_bits(virtualAddress, (VPO_Bit + TLBI_Bit - 1), VPO_Bit); //TLBI as bottom

  for (int i = 0; i < TLB_ASSOC; i++) {
    if (tlb[TLBI][i].tag == TLBT) {
      if (tlb[TLBI][i].valid == true) {
        Address = (tlb[TLBI][i].ppn << VPO_Bit) | VPO;
        //printf("%s\n", "TLB HIT");
        cout << "TLB HIT" << endl;//trying to get print working
        cout << Address << endl;
        return Address; //if found, print hit
      }
    }
  }
  //else if no hit
  //printf("%s\n", "TLB MISS");
  cout << "TLB MISS" << endl;
  if (pageTableValid[VPN] == 0) {
    //printf("%s\n", "PAGE FAULT");
    cout << "PAGE FAULT" << endl;
    exit(0); //if not found by here, fails
  } else { //else if found
    Address = (pageTable[VPN] << VPO_Bit) | VPO;
    cout << Address << endl;
    return Address;
  }
}

// print page table, can be used in debugging if desired
void printPageTable() {
  printf("Page Table:\nVPN  PPN Valid\n");
  for (int i = 0; i < 16; i++) {
    printf(" %02x | %02x %1d |\n", i, pageTable[i], pageTableValid[i]);
  }
  printf("\n");
}

// pretty print TLB, included for help with debugging
void printTLB() {
  printf("TLB: \n");
  // print headers
  printf("%4s|", "Set");
  for (int set = 0; set < TLB_ASSOC; set++)
    printf("%3s %4s %5s|", "Tag", "PPN", "Valid");
  // print set
  printf("\n");
  for (int set = 0; set < TLB_SIZE / TLB_ASSOC; set++) {
    printf("%3d |", set);
    // print entries
    for (int entry = 0; entry < TLB_ASSOC; entry++) {
      printf(" %02x   %02x  %3d |", tlb[set][entry].tag,
        tlb[set][entry].ppn, tlb[set][entry].valid);
    }
    printf("\n");
  }
  printf("\n");
}
// main function
int main(int argc, char * argv[]) {
  if (argc < 2) {
    printf("requries virtual address in HEX as argument!\n");
    return 0;
  }
  // set up TLB and page table (in constants.cpp)
  setupConstants();

  //take input address and parse it as a hexadecimal number
  int inputAddress = (int) strtol(argv[1], NULL, 16);

  //translate to a physical address and print results
  printf("Physical address: %x \n", translate(inputAddress));

  return 0;
}