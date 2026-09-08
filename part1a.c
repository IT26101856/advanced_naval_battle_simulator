#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define G 9.81
#define PI 3.14159265358979323846
#define MAX_ESCORTS 100

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


/* Generate random double value */
double randomDouble(double min, double max)
{
    double randomValue =
        (double)rand() / (double)RAND_MAX;

    return min + randomValue * (max - min);
}


/* Convert degree to radian */
double degreeToRadian(double degree)
{
    return degree * PI / 180.0;
}


/* Calculate distance between two ships */
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


/*
   Maximum attack range of Battleship.

   Battleship can use angles from 0 to 90 degrees.
   Maximum projectile range occurs at 45 degrees.
*/
double battleshipMaxRange(double velocity)
{
    return (velocity * velocity) / G;
}


/*
   Calculate minimum and maximum attack range
   of an Escort ship.
*/
void escortAttackRange(
    EscortShip ship,
    double *minimumRange,
    double *maximumRange)
{
    double thetaL =
        degreeToRadian(ship.minAngle);

    double thetaH =
        degreeToRadian(ship.maxAngle);

    double sinLow =
        sin(2.0 * thetaL);

    double sinHigh =
        sin(2.0 * thetaH);

    double minimumSin;

    if (sinLow < sinHigh)
        minimumSin = sinLow;
    else
        minimumSin = sinHigh;


    double maximumSin;

    /*
       Projectile range is maximum at 45 degrees.
    */
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


/*
   Calculate flight time of Battleship shell
   using maximum velocity and lower-angle solution.
*/
double calculateHitTime(
    double distance,
    double velocity)
{
    if (distance <= 0.0)
        return 0.0;

    double value =
        (G * distance) /
        (velocity * velocity);

    if (value > 1.0)
        return -1.0;

    double angle =
        0.5 * asin(value);

    double time =
        (2.0 *
         velocity *
         sin(angle)) / G;

    return time;
}


/* Randomly generate one Escort ship */
void generateEscort(
    EscortShip *ship,
    int id,
    int battlefieldSize,
    double battleshipMaxVelocity)
{
    ship->id = id;

    ship->alive = 1;

    ship->x =
        randomDouble(
            0.0,
            battlefieldSize);

    ship->y =
        randomDouble(
            0.0,
            battlefieldSize);


    int type =
        rand() % 5;


    double angleRange = 0.0;


    switch (type)
    {
        case 0:

            strcpy(ship->type, "EA");

            ship->impactPower = 0.08;

            angleRange = 20.0;

            /*
               According to the assignment table,
               EA maximum velocity = 1.2 * B Vmax
            */
            ship->maxVelocity =
                1.2 *
                battleshipMaxVelocity;

            ship->minVelocity =
                randomDouble(
                    0.25 *
                    ship->maxVelocity,

                    0.60 *
                    ship->maxVelocity);

            break;


        case 1:

            strcpy(ship->type, "EB");

            ship->impactPower = 0.06;

            angleRange = 30.0;

            ship->maxVelocity =
                randomDouble(
                    0.55 *
                    battleshipMaxVelocity,

                    0.95 *
                    battleshipMaxVelocity);

            ship->minVelocity =
                randomDouble(
                    0.25 *
                    ship->maxVelocity,

                    0.70 *
                    ship->maxVelocity);

            break;


        case 2:

            strcpy(ship->type, "EC");

            ship->impactPower = 0.07;

            angleRange = 25.0;

            ship->maxVelocity =
                randomDouble(
                    0.55 *
                    battleshipMaxVelocity,

                    0.95 *
                    battleshipMaxVelocity);

            ship->minVelocity =
                randomDouble(
                    0.25 *
                    ship->maxVelocity,

                    0.70 *
                    ship->maxVelocity);

            break;


        case 3:

            strcpy(ship->type, "ED");

            ship->impactPower = 0.05;

            angleRange = 50.0;

            ship->maxVelocity =
                randomDouble(
                    0.55 *
                    battleshipMaxVelocity,

                    0.95 *
                    battleshipMaxVelocity);

            ship->minVelocity =
                randomDouble(
                    0.25 *
                    ship->maxVelocity,

                    0.70 *
                    ship->maxVelocity);

            break;


        default:

            strcpy(ship->type, "EE");

            ship->impactPower = 0.04;

            angleRange = 70.0;

            ship->maxVelocity =
                randomDouble(
                    0.55 *
                    battleshipMaxVelocity,

                    0.95 *
                    battleshipMaxVelocity);

            ship->minVelocity =
                randomDouble(
                    0.25 *
                    ship->maxVelocity,

                    0.70 *
                    ship->maxVelocity);

            break;
    }


    /*
       Random minimum angle.

       max angle =
       minimum angle + angle range

       Keep maximum angle <= 90.
    */

    ship->minAngle =
        randomDouble(
            1.0,
            90.0 - angleRange);

    ship->maxAngle =
        ship->minAngle +
        angleRange;
}


/* Save initial battlefield data */
void saveInitialConditions(
    Battleship battleship,
    EscortShip escorts[],
    int numberOfEscorts,
    int battlefieldSize)
{
    FILE *file =
        fopen(
            "part1a_initial.txt",
            "w");

    if (file == NULL)
    {
        printf(
            "Error creating initial file.\n");

        return;
    }


    fprintf(
        file,
        "PART 1-A INITIAL CONDITIONS\n");

    fprintf(
        file,
        "===========================\n\n");


    fprintf(
        file,
        "Battlefield size: %d x %d\n\n",
        battlefieldSize,
        battlefieldSize);


    fprintf(
        file,
        "BATTLESHIP\n");

    fprintf(
        file,
        "Type: %c\n",
        battleship.type);

    fprintf(
        file,
        "Position: (%.2f, %.2f)\n",
        battleship.x,
        battleship.y);

    fprintf(
        file,
        "Maximum Velocity: %.2f\n\n",
        battleship.maxVelocity);


    fprintf(
        file,
        "ESCORT SHIPS\n");

    fprintf(
        file,
        "============\n\n");


    for (int i = 0;
         i < numberOfEscorts;
         i++)
    {
        fprintf(
            file,
            "Escort ID: %d\n",
            escorts[i].id);

        fprintf(
            file,
            "Type: %s\n",
            escorts[i].type);

        fprintf(
            file,
            "Position: (%.2f, %.2f)\n",
            escorts[i].x,
            escorts[i].y);

        fprintf(
            file,
            "Minimum Velocity: %.2f\n",
            escorts[i].minVelocity);

        fprintf(
            file,
            "Maximum Velocity: %.2f\n",
            escorts[i].maxVelocity);

        fprintf(
            file,
            "Minimum Angle: %.2f\n",
            escorts[i].minAngle);

        fprintf(
            file,
            "Maximum Angle: %.2f\n",
            escorts[i].maxAngle);

        fprintf(
            file,
            "Impact Power: %.2f\n\n",
            escorts[i].impactPower);
    }


    fclose(file);
}


/* Save final battlefield conditions */
void saveFinalConditions(
    Battleship battleship,
    EscortShip escorts[],
    int numberOfEscorts)
{
    FILE *file =
        fopen(
            "part1a_final.txt",
            "w");

    if (file == NULL)
    {
        printf(
            "Error creating final file.\n");

        return;
    }


    fprintf(
        file,
        "PART 1-A FINAL CONDITIONS\n");

    fprintf(
        file,
        "=========================\n\n");


    fprintf(
        file,
        "Battleship status: %s\n\n",
        battleship.alive ?
        "ALIVE" :
        "DESTROYED");


    fprintf(
        file,
        "Escort Ship Status\n");

    fprintf(
        file,
        "------------------\n");


    for (int i = 0;
         i < numberOfEscorts;
         i++)
    {
        fprintf(
            file,
            "E%d (%s): %s\n",
            escorts[i].id,
            escorts[i].type,

            escorts[i].alive ?
            "ALIVE" :
            "DESTROYED");
    }


    fclose(file);
}


/* Main program */
int main()
{
    int battlefieldSize;

    int numberOfEscorts;

    unsigned int seed;


    Battleship battleship;

    EscortShip escorts[MAX_ESCORTS];


    printf(
        "\n===============================\n");

    printf(
        " ADVANCED NAVAL BATTLE SIMULATOR\n");

    printf(
        "          PART 1-A\n");

    printf(
        "===============================\n\n");


    /* Random seed */

    printf(
        "Enter random seed value: ");

    scanf(
        "%u",
        &seed);

    srand(seed);


    /* Battlefield size */

    printf(
        "Enter battlefield size D: ");

    scanf(
        "%d",
        &battlefieldSize);


    if (battlefieldSize <= 0)
    {
        printf(
            "Invalid battlefield size.\n");

        return 1;
    }


    /* Number of Escort ships */

    printf(
        "Enter number of Escort ships: ");

    scanf(
        "%d",
        &numberOfEscorts);


    if (numberOfEscorts <= 0 ||
        numberOfEscorts > MAX_ESCORTS)
    {
        printf(
            "Number must be between 1 and %d.\n",
            MAX_ESCORTS);

        return 1;
    }


    /* Battleship type */

    printf(
        "\nBattleship Types\n");

    printf(
        "U - USS Iowa\n");

    printf(
        "M - MS King George V\n");

    printf(
        "R - Richelieu\n");

    printf(
        "S - Sovetsky Soyuz-class\n");


    printf(
        "\nEnter Battleship type: ");

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


    /* Battleship maximum velocity */

    printf(
        "Enter Battleship maximum shell velocity: ");

    scanf(
        "%lf",
        &battleship.maxVelocity);


    if (battleship.maxVelocity <= 0)
    {
        printf(
            "Velocity must be greater than zero.\n");

        return 1;
    }


    /* Battleship position */

    printf(
        "Enter Battleship X position: ");

    scanf(
        "%lf",
        &battleship.x);


    printf(
        "Enter Battleship Y position: ");

    scanf(
        "%lf",
        &battleship.y);


    if (battleship.x < 0 ||
        battleship.x > battlefieldSize ||
        battleship.y < 0 ||
        battleship.y > battlefieldSize)
    {
        printf(
            "Battleship position is outside battlefield.\n");

        return 1;
    }


    battleship.alive = 1;


    /*
       Generate Escort ships
    */

    for (int i = 0;
         i < numberOfEscorts;
         i++)
    {
        generateEscort(
            &escorts[i],
            i + 1,
            battlefieldSize,
            battleship.maxVelocity);
    }


    /*
       Save initial conditions
    */

    saveInitialConditions(
        battleship,
        escorts,
        numberOfEscorts,
        battlefieldSize);


    printf(
        "\nInitial conditions saved to part1a_initial.txt\n");


    /*
       Display generated battlefield
    */

    printf(
        "\n--------- BATTLEFIELD ---------\n");

    printf(
        "Battleship %c : (%.2f, %.2f)\n\n",
        battleship.type,
        battleship.x,
        battleship.y);


    for (int i = 0;
         i < numberOfEscorts;
         i++)
    {
        printf(
            "E%d %-2s : (%.2f, %.2f)\n",
            escorts[i].id,
            escorts[i].type,
            escorts[i].x,
            escorts[i].y);
    }


    /*
       STEP 1:
       Check whether any Escort ship
       can attack Battleship.
    */

    int sinkingEscortIndex = -1;


    for (int i = 0;
         i < numberOfEscorts;
         i++)
    {
        double distance =
            calculateDistance(
                escorts[i].x,
                escorts[i].y,
                battleship.x,
                battleship.y);


        double minimumRange;
        double maximumRange;


        escortAttackRange(
            escorts[i],
            &minimumRange,
            &maximumRange);


        printf(
            "\nE%d -> B distance = %.2f",
            escorts[i].id,
            distance);


        printf(
            " | Attack range %.2f - %.2f",
            minimumRange,
            maximumRange);


        if (distance >= minimumRange &&
            distance <= maximumRange)
        {
            printf(
                " | CAN HIT B");


            /*
               Use first / lowest index
               capable Escort as deterministic
               sinking ship.
            */

            if (sinkingEscortIndex == -1)
            {
                sinkingEscortIndex = i;
            }
        }
        else
        {
            printf(
                " | CANNOT HIT B");
        }
    }


    printf("\n");


    /*
       If an Escort can hit Battleship,
       Battleship sinks.
    */

    if (sinkingEscortIndex != -1)
    {
        battleship.alive = 0;


        printf(
            "\n===============================\n");

        printf(
            "BATTLESHIP DESTROYED!\n");

        printf(
            "Escort E%d (%s) sank the Battleship.\n",
            escorts[sinkingEscortIndex].id,
            escorts[sinkingEscortIndex].type);

        printf(
            "===============================\n");


        saveFinalConditions(
            battleship,
            escorts,
            numberOfEscorts);


        printf(
            "\nFinal conditions saved to part1a_final.txt\n");


        return 0;
    }


    /*
       If Battleship survives,
       Battleship attacks all Escorts
       inside its attack range.
    */

    printf(
        "\nBattleship survived Escort attacks.\n");


    double bRange =
        battleshipMaxRange(
            battleship.maxVelocity);


    printf(
        "\nBattleship maximum attack range: %.2f\n",
        bRange);


    FILE *hitFile =
        fopen(
            "part1a_hits.txt",
            "w");


    if (hitFile == NULL)
    {
        printf(
            "Error creating hit file.\n");

        return 1;
    }


    fprintf(
        hitFile,
        "PART 1-A ESCORT SHIPS HIT BY BATTLESHIP\n");

    fprintf(
        hitFile,
        "======================================\n\n");


    int hitCount = 0;

    double battleEndTime = 0.0;


    for (int i = 0;
         i < numberOfEscorts;
         i++)
    {
        double distance =
            calculateDistance(
                battleship.x,
                battleship.y,
                escorts[i].x,
                escorts[i].y);


        if (distance <= bRange)
        {
            escorts[i].alive = 0;

            hitCount++;


            double hitTime =
                calculateHitTime(
                    distance,
                    battleship.maxVelocity);


            printf(
                "Battleship HIT E%d (%s)",
                escorts[i].id,
                escorts[i].type);

            printf(
                " | Distance %.2f",
                distance);

            printf(
                " | Time %.2f seconds\n",
                hitTime);


            fprintf(
                hitFile,
                "Escort ID: %d\n",
                escorts[i].id);

            fprintf(
                hitFile,
                "Type: %s\n",
                escorts[i].type);

            fprintf(
                hitFile,
                "Distance: %.2f\n",
                distance);

            fprintf(
                hitFile,
                "Time to hit: %.2f seconds\n\n",
                hitTime);


            if (hitTime > battleEndTime)
            {
                battleEndTime = hitTime;
            }
        }
        else
        {
            printf(
                "E%d (%s) is outside Battleship range.\n",
                escorts[i].id,
                escorts[i].type);
        }
    }


    fclose(hitFile);


    printf(
        "\n===============================\n");

    printf(
        "PART 1-A RESULT\n");

    printf(
        "===============================\n");

    printf(
        "Battleship: SURVIVED\n");

    printf(
        "Escort ships destroyed: %d\n",
        hitCount);

    printf(
        "Battle end time: %.2f seconds\n",
        battleEndTime);


    saveFinalConditions(
        battleship,
        escorts,
        numberOfEscorts);


    printf(
        "\nResults saved to:\n");

    printf(
        "part1a_initial.txt\n");

    printf(
        "part1a_hits.txt\n");

    printf(
        "part1a_final.txt\n");


    return 0;
}
