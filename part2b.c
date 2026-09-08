#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define G 9.81
#define PI 3.14159265358979323846

#define MAX_ESCORTS 100
#define MAX_POINTS 50
#define MAX_BATTLE_TIME 300.0


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

    double impactPower;

    double firingInterval;
    double nextFireTime;

    int alive;

} EscortShip;


typedef struct
{
    char type;

    double x;
    double y;

    double maxVelocity;

    double health;

    double firingInterval;
    double nextFireTime;

    int alive;

} Battleship;


typedef struct
{
    double x;
    double y;

} PathPoint;


/* =====================================================
   RANDOM DOUBLE
   ===================================================== */

double randomDouble(double min, double max)
{
    return min +
           ((double)rand() / RAND_MAX) *
           (max - min);
}


/* =====================================================
   DEGREE TO RADIAN
   ===================================================== */

double degreeToRadian(double degree)
{
    return degree * PI / 180.0;
}


/* =====================================================
   DISTANCE
   ===================================================== */

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


/* =====================================================
   ESCORT ATTACK RANGE
   ===================================================== */

void escortAttackRange(
    EscortShip ship,
    double *minimumRange,
    double *maximumRange)
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


    double minSin;
    double maxSin;


    if (sinLow < sinHigh)
        minSin = sinLow;
    else
        minSin = sinHigh;


    /*
       Projectile maximum horizontal range
       happens at 45 degrees.
    */

    if (ship.minAngle <= 45.0 &&
        ship.maxAngle >= 45.0)
    {
        maxSin = 1.0;
    }
    else
    {
        if (sinLow > sinHigh)
            maxSin = sinLow;
        else
            maxSin = sinHigh;
    }


    *minimumRange =
        (ship.minVelocity *
         ship.minVelocity *
         minSin) / G;


    *maximumRange =
        (ship.maxVelocity *
         ship.maxVelocity *
         maxSin) / G;
}


/* =====================================================
   CHECK WHETHER ESCORT CAN HIT B
   ===================================================== */

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


    if (distance >= minRange &&
        distance <= maxRange)
    {
        return 1;
    }


    return 0;
}


/* =====================================================
   CHECK WHETHER B CAN HIT E
   ===================================================== */

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


    /*
       Try possible angles.

       If required shell velocity is
       <= B maximum shell velocity,
       B can hit the Escort.
    */

    for (double angle = minimumAngle;
         angle < 90.0;
         angle += 0.5)
    {
        double sinValue =
            sin(
                2.0 *
                degreeToRadian(angle));


        if (sinValue <= 0.000001)
            continue;


        double requiredVelocity =
            sqrt(
                (G * distance) /
                sinValue);


        if (requiredVelocity <=
            battleship.maxVelocity)
        {
            return 1;
        }
    }


    return 0;
}


/* =====================================================
   GENERATE ESCORT
   ===================================================== */

void generateEscort(
    EscortShip *ship,
    int id,
    int D,
    double bVmax,
    double teEA,
    double teEB,
    double teEC,
    double teED,
    double teEE)
{
    ship->id = id;

    ship->x =
        randomDouble(0, D);

    ship->y =
        randomDouble(0, D);

    ship->alive = 1;

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

            ship->impactPower =
                0.08;

            angleRange = 20;

            ship->maxVelocity =
                1.2 * bVmax;

            ship->firingInterval =
                teEA;

            break;


        case 1:

            strcpy(
                ship->type,
                "EB");

            ship->impactPower =
                0.06;

            angleRange = 30;

            ship->maxVelocity =
                randomDouble(
                    0.55 * bVmax,
                    0.95 * bVmax);

            ship->firingInterval =
                teEB;

            break;


        case 2:

            strcpy(
                ship->type,
                "EC");

            ship->impactPower =
                0.07;

            angleRange = 25;

            ship->maxVelocity =
                randomDouble(
                    0.55 * bVmax,
                    0.95 * bVmax);

            ship->firingInterval =
                teEC;

            break;


        case 3:

            strcpy(
                ship->type,
                "ED");

            ship->impactPower =
                0.05;

            angleRange = 50;

            ship->maxVelocity =
                randomDouble(
                    0.55 * bVmax,
                    0.95 * bVmax);

            ship->firingInterval =
                teED;

            break;


        default:

            strcpy(
                ship->type,
                "EE");

            ship->impactPower =
                0.04;

            angleRange = 70;

            ship->maxVelocity =
                randomDouble(
                    0.55 * bVmax,
                    0.95 * bVmax);

            ship->firingInterval =
                teEE;

            break;
    }


    ship->minVelocity =
        randomDouble(
            0.25 *
            ship->maxVelocity,

            0.65 *
            ship->maxVelocity);


    ship->minAngle =
        randomDouble(
            1,
            90 - angleRange);


    ship->maxAngle =
        ship->minAngle +
        angleRange;
}


