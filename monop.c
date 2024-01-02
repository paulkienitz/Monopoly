/*
This is a port of an old fortran program I wrote to simulate a monopoly
board ... the reason is not to get fresh answers to the monopoly question,
but to test the speed of my Amigas relative to the old mainframes I
originally ran this program on.  The DEC 2060 mainframe got about one million
monopoly turns done in thirty seconds, with very little other load on the
system ... the IBM 370 only got fifty thousand done in about 35 seconds, but
that might have been using only one fifth (or less) of the CPU.  Let's see
how my Amigas (1000 and 3000) do.

Result: My A3000 does one million iterations in 59 seconds.  About half the
speed of the DEC 2060 mainframe (the last of their 36 bit machines; about
five million dollars.)  The A1000 does one million iterations in 375 seconds.
If we assume that the A1000 has a speed of one MIPS (roughly true for simple
code), then that makes the A3000 about a 6.5 MIPS machine, and the DEC
mainframe rates 12-13 MIPS.  The IBM comes out to about 0.53 MIPS.

In emulation in UAE, a run of ten million turns took only two and a quarter
seconds, making over 1600 MIPS.  The host machine (a Core i7/8700K) was only
about 20% faster, showing the value of using a JIT compiler for emulation.
Yeah, this combiles and runs just fine in Visual Stupido.
*/


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define INSIZE 300

typedef unsigned short ushort;
typedef char *str;
#define put(s) fputs(s, stdout)
#define get(b) fgets(b, INSIZE, stdin)
typedef short bool;
#define false (bool) 0
#define true (bool) 1


short rent[40] = { 250, 0, 450, 0, 200, 550, 0, 550, 600, 0,
                   750, 70, 750, 900, 200, 950, 0, 950, 1000, 0,
                   1050, 0, 1050, 1100, 200, 1150, 1150, 70, 1200, 0,
                   1275, 1275, 0, 1400, 200, 0, 1500, 0, 2000, 0 };

#define VV 65535

ushort cost[40] = { 310, VV, 310, VV, 200, 350, VV, 350, 370, VV,
                    640, 150, 640, 660, 200, 680, VV, 680, 700, VV,
                    970, VV, 970, 990, 200, 1010, 1010, 150, 1030, VV,
                    1300, 1300, VV, 1320, 200, VV, 1350, VV, 1400, VV };


short members[10][4] = { {0, 2}, {5, 7, 8}, {10, 12, 13}, {15, 17, 18},
                     {20, 22, 23}, {25, 26, 28}, {30, 31, 33},
                     {36, 38}, {11, 27}, {4, 14, 24, 34} };

short memcount[10] = { 2, 3, 3, 3, 3, 3, 3, 2, 2, 4 };

str names[40] = {
    "Mediterranean", "Community Chest 1", "Baltic", "Income Tax",
    "Reading R.R.", "Oriental", "Chance 1", "Vermont", "Connecticut",
    "Just Visiting", "St. Charles", "Electric Co.", "States", "Virginia",
    "Pennsylvania R.R.", "St. James", "Community Chest 2", "Tennessee",
    "New York", "Free Parking", "Kentucky", "Chance 2", "Indiana",
    "Illinois", "B&O R.R.", "Atlantic", "Ventnor", "Water Works",
    "Marvin Gardens", "Go To Jail", "Pacific", "North Carolina",
    "Community Chest 3", "Pennsylvania", "Short Line R.R.", "Chance 3",
    "Park Place", "Luxury Tax", "Boardwalk", "Go" };

str grames[10] = { "Purple", "Light Blue", "Magenta", "Orange", "Red",
                   "Yellow", "Green", "Dark Blue", "Utilities", "Railroads" };


long offenses = 0, defenses = 0, rrpay = 0, rrpay2 = 0;

short players, goojfs = 0;

bool injail = false;



