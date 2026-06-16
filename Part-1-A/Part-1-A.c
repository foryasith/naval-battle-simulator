/* ============================================================
 * SE1012 - Advanced Naval Battle Simulator
 * Part 1-A: Basic battlefield setup + single-hit simulation
 * Author: [Your Name]
 * Date:   2024
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#define G          9.81
#define PI         3.14159265358979
#define MAX_E      100

/* ---- Escort type table (from assignment Table 1) ---- */
typedef struct {
    char   notation[3];
    char   typeName[40];
    char   gunName[40];
    double impactPower;
    double angleRange;
} EscortTypeInfo;

EscortTypeInfo escortTypes[5] = {
    {"EA","1936A-class Destroyer",    "SK C/34 naval gun",       0.08, 20},
    {"EB","Gabbiano-class Corvette",  "L/47 dual-purpose gun",   0.06, 30},
    {"EC","Matsu-class Destroyer",    "Type 89 dual-purpose gun",0.07, 25},
    {"ED","F-class Escort Ships",     "SK C/32 naval gun",       0.05, 50},
    {"EE","Japanese Kaibokan",        "4.7 inch naval guns",     0.04, 70}
};

/* ---- Battleship type table ---- */
typedef struct {
    char notation;
    char name[30];
    char gunName[40];
} BattleshipInfo;

BattleshipInfo battleshipTypes[4] = {
    {'U',"USS Iowa (BB-61)",     "50-caliber Mark 7 gun"},
    {'M',"MS King George V",     "(356 mm) Mark VII gun"},
    {'R',"Richelieu",            "(15 inch) Mle 1935 gun"},
    {'S',"Sovetsky Soyuz-class", "(16 inch) B-37 gun"}
};

/* ---- Escort ship instance ---- */
typedef struct {
    int    index;
    double x, y;
    int    typeIndex;
    double thetaL, thetaH;
    double vmin, vmax;
    double rmin, rmax;
    int    alive;
} EscortShip;

/* ---- Battleship instance ---- */
typedef struct {
    double x, y;
    char   type;
    int    typeIndex;
    double vmaxB;
    double rmax;
} Battleship;

/* ---- Helpers ---- */
double randDouble(double lo, double hi){
    return lo + (hi - lo)*((double)rand()/(double)RAND_MAX);
}

void computeRangeBounds(double tL, double tH,
                        double vmin, double vmax,
                        double *rmin, double *rmax){
    double tLr = tL*PI/180.0, tHr = tH*PI/180.0;
    double sinL = sin(2*tLr), sinH = sin(2*tHr);
    double sinMax, sinMin;
    if(tL <= 45.0 && tH >= 45.0) sinMax = 1.0;
    else sinMax = (sinL > sinH) ? sinL : sinH;
    sinMin = (sinL < sinH) ? sinL : sinH;
    if(sinMin < 0) sinMin = 0;
    *rmax = (vmax*vmax*sinMax)/G;
    *rmin = (vmin*vmin*sinMin)/G;
}

void printBanner(){
    printf("\n");
    printf("  ****************************************************\n");
    printf("  *      ADVANCED NAVAL BATTLE SIMULATOR             *\n");
    printf("  *      SE1012 - Part 1-A                           *\n");
    printf("  *      World War 2 Naval Combat Simulation         *\n");
    printf("  ****************************************************\n");
    printf("\n");
}

void pressEnter(){
    printf("\n  Press ENTER to continue...");
    getchar(); getchar();
}

