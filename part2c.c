#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define G 9.81
#define PI 3.14159265358979323846
#define MAX_ESCORTS 100
#define MAX_POINTS 50
#define MAX_TIME 300.0
#define EPS 0.000001

typedef struct
{
    int id;
    char type[3];

    double x;
    double y;

    double minVelocity;
    double maxVelocity;

    double minAngle;
    double maxAngle;

    double initialImpact;
    double gamma;

    double health;

    double firingInterval;
    double nextFireTime;

    int shotsFired;
    int alive;

} EscortShip;


typedef struct
{
    char type;

    double x;
    double y;

    double maxVelocity;

    double initialImpact;
    double gamma;

    double health;

    double firingInterval;
    double nextFireTime;

    int shotsFired;
    int alive;

} Battleship;


typedef struct
{
    double x;
    double y;

} PathPoint;


/* =========================
   RANDOM DOUBLE
   ========================= */

double randomDouble(double min, double max)
{
    return min +
           ((double)rand() / RAND_MAX) *
           (max - min);
}


/* =========================
   DEGREE -> RADIAN
   ========================= */

double degreeToRadian(double degree)
{
    return degree * PI / 180.0;
}


/* =========================
   DISTANCE
   ========================= */

double calculateDistance(
    double x1,
    double y1,
    double x2,
    double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    return sqrt(dx * dx + dy * dy);
}


/* =========================
   IMPACT POWER DEGRADATION

   IPn = IP0 * e^(-gamma*n)
   ========================= */

double currentImpact(
    double initialImpact,
    double gamma,
    int shotsFired)
{
    return initialImpact *
           exp(-gamma * shotsFired);
}


/* =========================
   ESCORT ATTACK RANGE
   ========================= */

void escortAttackRange(
    EscortShip ship,
    double *minRange,
    double *maxRange)
{
    double sinLow =
        sin(
            2.0 *
            degreeToRadian(
                ship.minAngle));

    double sinHigh =
        sin(
            2.0 *
            degreeToRadian(
                ship.maxAngle));


    double minimumSin;
    double maximumSin;


    if (sinLow < sinHigh)
        minimumSin = sinLow;
    else
        minimumSin = sinHigh;


    if (ship.minAngle <= 45.0 &&
        ship.maxAngle >= 45.0)
    {
        maximumSin = 1.0;
    }
    else
    {
        if (sinLow > sinHigh)
            maximumSin = sinLow;
        else
            maximumSin = sinHigh;
    }


    *minRange =
        (ship.minVelocity *
         ship.minVelocity *
         minimumSin) / G;


    *maxRange =
        (ship.maxVelocity *
         ship.maxVelocity *
         maximumSin) / G;
}


/* =========================
   CAN E HIT B?
   ========================= */

int escortCanHit(
    EscortShip escort,
    Battleship battleship)
{
    double distance =
        calculateDistance(
            escort.x,
            escort.y,
            battleship.x,
            battleship.y);


    double minRange;
    double maxRange;


    escortAttackRange(
        escort,
        &minRange,
        &maxRange);


    return
        distance >= minRange &&
        distance <= maxRange;
}


/* =========================
   CAN B HIT E?
   ========================= */

int battleshipCanHit(
    Battleship battleship,
    EscortShip escort,
    double minimumAngle)
{
    double distance =
        calculateDistance(
            battleship.x,
            battleship.y,
            escort.x,
            escort.y);


    double startAngle = minimumAngle;

    if (startAngle < 0.1)
        startAngle = 0.1;


    for (double angle = startAngle;
         angle < 90.0;
         angle += 0.5)
    {
        double s =
            sin(
                2.0 *
                degreeToRadian(angle));


        if (s <= EPS)
            continue;


        double requiredVelocity =
            sqrt(
                G * distance / s);


        if (requiredVelocity <=
            battleship.maxVelocity)
        {
            return 1;
        }
    }


    return 0;
}


/* =========================
   GENERATE ESCORT
   ========================= */

