#ifndef HAPPYRO_CHARACTER_APPEARANCE_HPP
#define HAPPYRO_CHARACTER_APPEARANCE_HPP
#include <common/mmo.hpp>

// Stable wire/save identifiers, independent of sprite job IDs.
inline int character_appearance_stage(int job) {
 switch (job) {
 case JOB_KNIGHT: return 1;
 case JOB_LORD_KNIGHT: return 2;
 case JOB_RUNE_KNIGHT: case JOB_RUNE_KNIGHT_T: return 3;
 case JOB_DRAGON_KNIGHT: return 4;
 case JOB_CRUSADER: return 11;
 case JOB_PALADIN: return 12;
 case JOB_ROYAL_GUARD: case JOB_ROYAL_GUARD_T: return 13;
 case JOB_IMPERIAL_GUARD: return 14;
 default: return 0; // Baby and unrelated jobs are intentionally excluded.
 }
}
inline bool character_appearance_allowed(int job, int choice) {
 if (choice == 0) return true;
 const int stage = character_appearance_stage(job);
 if (choice >= 1 && choice <= 3) return stage > choice && stage <= 4;
 if (choice >= 4 && choice <= 6) return stage > choice + 7 && stage <= 14;
 return false;
}
#endif
