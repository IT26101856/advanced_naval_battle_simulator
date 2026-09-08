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
    int alive;

} Battleship;

typedef struct
{
    double x;
    double y;

} PathPoint;


/* RANDOM NUMBER */
double randomDouble(double min, double max)
{
    return min +
           ((double)rand() / RAND_MAX) *
           (max - min);
}


/* DEGREE TO RADIAN */
double degreeToRadian(double degree)
{
    return degree * PI / 180.0;
}


/* DISTANCE */
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


/* PROJECTILE RANGE */
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


/* ESCORT ATTACK RANGE */
void escortAttackRange(
    EscortShip ship,
    double *minimumRange,
    double *maximumRange)
{
    double low =
        sin(2.0 *
            degreeToRadian(ship.minAngle));

    double high =
        sin(2.0 *
            degreeToRadian(ship.maxAngle));

    double minimumSin;
    double maximumSin;

    if (low < high)
        minimumSin = low;
    else
        minimumSin = high;


    if (ship.minAngle <= 45.0 &&
        ship.maxAngle >= 45.0)
    {
        maximumSin = 1.0;
    }
    else
    {
        if (low > high)
            maximumSin = low;
        else
            maximumSin = high;
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


/* BATTLESHIP MAXIMUM RANGE */
double battleshipMaxRange(double velocity)
{
    return
        (velocity * velocity) / G;
}


/* GENERATE ONE ESCORT SHIP */
void generateEscort(
    EscortShip *ship,
    int id,
    int D,
    double battleshipVmax)
{
    ship->id = id;

    ship->x =
        randomDouble(0, D);

    ship->y =
        randomDouble(0, D);

    ship->alive = 1;
    ship->hasFired = 0;


    int randomType =
        rand() % 5;

    double angleRange;


    switch (randomType)
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


/* GENERATE B PATH */
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


/* RESET ESCORT FIRING */
void resetEscortFiring(
    EscortShip escorts[],
    int N)
{
    for (int i = 0; i < N; i++)
    {
        escorts[i].hasFired = 0;
    }
}


/* ESCORT SHIPS ATTACK B */
void escortAttacks(
    Battleship *battleship,
    EscortShip escorts[],
    int N,
    FILE *fp)
{
    for (int i = 0; i < N; i++)
    {
        if (escorts[i].alive == 0)
            continue;

        if (escorts[i].hasFired == 1)
            continue;


        double distance =
            calculateDistance(
                escorts[i].x,
                escorts[i].y,
                battleship->x,
                battleship->y);


        double minimumRange;
        double maximumRange;


        escortAttackRange(
            escorts[i],
            &minimumRange,
            &maximumRange);


        if (distance >= minimumRange &&
            distance <= maximumRange)
        {
            escorts[i].hasFired = 1;


            battleship->health -=
                escorts[i].impactPower;


            printf(
                "E%d (%s) HIT Battleship",
                escorts[i].id,
                escorts[i].type);

            printf(
                " | Damage %.0f%%",
                escorts[i].impactPower * 100);

            printf(
                " | B Health %.2f%%\n",
                battleship->health * 100);


            if (fp != NULL)
            {
                fprintf(
                    fp,
                    "E%d (%s) HIT B | Damage %.0f%% | B Health %.2f%%\n",
                    escorts[i].id,
                    escorts[i].type,
                    escorts[i].impactPower * 100,
                    battleship->health * 100);
            }


            if (battleship->health <= 0)
            {
                battleship->health = 0;
                battleship->alive = 0;

                printf(
                    "Battleship DESTROYED!\n");

                if (fp != NULL)
                {
                    fprintf(
                        fp,
                        "Battleship DESTROYED!\n");
                }

                return;
            }
        }
    }
}


/* B ATTACKS ESCORT SHIPS */
void battleshipAttacks(
    Battleship *battleship,
    EscortShip escorts[],
    int N,
    double minimumAngle,
    FILE *fp)
{
    for (int i = 0; i < N; i++)
    {
        if (escorts[i].alive == 0)
            continue;


        double distance =
            calculateDistance(
                battleship->x,
                battleship->y,
                escorts[i].x,
                escorts[i].y);


        /*
           Try angles from minimum angle
           up to 90 degrees.
        */

        int canHit = 0;
        double selectedAngle = 0;


        for (double angle = minimumAngle;
             angle <= 90;
             angle += 1.0)
        {
            double range =
                projectileRange(
                    battleship->maxVelocity,
                    angle);


            /*
               Small tolerance for range checking
            */
            if (range >= distance)
            {
                canHit = 1;
                selectedAngle = angle;
                break;
            }
        }


        if (canHit)
        {
            escorts[i].alive = 0;


            printf(
                "B destroyed E%d (%s)",
                escorts[i].id,
                escorts[i].type);

            printf(
                " | Distance %.2f",
                distance);

            printf(
                " | Angle %.2f\n",
                selectedAngle);


            if (fp != NULL)
            {
                fprintf(
                    fp,
                    "B destroyed E%d (%s) | Distance %.2f | Angle %.2f\n",
                    escorts[i].id,
                    escorts[i].type,
                    distance,
                    selectedAngle);
            }
        }
    }
}


/* COUNT ALIVE ESCORT SHIPS */
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


/* PART 1-A STYLE SIMULATION */
void runStationarySimulation(
    Battleship battleship,
    EscortShip escorts[],
    int N)
{
    FILE *fp =
        fopen(
            "part1c_stationary.txt",
            "w");


    printf(
        "\n=================================\n");

    printf(
        " PART 1-C : 1-A STYLE SIMULATION\n");

    printf(
        "=================================\n");


    fprintf(
        fp,
        "PART 1-C STATIONARY SIMULATION\n\n");


    battleship.health = 1.0;
    battleship.alive = 1;


    resetEscortFiring(
        escorts,
        N);


    printf(
        "\nInitial B Health: 100%%\n");


    escortAttacks(
        &battleship,
        escorts,
        N,
        fp);


    if (battleship.alive)
    {
        battleshipAttacks(
            &battleship,
            escorts,
            N,
            0,
            fp);
    }


    double cumulativeImpact =
        1.0 - battleship.health;


    printf(
        "\nCumulative Impact on B: %.2f%%\n",
        cumulativeImpact * 100);


    printf(
        "Final B Health: %.2f%%\n",
        battleship.health * 100);


    fprintf(
        fp,
        "\nCumulative Impact: %.2f%%\n",
        cumulativeImpact * 100);

    fprintf(
        fp,
        "Final B Health: %.2f%%\n",
        battleship.health * 100);


    fclose(fp);
}


/* PART 1-B STYLE SIMULATION */
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
            "part1c_path_sim1.txt");
    }
    else
    {
        strcpy(
            filename,
            "part1c_path_sim2.txt");
    }


    FILE *fp =
        fopen(filename, "w");


    battleship.health = 1.0;
    battleship.alive = 1;


    resetEscortFiring(
        escorts,
        N);


    printf(
        "\n=================================\n");

    printf(
        " PART 1-C PATH SIMULATION %d\n",
        simulationNumber);

    printf(
        "=================================\n");


    fprintf(
        fp,
        "PART 1-C PATH SIMULATION %d\n\n",
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
            "\nIteration %d\n",
            point + 1);

        printf(
            "B Position: (%.2f, %.2f)\n",
            battleship.x,
            battleship.y);


        fprintf(
            fp,
            "\nIteration %d\n",
            point + 1);

        fprintf(
            fp,
            "B Position: %.2f %.2f\n",
            battleship.x,
            battleship.y);


        /*
           Escort ships attack first.
           Each E can attack only once
           during entire simulation.
        */

        escortAttacks(
            &battleship,
            escorts,
            N,
            fp);


        if (battleship.alive == 0)
        {
            printf(
                "Simulation ended at iteration %d\n",
                point + 1);

            break;
        }


        double minimumBAngle = 0;


        /*
           Simulation 2:
           gun jam after t iterations
        */

        if (simulationNumber == 2 &&
            (point + 1) > jamIteration)
        {
            minimumBAngle =
                jamMinAngle;

            printf(
                "Gun jam active. B angle = %.2f - 90 degrees\n",
                jamMinAngle);
        }


        battleshipAttacks(
            &battleship,
            escorts,
            N,
            minimumBAngle,
            fp);


        int remaining =
            countAliveEscorts(
                escorts,
                N);


        printf(
            "Remaining Escorts: %d\n",
            remaining);

        printf(
            "B Health: %.2f%%\n",
            battleship.health * 100);


        fprintf(
            fp,
            "Remaining Escorts: %d\n",
            remaining);

        fprintf(
            fp,
            "B Health: %.2f%%\n",
            battleship.health * 100);


        if (remaining == 0)
        {
            printf(
                "All Escort ships destroyed.\n");

            break;
        }
    }


    double cumulativeImpact =
        1.0 - battleship.health;


    printf(
        "\nFINAL RESULT\n");

    printf(
        "B Status: %s\n",
        battleship.alive ?
        "ALIVE" :
        "DESTROYED");

    printf(
        "B Health: %.2f%%\n",
        battleship.health * 100);

    printf(
        "Cumulative Impact: %.2f%%\n",
        cumulativeImpact * 100);


    fprintf(
        fp,
        "\nFINAL RESULT\n");

    fprintf(
        fp,
        "B Status: %s\n",
        battleship.alive ?
        "ALIVE" :
        "DESTROYED");

    fprintf(
        fp,
        "B Health: %.2f%%\n",
        battleship.health * 100);

    fprintf(
        fp,
        "Cumulative Impact: %.2f%%\n",
        cumulativeImpact * 100);


    fclose(fp);
}


