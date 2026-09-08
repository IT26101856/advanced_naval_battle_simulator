#include <stdio.h>
#include <stdlib.h>

void showMainMenu();
void startSimulationMenu();
void showInstructions();
void showStatistics();
void runPart(int partNumber);

int main()
{
    int choice;

    while (1)
    {
        showMainMenu();

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                startSimulationMenu();
                break;

            case 2:
                showInstructions();
                break;

            case 3:
                showStatistics();
                break;

            case 4:
                printf("\nExiting program...\n");
                printf("Thank you!\n");
                return 0;

            default:
                printf("\nInvalid choice. Try again.\n");
        }
    }

    return 0;
}


/* =========================
   MAIN MENU
   ========================= */

void showMainMenu()
{
    printf("\n====================================\n");
    printf("   ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("====================================\n");

    printf("1. Start Simulation\n");
    printf("2. Instructions\n");
    printf("3. Statistics\n");
    printf("4. Exit\n");

    printf("====================================\n");
}


/* =========================
   START SIMULATION MENU
   ========================= */

void startSimulationMenu()
{
    int choice;

    while (1)
    {
        printf("\n====================================\n");
        printf("         SIMULATION MENU\n");
        printf("====================================\n");

        printf("1. Part 1-A\n");
        printf("2. Part 1-B\n");
        printf("3. Part 1-C\n");
        printf("4. Part 2-A\n");
        printf("5. Part 2-B\n");
        printf("6. Part 2-C\n");
        printf("7. Return to Main Menu\n");

        printf("====================================\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice >= 1 && choice <= 6)
        {
            runPart(choice);
        }
        else if (choice == 7)
        {
            break;
        }
        else
        {
            printf("Invalid choice.\n");
        }
    }
}


/* =========================
   RUN SELECTED PART
   ========================= */

void runPart(int partNumber)
{
    printf("\nStarting simulation...\n\n");

    switch (partNumber)
    {
        case 1:
            system("./part1a.o");
            break;

        case 2:
            system("./part1b.o");
            break;

        case 3:
            system("./part1c.o");
            break;

        case 4:
            system("./part2a.0");
            break;

        case 5:
            system("./part2b.o");
            break;

        case 6:
            system("./part2c.o");
            break;
    }

    printf("\nSimulation completed.\n");
}


/* =========================
   INSTRUCTIONS
   ========================= */

void showInstructions()
{
    printf("\n====================================\n");
    printf("            INSTRUCTIONS\n");
    printf("====================================\n");

    printf("1. Select Start Simulation.\n");
    printf("2. Select the required assignment part.\n");
    printf("3. Enter battlefield and ship details.\n");
    printf("4. Enter a random seed when requested.\n");
    printf("5. Enter Battleship type U, M, R or S.\n");
    printf("6. Enter firing intervals and gamma values\n");
    printf("   when required by Parts 2-A, 2-B and 2-C.\n");
    printf("7. Simulation results are saved in text files.\n");
    printf("8. Select Statistics to view saved results.\n");

    printf("====================================\n");
}


/* =========================
   SHOW FILE CONTENT
   ========================= */

void displayFile(const char *filename)
{
    FILE *fp;
    char line[500];

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("\nNo saved data found in %s\n", filename);
        return;
    }

    printf("\n------------------------------------\n");
    printf("File: %s\n", filename);
    printf("------------------------------------\n");

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        printf("%s", line);
    }

    fclose(fp);
}


/* =========================
   STATISTICS
   ========================= */

void showStatistics()
{
    int choice;

    while (1)
    {
        printf("\n====================================\n");
        printf("             STATISTICS\n");
        printf("====================================\n");

        printf("1. Part 1-A Results\n");
        printf("2. Part 1-B Simulation 1\n");
        printf("3. Part 1-B Simulation 2\n");
        printf("4. Part 1-C Stationary\n");
        printf("5. Part 2-A Stationary\n");
        printf("6. Part 2-B Stationary\n");
        printf("7. Part 2-C Stationary\n");
        printf("8. Part 2-C Path Simulation 1\n");
        printf("9. Part 2-C Path Simulation 2\n");
        printf("10. Return\n");

        printf("====================================\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                displayFile("part1a_final.txt");
                break;

            case 2:
                displayFile("part1b_simulation1.txt");
                break;

            case 3:
                displayFile("part1b_simulation2.txt");
                break;

            case 4:
                displayFile("part1c_stationary.txt");
                break;

            case 5:
                displayFile("part2a_stationary.txt");
                break;

            case 6:
                displayFile("part2b_stationary.txt");
                break;

            case 7:
                displayFile("part2c_stationary.txt");
                break;

            case 8:
                displayFile("part2c_path_sim1.txt");
                break;

            case 9:
                displayFile("part2c_path_sim2.txt");
                break;

            case 10:
                return;

            default:
                printf("Invalid choice.\n");
        }
    }
}
