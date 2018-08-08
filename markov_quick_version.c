/*-------------------------------------------
// Copyright 2018, migu4917.  All rights reserved.
// Data Structure and Programming (Class of Information)
// Integrated Assignment "Markov Chain"
// Time: less than 1 sec
---------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NPREF (2)
#define NHASH 751643
#define MULTIPLIER 31
#define AVENUM 5
#define MAXN (500000)
#define MAXSIZE (7000000)

typedef struct _node
{
    char *pref[NPREF];
    struct _node *link;
    unsigned int nsuf, key_size;
    char ** suffix;
} HashState;

HashState *HasTable[NHASH];
HashState LinkPool[MAXN];
HashState * pos = LinkPool;
char buf[MAXSIZE], outbuf[MAXSIZE];

double seed = 997;
double rrand()
{
    static double lambda = 3125.0;
    static double m = 34359738337.0;
    seed = fmod(lambda * seed, m);
    return seed / m;
}

unsigned int hash(char *s[])
{
    register unsigned int i, h = 0;
    register char *p;
    for(i = 0; i < NPREF; ++i)
    {
        for(p = s[i]; *p; ++p)
        {
            h = MULTIPLIER * h + *p;
        }
        h %= NHASH;
    }
    return h;
}

inline int strcmp_asm(const char * cs, const char * ct)
{
    int d0, d1;
    register int __res;
    __asm__ __volatile__(
        "1:\tlodsb\n"
        "scasb\n"
        "jne 2f\n"
        "testb %%al,%%al\n"
        "jne 1b\n"
        "xorl %%eax,%%eax\n"
        "jmp 3f\n"
        "2:\tsbbl %%eax,%%eax\n"
        "orb $1,%%al\n"
        "3:"
        :"=a"(__res), "=&S"(d0), "=&D"(d1)
        :"1"(cs), "2"(ct)
    );
    return __res;
}

HashState * FindPrefix(char *prefix[])
{
    int index = hash(prefix);
    HashState *sp = HasTable[index];
    for(; sp != NULL; sp = sp->link)
        if(!strcmp_asm(prefix[0], sp->pref[0]) && !strcmp_asm(prefix[1], sp->pref[1]))
            return sp;
    sp = pos++;
    sp->pref[0] = prefix[0], sp->pref[1] = prefix[1];
    sp->key_size = AVENUM;
    sp->suffix = (char**)malloc(sizeof(char*) * AVENUM);
    sp->link = HasTable[index];
    HasTable[index] = sp;
    return sp;
}

inline void Suffixcreate(HashState *sp, char *w)
{
    ++sp->nsuf;
    if(sp->nsuf < sp->key_size)
        sp->suffix[sp->nsuf - 1] = w;
    else
    {
        sp->key_size *= 2;
        char ** suff = (char**)malloc(sizeof(char*) * sp->key_size);
        memcpy(suff, sp->suffix, sizeof(char*) * (sp->nsuf - 1));
        suff[sp->nsuf - 1] = w;
        free(sp->suffix);
        sp->suffix = suff;
    }
}

inline void add(char *prefix[], char *suffix)
{
    HashState * sp = FindPrefix(prefix);
    Suffixcreate(sp, suffix);
    prefix[0] = prefix[1];
    prefix[1] = suffix;
}

void buildHash(char *prefix[], FILE * f)
{
    fseek(f, 0, SEEK_END);
    unsigned int lsize = ftell(f);
    rewind(f);
    fread(buf, sizeof(char), lsize, f);
    fclose(f);
    register char * rear = buf, * head, * tail = buf + lsize;
    while(*rear <= 32)
        ++rear;
    head = rear;
    while(rear < tail)
    {
        while(rear < tail && *rear <= 32)
            ++rear;
        head = rear;
        while(*rear > 32)
            ++rear;
        *rear = 0;
        add(prefix, head);
    }
}

void createWords(unsigned int maxwords, FILE * outfp)
{
    HashState * sp;
    char *prefix[NPREF] = { "\0", "\0"};
    register char *des = outbuf - 1, *src, *w;
    unsigned int i;
    for(i = 0; i < maxwords; ++i)
    {
        sp = FindPrefix(prefix);
        w = (sp->nsuf == 1 ? sp->suffix[0] : sp->suffix[(int)(rrand() * sp->nsuf)]);
        if(*w == 0)
            break;
        src = w - 1;
        while((*++des = *++src))
            ;
        *des = 0x20;
        prefix[0] = prefix[1];
        prefix[1] = w;
    }
    fwrite(outbuf, sizeof(char), des - outbuf, outfp);
}

int main()
{
    char *prefix[NPREF] = { "\0", "\0"};
    FILE * infp, * outfp;
    unsigned int maxwords;
    scanf("%d", &maxwords);
    infp = fopen("article.txt", "rb");
    buildHash(prefix, infp);
    add(prefix, "\0");
    outfp = fopen("markov.txt", "wb");
    createWords(maxwords, outfp);
    fclose(outfp);
    return 0;
}
