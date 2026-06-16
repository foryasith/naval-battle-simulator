/* ============================================================
 * SE1012 - Advanced Naval Battle Simulator
 * Part 1-B Simulation 1: Battleship moves through k points
 * Author: [Your Name]
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define G      9.81
#define PI     3.14159265358979
#define MAX_E  100
#define MAX_K  20

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

typedef struct {
    int    index;
    double x, y;
    int    typeIndex;
    double thetaL, thetaH;
    double vmin, vmax;
    double rmin, rmax;
    int    alive;
} EscortShip;

typedef struct {
    double x, y;
    char   type;
    int    typeIndex;
    double vmaxB;
    double rmax;
} Battleship;

double randDouble(double lo, double hi){
    return lo + (hi-lo)*((double)rand()/(double)RAND_MAX);
}

void computeRangeBounds(double tL, double tH,
                        double vmin, double vmax,
                        double *rmin, double *rmax){
    double tLr=tL*PI/180.0, tHr=tH*PI/180.0;
    double sinL=sin(2*tLr), sinH=sin(2*tHr);
    double sinMax, sinMin;
    if(tL<=45.0 && tH>=45.0) sinMax=1.0;
    else sinMax=(sinL>sinH)?sinL:sinH;
    sinMin=(sinL<sinH)?sinL:sinH;
    if(sinMin<0) sinMin=0;
    *rmax=(vmax*vmax*sinMax)/G;
    *rmin=(vmin*vmin*sinMin)/G;
}

void printBanner(){
    printf("\n");
    printf("  ****************************************************\n");
    printf("  *      ADVANCED NAVAL BATTLE SIMULATOR             *\n");
    printf("  *      SE1012 - Part 1-B  Simulation 1             *\n");
    printf("  *      Battleship Moving Through k Points          *\n");
    printf("  ****************************************************\n\n");
}

int main(void){
    int        seed, N, D, k, i, step;
    Battleship B;
    EscortShip E[MAX_E];
    double     pathX[MAX_K], pathY[MAX_K];
    char       typeChar;
    FILE      *fp;

    printBanner();

    printf("  [SETUP] Enter random seed: ");
    scanf("%d",&seed);
    srand((unsigned int)seed);

    printf("  [SETUP] Enter canvas size D: ");
    scanf("%d",&D);

    do {
        printf("  [SETUP] Enter number of escort ships N (1-%d): ",MAX_E);
        scanf("%d",&N);
    } while(N<1||N>MAX_E);

    do {
        printf("  [SETUP] Enter number of path points k (2-%d): ",MAX_K);
        scanf("%d",&k);
    } while(k<2||k>MAX_K);

    printf("\n  Battleship types:\n");
    printf("    U-USS Iowa  M-King George V  R-Richelieu  S-Sovetsky\n");
    do {
        printf("  [SETUP] Choose type (U/M/R/S): ");
        scanf(" %c",&typeChar);
        typeChar=(char)toupper(typeChar);
    } while(typeChar!='U'&&typeChar!='M'&&typeChar!='R'&&typeChar!='S');

    B.type=typeChar;
    for(i=0;i<4;i++)
        if(battleshipTypes[i].notation==typeChar) B.typeIndex=i;

    printf("  [SETUP] Enter Vmax_B (0=random): ");
    scanf("%lf",&B.vmaxB);
    if(B.vmaxB<=0){
        B.vmaxB=randDouble(50,200);
        printf("  --> Vmax_B = %.2f m/s\n",B.vmaxB);
    }
    B.rmax=(B.vmaxB*B.vmaxB)/G;

    /* Generate k random path points */
    printf("\n  [GENERATING] Creating %d random path points...\n",k);
    for(i=0;i<k;i++){
        pathX[i]=randDouble(0,D);
        pathY[i]=randDouble(0,D);
        printf("  Point %d: (%.2f, %.2f)\n",i+1,pathX[i],pathY[i]);
    }

    /* Generate N escort ships */
    for(i=0;i<N;i++){
        E[i].index    =i;
        E[i].alive    =1;
        E[i].x        =randDouble(0,D);
        E[i].y        =randDouble(0,D);
        E[i].typeIndex=rand()%5;
        double ar=escortTypes[E[i].typeIndex].angleRange;
        E[i].thetaL=randDouble(0,90.0-ar);
        E[i].thetaH=E[i].thetaL+ar;
        E[i].vmin=randDouble(5,50);
        if(E[i].typeIndex==0){
            E[i].vmax=1.2*B.vmaxB;
        } else {
            double cap=(B.vmaxB>E[i].vmin+1)?B.vmaxB:E[i].vmin+2;
            E[i].vmax=randDouble(E[i].vmin+0.5,cap-0.01);
        }
        computeRangeBounds(E[i].thetaL,E[i].thetaH,
                           E[i].vmin,E[i].vmax,
                           &E[i].rmin,&E[i].rmax);
    }

    /* Save initial conditions */
    fp=fopen("Initial_Conditions.txt","w");
    fprintf(fp,"========================================\n");
    fprintf(fp," INITIAL CONDITIONS - Part 1B Sim 1\n");
    fprintf(fp,"========================================\n\n");
    fprintf(fp,"Canvas: %d x %d | Seed: %d\n\n",D,D,seed);
    fprintf(fp,"Battleship: %c (%s) | Vmax=%.2f | Range=%.2f\n\n",
            B.type,battleshipTypes[B.typeIndex].name,B.vmaxB,B.rmax);
    fprintf(fp,"Path Points:\n");
    for(i=0;i<k;i++)
        fprintf(fp,"  Point %d: (%.3f, %.3f)\n",i+1,pathX[i],pathY[i]);
    fprintf(fp,"\nEscort Ships:\n");
    for(i=0;i<N;i++){
        EscortTypeInfo *t=&escortTypes[E[i].typeIndex];
        fprintf(fp,"E[%d] %s pos=(%.2f,%.2f) range=[%.2f,%.2f]\n",
                E[i].index,t->notation,E[i].x,E[i].y,
                E[i].rmin,E[i].rmax);
    }
    fclose(fp);

    /* Open stats file */
    fp=fopen("Simulation_Statistics.txt","w");
    fprintf(fp,"========================================\n");
    fprintf(fp," SIMULATION STATISTICS - Part 1B Sim 1\n");
    fprintf(fp,"========================================\n\n");

    int   bSunk      = 0;
    int   sunkAtStep = 0;
    int   sinker     = -1;

    /* ---- Main simulation loop over k path points ---- */
    for(step=0; step<k && !bSunk; step++){
        B.x=pathX[step];
        B.y=pathY[step];

        printf("\n  --- Step %d | B at (%.2f, %.2f) ---\n",
               step+1,B.x,B.y);
        fprintf(fp,"--- Step %d | B at (%.3f, %.3f) ---\n",
                step+1,B.x,B.y);

        /* Check if any alive E can hit B */
        for(i=0;i<N;i++){
            if(!E[i].alive) continue;
            double dx=E[i].x-B.x, dy=E[i].y-B.y;
            double dist=sqrt(dx*dx+dy*dy);
            if(dist>=E[i].rmin && dist<=E[i].rmax){
                bSunk=1; sunkAtStep=step+1; sinker=i;
                break;
            }
        }

        if(bSunk) break;

        /* B fires at all E in its range */
        int stepHits=0;
        double maxT=0.0;
        for(i=0;i<N;i++){
            if(!E[i].alive) continue;
            double dx=E[i].x-B.x, dy=E[i].y-B.y;
            double dist=sqrt(dx*dx+dy*dy);
            if(dist<=B.rmax){
                double t=sqrt(2.0*dist/G);
                if(t>maxT) maxT=t;
                E[i].alive=0;
                stepHits++;
                printf("  B hit E[%d] (%s) dist=%.1f t=%.3fs\n",
                       E[i].index,
                       escortTypes[E[i].typeIndex].notation,dist,t);
                fprintf(fp,"  B hit E[%d] (%s) dist=%.3f time=%.4fs\n",
                        E[i].index,
                        escortTypes[E[i].typeIndex].notation,dist,t);
            }
        }
        printf("  Hits this step: %d | Step duration: %.3fs\n",
               stepHits,maxT);
        fprintf(fp,"  Hits: %d | Step duration: %.4fs\n\n",stepHits,maxT);
    }

    /* Save final conditions */
    FILE *fp2=fopen("Final_Conditions.txt","w");
    fprintf(fp2,"========================================\n");
    fprintf(fp2," FINAL CONDITIONS - Part 1B Sim 1\n");
    fprintf(fp2,"========================================\n\n");

    if(bSunk){
        printf("\n  !! B SUNK at step %d by E[%d] (%s) !!\n",
               sunkAtStep,sinker,
               escortTypes[E[sinker].typeIndex].notation);
        fprintf(fp,"B was SUNK at step %d by E[%d]\n",sunkAtStep,sinker);
        fprintf(fp2,"RESULT: B SUNK at step %d by E[%d] (%s)\n",
                sunkAtStep,sinker,
                escortTypes[E[sinker].typeIndex].notation);
    } else {
        int totalHit=0;
        for(i=0;i<N;i++) if(!E[i].alive) totalHit++;
        printf("\n  B SURVIVED all %d steps!\n",k);
        printf("  Total E ships destroyed: %d / %d\n",totalHit,N);
        fprintf(fp,"B SURVIVED all %d steps. E destroyed: %d/%d\n",
                k,totalHit,N);
        fprintf(fp2,"RESULT: B SURVIVED all %d steps.\n",k);
        fprintf(fp2,"E ships destroyed: %d / %d\n\n",totalHit,N);
    }

    fprintf(fp2,"Final ship status:\n");
    for(i=0;i<N;i++)
        fprintf(fp2,"E[%d] (%s): %s\n",E[i].index,
                escortTypes[E[i].typeIndex].notation,
                E[i].alive?"ALIVE":"DESTROYED");

    fclose(fp);
    fclose(fp2);

    printf("\n  [SAVED] Initial_Conditions.txt\n");
    printf("  [SAVED] Simulation_Statistics.txt\n");
    printf("  [SAVED] Final_Conditions.txt\n");
    printf("\n  Press ENTER to exit...");
    getchar(); getchar();
    return 0;
}