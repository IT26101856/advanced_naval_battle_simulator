#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define G 9.81
#define PI 3.14159265358979323846
#define MAX_ESCORTS 100
#define MAX_POINTS 50

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

    int alive;
    int hasFired;

} EscortShip;


typedef struct
{
    char type;

    double x;
    double y;

    double maxVelocity;

    double health;

    double firingInterval;

    int alive;

} Battleship;


typedef struct
{
    double x;
    double y;

} PathPoint;


/* =========================
   RANDOM NUMBER
   ========================= */

double randomDouble(double min, double max)
{
    return min +
           ((double)rand() / RAND_MAX) *
           (max - min);
}


/* =========================
   DEGREE TO RADIAN
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
   PROJECTILE RANGE
   ========================= */

double projectileRange(
    double velocity,
    double angle)
{
    double rad =
        degreeToRadian(angle);

    return
        (velocity * velocity *
         sin(2.0 * rad)) / G;
}


/* =========================
   ESCORT RANGE
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
   CHECK IF B CAN HIT E
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


    /*
       Try angles from minimumAngle
       to below 90 degrees.
    */

    for (double angle = minimumAngle;
         angle < 90.0;
         angle += 0.5)
    {
        double s =
            sin(
                2.0 *
                degreeToRadian(angle));


        if (s <= 0.000001)
            continue;


        /*
           Required velocity to reach
           this distance at this angle.
        */

        double requiredVelocity =
            sqrt(
                (G * distance) / s);


        if (requiredVelocity <=
            battleship.maxVelocity)
        {
            return 1;
        }
    }


    return 0;
}


/* =========================
   GENERATE ESCORT SHIP
   ========================= */

void generateEscort(
    EscortShip *ship,
    int id,
    int D,
    double bVmax)
{
    ship->id = id;

    ship->x =
        randomDouble(0, D);

    ship->y =
        randomDouble(0, D);

    ship->alive = 1;

    ship->hasFired = 0;


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

            angleRange =
                20;

            ship->maxVelocity =
                1.2 * bVmax;

            break;


        case 1:

            strcpy(
                ship->type,
                "EB");

            ship->impactPower =
                0.06;

            angleRange =
                30;

            ship->maxVelocity =
                randomDouble(
                    0.55 * bVmax,
                    0.95 * bVmax);

            break;


        case 2:

            strcpy(
                ship->type,
                "EC");

            ship->impactPower =
                0.07;

            angleRange =
                25;

            ship->maxVelocity =
                randomDouble(
                    0.55 * bVmax,
                    0.95 * bVmax);

            break;


        case 3:

            strcpy(
                ship->type,
                "ED");

            ship->impactPower =
                0.05;

            angleRange =
                50;

            ship->maxVelocity =
                randomDouble(
                    0.55 * bVmax,
                    0.95 * bVmax);

            break;


        default:

            strcpy(
                ship->type,
                "EE");

            ship->impactPower =
                0.04;

            angleRange =
                70;

            ship->maxVelocity =
                randomDouble(
                    0.55 * bVmax,
                    0.95 * bVmax);

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


/* =========================
   GENERATE B PATH
   ========================= */

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


/* =========================
   RESET ESCORT FIRING
   ========================= */

void resetEscortFiring(
    EscortShip escorts[],
    int N)
{
    for (int i = 0;
         i < N;
         i++)
    {
        escorts[i].hasFired = 0;
    }
}


/* =========================
   COUNT ALIVE ESCORTS
   ========================= */

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


/* =========================
   ESCORTS ATTACK B

   Part 2-A:
   Each E can still fire only once.
   ========================= */

void escortAttacks(
    Battleship *battleship,
    EscortShip escorts[],
    int N,
    FILE *fp)
{
    for (int i = 0;
         i < N;
         i++)
    {
        if (!escorts[i].alive)
            continue;


        if (escorts[i].hasFired)
            continue;


        double distance =
            calculateDistance(
                escorts[i].x,
                escorts[i].y,
                battleship->x,
                battleship->y);


        double minRange;
        double maxRange;


        escortAttackRange(
            escorts[i],
            &minRange,
            &maxRange);


        if (distance >= minRange &&
            distance <= maxRange)
        {
            escorts[i].hasFired = 1;


            battleship->health -=
                escorts[i].impactPower;


            if (battleship->health < 0)
                battleship->health = 0;


            printf(
                "E%d (%s) hit B | Damage %.0f%% | Health %.2f%%\n",
                escorts[i].id,
                escorts[i].type,
                escorts[i].impactPower * 100,
                battleship->health * 100);


            fprintf(
                fp,
                "E%d (%s) hit B | Damage %.0f%% | Health %.2f%%\n",
                escorts[i].id,
                escorts[i].type,
                escorts[i].impactPower * 100,
                battleship->health * 100);


            if (battleship->health <= 0)
            {
                battleship->alive = 0;

                printf(
                    "Battleship destroyed!\n");

                fprintf(
                    fp,
                    "Battleship destroyed!\n");

                return;
            }
        }
    }
}


/* =====================================================
   ATTACK STRATEGY

   Strategy:
   Attack the most dangerous reachable Escort first.

   Higher Impact Power = higher priority.

   Example:
   EA 8%
   EC 7%
   EB 6%
   ED 5%
   EE 4%
   ===================================================== */

int chooseTarget(
    Battleship battleship,
    EscortShip escorts[],
    int N,
    double minimumAngle)
{
    int bestIndex = -1;

    double highestImpact =
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
            highestImpact)
        {
            highestImpact =
                escorts[i].impactPower;

            bestIndex = i;
        }
    }


    return bestIndex;
}