short Card(short p)
{
    short c = rand() & 15;
    if (p == 6 || p == 21 || p == 35)       /* Chance */
        switch (c) {
            case 0: case 1:                 /* nearest R.R., pay double */
                p = 10 * ((p + 6) / 10) + 4;
                if (p >= 40) p -= 40;
                rrpay2++;
                return p;
            case 2:                         /* nearest utility */
                if (p >= 11 && p < 27) return 27;
                else return 11;
            case 3:                         /* go back three spaces */
                p -= 3;
                if (p < 0) p += 40;
                return p;
            case 4:                         /* go to jail */
                injail = true;
                return 9;
            case 5:                         /* advance to Reading R.R. */
                return 4;
            case 6:                         /* advance to St. Charles */
                return 10;
            case 7:                         /* Illinois */
                return 23;
            case 8:                         /* Boardwalk */
                return 38;
            case 9:                         /* Go */
                return 39;
            case 10:                        /* get outa jail free */
                goojfs++;
                return p;
            default:
                return p;
    } else {                                /* Community Chest */
        if (!c) {                           /* go to jail */
            injail = true;
            return 9;
        } else if (c == 1)                  /* advance to Go */
            return 39;
        else if (c == 2) {                  /* get outa jail free */
            goojfs++;
            return p;
        } else return p;
    }
    return 4962;                            /* aztec misfeature */
}



short Move(short p)
{
    static short doubles = 0;
    short d1 = rand() % 6, d2 = rand() % 6;

    if (p != 29 /* go to jail */ ) {
        if (d1 == d2) doubles++;
        else doubles = 0;
        if (doubles < 3) {
            p += d1 + d2 + 2;
            if (p >= 40) p -= 40;
            injail = false;
            if (p == 1 || p == 6 || p == 16 || p == 21 || p == 32 || p == 35)
                p = Card(p);
            if (!injail) return p;
        }
    }
    offenses++;
    doubles = 0;
    if (goojfs > 0) {
        goojfs--;
        defenses++;
        return 9;       /* just visiting */
    }
    if ((d1 = rand() % 6) != (d2 = rand() % 6) &&
        (d1 = rand() % 6) != (d2 = rand() % 6))
        d1 = rand() % 6, d2 = rand() % 6;
    return d1 + d2 + 11;
}



void main()
{
    char ibuf[INSIZE];
    long turns, turn, atol();
    short players, p;
    clock_t start;
    float prob[40], barg[40];
    static long counts[40] = { 0 };
    FILE *out = fopen("monopoly.results", "w");

    if (!out) {
        puts("\nCan't open results file!");
        exit(10);
    }
    do {
        put("\nHow many players? [2 to 8]  ");
        players = atoi(get(ibuf));
    } while (players < 2 || players > 8);
    do {
        put("\nHow many turns should I simulate? [1000 to 10000000]  ");
        turns = atol(get(ibuf));
    } while (turns < 1000 || turns > 10000000);
    start = clock();
    srand(start);
    p = 0;      /* "Go" */
    for (turn = 0; turn < turns; turn++) {
        p = Move(p);
        counts[p]++;
        if (!(p % 4)) rrpay++;
    }
    fputs("Square                  Probability    "
          "Cost      Rent      Bargain rating\n======                  "
          "===========    ====      ====      ==============\n", out);
    for (p = 0; p < 40; p++) {
        prob[p] = 40.0 * (float) counts[p] / turns;
        barg[p] = prob[p] * rent[p] / cost[p];
        fprintf(out, "%-23s %8.6f       ", names[p], prob[p]);
        if (cost[p] != VV)
            fprintf(out, "%4d      %4d      %8.6f\n", cost[p], rent[p], barg[p]);
        else fputc('\n', out);
    }
    fprintf(out, "\n\nThese figures are most accurate for a game with %d"
                 " players.\nIn %ld moves, the player went to jail %ld times,\n"
                 "escaping with a GOOJF card %.5f%% of the time.\n"
                 "He paid double at railroads %.5f%% of the time.\n\n",
                 players, turns, offenses, 100.0 * (float) defenses / offenses,
                 100.0 * (float) rrpay2 / rrpay);
    fputs("Monopoly          Bargain rating\n"
          "========          ==============\n", out);
    for (p = 0; p < 10; p++) {
        short pp, m, c = 0;
        float mbarg = 0.0;

        for (pp = 0; pp < memcount[p]; pp++) {
            m = members[p][pp];
            mbarg += prob[m] * rent[m];
            c += cost[m];
        }
        mbarg /= c;
        fprintf(out, "%-17s %8.6f\n", grames[p], mbarg);
    }
    fputc('\n', out);
    fclose(out);
    start = clock() - start;
    puts("\nDone.  File \"monopoly.results\" contains the results.");
    printf("That took %.2f seconds.\n", (float) start / CLOCKS_PER_SEC);
}
