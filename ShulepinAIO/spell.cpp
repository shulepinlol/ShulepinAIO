#include "spell.h"

#include <optional>

#include "hashes.h"
#include "utils.h"

namespace script
{
	/* Constructor */

	spell::spell(const int spell_slot, const float range)
	{
		spell_slot_ = spell_slot;
		range_ = range;
	}

	/* Settets */

	spell* spell::set_skillshot(const pred_sdk::spell_type spell_type, const pred_sdk::targetting_type targetting_type, 
		const float delay, const float radius, const float speed, const std::vector<pred_sdk::collision_type>& collision_data)
	{
		spell_type_ = spell_type;
		targetting_type_ = targetting_type;
		delay_ = delay;
		radius_ = radius;
		projectile_speed_ = speed;
		forbidden_collisions_ = collision_data;

		return this;
	}

	spell* spell::set_charged_spell(const float min_range, const float max_range, const float duration, const uint32_t charge_buff)
	{
		charged_spell_ = true;
		min_range_ = min_range;
		max_range_ = max_range;
		duration_ = duration;
		charge_buff_ = charge_buff;

		return this;
	}

	void spell::set_spell_slot(const int value)
	{
		spell_slot_ = value;
	}

	void spell::set_range(const float value)
	{
		range_ = value;
	}

	void spell::set_min_range(const float value)
	{
		min_range_ = value;
	}

	void spell::set_max_range(const float value)
	{
		max_range_ = value;
	}

	void spell::set_charged(const bool value)
	{
		charged_spell_ = value;
	}

	void spell::set_radius(const float value)
	{
		radius_ = value;
	}

	void spell::set_cast_range(const float value)
	{
		cast_range_ = value;
	}

	void spell::set_delay(const float value)
	{
		delay_ = value;
	}

	void spell::set_proc_delay(const float value)
	{
		proc_delay_ = value;
	}

	void spell::set_projectile_speed(const float value)
	{
		projectile_speed_ = value;
	}

	void spell::set_extension_override(const float value)
	{
		extension_override_ = value;
	}

	void spell::set_bypass_anti_buffering(const bool value)
	{
		bypass_anti_buffering_ = value;
	}

	void spell::set_spell_type(const pred_sdk::spell_type value)
	{
		spell_type_ = value;
	}

	void spell::set_targeting_type(const pred_sdk::targetting_type value)
	{
		targetting_type_ = value;
	}

	void spell::set_hitchance(const int value)
	{
		expected_hitchance_ = value;
	}

	void spell::set_source(game_object* value)
	{
		source_ = value;
	}

	void spell::set_source_position(const math::vector3 value)
	{
		source_position_ = value;
	}

	void spell::set_collision(const std::vector<pred_sdk::collision_type>& value)
	{
		forbidden_collisions_ = value;
	}

	void spell::set_expected_hit_types(const std::vector<pred_sdk::hit_type>& value)
	{
		expected_hit_types_ = value;
	}

	void spell::set_additional_target_selection_checks(const std::function<bool(game_object*)>& value)
	{
		additional_target_selection_checks_ = value;
	}

	/* Getters */

	int spell::get_spell_slot() const
	{
		return spell_slot_;
	}

	float spell::get_range() const
	{
		return range_;
	}

	float spell::get_min_range() const
	{
		return min_range_;
	}

	float spell::get_max_range() const
	{
		return max_range_;
	}

	bool spell::get_charged() const
	{
		return charged_spell_;
	}

	float spell::get_radius() const
	{
		return radius_;
	}

	float spell::get_cast_range() const
	{
		return cast_range_;
	}

	float spell::get_delay() const
	{
		return delay_;
	}

	float spell::get_proc_delay() const
	{
		return proc_delay_;
	}

	float spell::get_projectile_speed() const
	{
		return projectile_speed_;
	}

	float spell::get_extension_override() const
	{
		return extension_override_;
	}

	bool spell::get_bypass_anti_buffering() const
	{
		return bypass_anti_buffering_;
	}

	pred_sdk::spell_type spell::get_spell_type() const
	{
		return spell_type_;
	}

	pred_sdk::targetting_type spell::get_targeting_type() const
	{
		return targetting_type_;
	}

	int spell::get_hitchance() const
	{
		return expected_hitchance_;
	}

	game_object* spell::get_source() const
	{
		return source_;
	}

	math::vector3 spell::get_source_position() const
	{
		return source_position_;
	}

	std::vector<pred_sdk::collision_type> spell::get_collision() const
	{
		return forbidden_collisions_;
	}

