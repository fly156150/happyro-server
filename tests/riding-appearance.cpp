// Compile with -I src; run the resulting binary to validate the whitelist.
#include "common/riding_appearance.hpp"
#include <cassert>
int main() {
 for (int choice = 1; choice <= 3; ++choice) {
  assert(riding_appearance_allowed(JOB_DRAGON_KNIGHT, choice));
  assert(!riding_appearance_allowed(JOB_IMPERIAL_GUARD, choice));
 }
 for (int choice = 4; choice <= 6; ++choice) {
  assert(riding_appearance_allowed(JOB_IMPERIAL_GUARD, choice));
  assert(!riding_appearance_allowed(JOB_DRAGON_KNIGHT, choice));
 }
 assert(riding_appearance_allowed(JOB_LORD_KNIGHT, 1));
 assert(!riding_appearance_allowed(JOB_LORD_KNIGHT, 2));
 assert(riding_appearance_allowed(JOB_PALADIN, 4));
 assert(!riding_appearance_allowed(JOB_KNIGHT, 1));
 assert(!riding_appearance_allowed(JOB_BABY_RUNE_KNIGHT, 1));
 assert(!riding_appearance_allowed(JOB_MEISTER, 1));
 assert(!riding_appearance_allowed(JOB_DRAGON_KNIGHT, -1));
 assert(!riding_appearance_allowed(JOB_DRAGON_KNIGHT, 7));
 assert(riding_appearance_allowed(JOB_NOVICE, 0));
}