/* SAVE INITIAL CONDITIONS */
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
            "part1c_initial.txt",
            "w");


    fprintf(
        fp,
        "PART 1-C INITIAL CONDITIONS\n\n");


    fprintf(
        fp,
        "Battlefield: %d x %d\n",
        D,
        D);


    fprintf(
        fp,
        "Battleship Type: %c\n",
        battleship.type);

    fprintf(
        fp,
        "Battleship Vmax: %.2f\n",
        battleship.maxVelocity);

    fprintf(
        fp,
        "Battleship Start Position: %.2f %.2f\n\n",
        battleship.x,
        battleship.y);


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
            "Impact Power: %.2f\n",
            escorts[i].impactPower);

        fprintf(
            fp,
            "Velocity: %.2f - %.2f\n",
            escorts[i].minVelocity,
            escorts[i].maxVelocity);

        fprintf(
            fp,
            "Angle: %.2f - %.2f\n\n",
            escorts[i].minAngle,
            escorts[i].maxAngle);
    }


    fprintf(
        fp,
        "\nPATH\n");


    for (int i = 0; i < k; i++)
    {
        fprintf(
            fp,
            "Point %d: %.2f %.2f\n",
            i + 1,
            path[i].x,
            path[i].y);
    }


    fclose(fp);
}


