#include "ezxml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define LIMIT 1000

struct Kisi {
    char ad[MAX];
    char soyad[MAX];
    unsigned int numara;
};

// XML'den struct'a veri çekme
int dosyadanOku(struct Kisi kisiler[], int limit) {
    ezxml_t root = ezxml_parse_file("fihrist.xml");
    if (!root) {
        printf("XML dosyasý açýlamadý!\n");
        return 0;
    }

    int sayac = 0;
    for (ezxml_t kisi = ezxml_child(root, "kisi"); kisi; kisi = kisi->next) {
        if (sayac >= limit) break;

        const char* ad = ezxml_child(kisi, "ad") ? ezxml_child(kisi, "ad")->txt : "";
        const char* soyad = ezxml_child(kisi, "soyad") ? ezxml_child(kisi, "soyad")->txt : "";
        const char* numaraStr = ezxml_child(kisi, "numara") ? ezxml_child(kisi, "numara")->txt : "0";

        strncpy(kisiler[sayac].ad, ad, MAX - 1);
        kisiler[sayac].ad[MAX - 1] = '\0';

        strncpy(kisiler[sayac].soyad, soyad, MAX - 1);
        kisiler[sayac].soyad[MAX - 1] = '\0';

        kisiler[sayac].numara = (unsigned int)atoi(numaraStr);

        sayac++;
    }

    ezxml_free(root);
    return sayac;
}

// struct'tan XML'e yazma
void dosyayaYaz(struct Kisi kisiler[], int kisiSayisi) {
    FILE* dosya = fopen("fihrist.xml", "w");
    if (!dosya) {
        printf("Dosya açýlmadý!\n");
        return;
    }

    fprintf(dosya, "<kisiler>\n");
    for (int i = 0; i < kisiSayisi; i++) {
        fprintf(dosya, "  <kisi>\n");
        fprintf(dosya, "    <ad>%s</ad>\n", kisiler[i].ad);
        fprintf(dosya, "    <soyad>%s</soyad>\n", kisiler[i].soyad);
        fprintf(dosya, "    <numara>%u</numara>\n", kisiler[i].numara);
        fprintf(dosya, "  </kisi>\n");
    }
    fprintf(dosya, "</kisiler>\n");
    fclose(dosya);
}

int kaydet(struct Kisi kisiler[], int kisiSayisi, struct Kisi yeni) {
    if (kisiSayisi >= LIMIT) {
        printf("Fihrist dolu!\n");
        return kisiSayisi;
    }

    kisiler[kisiSayisi] = yeni;
    kisiSayisi++;

    dosyayaYaz(kisiler, kisiSayisi); // XML güncelle

    printf("Kaydedildi!\n");
    return kisiSayisi;
}

// Karþýlaþtýrma fonksiyonlarý
int karsilastirIsim(const void* a, const void* b) {
    return strcmp(((struct Kisi*)a)->ad, ((struct Kisi*)b)->ad);
}

int karsilastirSoyad(const void* a, const void* b) {
    return strcmp(((struct Kisi*)a)->soyad, ((struct Kisi*)b)->soyad);
}

int karsilastirNumara(const void* a, const void* b) {
    const struct Kisi* ka = (const struct Kisi*)a;
    const struct Kisi* kb = (const struct Kisi*)b;
    if (ka->numara < kb->numara) return -1;
    else if (ka->numara > kb->numara) return 1;
    else return 0;
}

// Arama fonksiyonlarý
void bulVeYazIsim(struct Kisi kisiler[], int n, char* aranan) {
    struct Kisi arananKisi = { 0 };
    strcpy(arananKisi.ad, aranan);
    qsort(kisiler, n, sizeof(struct Kisi), karsilastirIsim);
    struct Kisi* bulunan = bsearch(&arananKisi, kisiler, n, sizeof(struct Kisi), karsilastirIsim);
    if (!bulunan) {
        printf("Kayýt bulunamadý.\n");
        return;
    }
    int index = (int)(bulunan - kisiler);
    int i = index;
    while (i >= 0 && strcmp(kisiler[i].ad, aranan) == 0) i--;
    i++;
    while (i < n && strcmp(kisiler[i].ad, aranan) == 0) {
        printf("%s %s %u\n", kisiler[i].ad, kisiler[i].soyad, kisiler[i].numara);
        i++;
    }
}

