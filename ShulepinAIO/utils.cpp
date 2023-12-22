#include "sdk.hpp"
#include "utils.h"
#include <cstdint>

namespace utils
{
	// Spell shields
	constexpr uint32_t recall_buff = 1518452144;
	constexpr uint32_t edge_of_night_buff = -1336513801;
	constexpr uint32_t banshees_veil_buff = -718742588;
	constexpr uint32_t sivir_e_buff = -1616589467;
	constexpr uint32_t morgana_e_buff = -6564753;
	constexpr uint32_t nocturne_w_buff = 1689565216;

	// CC immunity
	constexpr uint32_t cc_fiora = 2129718281;
	constexpr uint32_t cc_malzahar = 870022257;
	constexpr uint32_t cc_olaf = 2365069888;
	constexpr uint32_t cc_rammus = 289028285;
	constexpr uint32_t cc_sion = 1507713866;
	constexpr uint32_t cc_udyr = 3144157204;
	constexpr uint32_t cc_warwick = 4009001011;
	constexpr uint32_t cc_volibear = 2809907847;

	// Invulnerabilities
	constexpr uint32_t invul_pantheon = 270113837;
	constexpr uint32_t invul_kayle = 1300289163;
	constexpr uint32_t invul_kindred = 3924676510;
	constexpr uint32_t invul_taric = 2599071170;
	constexpr uint32_t invul_xin = 3690498373;
	constexpr uint32_t invul_zilean = 2586248806;
	constexpr uint32_t invul_samira = 3963192991;
	constexpr uint32_t invul_tryndra = 1722755126;
	
	float get_ping()
	{
		return static_cast<float>(g_sdk->net_client->get_ping()) / 1000.f;
	}

	float get_percent_health(game_object* object)
	{
		return 100.f * object->get_hp() / object->get_max_hp();
	}

	float get_percent_mana(game_object* object)
	{
		return 100.f * object->get_par() / object->get_max_par();
	}

	float get_real_health(game_object* object, const std::string& damage_type, const bool use_pred, const float t)
	{
		const auto game_time = g_sdk->clock_facade->get_game_time();
		const auto hp = use_pred ? sdk::health_prediction->get_predicted_health(object, game_time + t) : object->get_hp();

		if (damage_type == "AD")
		{
			return hp + object->get_physical_shield();
		}
		if (damage_type == "AP")
		{
			return hp + object->get_magical_shield();
		}
		if (damage_type == "ALL")
		{
			return hp + object->get_all_shield();
		}
		return hp + object->get_all_shield();
	}

	float get_real_attack_range(game_object* target, game_object* source)
	{
		return source->get_attack_range() + target->get_bounding_radius() + source->get_bounding_radius();
	}

	bool is_enough_mana(const float value)
	{
		return get_percent_mana() > value;
	}

	bool is_me(game_object* object)
	{
		if (!object || !object->get_id())
		{
			return false;
		}
		
		const auto& player = g_sdk->object_manager->get_local_player();
		return object->get_id() == player->get_id();
	}

