#pragma once
#include <optional>

#include "sdk.hpp"

namespace script
{
    class spell
	{
    private:
        pred_sdk::spell_type spell_type_{};
        pred_sdk::targetting_type targetting_type_{};
        static inline int expected_hitchance_ = pred_sdk::hitchance::automatic;     // The expected hitchance
        game_object* source_{};                                                     // Source object, if none player will be taken
        math::vector3 source_position_{};                                           // Position where the skillshot originates from, if none player position will be taken
        bool bypass_anti_buffering_{};                                              // Allows casting spells while other spells are being cast (allow spell buffering)
        int spell_slot_ = -1;                                                       // Will be used to check for CD if expected_hitchance is automatic, if the CD of the spell is low we can spam it more
        float range_{};                                                             // Max range of the spell
        float min_range_{};                                                         // Min range of charged spell
        float max_range_{};                                                         // Max range of charged spell
        bool charged_spell_{};                                                      // Varus Q, Xerath Q, Pyke Q
        float radius_{};                                                            // Circle is the same as linear, use * 0.5f of the TOTAL radius (so only distance from player to one side)
        float cast_range_{};                                                        // The cast range of the spell for vector types: Viktor E, Rumble R
        float delay_{};                                                             // Cast delay
        float proc_delay_{};                                                        // Delay until the spell hits, for example syndra Q is static 0.6
        float projectile_speed_ = FLT_MAX;                                          // Projectile speed if any, FLT_MAX if the spell has no projectiles
        float extension_override_{};                                                // If we want to override the prediction extension
        std::vector<pred_sdk::collision_type> forbidden_collisions_{};              // Things we don't want to skillshot to collide with
        std::vector<pred_sdk::hit_type> expected_hit_types_{};                      // If we want special hit types only, fill them in here
        std::function<bool(game_object*)> additional_target_selection_checks_{};    // Custom function for target selection checks, example: target with specific buff(s) only

        float duration_{};
        uint32_t charge_buff_{};
        float spell_last_cast_t_{};

    public:
        /* Constructor */
        spell(int spell_slot, float range);

        /* Structs */
        struct pred_input_options
        {
            std::optional<pred_sdk::spell_type> spell_type;
            std::optional<pred_sdk::targetting_type> targetting_type;
            std::optional<int> expected_hitchance;
            std::optional<game_object*> source;
            std::optional<math::vector3> source_position;
            std::optional<bool> bypass_anti_buffering;
            std::optional<int> spell_slot;
            std::optional<float> range;
            std::optional<float> min_range;
            std::optional<float> max_range;
            std::optional<bool> charged_spell;
            std::optional<float> radius;
            std::optional<float> cast_range;
            std::optional<float> delay;
            std::optional<float> proc_delay;
            std::optional<float> projectile_speed;
            std::optional<float> extension_override;
            std::optional<std::vector<pred_sdk::collision_type>> forbidden_collisions;
            std::optional<std::vector<pred_sdk::hit_type>> expected_hit_types;
            std::optional<std::function<bool(game_object*)>> additional_target_selection_checks;
        };

        /* Setters */
        spell* set_skillshot(pred_sdk::spell_type spell_type, pred_sdk::targetting_type targetting_type, float delay, float radius, float speed, const std::vector<pred_sdk::collision_type>& collision_data);
        spell* set_charged_spell(float min_range, float max_range, float duration, uint32_t charge_buff);
        void set_spell_slot(int value);
        void set_range(float value);
        void set_min_range(float value);
        void set_max_range(float value);
        void set_charged(bool value);
        void set_radius(float value);
        void set_cast_range(float value);
        void set_delay(float value);
        void set_proc_delay(float value);
        void set_projectile_speed(float value);
        void set_extension_override(float value);
        void set_bypass_anti_buffering(bool value);
        void set_spell_type(pred_sdk::spell_type value);
        void set_targeting_type(pred_sdk::targetting_type value);
        void set_hitchance(int value);
        void set_source(game_object* value);
        void set_source_position(math::vector3 value);
        void set_collision(const std::vector<pred_sdk::collision_type>& value);
        void set_expected_hit_types(const std::vector<pred_sdk::hit_type>& value);
        void set_additional_target_selection_checks(const std::function<bool(game_object*)>& value);

