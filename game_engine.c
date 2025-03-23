#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "includes/game_engine.h"
#include "includes/inventor.h"

void start_game(Section sections[], int section_count) {
    char replay_choice;

    do {
        int current_section_id = 0; // Réinitialise la section actuelle à la première
        Inventory player_inventory;
        init_inventory(&player_inventory); // Réinitialise l'inventaire

        // Boucle principale pour jouer une partie
        while (current_section_id != -1) {
            current_section_id = play_section(&sections[current_section_id], &player_inventory);
        }

        // Message de fin et demande de rejouer
        printf("\n=== GAME OVER ===\n");
        printf("Voulez-vous rejouer ? (o/n) : ");
        scanf(" %c", &replay_choice);

        // Nettoyage du buffer d'entrée pour éviter des erreurs de lecture
        while (getchar() != '\n');

    } while (replay_choice == 'o' || replay_choice == 'O'); // Redémarre si le joueur entre "o" ou "O"

    printf("Merci d'avoir joué !\n");
}


int play_section(Section *section, Inventory *inventory) {
    narration(section->narration);
    collect_items(section, inventory);

    if (section->dialogue_count > 0) {
        dialogues(section->dialogues, section->dialogue_count);
    }
    if (section->choice_count > 0) {
        return process_choices(section->choices, section->choice_count, inventory);
    }
    if (section->is_ending) {
        ending(section->narration);
        return -1;
    }
    return -1;
}


void narration(const char *narration) {
    printf("\n--- Narrateur ---\n");
    typewriter_effect(narration, 50);
    printf("\n"); // Séparateur visuel
}

void dialogues(Dialogue dialogues[], int dialogue_count) {
    for (int i = 0; i < dialogue_count; i++) {
        typewriter_effect(dialogues[i].character, 50);
        typewriter_effect(dialogues[i].text, 50);
    }
}


int process_choices(Choice choices[], int choice_count, Inventory *inventory) {
    printf("\n--- Vos Choix  ---\n");
    for (int i = 0; i < choice_count; i++) {
        printf("%d. %s\n", i + 1, choices[i].text);
    }
    printf("%d. Consulter l'inventaire\n", choice_count + 1);

    int choice;
    printf("Votre choix : ");
    scanf("%d", &choice);

    while (choice < 1 || choice > choice_count + 1) {
        printf("Choix invalide. Veuillez réessayer : ");
        scanf("%d", &choice);
    }

    if (choice == choice_count + 1) {
        display_inventory(inventory); // Appelle la fonction pour afficher l'inventaire
        return process_choices(choices, choice_count, inventory); // Relance les choix
    }

    // Appliquer les conséquences du choix
    apply_consequence(choices[choice - 1].consequence_id, inventory,
                      choices[choice - 1].item_add, choices[choice - 1].item_remove);

    return choices[choice - 1].next_section_id;
}


void apply_consequence(int consequence_id, Inventory *inventory, const char *item_add, const char *item_remove) {
    if (consequence_id == 1 && item_add && strlen(item_add) > 0) {
        if (add_item(inventory, item_add)) {
            printf("Vous avez obtenu : %s\n", item_add);
        }
    } else if (consequence_id == 2 && item_remove && strlen(item_remove) > 0) {
        if (remove_item(inventory, item_remove)) {
            printf("Vous avez utilisé : %s\n", item_remove);
        } else {
            printf("Erreur : %s n'est pas dans votre inventaire.\n", item_remove);
        }
    }
}


int ending(const char *ending_text) {
    printf("\n=== Fin de l'histoire ===\n");
    typewriter_effect(ending_text, 50);

    char replay_choice;
    printf("\n=== GAME OVER ===\n");
    printf("Voulez-vous rejouer ? (o/n) : ");
    scanf(" %c", &replay_choice);

    // Nettoyer le buffer d'entrée pour éviter des erreurs de lecture
    while (getchar() != '\n');

    return (replay_choice == 'o' || replay_choice == 'O') ? 1 : 0;
}


void typewriter_effect(const char *text, int delay_ms) {
    while (*text) {
        usleep(delay_ms * 1000);
        putchar(*text++);
        fflush(stdout);
    }
    putchar('\n');
}
void collect_items(Section *section, Inventory *inventory) {
    for (int i = 0; i < section->item_count; i++) {
        if (section->items[i].type == ITEM_ADD) {
            add_item(inventory, section->items[i].name);
//            printf("Vous avez obtenu : %s\n", section->items[i].name);
        } else if (section->items[i].type == ITEM_REMOVE) {
            if (remove_item(inventory, section->items[i].name)) {
//                printf("Vous avez utilisé : %s\n", section->items[i].name);
            } else {
                printf("Erreur : %s n'est pas dans votre inventaire.\n", section->items[i].name);
            }
        }
    }
}