/* ================= MAIN ================= */

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

    EscortShip pathCopy1[MAX_ESCORTS];

    EscortShip pathCopy2[MAX_ESCORTS];


    PathPoint path[MAX_POINTS];


    printf(
        "\n=================================\n");

    printf(
        " ADVANCED NAVAL BATTLE SIMULATOR\n");

    printf(
        "           PART 1-C\n");

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
        "Enter Battleship maximum shell velocity: ");

    scanf(
        "%lf",
        &battleship.maxVelocity);


    printf(
        "Enter Battleship starting X: ");

    scanf(
        "%lf",
        &battleship.x);


    printf(
        "Enter Battleship starting Y: ");

    scanf(
        "%lf",
        &battleship.y);


    if (battleship.x < 0 ||
        battleship.x > D ||
        battleship.y < 0 ||
        battleship.y > D)
    {
        printf(
            "Invalid Battleship position.\n");

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
            "Theta minimum must be between 0 and 30.\n");

        return 1;
    }


    /* Generate Escort ships */

    for (int i = 0; i < N; i++)
    {
        generateEscort(
            &original[i],
            i + 1,
            D,
            battleship.maxVelocity);
    }


    /* Generate path */

    generatePath(
        path,
        k,
        D);


    /* Save original conditions */

    saveInitial(
        battleship,
        original,
        N,
        path,
        k,
        D);


    /*
       Make separate copies so each simulation
       starts with same Escort ships.
    */

    for (int i = 0; i < N; i++)
    {
        stationaryCopy[i] =
            original[i];

        pathCopy1[i] =
            original[i];

        pathCopy2[i] =
            original[i];
    }


    /*
       PART 1-A style simulation
       with percentage damage
    */

    runStationarySimulation(
        battleship,
        stationaryCopy,
        N);


    /*
       PART 1-B Simulation 1
    */

    runPathSimulation(
        1,
        battleship,
        pathCopy1,
        N,
        path,
        k,
        jamIteration,
        jamMinAngle);


    /*
       PART 1-B Simulation 2
       with gun jam
    */

    runPathSimulation(
        2,
        battleship,
        pathCopy2,
        N,
        path,
        k,
        jamIteration,
        jamMinAngle);


    printf(
        "\n=================================\n");

    printf(
        "PART 1-C COMPLETED\n");

    printf(
        "=================================\n");


    printf(
        "\nFiles created:\n");

    printf(
        "part1c_initial.txt\n");

    printf(
        "part1c_stationary.txt\n");

    printf(
        "part1c_path_sim1.txt\n");

    printf(
        "part1c_path_sim2.txt\n");


    return 0;
}
