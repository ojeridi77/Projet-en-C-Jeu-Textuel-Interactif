#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "story.h"
#include "inventor.h"

void start_game(Section sections[], int section_count);
int play_section(Section *section, Inventory *inventory);
void narration(const char *narration);
void dialogues(Dialogue dialogues[], int dialogue_count);
int process_choices(Choice choices[], int choice_count, Inventory *inventory);
void apply_consequence(int consequence_id, Inventory *inventory, const char *item_add, const char *item_remove);
int ending(const char *ending_text);
void collect_items(Section *section, Inventory *inventory);
void typewriter_effect(const char *text, int delay_ms);

#endif