/* =====================================================
   GENERATE PATH
   ===================================================== */

void generatePath(
    PathPoint path[],
    int k,
    int D)
{
    for (int i = 0;
         i < k;
         i++)
    {
        path[i].x =
            randomDouble(0, D);

        path[i].y =
            randomDouble(0, D);
    }
}


/* =====================================================
   COUNT ALIVE ESCORTS
   ===================================================== */

int countAliveEscorts(
    EscortShip escorts[],
    int N)
{
    int count = 0;


    for (int i = 0;
         i < N;
         i++)
    {
        if (escorts[i].alive)
            count++;
    }


    return count;
}


/* =====================================================
   RESET FIRE TIMES
   ===================================================== */

void resetFireTimes(
    Battleship *battleship,
    EscortShip escorts[],
    int N)
{
    battleship->nextFireTime =
        0.0;


    for (int i = 0;
         i < N;
         i++)
    {
        escorts[i].nextFireTime =
            0.0;
    }
}


/* =====================================================
   TARGET STRATEGY

   Highest impact power first.
   ===================================================== */

int chooseTarget(
    Battleship battleship,
    EscortShip escorts[],
    int N,
    double minimumAngle)
{
    int bestTarget = -1;

    double highestThreat =
        -1.0;


    for (int i = 0;
         i < N;
         i++)
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


        if (escorts[i].impactPower >
            highestThreat)
        {
            highestThreat =
                escorts[i].impactPower;

            bestTarget = i;
        }
    }


    return bestTarget;
}


/* =====================================================
   PROCESS ESCORT FIRING AT CURRENT TIME
   ===================================================== */

void processEscortFire(
    Battleship *battleship,
    EscortShip escorts[],
    int N,
    double currentTime,
    FILE *fp)
{
    for (int i = 0;
         i < N;
         i++)
    {
        if (!escorts[i].alive)
            continue;


        /*
           An Escort can now fire repeatedly.

           Example:
           TE = 4

           fire at:
           0, 4, 8, 12, 16...
        */

        while (
            escorts[i].nextFireTime
            <= currentTime + 0.0001)
        {
            double fireTime =
                escorts[i].nextFireTime;


            if (escortCanHit(
                    escorts[i],
                    *battleship))
            {
                battleship->health -=
                    escorts[i].impactPower;


                if (battleship->health < 0)
                {
                    battleship->health = 0;
                }


                printf(
                    "Time %.2f : E%d (%s) fires -> B",
                    fireTime,
                    escorts[i].id,
                    escorts[i].type);


                printf(
                    " | Damage %.0f%%",
                    escorts[i].impactPower * 100);


                printf(
                    " | B Health %.2f%%\n",
                    battleship->health * 100);


                fprintf(
                    fp,
                    "Time %.2f : E%d (%s) -> B | Damage %.0f%% | Health %.2f%%\n",
                    fireTime,
                    escorts[i].id,
                    escorts[i].type,
                    escorts[i].impactPower * 100,
                    battleship->health * 100);


                if (battleship->health <= 0)
                {
                    battleship->alive = 0;

                    printf(
                        "Battleship DESTROYED!\n");


                    fprintf(
                        fp,
                        "Battleship DESTROYED!\n");


                    return;
                }
            }


            /*
               Schedule next Escort shot.
            */

            escorts[i].nextFireTime +=
                escorts[i].firingInterval;
        }
    }
}