	bool is_enemy(game_object* object)
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		return object->get_team_id() != player->get_team_id();
	}

	bool is_auto_attack(spell_cast* cast)
	{
		return cast->is_basic_attack() || cast->is_special_attack();
	}

	bool is_spell_ready(const int spell_slot, const float extra_time)
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		const auto& spell_state = player->get_spell_cast_state(spell_slot);

		if (spell_state == 0)
		{
			return true;
		}

		const auto cooldown = player->get_spell(spell_slot)->get_cooldown();
		return cooldown < extra_time && (spell_state & (1 << 5)) != 0;
	}

	bool is_on_cc(game_object* hero, const float time, const float extra_time)
	{
		for (auto&& buff : hero->get_buffs())
		{
			if (!buff)
			{
				continue;
			}

			// ? Airborne, Sleep, Stasis, Suspension ? //
			if (buff->get_type() == buff_type::snare ||
				buff->get_type() == buff_type::stun ||
				buff->get_type() == buff_type::suppression ||
				buff->get_type() == buff_type::charm ||
				buff->get_type() == buff_type::flee ||
				buff->get_type() == buff_type::taunt ||
				buff->get_type() == buff_type::berserk)
			{
				const auto end_time = buff->get_end_time();

				if (time + extra_time > end_time - g_sdk->clock_facade->get_game_time())
				{
					continue;
				}
				
				return true;
			}
		}

		return false;
	}
	
	bool is_valid_target(game_object* hero, const float range)
	{
		const auto& player = g_sdk->object_manager->get_local_player();

		return
			hero &&
			hero->is_valid() && 
			hero->is_visible() && 
			hero->is_targetable() &&
			hero->get_position().distance(player->get_position()) <= range &&
			!is_invulnerable(hero);
	}

	buff_instance* is_recalling(game_object* hero)
	{
		return hero->get_buff_by_hash(recall_buff);
	}

	bool is_protected_by_spell_shield(game_object* hero)
	{
		if (hero->get_buff_by_hash(sivir_e_buff) ||
			hero->get_buff_by_hash(morgana_e_buff) ||
			hero->get_buff_by_hash(nocturne_w_buff) ||
			hero->get_buff_by_hash(edge_of_night_buff) ||
			hero->get_buff_by_hash(banshees_veil_buff))
		{
			return true;
		}

		return false;
	}

	bool is_immune_to_cc(game_object* hero)
	{
		if (hero->get_buff_by_hash(cc_fiora) ||
			hero->get_buff_by_hash(cc_malzahar) ||
			hero->get_buff_by_hash(cc_olaf) ||
			hero->get_buff_by_hash(cc_rammus) ||
			hero->get_buff_by_hash(cc_sion) ||
			hero->get_buff_by_hash(cc_udyr) ||
			hero->get_buff_by_hash(cc_warwick) ||
			hero->get_buff_by_hash(cc_volibear))
		{
			return true;
		}

		return false;
	}

	bool is_invulnerable(game_object* hero)
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		
		if (hero->get_buff_by_hash(invul_kayle) ||
			hero->get_buff_by_hash(invul_kindred) ||
			hero->get_buff_by_hash(invul_taric) ||
			hero->get_buff_by_hash(invul_zilean) && get_percent_health(hero) < 30.f ||
			hero->get_buff_by_hash(invul_xin) && player->get_position().distance(hero->get_position()) < 450.f ||
			hero->get_buff_by_hash(invul_pantheon) || // TODO: Add is_facing()
			hero->get_buff_by_hash(invul_samira) ||
			hero->get_buff_by_hash(invul_tryndra) && get_percent_health(hero) < 30.f) 
		{
			return true;
		}

		return false;
	}
	
	bool is_on_harass(const bool allow_to_use_in_other_modes, const bool global_var_state)
	{
		if (!global_var_state) return false;

		if (allow_to_use_in_other_modes)
		{
			return sdk::orbwalker->harass() || sdk::orbwalker->clear() || sdk::orbwalker->lasthit();
		}

		return sdk::orbwalker->harass();
	}

	bool is_position_inside_turret_range(const math::vector3 position, const float extra_range)
	{
		for (const auto& turret : g_sdk->object_manager->get_turrets())
		{
			if (!turret || !turret->is_valid() || turret->is_dead()) continue;

			const auto total_range = 750.f + turret->get_bounding_radius() +
				g_sdk->object_manager->get_local_player()->get_bounding_radius() + extra_range;

			if (position.distance(turret->get_position()) < total_range)
			{
				return true;
			}
		}

		return false;
	}

	game_object* get_target(const float range)
	{
		return sdk::target_selector->get_hero_target([&range](game_object* hero)
		{
			return is_valid_target(hero, range);
		});
	}

	game_object* get_target_with_list(const float range, std::unordered_map<std::string, bool> list)
	{
		return sdk::target_selector->get_hero_target([&range, &list](game_object* hero)
		{
			return is_valid_target(hero, range) && !list[hero->get_char_name()];
		});
	}

	uint32_t interpolate_color(uint32_t color1, uint32_t color2, float t)
	{
		uint8_t r1 = (color1 >> 16) & 0xFF;
		uint8_t g1 = (color1 >> 8) & 0xFF;
		uint8_t b1 = color1 & 0xFF;
		
		uint8_t r2 = (color2 >> 16) & 0xFF;
		uint8_t g2 = (color2 >> 8) & 0xFF;
		uint8_t b2 = color2 & 0xFF;
		
		uint8_t r = static_cast<uint8_t>(r1 + t * (r2 - r1));
		uint8_t g = static_cast<uint8_t>(g1 + t * (g2 - g1));
		uint8_t b = static_cast<uint8_t>(b1 + t * (b2 - b1));
		
		return (0xFF << 24) | (r << 16) | (g << 8) | b;
	}
}
