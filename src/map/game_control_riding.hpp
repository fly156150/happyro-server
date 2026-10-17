#ifndef HAPPYRO_GAME_CONTROL_RIDING_HPP
#define HAPPYRO_GAME_CONTROL_RIDING_HPP

#include <nlohmann/json.hpp>
struct map_session_data;
struct GameControlRidingResult {
	int status;
	nlohmann::json body;
};
nlohmann::json game_control_riding_snapshot(map_session_data* sd);
GameControlRidingResult game_control_riding_update(map_session_data* sd, const nlohmann::json& payload);
int game_control_riding_appearance(map_session_data* sd);
void game_control_riding_appearance_reconcile(map_session_data* sd);
GameControlRidingResult game_control_riding_appearance_update(map_session_data* sd, const nlohmann::json& payload);
#endif
