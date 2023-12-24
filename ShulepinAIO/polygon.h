#pragma once
#include "clipper.hpp"
#include "logging_utils.h"
#include "renderer_extensions.h"
#include "math_extensions.h"
#include "sdk.hpp"

namespace script
{
	struct polygon
	{
		std::vector<vector2> points;

		void clear()
		{
			points.clear();
		}

		void add(const vector2 point)
		{
			points.push_back(point);
		}

		void add(const vector3 point)
		{
			points.push_back(point.to_2d());
		}

		void add(const polygon& other)
		{
			for (auto& i : other.points)
			{
				points.push_back(i);
			}
		}

		bool is_inside(vector2 point)
		{
			auto p = ClipperLib::IntPoint(static_cast<ClipperLib::long64>(point.x), static_cast<ClipperLib::long64>(point.y));
			auto path = to_clipper_path();

			return ClipperLib::PointInPolygon(p, path) != 0;
		}

		bool is_outside(vector2 point)
		{
			return !is_inside(point);
		}

		std::vector<ClipperLib::IntPoint> to_clipper_path()
		{
			std::vector<ClipperLib::IntPoint> result;
			for (const auto& pt : points)
			{
				result.emplace_back(static_cast<ClipperLib::long64>(pt.x), static_cast<ClipperLib::long64>(pt.y));
			}
			return result;
		}

		void draw(uint32_t color, float thickness)
		{
			if (points.size() < 2)
			{
				return;
			}

			for (size_t i = 0; i < points.size(); ++i)
			{
				const auto& current = points[i];
				const auto& next = points[(i + 1) % points.size()];

				const auto current_3d = vector3(current.x, 0.f, current.y);
				const auto next_3d = vector3(next.x, 0.f, next.y);

				const auto screen_p1 = g_sdk->renderer->world_to_screen(math::vector3(current_3d.x, current_3d.y, current_3d.z));
				const auto screen_p2 = g_sdk->renderer->world_to_screen(math::vector3(next_3d.x, next_3d.y, next_3d.z));

				g_sdk->renderer->add_line_2d(screen_p1, screen_p2, thickness, color);
			}
		}
	};

	struct rectangle : public polygon
	{
		vector2 start;
		vector2 end;
		float width;

		rectangle() = default;

		rectangle(vector2 start, vector2 end, float width)
		{
			this->start = start;
			this->end = end;
			this->width = width;

			//UpdatePolygon();
		}

		vector2 get_direction()
		{
			return (this->end - this->start).normalized();
		}

		vector2 get_perpendicular()
		{
			return this->get_direction().perpendicular();
		}

		void update_polygon(int offset = 0, float override_width = -1)
		{
			clear();

			const auto fl_offset = static_cast<float>(offset);
			const auto fl_width = (override_width > 0 ? override_width : width + fl_offset);

			add(start + (get_perpendicular() * fl_width) - (get_direction() * fl_offset));
			add(start - (get_perpendicular() * fl_width) - (get_direction() * fl_offset));
			add(end - (get_perpendicular() * fl_width) + (get_direction() * fl_offset));
			add(end + (get_perpendicular() * fl_width) + (get_direction() * fl_offset));
		}
	};
}