void generateEscort(
    EscortShip *ship,
    int id,
    int D,
    double bVmax,
    double teEA,
    double teEB,
    double teEC,
    double teED,
    double teEE,
    double gammaEA,
    double gammaEB,
    double gammaEC,
    double gammaED,
    double gammaEE)
{
    ship->id = id;

    ship->x =
        randomDouble(0, D);

    ship->y =
        randomDouble(0, D);

    ship->health = 1.0;
    ship->alive = 1;

    ship->shotsFired = 0;
    ship->nextFireTime = 0.0;


    int type =
        rand() % 5;


    double angleRange;


    switch (type)
    {
        case 0:

            strcpy(
                ship->type,
                "EA");

            ship->initialImpact = 0.08;
            ship->firingInterval = teEA;
            ship->gamma = gammaEA;

            angleRange = 20;

            ship->maxVelocity =
                1.2 * bVmax;

            break;


        case 1:

            strcpy(
                ship->type,
                "EB");

            ship->initialImpact = 0.06;
            ship->firingInterval = teEB;
            ship->gamma = gammaEB;

            angleRange = 30;

            ship->maxVelocity =
                randomDouble(
                    0.55 * bVmax,
                    0.95 * bVmax);

            break;


        case 2:

            strcpy(
                ship->type,
                "EC");

            ship->initialImpact = 0.07;
            ship->firingInterval = teEC;
            ship->gamma = gammaEC;

            angleRange = 25;

            ship->maxVelocity =
                randomDouble(
                    0.55 * bVmax,
                    0.95 * bVmax);

            break;


        case 3:

            strcpy(
                ship->type,
                "ED");

            ship->initialImpact = 0.05;
            ship->firingInterval = teED;
            ship->gamma = gammaED;

            angleRange = 50;

            ship->maxVelocity =
                randomDouble(
                    0.55 * bVmax,
                    0.95 * bVmax);

            break;


        default:

            strcpy(
                ship->type,
                "EE");

            ship->initialImpact = 0.04;
            ship->firingInterval = teEE;
            ship->gamma = gammaEE;

            angleRange = 70;

            ship->maxVelocity =
                randomDouble(
                    0.55 * bVmax,
                    0.95 * bVmax);

            break;
    }


    ship->minVelocity =
        randomDouble(
            0.25 * ship->maxVelocity,
            0.65 * ship->maxVelocity);


    ship->minAngle =
        randomDouble(
            1,
            90 - angleRange);


    ship->maxAngle =
        ship->minAngle +
        angleRange;
}


/* =========================
   GENERATE PATH
   ========================= */

void generatePath(
    PathPoint path[],
    int k,
    int D)
{
    for (int i = 0; i < k; i++)
    {
        path[i].x =
            randomDouble(0, D);

        path[i].y =
            randomDouble(0, D);
    }
}


/* =========================
   COUNT ALIVE E
   ========================= */

int countAliveEscorts(
    EscortShip escorts[],
    int N)
{
    int count = 0;


    for (int i = 0; i < N; i++)
    {
        if (escorts[i].alive)
            count++;
    }


    return count;
}


/* =========================
   RESET SIMULATION
   ========================= */

void resetSimulation(
    Battleship *b,
    EscortShip escorts[],
    int N)
{
    b->health = 1.0;
    b->alive = 1;

    b->shotsFired = 0;
    b->nextFireTime = 0.0;


    for (int i = 0; i < N; i++)
    {
        escorts[i].health = 1.0;
        escorts[i].alive = 1;

        escorts[i].shotsFired = 0;
        escorts[i].nextFireTime = 0.0;
    }
}


/* =========================
   B TARGET STRATEGY

   Priority =
   current threat / firing interval
   ========================= */

int chooseTarget(
    Battleship battleship,
    EscortShip escorts[],
    int N,
    double minimumAngle)
{
    int best = -1;

    double bestScore = -1;


    for (int i = 0; i < N; i++)
    {
        if (!escorts[i].alive)
            continue;


        if (!battleshipCanHit(
                battleship,
                escorts[i],
                minimumAngle))
        {
            continue;
        }


        double impact =
            currentImpact(
                escorts[i].initialImpact,
                escorts[i].gamma,
                escorts[i].shotsFired);


        double score =
            impact /
            escorts[i].firingInterval;


        if (score > bestScore)
        {
            bestScore = score;
            best = i;
        }
    }


    return best;
}


