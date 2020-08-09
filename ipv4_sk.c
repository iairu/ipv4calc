#include <stdio.h>
#include <stdlib.h>

// umocnovanie v type int
int p(int zaklad, int exponent) {
    if (exponent == 0) {
        return 1;
    } else if (exponent == 1) {
        return zaklad;
    }

    int vysledok = zaklad;
    while(exponent-1) {
        vysledok*=zaklad;
        exponent--;
    }
    return vysledok;
}

// pauza pred zatvorenim a informovanie pouzivatela
void _exit(int code) {
    printf("\n\nStlac akukolvek klavesu pre zatvorenie ...");
    getchar();
    exit(code);
}

int main() {
    int oktet[4] = {0};
    int maska[4] = {0};
    int maska_;
    int i,j,k;

    int oktet_n[4] = {0};
    int oktet_b[4] = {0};
    int oktet_r[4] = {0};
    unsigned long max; // unsigned long (ULONG_MAX) presne sedi s maximalnym poctom IPv4

    // pouzivatelsky vstup
    printf("Zadaj IPv4 a masku (vo formate max. 255.255.255.255/32):\n");
    if (scanf("%d.%d.%d.%d/%d",&oktet[0],&oktet[1],&oktet[2],&oktet[3],&maska_) < 5) {
        printf("FATAL: Zle formatovany vstup\n");
        fflush(stdin); // vyhodi ENTER z bufferu ak chyba
        _exit(0);
    }
    fflush(stdin); // vyhodi ENTER z bufferu ak nebola chyba

    // kontrola vstupu
    if(maska_>32 || maska_ < 0) {
        printf("FATAL: Zle zadana maska (nad 32)\n");
        _exit(0);
    }
    for(i=0;i<4;i++) {
        if(oktet[i]>255 || oktet[i] < 0) {
            printf("FATAL: Zle zadana adresa (oktet nad 255)\n");
            _exit(0);
        }
    }

    // vypocet dlheho tvaru masky
    j = maska_;
    for(i=0;j>=8;i++) { // ak je maska >= 8 tak bude urcite oktet s 255
        maska[i] = 255;
        j-=8;
    }
    if(j>0) { // ak je zvysna maska 1 az 7
        k=7; // exponenty dvojky
        while(j>0) {
            maska[i] += p(2,k); //2^7,2^6,2^5,...
            k--;
            j--;
        }
    }

    // binarny sucin -> cislo siete
    for(i=0;i<4;i++)
        oktet_n[i] = maska[i]&oktet[i];

    // binarny sucet s inverznou maskou -> broadcast siete
    for(i=0;i<4;i++)
        oktet_b[i] = 255-maska[i]|oktet[i];

    // vypocet max pripojitelnych zariadeni
    max = 1;
    for(i=0;i<4;i++) {
        oktet_r[i] = oktet_b[i] - oktet_n[i] + 1; // +1 aby sa dali prenasobit
        max *= oktet_r[i];
    }
    if (max - 2 < 0) // odpocet broadcastu a cisla siete, lebo to niesu pripojitelne moznosti
        max = 0;
    else
        max -= 2;

    // vypis
    printf("\nPre IP %d.%d.%d.%d s maskou (netmask) %d plati:\n",oktet[0],oktet[1],oktet[2],oktet[3],maska_);
    printf("> Dlhy tvar masky (netmask):\t\t %d.%d.%d.%d\n",maska[0],maska[1],maska[2],maska[3]);
    printf("> Inverzna maska (wildcard):\t\t %d.%d.%d.%d\n",255-maska[0],255-maska[1],255-maska[2],255-maska[3]);

    printf("> Cislo siete (network):\t\t %d.%d.%d.%d/%d\n",oktet_n[0],oktet_n[1],oktet_n[2],oktet_n[3]); // binarne suciny
    printf("> Broadcast siete:\t\t\t %d.%d.%d.%d\n",oktet_b[0],oktet_b[1],oktet_b[2],oktet_b[3]); // binarne sucty s inverznou maskou

    printf("> Minimalna priraditelna adresa:\t %d.%d.%d.%d\n",oktet_n[0],oktet_n[1],oktet_n[2],oktet_n[3]+1); // cislo siete + 1
    printf("> Maximalna priraditelna adresa:\t %d.%d.%d.%d\n",oktet_b[0],oktet_b[1],oktet_b[2],oktet_b[3]-1); // broadcast siete - 1

    printf("> Pocet pripojitelnych zariadeni:\t %lu",max);

    _exit(0);
    return 0;
}