        /* Getters */
        [[nodiscard]] int get_spell_slot() const;
        [[nodiscard]] float get_range() const;
        [[nodiscard]] float get_min_range() const;
        [[nodiscard]] float get_max_range() const;
        [[nodiscard]] bool get_charged() const;
        [[nodiscard]] float get_radius() const;
        [[nodiscard]] float get_cast_range() const;
        [[nodiscard]] float get_delay() const;
        [[nodiscard]] float get_proc_delay() const;
        [[nodiscard]] float get_projectile_speed() const;
        [[nodiscard]] float get_extension_override() const;
        [[nodiscard]] bool get_bypass_anti_buffering() const;
        [[nodiscard]] pred_sdk::spell_type get_spell_type() const;
        [[nodiscard]] pred_sdk::targetting_type get_targeting_type() const;
        [[nodiscard]] int get_hitchance() const;
        [[nodiscard]] game_object* get_source() const;
        [[nodiscard]] math::vector3 get_source_position() const;
        [[nodiscard]] std::vector<pred_sdk::collision_type> get_collision() const;
        [[nodiscard]] std::vector<pred_sdk::hit_type> get_expected_hit_types() const;
        [[nodiscard]] std::function<bool(game_object*)> get_additional_target_selection_checks() const;
        [[nodiscard]] spell_entry* get_spell() const;
        [[nodiscard]] uint8_t get_type() const;
        [[nodiscard]] float get_cast_range_from_core() const;
        [[nodiscard]] uint32_t get_charges() const;
        [[nodiscard]] int get_level() const;
        [[nodiscard]] float get_cooldown() const;
        [[nodiscard]] uint8_t get_toggle_state() const;
        [[nodiscard]] void* get_icon() const;
        [[nodiscard]] spell_static_data* get_static_data() const;
        [[nodiscard]] pred_sdk::spell_data get_prediction_input(int hitchance = expected_hitchance_, const std::optional<pred_input_options>& options = std::nullopt) const;
        [[nodiscard]] pred_sdk::pred_data get_prediction(const pred_sdk::spell_data& pred_input) const;
        [[nodiscard]] pred_sdk::pred_data get_prediction(game_object* target, const pred_sdk::spell_data& pred_input) const;
        [[nodiscard]] pred_sdk::pred_data get_prediction(int hitchance = expected_hitchance_, const std::optional<pred_input_options>& options = std::nullopt) const;
        [[nodiscard]] pred_sdk::pred_data get_prediction(game_object* target, int hitchance = expected_hitchance_, const std::optional<pred_input_options>& options = std::nullopt) const;
        [[nodiscard]] float get_charged_time(float duration = 1.5f) const;
        [[nodiscard]] float get_charged_range(float duration = 1.5f) const;
        [[nodiscard]] float get_spell_hit_time(math::vector3 pos, game_object* target = nullptr, const std::optional<pred_input_options>& options = std::nullopt) const;

        /* Spell state methods */
        [[nodiscard]] bool is_ready(float extra_time = 0.f) const;
        [[nodiscard]] bool is_issue_order_passed(float value) const;
        [[nodiscard]] bool is_enough_mana_pct(float value) const;
        [[nodiscard]] bool is_charging() const;

        /* Spell casting methods */
        bool release(math::vector3 cast_position, bool release_cast = true, float t = 0.5f);
        bool cast_spell(float t = 0.5f);
        bool cast_spell(game_object* target, float t = 0.5f);
        bool cast_spell(math::vector3 cast_position, float t = 0.5f);
        bool cast_spell(math::vector3 start_position, math::vector3 end_position, float t = 0.5f);
    };
}