/* =========================
   B FIRES
   ========================= */

int battleshipFire(
    Battleship *b,
    EscortShip escorts[],
    int N,
    double minimumAngle,
    double time,
    FILE *fp)
{
    int target =
        chooseTarget(
            *b,
            escorts,
            N,
            minimumAngle);


    if (target == -1)
        return 0;


    /*
       Calculate B current impact BEFORE
       increasing shot count.

       First shot:
       n = 0
       IP = IP0
    */

    double impact =
        currentImpact(
            b->initialImpact,
            b->gamma,
            b->shotsFired);


    escorts[target].health -=
        impact;


    if (escorts[target].health < 0)
        escorts[target].health = 0;


    printf(
        "Time %.2f : B Shot %d -> E%d (%s)"
        " | IP %.4f"
        " | E Health %.2f%%\n",
        time,
        b->shotsFired + 1,
        escorts[target].id,
        escorts[target].type,
        impact,
        escorts[target].health * 100);


    fprintf(
        fp,
        "Time %.2f : B Shot %d -> E%d (%s)"
        " | IP %.4f"
        " | E Health %.2f%%\n",
        time,
        b->shotsFired + 1,
        escorts[target].id,
        escorts[target].type,
        impact,
        escorts[target].health * 100);


    b->shotsFired++;


    b->nextFireTime =
        time +
        b->firingInterval;


    if (escorts[target].health <= 0)
    {
        escorts[target].alive = 0;


        printf(
            "E%d DESTROYED!\n",
            escorts[target].id);


        fprintf(
            fp,
            "E%d DESTROYED!\n",
            escorts[target].id);
    }


    return 1;
}


/* =========================
   ESCORTS FIRE
   ========================= */

void escortsFire(
    Battleship *b,
    EscortShip escorts[],
    int N,
    double time,
    FILE *fp)
{
    for (int i = 0; i < N; i++)
    {
        if (!escorts[i].alive)
            continue;


        if (!escortCanHit(
                escorts[i],
                *b))
        {
            continue;
        }


        if (escorts[i].nextFireTime >
            time + EPS)
        {
            continue;
        }


        double impact =
            currentImpact(
                escorts[i].initialImpact,
                escorts[i].gamma,
                escorts[i].shotsFired);


        b->health -= impact;


        if (b->health < 0)
            b->health = 0;


        printf(
            "Time %.2f : E%d (%s) Shot %d -> B"
            " | IP %.4f"
            " | B Health %.2f%%\n",
            time,
            escorts[i].id,
            escorts[i].type,
            escorts[i].shotsFired + 1,
            impact,
            b->health * 100);


        fprintf(
            fp,
            "Time %.2f : E%d (%s) Shot %d -> B"
            " | IP %.4f"
            " | B Health %.2f%%\n",
            time,
            escorts[i].id,
            escorts[i].type,
            escorts[i].shotsFired + 1,
            impact,
            b->health * 100);


        escorts[i].shotsFired++;


        escorts[i].nextFireTime =
            time +
            escorts[i].firingInterval;


        if (b->health <= 0)
        {
            b->alive = 0;


            printf(
                "BATTLESHIP DESTROYED!\n");


            fprintf(
                fp,
                "BATTLESHIP DESTROYED!\n");


            return;
        }
    }
}


/* =========================
   FIND NEXT EVENT TIME
   ========================= */

double nextEventTime(
    Battleship b,
    EscortShip escorts[],
    int N,
    double minimumAngle,
    double currentTime)
{
    double next =
        MAX_TIME + 1;


    /*
       B event
    */

    if (chooseTarget(
            b,
            escorts,
            N,
            minimumAngle) != -1)
    {
        if (b.nextFireTime >=
            currentTime - EPS)
        {
            next = b.nextFireTime;
        }
    }


    /*
       E events
    */

    for (int i = 0; i < N; i++)
    {
        if (!escorts[i].alive)
            continue;


        if (!escortCanHit(
                escorts[i],
                b))
        {
            continue;
        }


        if (escorts[i].nextFireTime >=
            currentTime - EPS &&
            escorts[i].nextFireTime < next)
        {
            next =
                escorts[i].nextFireTime;
        }
    }


    return next;
}