/* =====================================================
   B FIRE ONE SHOT
   ===================================================== */

int battleshipFire(
    Battleship *battleship,
    EscortShip escorts[],
    int N,
    double minimumAngle,
    double currentTime,
    FILE *fp,
    int *attackNumber)
{
    int target =
        chooseTarget(
            *battleship,
            escorts,
            N,
            minimumAngle);


    if (target == -1)
    {
        return 0;
    }


    printf(
        "Time %.2f : B Attack #%d -> E%d (%s)",
        currentTime,
        *attackNumber,
        escorts[target].id,
        escorts[target].type);


    printf(
        " | Threat %.0f%%\n",
        escorts[target].impactPower * 100);


    fprintf(
        fp,
        "Time %.2f : B Attack #%d -> E%d (%s) | Threat %.0f%%\n",
        currentTime,
        *attackNumber,
        escorts[target].id,
        escorts[target].type,
        escorts[target].impactPower * 100);


    /*
       Part 2-B still uses Part 1-C rule:
       one B shell destroys an E ship.
    */

    escorts[target].alive = 0;


    printf(
        "E%d destroyed.\n",
        escorts[target].id);


    fprintf(
        fp,
        "E%d destroyed.\n",
        escorts[target].id);


    (*attackNumber)++;


    battleship->nextFireTime =
        currentTime +
        battleship->firingInterval;


    return 1;
}


/* =====================================================
   RUN BATTLE AT CURRENT POSITION
   ===================================================== */

void runBattleAtPosition(
    Battleship *battleship,
    EscortShip escorts[],
    int N,
    double minimumAngle,
    double *globalTime,
    FILE *fp)
{
    int attackNumber = 1;


    /*
       Continue until:
       - B destroyed
       - no alive Escorts
       - no reachable target
       - safety time reached
    */

    while (
        battleship->alive &&
        countAliveEscorts(
            escorts,
            N) > 0 &&
        *globalTime < MAX_BATTLE_TIME)
    {
        /*
           Escort ships get firing opportunities
           up to current global time.
        */

        processEscortFire(
            battleship,
            escorts,
            N,
            *globalTime,
            fp);


        if (!battleship->alive)
            break;


        /*
           If it is time for B to fire.
        */

        if (*globalTime + 0.0001 >=
            battleship->nextFireTime)
        {
            int fired =
                battleshipFire(
                    battleship,
                    escorts,
                    N,
                    minimumAngle,
                    *globalTime,
                    fp,
                    &attackNumber);


            /*
               No reachable Escort at this position.
            */

            if (!fired)
            {
                printf(
                    "No reachable Escort target at this position.\n");


                fprintf(
                    fp,
                    "No reachable Escort target at this position.\n");


                break;
            }
        }


        /*
           Move simulation time to next event.

           Normally this becomes the next B firing time.
        */

        double nextTime =
            battleship->nextFireTime;


        /*
           Check if any Escort fires earlier.
        */

        for (int i = 0;
             i < N;
             i++)
        {
            if (!escorts[i].alive)
                continue;


            if (escorts[i].nextFireTime <
                nextTime)
            {
                nextTime =
                    escorts[i].nextFireTime;
            }
        }


        /*
           Avoid getting stuck at same time.
        */

        if (nextTime <=
            *globalTime + 0.0001)
        {
            nextTime =
                *globalTime + 0.01;
        }


        *globalTime =
            nextTime;
    }
}


/* =====================================================
   STATIONARY SIMULATION
   ===================================================== */