/* =========================
   B ATTACKS WITH TB DELAY
   ========================= */

void battleshipAttackStrategy(
    Battleship *battleship,
    EscortShip escorts[],
    int N,
    double minimumAngle,
    double *currentTime,
    FILE *fp)
{
    int attackNumber = 1;


    while (1)
    {
        int target =
            chooseTarget(
                *battleship,
                escorts,
                N,
                minimumAngle);


        if (target == -1)
        {
            printf(
                "No reachable Escort target.\n");

            fprintf(
                fp,
                "No reachable Escort target.\n");

            break;
        }


        /*
           Fire selected target
        */

        printf(
            "Time %.2f sec : Attack #%d -> E%d (%s)",
            *currentTime,
            attackNumber,
            escorts[target].id,
            escorts[target].type);


        printf(
            " [Impact threat %.0f%%]\n",
            escorts[target].impactPower * 100);


        fprintf(
            fp,
            "Time %.2f sec : Attack #%d -> E%d (%s) [Threat %.0f%%]\n",
            *currentTime,
            attackNumber,
            escorts[target].id,
            escorts[target].type,
            escorts[target].impactPower * 100);


        /*
           In Part 2-A / Part 1-C rule:
           one B hit destroys one E.
        */

        escorts[target].alive = 0;


        printf(
            "E%d destroyed.\n",
            escorts[target].id);


        fprintf(
            fp,
            "E%d destroyed.\n",
            escorts[target].id);


        /*
           Time between consecutive B shots
        */

        *currentTime +=
            battleship->firingInterval;


        attackNumber++;
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
            "part2a_stationary.txt",
            "w");


    if (fp == NULL)
    {
        printf(
            "Cannot create output file.\n");

        return;
    }


    battleship.health = 1.0;
    battleship.alive = 1;


    resetEscortFiring(
        escorts,
        N);


    double currentTime = 0;


    printf(
        "\n=================================\n");

    printf(
        " PART 2-A STATIONARY SIMULATION\n");

    printf(
        "=================================\n");


    fprintf(
        fp,
        "PART 2-A STATIONARY SIMULATION\n\n");


    printf(
        "B firing interval TB = %.2f seconds\n\n",
        battleship.firingInterval);


    /*
       E ships attack once
    */

    escortAttacks(
        &battleship,
        escorts,
        N,
        fp);


    if (battleship.alive)
    {
        printf(
            "\nB ATTACK ORDER\n");

        printf(
            "--------------\n");


        fprintf(
            fp,
            "\nB ATTACK ORDER\n");


        battleshipAttackStrategy(
            &battleship,
            escorts,
            N,
            0,
            &currentTime,
            fp);
    }


    printf(
        "\nFinal B Health = %.2f%%\n",
        battleship.health * 100);


    printf(
        "Total attack time = %.2f seconds\n",
        currentTime);


    fprintf(
        fp,
        "\nFinal B Health = %.2f%%\n",
        battleship.health * 100);

    fprintf(
        fp,
        "Total attack time = %.2f seconds\n",
        currentTime);


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
            "part2a_path_sim1.txt");
    }
    else
    {
        strcpy(
            filename,
            "part2a_path_sim2.txt");
    }


    FILE *fp =
        fopen(filename, "w");


    if (fp == NULL)
    {
        printf(
            "Cannot create output file.\n");

        return;
    }


    battleship.health = 1.0;
    battleship.alive = 1;


    resetEscortFiring(
        escorts,
        N);


    double currentTime = 0;


    printf(
        "\n=================================\n");

    printf(
        " PART 2-A PATH SIMULATION %d\n",
        simulationNumber);

    printf(
        "=================================\n");


    fprintf(
        fp,
        "PART 2-A PATH SIMULATION %d\n\n",
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
            "B Position = %.2f %.2f\n",
            battleship.x,
            battleship.y);


        /*
           E attack B.
           Each E only once.
        */

        escortAttacks(
            &battleship,
            escorts,
            N,
            fp);


        if (!battleship.alive)
        {
            printf(
                "Simulation stopped.\n");

            fprintf(
                fp,
                "Simulation stopped.\n");

            break;
        }


        double minimumAngle = 0;


        /*
           Simulation 2 gun jam.
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


        printf(
            "\nAttack order at Iteration %d:\n",
            point + 1);


        fprintf(
            fp,
            "\nAttack order:\n");


        battleshipAttackStrategy(
            &battleship,
            escorts,
            N,
            minimumAngle,
            &currentTime,
            fp);


        int remaining =
            countAliveEscorts(
                escorts,
                N);


        printf(
            "Remaining E ships: %d\n",
            remaining);


        printf(
            "B Health: %.2f%%\n",
            battleship.health * 100);


        printf(
            "Current Battle Time: %.2f sec\n",
            currentTime);


        fprintf(
            fp,
            "Remaining E ships: %d\n",
            remaining);

        fprintf(
            fp,
            "B Health: %.2f%%\n",
            battleship.health * 100);

        fprintf(
            fp,
            "Current Battle Time: %.2f sec\n",
            currentTime);


        if (remaining == 0)
        {
            printf(
                "All Escort ships destroyed.\n");

            fprintf(
                fp,
                "All Escort ships destroyed.\n");

            break;
        }
    }


    printf(
        "\nFINAL STATUS\n");

    printf(
        "B = %s\n",
        battleship.alive ?
        "ALIVE" :
        "DESTROYED");

    printf(
        "B Health = %.2f%%\n",
        battleship.health * 100);

    printf(
        "Total Battle Time = %.2f sec\n",
        currentTime);


    fprintf(
        fp,
        "\nFINAL STATUS\n");

    fprintf(
        fp,
        "B = %s\n",
        battleship.alive ?
        "ALIVE" :
        "DESTROYED");

    fprintf(
        fp,
        "B Health = %.2f%%\n",
        battleship.health * 100);

    fprintf(
        fp,
        "Total Battle Time = %.2f sec\n",
        currentTime);


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
    int D)
{
    FILE *fp =
        fopen(
            "part2a_initial.txt",
            "w");


    if (fp == NULL)
    {
        return;
    }


    fprintf(
        fp,
        "PART 2-A INITIAL CONDITIONS\n\n");


    fprintf(
        fp,
        "Battlefield = %d x %d\n",
        D,
        D);


    fprintf(
        fp,
        "B Type = %c\n",
        battleship.type);


    fprintf(
        fp,
        "B Vmax = %.2f\n",
        battleship.maxVelocity);


    fprintf(
        fp,
        "B Firing Interval TB = %.2f seconds\n",
        battleship.firingInterval);


    fprintf(
        fp,
        "B Starting Position = %.2f %.2f\n\n",
        battleship.x,
        battleship.y);


    fprintf(
        fp,
        "ESCORT SHIPS\n");


    for (int i = 0;
         i < N;
         i++)
    {
        fprintf(
            fp,
            "\nE%d %s\n",
            escorts[i].id,
            escorts[i].type);


        fprintf(
            fp,
            "Position: %.2f %.2f\n",
            escorts[i].x,
            escorts[i].y);


        fprintf(
            fp,
            "Impact Power: %.2f\n",
            escorts[i].impactPower);


        fprintf(
            fp,
            "Velocity: %.2f - %.2f\n",
            escorts[i].minVelocity,
            escorts[i].maxVelocity);


        fprintf(
            fp,
            "Angle: %.2f - %.2f\n",
            escorts[i].minAngle,
            escorts[i].maxAngle);
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
        "           PART 2-A\n");

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


    if (battleship.maxVelocity <= 0 ||
        battleship.x < 0 ||
        battleship.x > D ||
        battleship.y < 0 ||
        battleship.y > D)
    {
        printf(
            "Invalid Battleship data.\n");

        return 1;
    }


    /*
       NEW PART 2-A INPUT
    */

    printf(
        "Enter Battleship firing interval TB (seconds): ");

    scanf(
        "%lf",
        &battleship.firingInterval);


    if (battleship.firingInterval <= 0)
    {
        printf(
            "Firing interval must be greater than 0.\n");

        return 1;
    }


    battleship.health = 1.0;
    battleship.alive = 1;


    printf(
        "Enter number of path points k: ");

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
            battleship.maxVelocity);
    }


    /*
       Generate common path.
    */

    generatePath(
        path,
        k,
        D);


    /*
       Save initial conditions.
    */

    saveInitial(
        battleship,
        original,
        N,
        path,
        k,
        D);


    /*
       Separate copies so every simulation
       begins from same conditions.
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
       1-A / 1-C style stationary
    */

    runStationary(
        battleship,
        stationaryCopy,
        N);


    /*
       1-B Simulation 1
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
       1-B Simulation 2
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
        "PART 2-A COMPLETED\n");

    printf(
        "=================================\n");


    printf(
        "\nFiles created:\n");

    printf(
        "part2a_initial.txt\n");

    printf(
        "part2a_stationary.txt\n");

    printf(
        "part2a_path_sim1.txt\n");

    printf(
        "part2a_path_sim2.txt\n");


    return 0;
}