/* =========================
   BATTLE AT ONE POSITION
   ========================= */

void battleAtPosition(
    Battleship *b,
    EscortShip escorts[],
    int N,
    double minimumAngle,
    double *globalTime,
    FILE *fp)
{
    while (
        b->alive &&
        countAliveEscorts(
            escorts,
            N) > 0 &&
        *globalTime <= MAX_TIME)
    {
        double eventTime =
            nextEventTime(
                *b,
                escorts,
                N,
                minimumAngle,
                *globalTime);


        /*
           No one can attack anyone
           from current position.
        */

        if (eventTime > MAX_TIME)
        {
            fprintf(
                fp,
                "No more attack events at this position.\n");

            break;
        }


        *globalTime =
            eventTime;


        /*
           Escort fires first if events
           happen at exactly same time.
        */

        escortsFire(
            b,
            escorts,
            N,
            *globalTime,
            fp);


        if (!b->alive)
            break;


        /*
           B fires if its firing time
           has arrived.
        */

        if (b->nextFireTime <=
            *globalTime + EPS)
        {
            battleshipFire(
                b,
                escorts,
                N,
                minimumAngle,
                *globalTime,
                fp);
        }


        /*
           Prevent accidental same-time loop.
        */

        *globalTime += 0.00001;
    }
}


/* =========================
   SAVE CURRENT IMPACT DATA
   ========================= */

void saveFinalStatus(
    FILE *fp,
    Battleship b,
    EscortShip escorts[],
    int N)
{
    fprintf(
        fp,
        "\n============================\n");

    fprintf(
        fp,
        "FINAL STATUS\n");

    fprintf(
        fp,
        "============================\n");


    fprintf(
        fp,
        "B Status: %s\n",
        b.alive ?
        "ALIVE" :
        "DESTROYED");


    fprintf(
        fp,
        "B Health: %.2f%%\n",
        b.health * 100);


    fprintf(
        fp,
        "B Shots Fired: %d\n",
        b.shotsFired);


    fprintf(
        fp,
        "B Current Impact Factor: %.6f\n",
        currentImpact(
            b.initialImpact,
            b.gamma,
            b.shotsFired));


    for (int i = 0; i < N; i++)
    {
        fprintf(
            fp,
            "\nE%d (%s)\n",
            escorts[i].id,
            escorts[i].type);


        fprintf(
            fp,
            "Status: %s\n",
            escorts[i].alive ?
            "ALIVE" :
            "DESTROYED");


        fprintf(
            fp,
            "Health: %.2f%%\n",
            escorts[i].health * 100);


        fprintf(
            fp,
            "Shots Fired: %d\n",
            escorts[i].shotsFired);


        fprintf(
            fp,
            "Current Impact Factor: %.6f\n",
            currentImpact(
                escorts[i].initialImpact,
                escorts[i].gamma,
                escorts[i].shotsFired));
    }
}


/* =========================
   STATIONARY SIMULATION
   ========================= */

void runStationary(
    Battleship battleship,
    EscortShip escorts[],
    int N)
{
    FILE *fp =
        fopen(
            "part2c_stationary.txt",
            "w");


    if (fp == NULL)
    {
        printf(
            "File error.\n");

        return;
    }


    resetSimulation(
        &battleship,
        escorts,
        N);


    double time = 0.0;


    printf(
        "\n=================================\n");

    printf(
        " PART 2-C STATIONARY SIMULATION\n");

    printf(
        "=================================\n");


    fprintf(
        fp,
        "PART 2-C STATIONARY SIMULATION\n\n");


    battleAtPosition(
        &battleship,
        escorts,
        N,
        0.0,
        &time,
        fp);


    printf(
        "\nB Status = %s\n",
        battleship.alive ?
        "ALIVE" :
        "DESTROYED");


    printf(
        "B Health = %.2f%%\n",
        battleship.health * 100);


    printf(
        "B Shots = %d\n",
        battleship.shotsFired);


    printf(
        "Final Time = %.2f sec\n",
        time);


    saveFinalStatus(
        fp,
        battleship,
        escorts,
        N);


    fclose(fp);
}


