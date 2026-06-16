/* ============================================================
 * SE1012 - Advanced Naval Battle Simulator
 * Part 2-A (Optional): Reload time + attack order strategy
 * Strategy: B attacks closest E ships first to maximize hits
 *           within its reload budget
 * Author: [Your Name]
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

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
    double reloadTime;  /* T_E: time between consecutive E firings */
} EscortTypeInfo;

EscortTypeInfo escortTypes[5] = {
    {"EA","1936A-class Destroyer",    "SK C/34 naval gun",       0.08,20,0},
    {"EB","Gabbiano-class Corvette",  "L/47 dual-purpose gun",   0.06,30,0},
    {"EC","Matsu-class Destroyer",    "Type 89 dual-purpose gun",0.07,25,0},
    {"ED","F-class Escort Ships",     "SK C/32 naval gun",       0.05,50,0},
    {"EE","Japanese Kaibokan",        "4.7 inch naval guns",     0.04,70,0}
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
    double nextFireTime; /* tracks when this E can fire next */
} EscortShip;

typedef struct {
    double x, y;
    char   type;
    int    typeIndex;
    double vmaxB;
    double rmax;
    double health;
    double reloadTime;  /* T_B: time between consecutive B firings */
} Battleship;

double randDouble(double lo, double hi){
    return lo+(hi-lo)*((double)rand()/(double)RAND_MAX);
}

void computeRangeBounds(double tL,double tH,
                        double vmin,double vmax,
                        double *rmin,double *rmax){
    double tLr=tL*PI/180.0,tHr=tH*PI/180.0;
    double sinL=sin(2*tLr),sinH=sin(2*tHr);
    double sinMax,sinMin;
    if(tL<=45.0&&tH>=45.0) sinMax=1.0;
    else sinMax=(sinL>sinH)?sinL:sinH;
    sinMin=(sinL<sinH)?sinL:sinH;
    if(sinMin<0) sinMin=0;
    *rmax=(vmax*vmax*sinMax)/G;
    *rmin=(vmin*vmin*sinMin)/G;
}

/* Sort escort ships by distance to B (closest first) - bubble sort */
void sortByDistance(int order[], double dists[], int count){
    int i,j,tmp;
    double dtmp;
    for(i=0;i<count-1;i++){
        for(j=0;j<count-1-i;j++){
            if(dists[j]>dists[j+1]){
                dtmp=dists[j]; dists[j]=dists[j+1]; dists[j+1]=dtmp;
                tmp=order[j];  order[j]=order[j+1];  order[j+1]=tmp;
            }
        }
    }
}

void printBanner(){
    printf("\n");
    printf("  ****************************************************\n");
    printf("  *      ADVANCED NAVAL BATTLE SIMULATOR             *\n");
    printf("  *      SE1012 - Part 2-A (Optional)                *\n");
    printf("  *      Reload Time + Attack Order Strategy         *\n");
    printf("  ****************************************************\n\n");
}

