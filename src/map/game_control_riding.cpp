#include "game_control_riding.hpp"
#include <algorithm>
#include <vector>

#include "pc.hpp"
#include "skill.hpp"
#include "status.hpp"
#include "chrif.hpp"
#include "log.hpp"
#include "script.hpp"
#include <common/strlib.hpp>

namespace {
struct Rental {
	const char* kind;
	int skill;
	int fee;
	bool eligible;
	bool active;
};

// Sources: npc/kafras/{kafras,functions_kafras}.txt (Prontera),
// npc/merchants/renters.txt and npc/re/merchants/renters.txt.
std::vector<Rental> rentals(map_session_data* sd) {
	const auto job = sd->class_ & MAPID_THIRDMASK;
	const bool mechanic = job == MAPID_MECHANIC;
	const bool crusader = job == MAPID_CRUSADER;
	return {
		{"cart", MC_PUSHCART, mechanic ? 0 : 800, (sd->class_ & MAPID_FIRSTMASK) == MAPID_MERCHANT, !!pc_iscarton(sd)},
		{"peco", KN_RIDING, crusader ? 3500 : 2500, job == MAPID_KNIGHT || crusader, !!pc_isriding(sd) && job != MAPID_ROYAL_GUARD},
		{"dragon", RK_DRAGONTRAINING, 0, job == MAPID_RUNE_KNIGHT, !!pc_isridingdragon(sd)},
		{"gryphon", KN_RIDING, 0, job == MAPID_ROYAL_GUARD, !!pc_isriding(sd) && job == MAPID_ROYAL_GUARD},
		{"mado", NC_MADOLICENCE, 0, mechanic, !!pc_ismadogear(sd)},
	};
}

const char* blocked(map_session_data* sd, const Rental& rental) {
	if (pc_isdead(sd) || sd->npc_id || sd->trade_partner.id || sd->state.storage_flag || sd->state.vending || sd->state.buyingstore)
		return "riding_busy";
	if (rental.active) return "";
	if (!rental.eligible) return "riding_job_required";
	if (!pc_checkskill(sd, rental.skill)) return "riding_skill_required";
	if (std::string(rental.kind) != "cart" && (sd->sc.getSCE(SC_ALL_RIDING)
		|| (sd->sc.option & (OPTION_RIDING | OPTION_DRAGON | OPTION_MADOGEAR)))) return "riding_conflict";
	return "";
}

bool uses_ticket(map_session_data* sd, const Rental& rental) {
	return !rental.active && std::string(rental.kind) == "cart" && rental.fee > 0 && pc_search_inventory(sd, 7061) >= 0;
}
}

nlohmann::json game_control_riding_snapshot(map_session_data* sd) {
	auto entries = nlohmann::json::array();
	for (const auto& rental : rentals(sd)) {
		if (!rental.eligible && !rental.active) continue;
		const bool ticket = uses_ticket(sd, rental);
		const int fee = rental.active || ticket ? 0 : rental.fee;
		std::string reason = blocked(sd, rental);
		if (reason.empty() && sd->status.zeny < fee) reason = "riding_insufficient_zeny";
		entries.push_back({{"kind", rental.kind}, {"active", rental.active}, {"fee", fee},
			{"ticket", ticket}, {"reason", reason}});
	}
	return entries;
}

GameControlRidingResult game_control_riding_update(map_session_data* sd, const nlohmann::json& payload) {
	const auto error = [](const char* code, int status = 409) -> GameControlRidingResult {
		return {status, {{"error", {{"code", code}}}}};
	};
	if (!payload.is_object() || payload.size() != 4 || !payload.contains("kind") || !payload["kind"].is_string()
		|| !payload.contains("enabled") || !payload["enabled"].is_boolean()
		|| !payload.contains("fee") || !payload["fee"].is_number_integer() || payload["fee"] < 0 || payload["fee"] > 3500
		|| !payload.contains("ticket") || !payload["ticket"].is_boolean()) return error("invalid_parameter", 400);
	const auto kind = payload["kind"].get<std::string>();
	const bool enabled = payload["enabled"].get<bool>();
	const auto options = rentals(sd);
	const auto found = std::find_if(options.begin(), options.end(), [&](const Rental& rental) { return kind == rental.kind; });
	if (found == options.end()) return error("invalid_parameter", 400);
	const auto& rental = *found;
	if (rental.active == enabled) return error("riding_state_changed");
	const auto reason = blocked(sd, rental);
	if (*reason) return error(reason);
	const bool ticket = uses_ticket(sd, rental);
	const int fee = enabled && !ticket ? rental.fee : 0;
	if (payload["fee"] != fee || payload["ticket"] != ticket) return error("riding_quote_changed");
	if (sd->status.zeny < fee) return error("riding_insufficient_zeny");
	// All validation completes before spending resources. Map commands execute on the map thread.
	if (ticket && pc_delitem(sd, pc_search_inventory(sd, 7061), 1, 0, 0, LOG_TYPE_COMMAND)) return error("riding_quote_changed");
	if (fee && pc_payzeny(sd, fee, LOG_TYPE_COMMAND)) return error("riding_insufficient_zeny");
	if (kind == "cart") {
		pc_setcart(sd, enabled ? 1 : 0);
		if (enabled && fee) pc_setglobalreg(sd, add_str("RESRVPTS"), pc_readglobalreg(sd, add_str("RESRVPTS")) + 48);
	} else if (kind == "mado") {
		// Only this option changes; a coexisting cart remains installed.
		pc_setoption(sd, enabled ? sd->sc.option | OPTION_MADOGEAR : sd->sc.option & ~OPTION_MADOGEAR);
	} else if (kind == "dragon") {
		pc_setoption(sd, enabled ? sd->sc.option | OPTION_DRAGON1 : sd->sc.option & ~OPTION_DRAGON);
	} else {
		pc_setoption(sd, enabled ? sd->sc.option | OPTION_RIDING : sd->sc.option & ~OPTION_RIDING);
	}
	chrif_save(sd, CSAVE_NORMAL);
	return {200, {{"data", {{"result", {{"char_id", sd->status.char_id}, {"riding", game_control_riding_snapshot(sd)}}}}}}};
}