void runStationary(
    Battleship battleship,
    EscortShip escorts[],
    int N)
{
    FILE *fp =
        fopen(
            "part2b_stationary.txt",
            "w");


    if (fp == NULL)
    {
        printf(
            "File error.\n");

        return;
    }


    battleship.health = 1.0;
    battleship.alive = 1;


    resetFireTimes(
        &battleship,
        escorts,
        N);


    double time = 0.0;


    printf(
        "\n=================================\n");

    printf(
        " PART 2-B STATIONARY SIMULATION\n");

    printf(
        "=================================\n");


    fprintf(
        fp,
        "PART 2-B STATIONARY SIMULATION\n\n");


    runBattleAtPosition(
        &battleship,
        escorts,
        N,
        0.0,
        &time,
        fp);


    printf(
        "\nFINAL RESULT\n");


    printf(
        "Battleship = %s\n",
        battleship.alive ?
        "ALIVE" :
        "DESTROYED");


    printf(
        "Health = %.2f%%\n",
        battleship.health * 100);


    printf(
        "Battle time = %.2f sec\n",
        time);


    fprintf(
        fp,
        "\nFINAL RESULT\n");


    fprintf(
        fp,
        "Battleship = %s\n",
        battleship.alive ?
        "ALIVE" :
        "DESTROYED");


    fprintf(
        fp,
        "Health = %.2f%%\n",
        battleship.health * 100);


    fprintf(
        fp,
        "Battle time = %.2f sec\n",
        time);


    fclose(fp);
}


/* =====================================================
   PATH SIMULATION
   ===================================================== */

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
            "part2b_path_sim1.txt");
    }
    else
    {
        strcpy(
            filename,
            "part2b_path_sim2.txt");
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


    battleship.health = 1.0;
    battleship.alive = 1;


    resetFireTimes(
        &battleship,
        escorts,
        N);


    double globalTime = 0.0;


    printf(
        "\n=================================\n");

    printf(
        " PART 2-B PATH SIMULATION %d\n",
        simulationNumber);

    printf(
        "=================================\n");


    fprintf(
        fp,
        "PART 2-B PATH SIMULATION %d\n\n",
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
            "\n--------- ITERATION %d ---------\n",
            point + 1);


        printf(
            "B Position = (%.2f, %.2f)\n",
            battleship.x,
            battleship.y);


        fprintf(
            fp,
            "\nITERATION %d\n",
            point + 1);


        fprintf(
            fp,
            "B Position = %.2f %.2f\n",
            battleship.x,
            battleship.y);


        double minimumAngle =
            0.0;


        /*
           Simulation 2:
           gun jam after t iterations
        */

        if (simulationNumber == 2 &&
            (point + 1) > jamIteration)
        {
            minimumAngle =
                jamMinAngle;


            printf(
                "Gun jam active: %.2f - 90 degrees\n",
                jamMinAngle);


            fprintf(
                fp,
                "Gun jam active: %.2f - 90 degrees\n",
                jamMinAngle);
        }


        runBattleAtPosition(
            &battleship,
            escorts,
            N,
            minimumAngle,
            &globalTime,
            fp);


        if (!battleship.alive)
        {
            printf(
                "Battleship destroyed. Simulation stopped.\n");

            break;
        }


        int remaining =
            countAliveEscorts(
                escorts,
                N);


        printf(
            "Remaining Escorts = %d\n",
            remaining);


        printf(
            "B Health = %.2f%%\n",
            battleship.health * 100);


        printf(
            "Current Time = %.2f sec\n",
            globalTime);


        fprintf(
            fp,
            "Remaining Escorts = %d\n",
            remaining);


        fprintf(
            fp,
            "B Health = %.2f%%\n",
            battleship.health * 100);


        fprintf(
            fp,
            "Current Time = %.2f sec\n",
            globalTime);


        if (remaining == 0)
        {
            printf(
                "All Escort ships destroyed.\n");

            break;
        }
    }


    fprintf(
        fp,
        "\nFINAL STATUS\n");


    fprintf(
        fp,
        "Battleship = %s\n",
        battleship.alive ?
        "ALIVE" :
        "DESTROYED");


    fprintf(
        fp,
        "Health = %.2f%%\n",
        battleship.health * 100);


    fprintf(
        fp,
        "Battle Time = %.2f sec\n",
        globalTime);


    printf(
        "\nFINAL STATUS\n");


    printf(
        "Battleship = %s\n",
        battleship.alive ?
        "ALIVE" :
        "DESTROYED");


    printf(
        "Health = %.2f%%\n",
        battleship.health * 100);


    printf(
        "Battle Time = %.2f sec\n",
        globalTime);


    fclose(fp);
}


/* =====================================================
   SAVE INITIAL CONDITIONS
   ===================================================== */

