#include <stdio.h>
#include <stdlib.h>

// power for int type
int p(int base, int exponent) {
    if (exponent == 0) {
        return 1;
    } else if (exponent == 1) {
        return base;
    }

    int result = base;
    while(exponent-1) {
        result*=base;
        exponent--;
    }
    return result;
}

int main() {
    int octet[4] = {0};
    int mask[4] = {0};
    int mask_;
    int i,j,k;

    int octet_n[4] = {0};
    int octet_b[4] = {0};
    int octet_r[4] = {0};
    unsigned long max; // unsigned long (ULONG_MAX) presne sedi s maximalnym poctom IPv4

    // pouzivatelsky vstup
    printf("Input IPv4 and mask (format max. 255.255.255.255/32):\n");
    if (scanf("%d.%d.%d.%d/%d",&octet[0],&octet[1],&octet[2],&octet[3],&mask_) < 5) {
        printf("FATAL: Incorrectly formated entry\n");
        exit(0);
    }

    // kontrola vstupu
    if(mask_>32 || mask_ < 0) {
        printf("FATAL: Incorrectly input mask (over 32)\n");
        exit(0);
    }
    for(i=0;i<4;i++) {
        if(octet[i]>255 || octet[i] < 0) {
            printf("FATAL: Incorrectly input address (octet over 255)\n");
            exit(0);
        }
    }

    // long mask format calc
    j = mask_;
    for(i=0;j>=8;i++) { // if mask >= 8 then octet definitely 255
        mask[i] = 255;
        j-=8;
    }
    if(j>0) { // if remaining mask 1 to 7
        k=7; // exponents of 2
        while(j>0) {
            mask[i] += p(2,k); //2^7,2^6,2^5,...
            k--;
            j--;
        }
    }

    // bitwise multiplication -> network
    for(i=0;i<4;i++)
        octet_n[i] = mask[i]&octet[i];

    // bitwise addition w/ inverse mask -> broadcast
    for(i=0;i<4;i++)
        octet_b[i] = 255-mask[i]|octet[i];

    // calc max connectable devices
    max = 1;
    for(i=0;i<4;i++) {
        octet_r[i] = octet_b[i] - octet_n[i] + 1; // +1 to make them multipliable
        max *= octet_r[i];
    }
    if (max - 2 < 0) // subtract broadcast & network, because they're unconnectable
        max = 0;
    else
        max -= 2;

    // vypis
    printf("\nFor IP %d.%d.%d.%d with netmask %d is true:\n",octet[0],octet[1],octet[2],octet[3],mask_);
    printf("> Long mask format (netmask):\t\t %d.%d.%d.%d\n",mask[0],mask[1],mask[2],mask[3]);
    printf("> Inverse mask (wildcard):\t\t %d.%d.%d.%d\n",255-mask[0],255-mask[1],255-mask[2],255-mask[3]);

    printf("> Network:\t\t %d.%d.%d.%d/%d\n",octet_n[0],octet_n[1],octet_n[2],octet_n[3]); // binarne suciny
    printf("> Broadcast:\t\t\t %d.%d.%d.%d\n",octet_b[0],octet_b[1],octet_b[2],octet_b[3]); // binarne sucty s inverznou maskou

    printf("> Minimal usable address:\t %d.%d.%d.%d\n",octet_n[0],octet_n[1],octet_n[2],octet_n[3]+1); // cislo siete + 1
    printf("> Maximal usable address:\t %d.%d.%d.%d\n",octet_b[0],octet_b[1],octet_b[2],octet_b[3]-1); // broadcast siete - 1

    printf("> No. of connectable devices:\t %lu",max);

    return 0;
}