int main(void){
    int        seed,N,D,k,i,step;
    Battleship B;
    EscortShip E[MAX_E];
    double     pathX[MAX_K],pathY[MAX_K];
    char       typeChar;
    FILE      *fp,*fp2;

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
        printf("  [SETUP] Enter number of path points k (1-%d): ",MAX_K);
        scanf("%d",&k);
    } while(k<1||k>MAX_K);

    printf("\n  Battleship types:\n");
    printf("    U-USS Iowa  M-King George V  R-Richelieu  S-Sovetsky\n");
    do {
        printf("  [SETUP] Choose battleship type (U/M/R/S): ");
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

    printf("  [SETUP] Enter B reload time T_B in seconds (0=random): ");
    scanf("%lf",&B.reloadTime);
    if(B.reloadTime<=0){
        B.reloadTime=randDouble(1,10);
        printf("  --> T_B = %.2f s\n",B.reloadTime);
    }

    /* Set random reload times for each E type */
    printf("\n  [SETUP] Setting reload times for escort types...\n");
    for(i=0;i<5;i++){
        escortTypes[i].reloadTime=randDouble(2,15);
        printf("  %s reload time: %.2f s\n",
               escortTypes[i].notation,escortTypes[i].reloadTime);
    }

    B.rmax  =(B.vmaxB*B.vmaxB)/G;
    B.health=1.0;

    /* Generate k random path points */
    for(i=0;i<k;i++){
        pathX[i]=randDouble(0,D);
        pathY[i]=randDouble(0,D);
    }
    B.x=pathX[0]; B.y=pathY[0];

    /* Generate N escort ships */
    for(i=0;i<N;i++){
        E[i].index       =i;
        E[i].alive       =1;
        E[i].nextFireTime=0.0;
        E[i].x           =randDouble(0,D);
        E[i].y           =randDouble(0,D);
        E[i].typeIndex   =rand()%5;
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
    fprintf(fp," INITIAL CONDITIONS - Part 2A\n");
    fprintf(fp,"========================================\n\n");
    fprintf(fp,"Canvas: %d x %d | Seed: %d\n",D,D,seed);
    fprintf(fp,"Battleship: %c (%s) | Vmax=%.2f | Reload=%.2fs\n",
            B.type,battleshipTypes[B.typeIndex].name,
            B.vmaxB,B.reloadTime);
    fprintf(fp,"Strategy: Attack closest E ships first\n\n");
    fprintf(fp,"Escort type reload times:\n");
    for(i=0;i<5;i++)
        fprintf(fp,"  %s: %.2f s\n",
                escortTypes[i].notation,escortTypes[i].reloadTime);
    fprintf(fp,"\nEscort Ships:\n");
    for(i=0;i<N;i++){
        EscortTypeInfo *tt=&escortTypes[E[i].typeIndex];
        fprintf(fp,"E[%d] %s reload=%.2fs pos=(%.2f,%.2f) "
                "range=[%.2f,%.2f] impact=%.0f%%\n",
                E[i].index,tt->notation,tt->reloadTime,
                E[i].x,E[i].y,E[i].rmin,E[i].rmax,
                tt->impactPower*100.0);
    }
    fclose(fp);

    /* Stats file */
    fp=fopen("Simulation_Statistics.txt","w");
    fprintf(fp,"========================================\n");
    fprintf(fp," SIMULATION STATISTICS - Part 2A\n");
    fprintf(fp,"========================================\n\n");

    printf("\n  ============ SIMULATION RUNNING ============\n");
    printf("  Strategy: B attacks closest enemy first\n");
    printf("  B reload time: %.2f s\n\n",B.reloadTime);

    /* ---- Main loop ---- */
    for(step=0;step<k;step++){
        B.x=pathX[step]; B.y=pathY[step];
        double simTime=0.0; /* time within this step */

        printf("\n  === Step %d | B at (%.2f,%.2f) | Health=%.1f%% ===\n",
               step+1,B.x,B.y,B.health*100.0);
        fprintf(fp,"--- Step %d | B at (%.3f,%.3f) | Health=%.1f%% ---\n",
                step+1,B.x,B.y,B.health*100.0);

        /* Build sorted attack order (closest first) */
        int    order[MAX_E];
        double dists[MAX_E];
        int    inRange=0;

        for(i=0;i<N;i++){
            if(!E[i].alive) continue;
            double dx=E[i].x-B.x,dy=E[i].y-B.y;
            double dist=sqrt(dx*dx+dy*dy);
            if(dist<=B.rmax){
                order[inRange]=i;
                dists[inRange]=dist;
                inRange++;
            }
        }
        sortByDistance(order,dists,inRange);

        /* Print attack order */
        if(inRange>0){
            printf("  Attack order (closest first): ");
            fprintf(fp,"  Attack order: ");
            for(i=0;i<inRange;i++){
                printf("E[%d] ",order[i]);
                fprintf(fp,"E[%d] ",order[i]);
            }
            printf("\n");
            fprintf(fp,"\n");
        }

        /* B fires at E ships in order, with reload time between shots */
        for(i=0;i<inRange;i++){
            int idx=order[i];
            if(!E[idx].alive) continue;
            double dist=dists[i];
            double flightT=sqrt(2.0*dist/G);
            simTime+=B.reloadTime; /* reload before firing */
            E[idx].alive=0;
            printf("  t=%.2fs: B fires at E[%d] (%s) dist=%.1f "
                   "flight=%.3fs\n",
                   simTime,idx,
                   escortTypes[E[idx].typeIndex].notation,dist,flightT);
            fprintf(fp,"  t=%.3fs B->E[%d] (%s) dist=%.3f "
                    "flight=%.4fs\n",
                    simTime,idx,
                    escortTypes[E[idx].typeIndex].notation,dist,flightT);
        }

        /* E ships fire at B (with reload time; impact power applies) */
        for(i=0;i<N;i++){
            if(!E[i].alive) continue;
            double dx=E[i].x-B.x,dy=E[i].y-B.y;
            double dist=sqrt(dx*dx+dy*dy);
            if(dist>=E[i].rmin && dist<=E[i].rmax){
                double eReload=escortTypes[E[i].typeIndex].reloadTime;
                /* E fires while B health > 0 and simTime budget allows */
                double eFire=E[i].nextFireTime;
                while(eFire<=simTime && B.health>0){
                    double dmg=escortTypes[E[i].typeIndex].impactPower;
                    B.health-=dmg;
                    if(B.health<0) B.health=0;
                    printf("  t=%.2fs: E[%d] (%s) hit B! dmg=%.0f%% "
                           "B health=%.1f%%\n",
                           eFire,i,
                           escortTypes[E[i].typeIndex].notation,
                           dmg*100.0,B.health*100.0);
                    fprintf(fp,"  t=%.3fs E[%d]->B dmg=%.0f%% "
                            "B health=%.1f%%\n",
                            eFire,i,dmg*100.0,B.health*100.0);
                    eFire+=eReload;
                }
                E[i].nextFireTime=eFire;
            }
        }

        fprintf(fp,"  B health after step %d: %.1f%%\n\n",
                step+1,B.health*100.0);

        if(B.health<=0){
            printf("\n  !! B DESTROYED at step %d !!\n",step+1);
            fprintf(fp,"B DESTROYED at step %d\n",step+1);
            break;
        }
    }

    /* Final conditions */
    fp2=fopen("Final_Conditions.txt","w");
    fprintf(fp2,"========================================\n");
    fprintf(fp2," FINAL CONDITIONS - Part 2A\n");
    fprintf(fp2,"========================================\n\n");

    int totalDestroyed=0;
    for(i=0;i<N;i++) if(!E[i].alive) totalDestroyed++;

    if(B.health<=0){
        printf("\n  RESULT: B DESTROYED\n");
        fprintf(fp2,"RESULT: B DESTROYED\n");
    } else {
        printf("\n  RESULT: B SURVIVED with %.1f%% health!\n",
               B.health*100.0);
        fprintf(fp2,"RESULT: B SURVIVED\n");
        fprintf(fp2,"B final health   : %.1f%%\n",B.health*100.0);
        fprintf(fp2,"Cumulative damage: %.1f%%\n",(1.0-B.health)*100.0);
    }
    fprintf(fp2,"E ships destroyed: %d / %d\n\n",totalDestroyed,N);
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