/* =========================
   PATH SIMULATION
   ========================= */

void runPathSimulation(
    int simulationNumber,
    Battleship battleship,
    EscortShip escorts[],
    int N,
    PathPoint path[],
    int k,
    int jamIteration,
    double jamMinAngle)
{
    char filename[50];


    if (simulationNumber == 1)
    {
        strcpy(
            filename,
            "part2c_path_sim1.txt");
    }
    else
    {
        strcpy(
            filename,
            "part2c_path_sim2.txt");
    }


    FILE *fp =
        fopen(
            filename,
            "w");


    if (fp == NULL)
    {
        printf(
            "File error.\n");

        return;
    }


    resetSimulation(
        &battleship,
        escorts,
        N);


    double globalTime = 0.0;


    printf(
        "\n=================================\n");

    printf(
        " PART 2-C PATH SIMULATION %d\n",
        simulationNumber);

    printf(
        "=================================\n");


    fprintf(
        fp,
        "PART 2-C PATH SIMULATION %d\n\n",
        simulationNumber);


    for (int point = 0;
         point < k;
         point++)
    {
        battleship.x =
            path[point].x;

        battleship.y =
            path[point].y;


        printf(
            "\n--- ITERATION %d ---\n",
            point + 1);


        printf(
            "B position = (%.2f, %.2f)\n",
            battleship.x,
            battleship.y);


        fprintf(
            fp,
            "\nITERATION %d\n",
            point + 1);


        fprintf(
            fp,
            "B position = %.2f %.2f\n",
            battleship.x,
            battleship.y);


        double minimumAngle =
            0.0;


        if (simulationNumber == 2 &&
            (point + 1) > jamIteration)
        {
            minimumAngle =
                jamMinAngle;


            printf(
                "Gun jam ACTIVE: %.2f - 90 degrees\n",
                jamMinAngle);


            fprintf(
                fp,
                "Gun jam ACTIVE: %.2f - 90 degrees\n",
                jamMinAngle);
        }


        battleAtPosition(
            &battleship,
            escorts,
            N,
            minimumAngle,
            &globalTime,
            fp);


        printf(
            "B Health = %.2f%%\n",
            battleship.health * 100);


        printf(
            "Remaining E = %d\n",
            countAliveEscorts(
                escorts,
                N));


        if (!battleship.alive)
        {
            printf(
                "Battleship destroyed. Simulation stopped.\n");

            break;
        }


        if (countAliveEscorts(
                escorts,
                N) == 0)
        {
            printf(
                "All Escort ships destroyed.\n");

            break;
        }


        if (globalTime >= MAX_TIME)
        {
            printf(
                "Maximum simulation time reached.\n");

            break;
        }
    }


    saveFinalStatus(
        fp,
        battleship,
        escorts,
        N);


    fclose(fp);
}


/* =========================
   SAVE INITIAL CONDITIONS
   ========================= */