/* ============================================================ */
int main(void){
    int         seed, N, D, i;
    Battleship  B;
    EscortShip  E[MAX_E];
    FILE       *fp;
    char        typeChar;

    printBanner();

    /* ---------- User inputs ---------- */
    printf("  [SETUP] Enter random seed (any number): ");
    scanf("%d",&seed);
    srand((unsigned int)seed);

    printf("  [SETUP] Enter canvas size D (battlefield is D x D): ");
    scanf("%d",&D);

    do {
        printf("  [SETUP] Enter number of escort ships N (1-%d): ",MAX_E);
        scanf("%d",&N);
    } while(N < 1 || N > MAX_E);

    printf("\n  Available battleship types:\n");
    printf("    U - USS Iowa (BB-61)     | 50-caliber Mark 7 gun\n");
    printf("    M - MS King George V     | (356 mm) Mark VII gun\n");
    printf("    R - Richelieu            | (15 inch) Mle 1935 gun\n");
    printf("    S - Sovetsky Soyuz-class | (16 inch) B-37 gun\n");
    do {
        printf("  [SETUP] Choose battleship type (U/M/R/S): ");
        scanf(" %c",&typeChar);
        typeChar = (char)toupper(typeChar);
    } while(typeChar!='U' && typeChar!='M' &&
            typeChar!='R' && typeChar!='S');

    B.type = typeChar;
    for(i=0;i<4;i++)
        if(battleshipTypes[i].notation == typeChar)
            B.typeIndex = i;

    printf("  [SETUP] Enter Vmax for battleship shell\n");
    printf("          (enter 0 to randomly generate, max 200 m/s): ");
    scanf("%lf",&B.vmaxB);
    if(B.vmaxB <= 0){
        B.vmaxB = randDouble(50,200);
        printf("  --> Randomly generated Vmax_B = %.2f m/s\n",B.vmaxB);
    }

    printf("  [SETUP] Enter battleship starting position as: x y\n");
    printf("          (enter -1 -1 to randomly generate): ");
    scanf("%lf %lf",&B.x,&B.y);
    if(B.x < 0 || B.y < 0){
        B.x = randDouble(0,D);
        B.y = randDouble(0,D);
        printf("  --> Randomly generated position = (%.2f, %.2f)\n",
               B.x, B.y);
    }

    B.rmax = (B.vmaxB * B.vmaxB) / G; /* max range at theta=45 */

    /* ---------- Generate escort ships ---------- */
    printf("\n  [GENERATING] Placing %d escort ships on battlefield...\n",N);
    for(i=0;i<N;i++){
        E[i].index     = i;
        E[i].alive     = 1;
        E[i].x         = randDouble(0,D);
        E[i].y         = randDouble(0,D);
        E[i].typeIndex = rand()%5;

        double ar = escortTypes[E[i].typeIndex].angleRange;
        E[i].thetaL = randDouble(0, 90.0 - ar);
        E[i].thetaH = E[i].thetaL + ar;
        E[i].vmin   = randDouble(5, 50);

        if(E[i].typeIndex == 0){            /* EA: vmax = 1.2 * VmaxB */
            E[i].vmax = 1.2 * B.vmaxB;
        } else {
            double cap = (B.vmaxB > E[i].vmin+1) ? B.vmaxB : E[i].vmin+2;
            E[i].vmax = randDouble(E[i].vmin+0.5, cap-0.01);
        }

        computeRangeBounds(E[i].thetaL, E[i].thetaH,
                           E[i].vmin, E[i].vmax,
                           &E[i].rmin, &E[i].rmax);
    }
    printf("  [DONE] Escort ships placed.\n");

    /* ---------- Save initial conditions ---------- */
    fp = fopen("Initial_Conditions.txt","w");
    fprintf(fp,"========================================\n");
    fprintf(fp," INITIAL BATTLEFIELD CONDITIONS\n");
    fprintf(fp," SE1012 Naval Battle Simulator - Part 1A\n");
    fprintf(fp,"========================================\n\n");
    fprintf(fp,"Canvas Size    : %d x %d\n",D,D);
    fprintf(fp,"Random Seed    : %d\n\n",seed);
    fprintf(fp,"--- BATTLESHIP (B) ---\n");
    fprintf(fp,"Type     : %c  (%s)\n",
            B.type, battleshipTypes[B.typeIndex].name);
    fprintf(fp,"Gun      : %s\n",battleshipTypes[B.typeIndex].gunName);
    fprintf(fp,"Position : (%.3f, %.3f)\n",B.x,B.y);
    fprintf(fp,"Vmin_B   : 0.000 m/s\n");
    fprintf(fp,"Vmax_B   : %.3f m/s\n",B.vmaxB);
    fprintf(fp,"Angle    : 0 - 90 degrees\n");
    fprintf(fp,"Max Range: %.3f\n\n",B.rmax);
    fprintf(fp,"--- ESCORT SHIPS (E) ---\n");
    for(i=0;i<N;i++){
        EscortTypeInfo *t = &escortTypes[E[i].typeIndex];
        fprintf(fp,"E[%d] | Type: %s (%s)\n",
                E[i].index, t->notation, t->typeName);
        fprintf(fp,"     | Gun : %s\n",t->gunName);
        fprintf(fp,"     | Pos : (%.3f, %.3f)\n",E[i].x,E[i].y);
        fprintf(fp,"     | Angles  : [%.2f, %.2f] deg\n",
                E[i].thetaL,E[i].thetaH);
        fprintf(fp,"     | Velocity: [%.3f, %.3f] m/s\n",
                E[i].vmin,E[i].vmax);
        fprintf(fp,"     | Attack range (annulus): [%.3f, %.3f]\n",
                E[i].rmin,E[i].rmax);
        fprintf(fp,"     | Impact power: %.2f\n\n",t->impactPower);
    }
    fclose(fp);
    printf("\n  [SAVED] Initial_Conditions.txt\n");

    /* ---------- Run simulation ---------- */
    printf("\n  ============ SIMULATION RUNNING ============\n");

    int    sinkerIndex = -1;
    int    hitCount    = 0;
    double maxTime     = 0.0;

    /* Check if any E can hit B */
    for(i=0;i<N;i++){
        double dx   = E[i].x - B.x;
        double dy   = E[i].y - B.y;
        double dist = sqrt(dx*dx + dy*dy);
        if(dist >= E[i].rmin && dist <= E[i].rmax){
            sinkerIndex = i;
            break;
        }
    }

    /* ---------- Save final conditions ---------- */
    fp = fopen("Final_Conditions.txt","w");
    fprintf(fp,"========================================\n");
    fprintf(fp," FINAL BATTLEFIELD CONDITIONS\n");
    fprintf(fp," SE1012 Naval Battle Simulator - Part 1A\n");
    fprintf(fp,"========================================\n\n");

    if(sinkerIndex != -1){
        double dx   = E[sinkerIndex].x - B.x;
        double dy   = E[sinkerIndex].y - B.y;
        double dist = sqrt(dx*dx+dy*dy);

        printf("\n  !! BATTLESHIP SUNK !!\n");
        printf("     Sunk by: E[%d] (type %s)\n",
               sinkerIndex,
               escortTypes[E[sinkerIndex].typeIndex].notation);

        fprintf(fp,"RESULT: Battleship B was SUNK.\n\n");
        fprintf(fp,"Sunk by        : E[%d] (type %s - %s)\n",
                sinkerIndex,
                escortTypes[E[sinkerIndex].typeIndex].notation,
                escortTypes[E[sinkerIndex].typeIndex].typeName);
        fprintf(fp,"Distance to B  : %.3f units\n",dist);
        fprintf(fp,"E[%d] Position : (%.3f, %.3f)\n",
                sinkerIndex,
                E[sinkerIndex].x, E[sinkerIndex].y);
        fprintf(fp,"B   Position   : (%.3f, %.3f)\n\n",B.x,B.y);

    } else {
        /* B fires at all E in range */
        FILE *fp2 = fopen("Simulation_Statistics.txt","w");
        fprintf(fp2,"========================================\n");
        fprintf(fp2," SIMULATION STATISTICS - Part 1A\n");
        fprintf(fp2,"========================================\n\n");
        fprintf(fp2,"%-6s %-5s %-20s %-14s\n",
                "E idx","Type","Position","Time to hit(s)");
        fprintf(fp2,"----------------------------------------------\n");

        for(i=0;i<N;i++){
            double dx   = E[i].x - B.x;
            double dy   = E[i].y - B.y;
            double dist = sqrt(dx*dx + dy*dy);
            if(dist <= B.rmax){
                double t = sqrt(2.0*dist/G);
                if(t > maxTime) maxTime = t;
                hitCount++;
                E[i].alive = 0;
                printf("  B hit E[%d] (%s) | dist=%.1f | time=%.3fs\n",
                       E[i].index,
                       escortTypes[E[i].typeIndex].notation,
                       dist, t);
                fprintf(fp2,"%-6d %-5s (%.2f,%.2f)         %-14.4f\n",
                        E[i].index,
                        escortTypes[E[i].typeIndex].notation,
                        E[i].x, E[i].y, t);
            }
        }

        fprintf(fp2,"\nTotal E ships hit : %d / %d\n",hitCount,N);
        fprintf(fp2,"Battle end time   : %.4f seconds\n",maxTime);
        fclose(fp2);
        printf("\n  [SAVED] Simulation_Statistics.txt\n");

        printf("\n  ======= RESULT =======\n");
        printf("  Battleship SURVIVED!\n");
        printf("  Escort ships hit : %d / %d\n",hitCount,N);
        printf("  Battle duration  : %.4f seconds\n",maxTime);

        fprintf(fp,"RESULT: Battleship B SURVIVED.\n\n");
        fprintf(fp,"E ships hit  : %d / %d\n",hitCount,N);
        fprintf(fp,"Battle time  : %.4f seconds\n\n",maxTime);
        fprintf(fp,"--- Final status of all escort ships ---\n");
        for(i=0;i<N;i++){
            fprintf(fp,"E[%d] (%s) : %s\n",
                    E[i].index,
                    escortTypes[E[i].typeIndex].notation,
                    E[i].alive ? "ALIVE" : "DESTROYED");
        }
    }
    fclose(fp);
    printf("\n  [SAVED] Final_Conditions.txt\n");

    pressEnter();
    return 0;
}