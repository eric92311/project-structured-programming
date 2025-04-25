#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

// Game constants
#define MAX_ROUNDS 4
#define STEAL_CHANCE 25  // 25% chance to steal
#define TOURNAMENT_ROUNDS 3

typedef struct {
    int score;
    int wins;
    int attempts[4];  // tracks shot attempts by type (0: layup, 1: mid, 2: three, 3: dunk)
    int makes[4];     // tracks shots made by type
    char name[20];    // player name
} Player;

void clear_screen() {
    system("clear || cls");
}

void print_header() {
    clear_screen();
    printf("================================\n");
    printf("   BASKETBALL TOURNAMENT MODE   \n");
    printf("================================\n\n");
}

void print_shot_animation(const char* shot_type) {
    printf("\n%s ", shot_type);
    for (int i = 0; i < 3; i++) {
        printf(".");
        fflush(stdout);
        usleep(400000);
    }
}

int attempt_steal() {
    if ((rand() % 100) < STEAL_CHANCE) {
        printf("DEFENSIVE PLAY! ");
        print_shot_animation("Stealing the ball");
        printf(" STOLEN!\n");
        return 1;
    }
    return 0;
}

int shoot_ball(int shot_type, Player* player) {
    int success_rate, points;
    const char* shot_name;

    switch(shot_type) {
        case 1:
            success_rate = 85; points = 1; shot_name = "Layup"; break;
        case 2:
            success_rate = 65; points = 2; shot_name = "Mid-range shot"; break;
        case 3:
            success_rate = 45; points = 3; shot_name = "Three-pointer"; break;
        case 4:
            success_rate = 70; points = 2; shot_name = "Dunk attempt"; break;
        default:
            return 0;
    }

    player->attempts[shot_type-1]++;
    print_shot_animation(shot_name);

    if ((rand() % 100) < success_rate) {
        printf(" SWISH! +%d points!\n", points);
        player->makes[shot_type-1]++;
        return points;
    } else {
        printf(" CLANK! Missed!\n");
        return 0;
    }
}

void print_stats(Player p) {
    printf("\n%s's Stats:\n", p.name);
    printf("Total Score: %d\n", p.score);

    const char* shot_types[] = {"Layups", "Mid-range", "3-pointers", "Dunks"};
    for (int i = 0; i < 4; i++) {
        if (p.attempts[i] > 0) {
            int percentage = (p.makes[i] * 100) / p.attempts[i];
            printf("%s: %d/%d (%d%%)\n", shot_types[i], p.makes[i], p.attempts[i], percentage);
        }
    }
}

void wait_for_enter() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void halftime_report(Player p1, Player p2) {
    print_header();
    printf("======= HALFTIME REPORT =======\n");
    print_stats(p1);
    print_stats(p2);
    printf("\nPress Enter to continue...");
    wait_for_enter();
}

void play_game(Player* p1, Player* p2) {
    p1->score = p2->score = 0;
    for (int i = 0; i < 4; i++) {
        p1->attempts[i] = p1->makes[i] = 0;
        p2->attempts[i] = p2->makes[i] = 0;
    }

    for (int quarter = 1; quarter <= MAX_ROUNDS; quarter++) {
        print_header();
        printf("\n=== QUARTER %d ===\n", quarter);

        for (int i = 0; i < 2; i++) {
            Player* current = i == 0 ? p1 : p2;
            Player* opponent = i == 0 ? p2 : p1;

            printf("\n%s's Turn (Score: %d)\n", current->name, current->score);
            printf("Choose your shot:\n");
            printf("1. Layup (1pt, 85%%)\n");
            printf("2. Mid-range (2pts, 65%%)\n");
            printf("3. Three-pointer (3pts, 45%%)\n");
            printf("4. Dunk (2pts, 70%%, can be blocked)\n");
            printf("Selection: ");

            int shot_choice;
            if (scanf("%d", &shot_choice) != 1 || shot_choice < 1 || shot_choice > 4) {
                printf("Invalid choice! You lose your turn.\n");
                wait_for_enter();
                continue;
            }

            wait_for_enter(); // clear buffer

            if (shot_choice == 4 && attempt_steal()) {
                printf("%s blocked the dunk!\n", opponent->name);
            } else {
                current->score += shoot_ball(shot_choice, current);
            }
        }

        if (quarter == 2) {
            halftime_report(*p1, *p2);
        }
    }

    // Determine winner of this game
    print_header();
    printf("====== GAME RESULTS ======\n");
    print_stats(*p1);
    print_stats(*p2);

    if (p1->score > p2->score) {
        printf("\n%s WINS THIS GAME!\n", p1->name);
        p1->wins++;
    } else if (p2->score > p1->score) {
        printf("\n%s WINS THIS GAME!\n", p2->name);
        p2->wins++;
    } else {
        printf("\nIT'S A TIE!\n");
    }

    printf("\nPress Enter to continue...");
    wait_for_enter();
}

int main() {
    srand(time(0));

    Player player1 = {0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, ""};
    Player player2 = {0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, ""};

    print_header();
    printf("Enter Player 1 name: ");
    fgets(player1.name, 20, stdin);
    player1.name[strcspn(player1.name, "\n")] = 0;

    printf("Enter Player 2 name: ");
    fgets(player2.name, 20, stdin);
    player2.name[strcspn(player2.name, "\n")] = 0;

    wait_for_enter(); // clean leftover newline

    // Tournament loop
    for (int round = 1; round <= TOURNAMENT_ROUNDS; round++) {
        print_header();
        printf("====== TOURNAMENT ROUND %d ======\n", round);
        printf("%s vs %s\n", player1.name, player2.name);
        printf("Current Wins: %s (%d) - %s (%d)\n",
               player1.name, player1.wins, player2.name, player2.wins);
        printf("\nPress Enter to start the game...");
        wait_for_enter();

        play_game(&player1, &player2);
    }

    // Tournament results
    print_header();
    printf("===== TOURNAMENT FINAL RESULTS =====\n");
    printf("%s: %d wins\n", player1.name, player1.wins);
    printf("%s: %d wins\n", player2.name, player2.wins);

    if (player1.wins > player2.wins) {
        printf("\n%s IS THE TOURNAMENT CHAMPION!\n", player1.name);
    } else if (player2.wins > player1.wins) {
        printf("\n%s IS THE TOURNAMENT CHAMPION!\n", player2.name);
    } else {
        printf("\nTHE TOURNAMENT ENDS IN A TIE!\n");
    }

    printf("\nThanks for playing!\n");
    return 0;
}
