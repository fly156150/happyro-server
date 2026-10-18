// Compile with -I src; run the resulting binary to validate the whitelist.
#include "common/character_appearance.hpp"
#include <cassert>
int main() {
 for (int choice = 1; choice <= 3; ++choice) {
  assert(character_appearance_allowed(JOB_DRAGON_KNIGHT, choice));
  assert(!character_appearance_allowed(JOB_IMPERIAL_GUARD, choice));
 }
 for (int choice = 4; choice <= 6; ++choice) {
  assert(character_appearance_allowed(JOB_IMPERIAL_GUARD, choice));
  assert(!character_appearance_allowed(JOB_DRAGON_KNIGHT, choice));
 }
 assert(character_appearance_allowed(JOB_LORD_KNIGHT, 1));
 assert(!character_appearance_allowed(JOB_LORD_KNIGHT, 2));
 assert(character_appearance_allowed(JOB_PALADIN, 4));
 assert(!character_appearance_allowed(JOB_KNIGHT, 1));
 assert(!character_appearance_allowed(JOB_BABY_RUNE_KNIGHT, 1));
 assert(!character_appearance_allowed(JOB_MEISTER, 1));
 assert(!character_appearance_allowed(JOB_DRAGON_KNIGHT, -1));
 assert(!character_appearance_allowed(JOB_DRAGON_KNIGHT, 7));
 assert(character_appearance_allowed(JOB_NOVICE, 0));
}
