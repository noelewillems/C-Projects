// Created by Noel Willems - a virtual memory manager simulation (paging)
// Run like: vmm InputFile.txt BACKING_STORE.bin
// 256 frames of 256 bytes each

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define TLB_LENGTH 16
#define PAGE_TABLE_LENGTH 256
#define PHYS_MEM_LENGTH 256
#define PAGE_SIZE 256

typedef struct Entry {
    int pageNum;
    int frameNum;
} Entry;

// TLB: array of 16 Entries
Entry tlb[TLB_LENGTH];

// Page table: array of 256 Entries
Entry page_table[PAGE_TABLE_LENGTH];

// Physical memory: array of 256 pointers to char arrays
char* physical_memory[PHYS_MEM_LENGTH];

void loadFromBackingStore(int pageNum, int offset, char* file_name) {
    unsigned char page[PAGE_SIZE];
    FILE *fbin = fopen(file_name, "rb");
    if(fbin == NULL) {
        printf("Error opening file %s.\n", file_name);
        exit(0);
    }
    fseek(fbin,pageNum*PAGE_SIZE,SEEK_SET);
    fread(page,PAGE_SIZE,1,fbin);      
    printf("     page: %d, offset: %d, data: %d\n",pageNum,offset, page[offset]);
    fclose(fbin);    
}

// Check to see if a page number is in the page table.
int checkPageTable(int pageNum) {
    int check = 0; // 0 if not in page table, 1 if in page table
    for(int i = 0; i < PAGE_TABLE_LENGTH; i++) {
        if(sizeof(page_table[i].pageNum) == 0) { // if page table element is not null
            if(page_table[i].pageNum == 0) {
                check = 1;
                break;
            } 
        }
    }
    return check;
}

// Check to see if a page number is in the TLB.
int checkTLB(int pageNum) {
    int check = 0; // 0 if not in TLB, 1 if in TLB
    for(int i = 0; i < TLB_LENGTH; i++) {
        if(sizeof(tlb[i].pageNum) == 0) { // If TLB element is not null
            if(tlb[i].pageNum == pageNum) {
                check = 1; 
                break;
            } 
        }
    }
    return check;
}

// Run: process input file
void run(char* input_file_name, char* backing_store_file_name) {
    // Processing input file - if error, output message and return.
    FILE* inputFile = fopen(input_file_name, "r"); 
    if (inputFile == NULL) {
        perror("Error opening file.\n");
    }
    char line[128];
    char* pos;
    while (fgets(line, sizeof(line), inputFile)) {
        int address = 0;
        int page = 0;
        int offset = 0;
        line[strlen(line)] = '\0'; // Null-terminating the line
        if ((pos = strchr(line, '\n')) != NULL) { *pos = '\0'; } // For newline chars
        if ((pos = strchr(line, '\r')) != NULL) { *pos = '\0'; } // For carriage returns
        if (line[0] == '\0') { // Ignore null lines
        } else {
            // Next 3 lines of code are provided by Dr. Hunt 
            address = atoi(line);
            page = (0x0000FF00 & address) >> 8;
            offset = 0x000000FF & address;
        }
        printf("For address %d, page is %d and offset is %d\n",address, page, offset);
        if(checkTLB(page) == 0) {
            printf("     Not in TLB; checking page table.\n");
            if(checkPageTable(page) == 0) {
                printf("     Not in page table either; PAGE FAULT; loading from backing store.\n");
                loadFromBackingStore(page, offset, backing_store_file_name);
            } 
        } else if(checkTLB(page) == 1) {
            printf("     In TLB! Fetch frame number.\n");
        }
    }
}

int main(int argc, char* argv[]) {
    // Read input file of InputFile.txt
    if(argc != 3) {
        printf("Please run like: ./vmm <input file name> <backing store bin file name>\n");
        exit(0);
    }
    char* input_file_name = argv[1];
    char* backing_store_file_name = argv[2];
    run(input_file_name, backing_store_file_name);
}