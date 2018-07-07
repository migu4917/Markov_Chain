/*-------------------------------------------

// Copyright 2018, migu4917.  All rights reserved.

// Data Structure and Programming (Class of Information)
// Integrated Assignment "Markov Chain"
// Time: about 5~6 sec

---------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define HASHSIZE 2001369

typedef struct _suf {
    void * word, * next;
} _Suffix;

typedef struct _node {
    void * pref[2], * link;
    struct _suf * suffix, * tail;
    int nsuf;
} HashNode;

void * ListHash[HASHSIZE];

double seed = 997;
double rrand() {
    double lambda = 3125.0, m = 34359738337.0;
    seed = fmod(lambda * seed, m);
    return seed / m;
}

unsigned int hash(char *s[]) {
    unsigned int h = 0;
    char *p;
    for (p = s[0]; *p; ++p)
        h = 131 * h + *p;
    for (p = s[1]; *p; ++p)
        h = 131 * h + *p;
    return h % HASHSIZE;
}

HashNode * FindPrefix(char *prefix[]) {
    int value = hash(prefix);
    HashNode * sp = ListHash[value];
    for ( ; sp != NULL; sp = sp->link)
        if (strcmp(prefix[0], sp->pref[0]) == 0 && strcmp(prefix[1], sp->pref[1]) == 0)
            return sp;
    sp = (HashNode*)malloc(sizeof(HashNode));
    memset(sp, 0, sizeof(HashNode));
    sp->pref[0] = prefix[0], sp->pref[1] = prefix[1];
    sp->link = ListHash[value];
    ListHash[value] = sp;
    return sp;
}

void add(char *prefix[], char *suffix) {
    HashNode * sp = FindPrefix(prefix);
    ++sp->nsuf;
    _Suffix * suf = (_Suffix*)malloc(sizeof(_Suffix));
    suf->word = suffix;
    if(sp->suffix == NULL)
        sp->suffix = sp->tail = suf;
    sp->tail->next = suf;
    sp->tail = suf;
    prefix[0] = prefix[1];
    prefix[1] = suffix;
}

void Produce(int maxn, FILE * outfp) {
    char *prefix[2] = { "\0", "\0"}, *w;
    unsigned int j;
    while (maxn--) {
        HashNode * sp = FindPrefix(prefix);
        if(sp->nsuf == 1)
            w = sp->suffix->word;
        else {
            int n = (int)(rrand() * sp->nsuf);
            _Suffix * suf = sp->suffix;
            for (j = 0; j < n; ++j)
                suf = suf->next;
            w = suf->word;
        }
        if (strcmp(w, "__EOF__") == 0)
            return;
        fprintf(outfp, "%s ", w);
        prefix[0] = prefix[1];
        prefix[1] = w;
    }
}

int main() {
    char *prefix[2] = { "\0", "\0"}, buf[200];
    FILE * infp, * outfp;
    int maxn;
    infp = fopen("article.txt", "r");
    outfp = fopen("markov.txt", "w");
    while(~fscanf(infp, "%s", buf))
        add(prefix, strdup(buf));
    add(prefix, "__EOF__");
    scanf("%d", &maxn);
    Produce(maxn, outfp);
    fclose(infp);
    fclose(outfp);
    return 0;
}
