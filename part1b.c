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

} EscortShip;


typedef struct
{
    char type;

    double x;
    double y;

    double maxVelocity;

    int alive;

} Battleship;


typedef struct
{
    double x;
    double y;

} PathPoint;


/* ---------------- RANDOM NUMBER ---------------- */

double randomDouble(double min, double max)
{
    double r = (double)rand() / RAND_MAX;

    return min + r * (max - min);
}


/* ---------------- DEGREE TO RADIAN ---------------- */

double degreeToRadian(double degree)
{
    return degree * PI / 180.0;
}


/* ---------------- DISTANCE ---------------- */

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


/* ---------------- PROJECTILE RANGE ---------------- */

double projectileRange(
    double velocity,
    double angle)
{
    double radian =
        degreeToRadian(angle);

    return
        (velocity * velocity *
         sin(2.0 * radian)) / G;
}


/* ---------------- ESCORT ATTACK RANGE ---------------- */

void escortAttackRange(
    EscortShip ship,
    double *minimumRange,
    double *maximumRange)
{
    double thetaLow =
        degreeToRadian(ship.minAngle);

    double thetaHigh =
        degreeToRadian(ship.maxAngle);


    double sinLow =
        sin(2.0 * thetaLow);

    double sinHigh =
        sin(2.0 * thetaHigh);


    double minimumSin;

    if (sinLow < sinHigh)
        minimumSin = sinLow;
    else
        minimumSin = sinHigh;


    double maximumSin;


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


    *minimumRange =
        (ship.minVelocity *
         ship.minVelocity *
         minimumSin) / G;


    *maximumRange =
        (ship.maxVelocity *
         ship.maxVelocity *
         maximumSin) / G;
}


/* ---------------- GENERATE ESCORT ---------------- */

void generateEscort(
    EscortShip *ship,
    int id,
    int D,
    double battleshipVmax)
{
    ship->id = id;

    ship->alive = 1;


    ship->x =
        randomDouble(0, D);

    ship->y =
        randomDouble(0, D);


    int type =
        rand() % 5;


    double angleRange;


    switch (type)
    {
        case 0:

            strcpy(ship->type, "EA");

            ship->impactPower = 0.08;

            angleRange = 20;

            ship->maxVelocity =
                1.2 * battleshipVmax;

            break;


        case 1:

            strcpy(ship->type, "EB");

            ship->impactPower = 0.06;

            angleRange = 30;

            ship->maxVelocity =
                randomDouble(
                    0.55 * battleshipVmax,
                    0.95 * battleshipVmax);

            break;


        case 2:

            strcpy(ship->type, "EC");

            ship->impactPower = 0.07;

            angleRange = 25;

            ship->maxVelocity =
                randomDouble(
                    0.55 * battleshipVmax,
                    0.95 * battleshipVmax);

            break;


        case 3:

            strcpy(ship->type, "ED");

            ship->impactPower = 0.05;

            angleRange = 50;

            ship->maxVelocity =
                randomDouble(
                    0.55 * battleshipVmax,
                    0.95 * battleshipVmax);

            break;


        default:

            strcpy(ship->type, "EE");

            ship->impactPower = 0.04;

            angleRange = 70;

            ship->maxVelocity =
                randomDouble(
                    0.55 * battleshipVmax,
                    0.95 * battleshipVmax);

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


/* ---------------- GENERATE PATH ---------------- */

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


/* ---------------- SAVE INITIAL DATA ---------------- */

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
            "part1b_initial.txt",
            "w");


    if (fp == NULL)
    {
        printf(
            "Cannot create initial file.\n");

        return;
    }


    fprintf(
        fp,
        "PART 1-B INITIAL CONDITIONS\n");

    fprintf(
        fp,
        "===========================\n\n");


    fprintf(
        fp,
        "Battlefield: %d x %d\n\n",
        D,
        D);


    fprintf(
        fp,
        "Battleship Type: %c\n",
        battleship.type);

    fprintf(
        fp,
        "Maximum Velocity: %.2f\n\n",
        battleship.maxVelocity);


    fprintf(
        fp,
        "ESCORT SHIPS\n");
    fprintf(
        fp,
        "------------\n");


    for (int i = 0; i < N; i++)
    {
        fprintf(
            fp,
            "E%d %s\n",
            escorts[i].id,
            escorts[i].type);

        fprintf(
            fp,
            "Position: %.2f %.2f\n",
            escorts[i].x,
            escorts[i].y);

        fprintf(
            fp,
            "Vmin: %.2f\n",
            escorts[i].minVelocity);

        fprintf(
            fp,
            "Vmax: %.2f\n",
            escorts[i].maxVelocity);

        fprintf(
            fp,
            "Angle: %.2f - %.2f\n\n",
            escorts[i].minAngle,
            escorts[i].maxAngle);
    }


    fprintf(
        fp,
        "\nBATTLESHIP PATH\n");

    fprintf(
        fp,
        "---------------\n");


    for (int i = 0; i < k; i++)
    {
        fprintf(
            fp,
            "Point %d = (%.2f, %.2f)\n",
            i + 1,
            path[i].x,
            path[i].y);
    }


    fclose(fp);
}