void bulVeYazSoyad(struct Kisi kisiler[], int n, char* aranan) {
    struct Kisi arananKisi = { 0 };
    strcpy(arananKisi.soyad, aranan);
    qsort(kisiler, n, sizeof(struct Kisi), karsilastirSoyad);
    struct Kisi* bulunan = bsearch(&arananKisi, kisiler, n, sizeof(struct Kisi), karsilastirSoyad);
    if (!bulunan) {
        printf("Kayýt bulunamadý.\n");
        return;
    }
    int index = (int)(bulunan - kisiler);
    int i = index;
    while (i >= 0 && strcmp(kisiler[i].soyad, aranan) == 0) i--;
    i++;
    while (i < n && strcmp(kisiler[i].soyad, aranan) == 0) {
        printf("%s %s %u\n", kisiler[i].ad, kisiler[i].soyad, kisiler[i].numara);
        i++;
    }
}

void bulVeYazNumara(struct Kisi kisiler[], int n, unsigned int aranan) {
    struct Kisi arananKisi = { 0 };
    arananKisi.numara = aranan;
    qsort(kisiler, n, sizeof(struct Kisi), karsilastirNumara);
    struct Kisi* bulunan = bsearch(&arananKisi, kisiler, n, sizeof(struct Kisi), karsilastirNumara);
    if (!bulunan) {
        printf("Kayýt bulunamadý.\n");
        return;
    }
    int index = (int)(bulunan - kisiler);
    int i = index;
    while (i >= 0 && kisiler[i].numara == aranan) i--;
    i++;
    while (i < n && kisiler[i].numara == aranan) {
        printf("%s %s %u\n", kisiler[i].ad, kisiler[i].soyad, kisiler[i].numara);
        i++;
    }
}

int main() {
    struct Kisi kisiler[LIMIT];
    int kisiSayisi = dosyadanOku(kisiler, LIMIT); // XML'den kendi struct'ýmýza çekiyoruz
    char secim[20];

    while (1) {
        printf("\nFIHRIST\n");
        printf("kaydet\n");
        printf("listele\n");
        printf("bul\n");
        printf("cikmak icin q\n");
        printf("Isleminizi girin: ");
        scanf("%19s", secim);

        if (strcmp(secim, "q") == 0) {
            printf("Programdan cikiliyor.\n");
            break;
        }
        else if (strcmp(secim, "kaydet") == 0) {
            struct Kisi yeni;
            while (1) {
                printf("Isim girin (q ile cik): ");
                scanf("%99s", yeni.ad);
                if (strcmp(yeni.ad, "q") == 0) break;

                printf("Soyisim girin: ");
                scanf("%99s", yeni.soyad);

                printf("Numara girin: ");
                scanf("%u", &yeni.numara);

                kisiSayisi = kaydet(kisiler, kisiSayisi, yeni);
            }
        }
        else if (strcmp(secim, "listele") == 0) {
            for (int i = 0; i < kisiSayisi; i++) {
                printf("%s %s %u\n", kisiler[i].ad, kisiler[i].soyad, kisiler[i].numara);
            }
        }
        else if (strcmp(secim, "bul") == 0) {
            char aramaSecimi;
            char arananStr[MAX];
            unsigned int arananNumara;

            printf("\nNe aramak istiyorsun?\n");
            printf("a. Isim\n");
            printf("b. Soyisim\n");
            printf("c. Numara\n");
            printf("Seciminizi girin: ");
            scanf(" %c", &aramaSecimi);

            if (aramaSecimi == 'a') {
                printf("Aranacak ismi girin: ");
                scanf("%99s", arananStr);
                bulVeYazIsim(kisiler, kisiSayisi, arananStr);
            }
            else if (aramaSecimi == 'b') {
                printf("Aranacak soyismi girin: ");
                scanf("%99s", arananStr);
                bulVeYazSoyad(kisiler, kisiSayisi, arananStr);
            }
            else if (aramaSecimi == 'c') {
                printf("Aranacak numarayi girin: ");
                scanf("%u", &arananNumara);
                bulVeYazNumara(kisiler, kisiSayisi, arananNumara);
            }
            else {
                printf("Gecersiz secim yaptiniz.\n");
            }
        }
        else {
            printf("Gecersiz secim yaptiniz.\n");
        }
    }

    return 0;
}