void saveInitial(
    Battleship battleship,
    EscortShip escorts[],
    int N,
    PathPoint path[],
    int k,
    int D,
    unsigned int seed)
{
    FILE *fp =
        fopen(
            "part2c_initial.txt",
            "w");


    if (fp == NULL)
        return;


    fprintf(
        fp,
        "PART 2-C INITIAL CONDITIONS\n\n");


    fprintf(
        fp,
        "Random Seed = %u\n",
        seed);


    fprintf(
        fp,
        "Battlefield = %d x %d\n\n",
        D,
        D);


    fprintf(
        fp,
        "Battleship Type = %c\n",
        battleship.type);


    fprintf(
        fp,
        "B Position = %.2f %.2f\n",
        battleship.x,
        battleship.y);


    fprintf(
        fp,
        "B Vmax = %.2f\n",
        battleship.maxVelocity);


    fprintf(
        fp,
        "B TB = %.2f\n",
        battleship.firingInterval);


    fprintf(
        fp,
        "B Initial Impact = %.2f\n",
        battleship.initialImpact);


    fprintf(
        fp,
        "B Gamma = %.4f\n\n",
        battleship.gamma);


    fprintf(
        fp,
        "ESCORT SHIPS\n");


    for (int i = 0; i < N; i++)
    {
        fprintf(
            fp,
            "\nE%d (%s)\n",
            escorts[i].id,
            escorts[i].type);


        fprintf(
            fp,
            "Position = %.2f %.2f\n",
            escorts[i].x,
            escorts[i].y);


        fprintf(
            fp,
            "Velocity = %.2f - %.2f\n",
            escorts[i].minVelocity,
            escorts[i].maxVelocity);


        fprintf(
            fp,
            "Angle = %.2f - %.2f\n",
            escorts[i].minAngle,
            escorts[i].maxAngle);


        fprintf(
            fp,
            "Initial Impact = %.2f\n",
            escorts[i].initialImpact);


        fprintf(
            fp,
            "TE = %.2f\n",
            escorts[i].firingInterval);


        fprintf(
            fp,
            "Gamma = %.4f\n",
            escorts[i].gamma);
    }


    fprintf(
        fp,
        "\nB PATH\n");


    for (int i = 0; i < k; i++)
    {
        fprintf(
            fp,
            "Point %d = %.2f %.2f\n",
            i + 1,
            path[i].x,
            path[i].y);
    }


    fclose(fp);
}


/* =========================
   MAIN
   ========================= */