/* ---------------- RUN ONE SIMULATION ---------------- */

void runSimulation(
    int simulationNumber,
    Battleship battleship,
    EscortShip escorts[],
    int N,
    PathPoint path[],
    int k,
    int jamIteration,
    double jamMinAngle,
    double firingAngles[MAX_POINTS][MAX_ESCORTS])
{
    char fileName[50];


    if (simulationNumber == 1)
    {
        strcpy(
            fileName,
            "part1b_simulation1.txt");
    }
    else
    {
        strcpy(
            fileName,
            "part1b_simulation2.txt");
    }


    FILE *fp =
        fopen(fileName, "w");


    if (fp == NULL)
    {
        printf(
            "Cannot create simulation file.\n");

        return;
    }


    fprintf(
        fp,
        "PART 1-B SIMULATION %d\n",
        simulationNumber);

    fprintf(
        fp,
        "========================\n\n");


    battleship.alive = 1;


    for (int point = 0;
         point < k;
         point++)
    {
        battleship.x =
            path[point].x;

        battleship.y =
            path[point].y;


        printf(
            "\n--------------------------------\n");

        printf(
            "Simulation %d - Iteration %d\n",
            simulationNumber,
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
            "B Position: %.2f %.2f\n",
            battleship.x,
            battleship.y);


        /*
            FIRST:
            Check whether Escort ship can destroy B.
        */

        int sinkingEscort = -1;


        for (int i = 0; i < N; i++)
        {
            if (escorts[i].alive == 0)
                continue;


            double distance =
                calculateDistance(
                    battleship.x,
                    battleship.y,
                    escorts[i].x,
                    escorts[i].y);


            double minRange;
            double maxRange;


            escortAttackRange(
                escorts[i],
                &minRange,
                &maxRange);


            if (distance >= minRange &&
                distance <= maxRange)
            {
                sinkingEscort = i;

                break;
            }
        }


        if (sinkingEscort != -1)
        {
            battleship.alive = 0;


            printf(
                "Battleship destroyed by E%d (%s)\n",
                escorts[sinkingEscort].id,
                escorts[sinkingEscort].type);


            fprintf(
                fp,
                "Battleship DESTROYED by E%d (%s)\n",
                escorts[sinkingEscort].id,
                escorts[sinkingEscort].type);


            /*
                Simulation stops immediately.
            */

            break;
        }


        /*
            If Battleship survives,
            attack Escort ships.
        */

        int destroyedThisIteration = 0;


        for (int i = 0; i < N; i++)
        {
            if (escorts[i].alive == 0)
                continue;


            double distance =
                calculateDistance(
                    battleship.x,
                    battleship.y,
                    escorts[i].x,
                    escorts[i].y);


            double angle =
                firingAngles[point][i];


            /*
               Simulation 2:
               After jamIteration,
               angle cannot be below jamMinAngle.
            */

            if (simulationNumber == 2 &&
                (point + 1) > jamIteration)
            {
                if (angle < jamMinAngle)
                {
                    angle =
                        jamMinAngle;
                }
            }


            double range =
                projectileRange(
                    battleship.maxVelocity,
                    angle);


            if (distance <= range)
            {
                escorts[i].alive = 0;

                destroyedThisIteration++;


                printf(
                    "B destroyed E%d (%s)",
                    escorts[i].id,
                    escorts[i].type);

                printf(
                    " | angle %.2f",
                    angle);

                printf(
                    " | range %.2f",
                    range);

                printf(
                    " | distance %.2f\n",
                    distance);


                fprintf(
                    fp,
                    "B destroyed E%d (%s)",
                    escorts[i].id,
                    escorts[i].type);

                fprintf(
                    fp,
                    " angle %.2f",
                    angle);

                fprintf(
                    fp,
                    " range %.2f",
                    range);

                fprintf(
                    fp,
                    " distance %.2f\n",
                    distance);
            }
        }


        if (destroyedThisIteration == 0)
        {
            printf(
                "No Escort ship destroyed in this iteration.\n");

            fprintf(
                fp,
                "No Escort ship destroyed.\n");
        }


        /*
            Check whether all Escort ships
            have been destroyed.
        */

        int aliveCount = 0;


        for (int i = 0; i < N; i++)
        {
            if (escorts[i].alive)
            {
                aliveCount++;
            }
        }


        printf(
            "Escort ships remaining: %d\n",
            aliveCount);


        fprintf(
            fp,
            "Escort ships remaining: %d\n",
            aliveCount);


        if (aliveCount == 0)
        {
            printf(
                "All Escort ships destroyed.\n");

            fprintf(
                fp,
                "All Escort ships destroyed.\n");

            break;
        }
    }


    fprintf(
        fp,
        "\nFINAL STATUS\n");

    fprintf(
        fp,
        "------------\n");


    if (battleship.alive)
    {
        fprintf(
            fp,
            "Battleship: ALIVE\n");
    }
    else
    {
        fprintf(
            fp,
            "Battleship: DESTROYED\n");
    }


    for (int i = 0; i < N; i++)
    {
        fprintf(
            fp,
            "E%d (%s): %s\n",
            escorts[i].id,
            escorts[i].type,

            escorts[i].alive ?
            "ALIVE" :
            "DESTROYED");
    }


    fclose(fp);


    printf(
        "\nSimulation %d results saved to %s\n",
        simulationNumber,
        fileName);
}