void saveInitial(
    Battleship battleship,
    EscortShip escorts[],
    int N,
    PathPoint path[],
    int k,
    int D)
{
    FILE *fp =
        fopen(
            "part2b_initial.txt",
            "w");


    if (fp == NULL)
        return;


    fprintf(
        fp,
        "PART 2-B INITIAL CONDITIONS\n\n");


    fprintf(
        fp,
        "Battlefield = %d x %d\n",
        D,
        D);


    fprintf(
        fp,
        "Battleship Type = %c\n",
        battleship.type);


    fprintf(
        fp,
        "B Vmax = %.2f\n",
        battleship.maxVelocity);


    fprintf(
        fp,
        "B TB = %.2f sec\n\n",
        battleship.firingInterval);


    fprintf(
        fp,
        "ESCORT SHIPS\n");


    for (int i = 0;
         i < N;
         i++)
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
            "Impact Power = %.2f\n",
            escorts[i].impactPower);


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
            "TE = %.2f sec\n",
            escorts[i].firingInterval);
    }


    fprintf(
        fp,
        "\nPATH\n");


    for (int i = 0;
         i < k;
         i++)
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


/* =====================================================
   MAIN
   ===================================================== */

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


    Battleship battleship;


    EscortShip original[MAX_ESCORTS];

    EscortShip stationaryCopy[MAX_ESCORTS];

    EscortShip simulation1[MAX_ESCORTS];

    EscortShip simulation2[MAX_ESCORTS];


    PathPoint path[MAX_POINTS];


    printf(
        "\n=================================\n");

    printf(
        " ADVANCED NAVAL BATTLE SIMULATOR\n");

    printf(
        "           PART 2-B\n");

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
            "Invalid input.\n");

        return 1;
    }


    printf(
        "\nU - USS Iowa\n");

    printf(
        "M - MS King George V\n");

    printf(
        "R - Richelieu\n");

    printf(
        "S - Sovetsky Soyuz-class\n");


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


    if (battleship.maxVelocity <= 0 ||
        battleship.firingInterval <= 0 ||
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


    /*
       NEW PART 2-B:
       Firing interval for each E type.
    */

    printf(
        "\nEnter EA firing interval TE: ");

    scanf(
        "%lf",
        &teEA);


    printf(
        "Enter EB firing interval TE: ");

    scanf(
        "%lf",
        &teEB);


    printf(
        "Enter EC firing interval TE: ");

    scanf(
        "%lf",
        &teEC);


    printf(
        "Enter ED firing interval TE: ");

    scanf(
        "%lf",
        &teED);


    printf(
        "Enter EE firing interval TE: ");

    scanf(
        "%lf",
        &teEE);


    if (teEA <= 0 ||
        teEB <= 0 ||
        teEC <= 0 ||
        teED <= 0 ||
        teEE <= 0)
    {
        printf(
            "All TE values must be greater than zero.\n");

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
            "k must be between 2 and %d.\n",
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
            "t must be less than k.\n");

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
            "Theta must be between 0 and 30.\n");

        return 1;
    }


    /*
       Generate original Escort ships.
    */

    for (int i = 0;
         i < N;
         i++)
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
            teEE);
    }


    /*
       Common path
    */

    generatePath(
        path,
        k,
        D);


    /*
       Save initial conditions
    */

    saveInitial(
        battleship,
        original,
        N,
        path,
        k,
        D);


    /*
       Same starting conditions
       for all simulations.
    */

    for (int i = 0;
         i < N;
         i++)
    {
        stationaryCopy[i] =
            original[i];

        simulation1[i] =
            original[i];

        simulation2[i] =
            original[i];
    }


    /*
       Stationary simulation
    */

    runStationary(
        battleship,
        stationaryCopy,
        N);


    /*
       Path simulation 1
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
       Path simulation 2
       with gun jam
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
        "PART 2-B COMPLETED\n");

    printf(
        "=================================\n");


    printf(
        "\nFiles created:\n");

    printf(
        "part2b_initial.txt\n");

    printf(
        "part2b_stationary.txt\n");

    printf(
        "part2b_path_sim1.txt\n");

    printf(
        "part2b_path_sim2.txt\n");


    return 0;
}
