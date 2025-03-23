#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes/story.h"
#include <dirent.h> // Pour parcourir un répertoire

int load_story(const char *filename, Section sections[]) {
    FILE *file = fopen("C:/Users/alain/OneDrive/Documents/X3(ESIEA)/Back_to_the_past/stories/story.txt", "r");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        return 0;
    }

    char line[256];
    int section_index = -1;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0';

        if (strncmp(line, "# Section", 9) == 0) {
            section_index++;
            if (section_index >= MAX_SECTIONS) {
                printf("Erreur : Trop de sections dans le fichier.\n");
                fclose(file);
                return 0;
            }
            memset(&sections[section_index], 0, sizeof(Section));
            sections[section_index].id = section_index;
        } else if (strncmp(line, "[Narration]", 11) == 0) {
            char narration[MAX_TEXT_LENGTH] = ""; // Tampon temporaire pour accumuler les lignes
            while (fgets(line, sizeof(line), file)) {
                line[strcspn(line, "\r\n")] = '\0'; // Supprime les sauts de ligne
                if (strncmp(line, "[/Narration]", 12) == 0) {
                    break; // Arrête la lecture à la balise de fin
                }
                strncat(narration, line, MAX_TEXT_LENGTH - strlen(narration) - 1); // Concatène les lignes
                strncat(narration, " ", MAX_TEXT_LENGTH - strlen(narration) - 1); // Ajoute un espace
            }
            strncpy(sections[section_index].narration, narration, MAX_TEXT_LENGTH - 1);
        } else if (strncmp(line, "[Dialogue]", 10) == 0) {
            int dialogue_index = 0;
            while (dialogue_index < MAX_DIALOGUES && fgets(line, sizeof(line), file)) {
                if (line[0] == '-') {
                    char *colon = strchr(line, ':');
                    if (colon) {
                        *colon = '\0';
                        strncpy(sections[section_index].dialogues[dialogue_index].character, line + 2, MAX_NAME_LENGTH - 1);
                        strncpy(sections[section_index].dialogues[dialogue_index].text, colon + 2, MAX_TEXT_LENGTH - 1);
                        dialogue_index++;
                    }
                } else {
                    break;
                }
            }
            sections[section_index].dialogue_count = dialogue_index;
        } else if (strncmp(line, "[Choix]", 7) == 0) {
            int choice_index = 0;
            while (choice_index < MAX_CHOICES && fgets(line, sizeof(line), file)) {
                char *arrow = strstr(line, "->");
                if (arrow) {
                    *arrow = '\0';
                    sscanf(arrow + 3, " Section : %d", &sections[section_index].choices[choice_index].next_section_id);
                    char *id_marker = strstr(line, "ID_Conséquence : ");
                    if (id_marker) {
                        sscanf(id_marker + 17, "%d", &sections[section_index].choices[choice_index].consequence_id);
                    }
                    strncpy(sections[section_index].choices[choice_index].text, line, MAX_TEXT_LENGTH - 1);
                    choice_index++;
                } else {
                    break;
                }
            }
            sections[section_index].choice_count = choice_index;
        } else if (line[0] == '"' && line[strlen(line) - 1] == '"') {
            // Ajouter un objet (ITEM_ADD)
            if (sections[section_index].item_count < MAX_ITEMS) {
                strncpy(sections[section_index].items[sections[section_index].item_count].name, line + 1, MAX_NAME_LENGTH - 2);
                sections[section_index].items[sections[section_index].item_count].name[strlen(line) - 2] = '\0';
                sections[section_index].items[sections[section_index].item_count].type = ITEM_ADD;
                sections[section_index].item_count++;
//                printf("DEBUG: ITEM_ADD détecté : %s\n", sections[section_index].items[sections[section_index].item_count - 1].name);
            }
        } else if (line[0] == '<' && line[strlen(line) - 1] == '>') {
            // Retirer un objet (ITEM_REMOVE)
            if (sections[section_index].item_count < MAX_ITEMS) {
                strncpy(sections[section_index].items[sections[section_index].item_count].name, line + 1, MAX_NAME_LENGTH - 2);
                sections[section_index].items[sections[section_index].item_count].name[strlen(line) - 2] = '\0';
                sections[section_index].items[sections[section_index].item_count].type = ITEM_REMOVE;
                sections[section_index].item_count++;
//                printf("DEBUG: ITEM_REMOVE détecté : %s\n", sections[section_index].items[sections[section_index].item_count - 1].name);
            }
        }
        else if (strncmp(line, "[Fin]", 5) == 0) {
            sections[section_index].is_ending = 1;
        }
    }

    fclose(file);
    return section_index + 1;
}

Section* get_section_by_id(Section sections[], int section_count, int id) {
    for (int i = 0; i < section_count; i++) {
        if (sections[i].id == id) {
            return &sections[i];
        }
    }
    return NULL;
}


int select_story(char *selected_file) {
    printf("\n--- Histoires disponibles ---\n");
    printf("\n"); // Séparateur visuel
    struct dirent *entry;
    DIR *dir = opendir("C:/Users/alain/OneDrive/Documents/X3(ESIEA)/Back_to_the_past/stories"); // Répertoire contenant les fichiers .txt
    if (!dir) {
        printf("Erreur : Impossible d'ouvrir le répertoire des histoires.\n");
        return 0;
    }

    char stories[100][256]; // Tableau pour stocker les noms des fichiers
    int story_count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".txt")) { // Vérifie si le fichier a l'extension .txt
            story_count++;
            printf("%d. %s\n", story_count, entry->d_name);
            strncpy(stories[story_count - 1], entry->d_name, 255);
        }
    }
    closedir(dir);

    if (story_count == 0) {
        printf("Aucune histoire disponible.\n");
        return 0;
    }

    int choice;
    printf("Choisissez une histoire (1-%d) : ", story_count);
    scanf("%d", &choice);

    while (choice < 1 || choice > story_count) {
        printf("Choix invalide. Veuillez réessayer : ");
        scanf("%d", &choice);
    }
    snprintf(selected_file, 256, "./stories/%s", stories[choice - 1]);
    return 1;
}