int main()
{
    int D;
    int N;
    int k;

    unsigned int seed;

    int jamIteration;

    double jamMinAngle;


    double teEA;
    double teEB;
    double teEC;
    double teED;
    double teEE;


    double gammaEA;
    double gammaEB;
    double gammaEC;
    double gammaED;
    double gammaEE;


    Battleship battleship;


    EscortShip original[MAX_ESCORTS];
    EscortShip stationary[MAX_ESCORTS];
    EscortShip simulation1[MAX_ESCORTS];
    EscortShip simulation2[MAX_ESCORTS];


    PathPoint path[MAX_POINTS];


    printf(
        "\n=================================\n");

    printf(
        " ADVANCED NAVAL BATTLE SIMULATOR\n");

    printf(
        "           PART 2-C\n");

    printf(
        "=================================\n\n");


    printf(
        "Enter random seed: ");

    scanf(
        "%u",
        &seed);

    srand(seed);


    printf(
        "Enter battlefield size D: ");

    scanf(
        "%d",
        &D);


    printf(
        "Enter number of Escort ships: ");

    scanf(
        "%d",
        &N);


    if (D <= 0 ||
        N <= 0 ||
        N > MAX_ESCORTS)
    {
        printf(
            "Invalid battlefield or N.\n");

        return 1;
    }


    printf(
        "\nU - USS Iowa\n");

    printf(
        "M - King George V\n");

    printf(
        "R - Richelieu\n");

    printf(
        "S - Sovetsky Soyuz\n");


    printf(
        "Enter Battleship type: ");

    scanf(
        " %c",
        &battleship.type);


    if (battleship.type != 'U' &&
        battleship.type != 'M' &&
        battleship.type != 'R' &&
        battleship.type != 'S')
    {
        printf(
            "Invalid Battleship type.\n");

        return 1;
    }


    printf(
        "Enter B maximum shell velocity: ");

    scanf(
        "%lf",
        &battleship.maxVelocity);


    printf(
        "Enter B starting X: ");

    scanf(
        "%lf",
        &battleship.x);


    printf(
        "Enter B starting Y: ");

    scanf(
        "%lf",
        &battleship.y);


    printf(
        "Enter B firing interval TB: ");

    scanf(
        "%lf",
        &battleship.firingInterval);


    /*
       PDF says previous B impact power = 1
    */

    battleship.initialImpact = 1.0;


    /*
       B gamma should normally be
       relatively close to zero.
    */

    printf(
        "Enter B gamma: ");

    scanf(
        "%lf",
        &battleship.gamma);


    if (battleship.maxVelocity <= 0 ||
        battleship.firingInterval <= 0 ||
        battleship.gamma < 0 ||
        battleship.x < 0 ||
        battleship.x > D ||
        battleship.y < 0 ||
        battleship.y > D)
    {
        printf(
            "Invalid Battleship data.\n");

        return 1;
    }


    battleship.health = 1.0;
    battleship.alive = 1;
    battleship.shotsFired = 0;
    battleship.nextFireTime = 0;


    /*
       Escort firing intervals
    */

    printf(
        "\nEnter EA TE: ");

    scanf(
        "%lf",
        &teEA);


    printf(
        "Enter EB TE: ");

    scanf(
        "%lf",
        &teEB);


    printf(
        "Enter EC TE: ");

    scanf(
        "%lf",
        &teEC);


    printf(
        "Enter ED TE: ");

    scanf(
        "%lf",
        &teED);


    printf(
        "Enter EE TE: ");

    scanf(
        "%lf",
        &teEE);


    /*
       Escort gamma values
    */

    printf(
        "\nEnter EA gamma: ");

    scanf(
        "%lf",
        &gammaEA);


    printf(
        "Enter EB gamma: ");

    scanf(
        "%lf",
        &gammaEB);


    printf(
        "Enter EC gamma: ");

    scanf(
        "%lf",
        &gammaEC);


    printf(
        "Enter ED gamma: ");

    scanf(
        "%lf",
        &gammaED);


    printf(
        "Enter EE gamma: ");

    scanf(
        "%lf",
        &gammaEE);


    if (teEA <= 0 ||
        teEB <= 0 ||
        teEC <= 0 ||
        teED <= 0 ||
        teEE <= 0 ||
        gammaEA < 0 ||
        gammaEB < 0 ||
        gammaEC < 0 ||
        gammaED < 0 ||
        gammaEE < 0)
    {
        printf(
            "Invalid TE or gamma values.\n");

        return 1;
    }


    printf(
        "\nEnter number of path points k: ");

    scanf(
        "%d",
        &k);


    if (k <= 1 ||
        k > MAX_POINTS)
    {
        printf(
            "k must be 2-%d.\n",
            MAX_POINTS);

        return 1;
    }


    printf(
        "Enter gun jam iteration t: ");

    scanf(
        "%d",
        &jamIteration);


    if (jamIteration < 1 ||
        jamIteration >= k)
    {
        printf(
            "t must be > 0 and < k.\n");

        return 1;
    }


    printf(
        "Enter theta minimum after jam (1-29): ");

    scanf(
        "%lf",
        &jamMinAngle);


    if (jamMinAngle <= 0 ||
        jamMinAngle >= 30)
    {
        printf(
            "Invalid jam angle.\n");

        return 1;
    }


    /*
       Generate Escort ships
    */

    for (int i = 0; i < N; i++)
    {
        generateEscort(
            &original[i],
            i + 1,
            D,
            battleship.maxVelocity,

            teEA,
            teEB,
            teEC,
            teED,
            teEE,

            gammaEA,
            gammaEB,
            gammaEC,
            gammaED,
            gammaEE);
    }


    /*
       Generate same path
    */

    generatePath(
        path,
        k,
        D);


    /*
       Save initial setup
    */

    saveInitial(
        battleship,
        original,
        N,
        path,
        k,
        D,
        seed);


    /*
       Separate copies
    */

    for (int i = 0; i < N; i++)
    {
        stationary[i] =
            original[i];

        simulation1[i] =
            original[i];

        simulation2[i] =
            original[i];
    }


    /*
       Stationary Part 1-C style
    */

    runStationary(
        battleship,
        stationary,
        N);


    /*
       Path Simulation 1
    */

    runPathSimulation(
        1,
        battleship,
        simulation1,
        N,
        path,
        k,
        jamIteration,
        jamMinAngle);


    /*
       Path Simulation 2
    */

    runPathSimulation(
        2,
        battleship,
        simulation2,
        N,
        path,
        k,
        jamIteration,
        jamMinAngle);


    printf(
        "\n=================================\n");

    printf(
        " PART 2-C COMPLETED\n");

    printf(
        "=================================\n");


    printf(
        "\nFiles created:\n");

    printf(
        "part2c_initial.txt\n");

    printf(
        "part2c_stationary.txt\n");

    printf(
        "part2c_path_sim1.txt\n");

    printf(
        "part2c_path_sim2.txt\n");


    return 0;
}