/* ================= MAIN ================= */

int main()
{
    int D;
    int N;
    int k;

    unsigned int seed;


    Battleship battleship;


    EscortShip originalEscorts[MAX_ESCORTS];

    EscortShip simulation1Escorts[MAX_ESCORTS];

    EscortShip simulation2Escorts[MAX_ESCORTS];


    PathPoint path[MAX_POINTS];


    double firingAngles[MAX_POINTS][MAX_ESCORTS];


    int jamIteration;

    double jamMinAngle;


    printf(
        "\n=================================\n");

    printf(
        " ADVANCED NAVAL BATTLE SIMULATOR\n");

    printf(
        "           PART 1-B\n");

    printf(
        "=================================\n\n");


    /* Random seed */

    printf(
        "Enter random seed: ");

    scanf(
        "%u",
        &seed);

    srand(seed);


    /* Battlefield */

    printf(
        "Enter battlefield size D: ");

    scanf(
        "%d",
        &D);


    if (D <= 0)
    {
        printf(
            "Invalid battlefield size.\n");

        return 1;
    }


    /* Escort count */

    printf(
        "Enter number of Escort ships: ");

    scanf(
        "%d",
        &N);


    if (N <= 0 ||
        N > MAX_ESCORTS)
    {
        printf(
            "Escort count must be 1-%d.\n",
            MAX_ESCORTS);

        return 1;
    }


    /* Battleship type */

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


    /* Battleship shell velocity */

    printf(
        "Enter Battleship maximum shell velocity: ");

    scanf(
        "%lf",
        &battleship.maxVelocity);


    if (battleship.maxVelocity <= 0)
    {
        printf(
            "Invalid velocity.\n");

        return 1;
    }


    battleship.alive = 1;


    /* Number of path points */

    printf(
        "Enter number of path points k: ");

    scanf(
        "%d",
        &k);


    if (k <= 0 ||
        k > MAX_POINTS)
    {
        printf(
            "k must be between 1 and %d.\n",
            MAX_POINTS);

        return 1;
    }


    /*
       Generate Escort ships once.

       These are the SAME initial conditions
       for Simulation 1 and Simulation 2.
    */

    for (int i = 0; i < N; i++)
    {
        generateEscort(
            &originalEscorts[i],
            i + 1,
            D,
            battleship.maxVelocity);
    }


    /*
       Generate Battleship path once.
    */

    generatePath(
        path,
        k,
        D);


    /*
       Generate Battleship firing angles once.

       Both simulations use the same values.
    */

    for (int point = 0;
         point < k;
         point++)
    {
        for (int i = 0;
             i < N;
             i++)
        {
            firingAngles[point][i] =
                randomDouble(
                    1.0,
                    89.0);
        }
    }


    /*
       Setup Simulation 2 jam.
    */

    printf(
        "\nEnter gun jam iteration t: ");

    scanf(
        "%d",
        &jamIteration);


    if (jamIteration < 1 ||
        jamIteration >= k)
    {
        printf(
            "t must be greater than 0 and less than k.\n");

        return 1;
    }


    printf(
        "Enter theta minimum after jam (1-29 degrees): ");

    scanf(
        "%lf",
        &jamMinAngle);


    if (jamMinAngle <= 0 ||
        jamMinAngle >= 30)
    {
        printf(
            "Theta minimum must be between 0 and 30.\n");

        return 1;
    }


    /*
       Save initial conditions.
    */

    saveInitial(
        battleship,
        originalEscorts,
        N,
        path,
        k,
        D);


    /*
       COPY original Escorts.

       Simulation 1 and Simulation 2
       must start from same conditions.
    */

    for (int i = 0; i < N; i++)
    {
        simulation1Escorts[i] =
            originalEscorts[i];

        simulation2Escorts[i] =
            originalEscorts[i];
    }


    printf(
        "\n\n===== SIMULATION 1 =====\n");


    runSimulation(
        1,
        battleship,
        simulation1Escorts,
        N,
        path,
        k,
        jamIteration,
        jamMinAngle,
        firingAngles);


    printf(
        "\n\n===== SIMULATION 2 =====\n");


    printf(
        "Gun becomes restricted after iteration %d\n",
        jamIteration);

    printf(
        "New minimum angle = %.2f degrees\n",
        jamMinAngle);


    runSimulation(
        2,
        battleship,
        simulation2Escorts,
        N,
        path,
        k,
        jamIteration,
        jamMinAngle,
        firingAngles);


    printf(
        "\n=================================\n");

    printf(
        "PART 1-B COMPLETED\n");

    printf(
        "=================================\n");


    printf(
        "\nFiles created:\n");

    printf(
        "part1b_initial.txt\n");

    printf(
        "part1b_simulation1.txt\n");

    printf(
        "part1b_simulation2.txt\n");


    return 0;
}