	std::vector<pred_sdk::hit_type> spell::get_expected_hit_types() const
	{
		return expected_hit_types_;
	}

	std::function<bool(game_object*)> spell::get_additional_target_selection_checks() const
	{
		return additional_target_selection_checks_;
	}

	spell_entry* spell::get_spell() const
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		return player->get_spell(spell_slot_);
	}

	uint8_t spell::get_type() const
	{
		return this->get_spell()->get_type();
	}

	float spell::get_cast_range_from_core() const
	{
		return this->get_spell()->get_cast_range();
	}

	uint32_t spell::get_charges() const
	{
		return this->get_spell()->get_charges();
	}

	int spell::get_level() const
	{
		return this->get_spell()->get_level();
	}

	float spell::get_cooldown() const
	{
		return this->get_spell()->get_cooldown();
	}

	uint8_t spell::get_toggle_state() const
	{
		return this->get_spell()->get_toggle_state();
	}

	void* spell::get_icon() const
	{
		return this->get_spell()->get_data()->get_icon();
	}

	spell_static_data* spell::get_static_data() const
	{
		return this->get_spell()->get_data()->get_static_data();
	}

	pred_sdk::spell_data spell::get_prediction_input(const int hitchance, const std::optional<pred_input_options>& options) const
	{
		pred_sdk::spell_data pred_input{};

		pred_input.spell_type = spell_type_;
		pred_input.targetting_type = targetting_type_;
		pred_input.expected_hitchance = hitchance;

		pred_input.spell_slot = spell_slot_;
		pred_input.range = range_;
		pred_input.radius = radius_;
		pred_input.delay = delay_;
		pred_input.proc_delay = proc_delay_;
		pred_input.projectile_speed = projectile_speed_;

		pred_input.forbidden_collisions = forbidden_collisions_;

		if (options)
		{
			if (options->source.has_value())
			{
				pred_input.source = options->source.value();
			}
			if (options->source_position.has_value())
			{
				pred_input.source_position = options->source_position.value();
			}
			if (options->bypass_anti_buffering.has_value())
			{
				pred_input.bypass_anti_buffering = options->bypass_anti_buffering.value();
			}
			if (options->cast_range.has_value())
			{
				pred_input.cast_range = options->cast_range.value();
			}
			if (options->proc_delay.has_value())
			{
				pred_input.proc_delay = options->proc_delay.value();
			}
			if (options->extension_override.has_value())
			{
				pred_input.extension_override = options->extension_override.value();
			}
			if (options->expected_hit_types.has_value())
			{
				pred_input.expected_hit_types = options->expected_hit_types.value();
			}
			if (options->additional_target_selection_checks.has_value())
			{
				pred_input.additional_target_selection_checks = options->additional_target_selection_checks.value();
			}
			if (options->forbidden_collisions.has_value())
			{
				pred_input.forbidden_collisions = options->forbidden_collisions.value();
			}
		}

		return pred_input;
	}

	pred_sdk::pred_data spell::get_prediction(const pred_sdk::spell_data& pred_input) const
	{
		return sdk::prediction->predict(pred_input);
	}

	pred_sdk::pred_data spell::get_prediction(game_object* target, const pred_sdk::spell_data& pred_input) const
	{
		return sdk::prediction->predict(target, pred_input);
	}

	pred_sdk::pred_data spell::get_prediction(const int hitchance, const std::optional<pred_input_options>& options) const
	{
		return sdk::prediction->predict(this->get_prediction_input(hitchance, options));
	}

	pred_sdk::pred_data spell::get_prediction(game_object* target, const int hitchance, const std::optional<pred_input_options>& options) const
	{
		return sdk::prediction->predict(target, this->get_prediction_input(hitchance, options));
	}

	float spell::get_charged_time(const float duration) const
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		const auto buff = player->get_buff_by_hash(charge_buff_);
		const auto time = g_sdk->clock_facade->get_game_time();

		if (buff)
		{
			return fmaxf(0.f, fminf(1.f, (time - buff->get_start_time() + 0.033f) / duration));
		}

		return 0.f;
	}

	float spell::get_charged_range(const float duration) const
	{
		return min_range_ + fminf((max_range_ - min_range_), (max_range_ - min_range_) * this->get_charged_time(duration));
	}

	float spell::get_spell_hit_time(const math::vector3 pos, game_object* target, const std::optional<pred_input_options>& options) const
	{
		auto pred_input = this->get_prediction_input(0, options);
		return sdk::prediction->util()->get_spell_hit_time(pred_input, pos, target);
	}


	/* Spell state methods */

	bool spell::is_ready(const float extra_time) const
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		const auto& spell_state = player->get_spell_cast_state(spell_slot_);

		if (spell_state == 0)
		{
			return true;
		}

		const auto cooldown = this->get_cooldown();
		return cooldown < extra_time && (spell_state & (1 << 5)) != 0;
	}

	bool spell::is_issue_order_passed(const float value) const
	{
		const auto& time = g_sdk->clock_facade->get_game_time();
		return spell_last_cast_t_ + value < time;
	}

	bool spell::is_mood_order_passed(const float value) const
	{
		const auto& time = g_sdk->clock_facade->get_game_time();
		return spell_last_mood_cast_t_ + value < time;
	}

	bool spell::is_enough_mana_pct(const float value) const
	{
		return utils::is_enough_mana(value);
	}

	bool spell::is_charging() const
	{
		return get_charged_time() > 0.f;
	}

	bool spell::is_in_range(const math::vector3 cast_position) const
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		return cast_position.distance(player->get_server_position()) < range_;
	}


	bool spell::can_cast(game_object* target) const
	{
		return sdk::orbwalker->can_spell(target, delay_);
	}

	/* Spell casting methods */

	bool spell::release(const math::vector3 cast_position, const bool release_cast, const float t)
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		const auto& time = g_sdk->clock_facade->get_game_time();

		if (spell_last_cast_t_ + t < time && cast_position.distance(player->get_server_position()) < this->get_charged_range(duration_))
		{
			player->update_chargeable_spell(spell_slot_, cast_position, release_cast);
			spell_last_cast_t_ = time;
			g_sdk->log_console("[+] spell order %d was issued at %f", spell_slot_, spell_last_cast_t_);
			return true;
		}

		return false;
	}

	bool spell::cast_spell(const float t)
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		const auto& time = g_sdk->clock_facade->get_game_time();

		if (spell_last_cast_t_ + t < time)
		{
			player->cast_spell(spell_slot_);
			spell_last_cast_t_ = time;
			g_sdk->log_console("[+] spell order %d was issued at %f", spell_slot_, spell_last_cast_t_);
			return true;
		}

		return false;
	}

	bool spell::cast_spell(const math::vector3 cast_position, const float t)
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		const auto& time = g_sdk->clock_facade->get_game_time();

		if (spell_last_cast_t_ + t < time)
		{
			player->cast_spell(spell_slot_, cast_position);
			spell_last_cast_t_ = time;
			g_sdk->log_console("[+] spell order %d was issued at %f", spell_slot_, spell_last_cast_t_);
			return true;
		}

		return false;
	}

	bool spell::cast_spell(game_object* target, const float t)
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		const auto& time = g_sdk->clock_facade->get_game_time();

		if (spell_last_cast_t_ + t < time)
		{
			player->cast_spell(spell_slot_, target);
			spell_last_cast_t_ = time;
			g_sdk->log_console("[+] spell order %d was issued at %f", spell_slot_, spell_last_cast_t_);
			return true;
		}

		return false;
	}

	bool spell::cast_spell(const math::vector3 start_position, const math::vector3 end_position, const float t)
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		const auto& time = g_sdk->clock_facade->get_game_time();

		if (spell_last_cast_t_ + t < time)
		{
			player->cast_spell(spell_slot_, start_position, end_position);
			spell_last_cast_t_ = time;
			g_sdk->log_console("[+] spell order %d was issued at %f", spell_slot_, spell_last_cast_t_);
			return true;
		}

		return false;
	}

	bool spell::cast_spell_on_hitchance(game_object* target, const int hitchance, const std::optional<pred_input_options>& options, const float t)
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		const auto& time = g_sdk->clock_facade->get_game_time();
		const auto& pred = this->get_prediction(target, hitchance, options);

		if (pred.is_valid)
		{
			return this->cast_spell(pred.cast_position, t);
		}

		return false;
	}

	bool spell::cast_mood(const float t)
	{
		const auto& player = g_sdk->object_manager->get_local_player();
		const auto& time = g_sdk->clock_facade->get_game_time();

		if (spell_last_mood_cast_t_ + t < time && player->cast_hwei_mood(spell_slot_))
		{
			spell_last_mood_cast_t_ = time;
			g_sdk->log_console("[+] mood %d was issued at %f", spell_slot_, spell_last_cast_t_);
			return true;
		}

		return false;
	}
}
