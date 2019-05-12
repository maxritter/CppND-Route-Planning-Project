/**
 * Include section
 */

#include <optional>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <io2d.h>
#include "route_model.h"
#include "render.h"
#include "route_planner.h"

using namespace std::experimental;

/**
 * Read an OSM map from file, returns nullopt for error
 */
static std::optional<std::vector<std::byte>> ReadFile(const std::string& path)
{
	/* Read binary file and jump to the end */
	std::ifstream is{ path, std::ios::binary | std::ios::ate };
	if (!is)
	{
		return std::nullopt;
	}

	/* Now we can calculate our size and allocate a byte vector for storage */
	const auto size = is.tellg();
	std::vector<std::byte> contents(size);

	/* Go back to the beginning */
	is.seekg(0);
	is.read(reinterpret_cast<char*>(contents.data()), size);

	/* Return map or nullopt */
	if (contents.empty())
	{
		return std::nullopt;
	}
	return std::move(contents);
}

int main(int argc, const char** argv)
{
	std::string osm_data_file = "";

	/* Parse command line arguments */
	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			if (std::string_view{ argv[i] } == "-f" && ++i < argc)
			{
				osm_data_file = argv[i];
			}
		}
	}

	/* Invalid usage */
	else
	{
		std::cout << "Usage: [executable] [-f filename.osm]" << std::endl;
	}

	/* Read map data from file */
	std::vector<std::byte> osm_data;
	if (osm_data.empty() && !osm_data_file.empty())
	{
		std::cout << "Reading OpenStreetMap data from the following file: " << osm_data_file << std::endl;
		auto data = ReadFile(osm_data_file);
		if (!data)
		{
			std::cout << "Failed to read." << std::endl;
		}
		else
		{
			osm_data = std::move(*data);
		}
	}

	/* Read user input */
	float start_x, start_y, end_x, end_y;
	std::cout << "Enter start x btw. 0 - 99: ";
	std::cin >> start_x;
	std::cout << "Enter start y btw. 0 - 99: ";
	std::cin >> start_y;
	std::cout << "Enter end x btw. 0 - 99: ";
	std::cin >> end_x;
	std::cout << "Enter end y btw. 0 - 99: ";
	std::cin >> end_y;

	/* Build our route model with osm_data */
	RouteModel model{ osm_data };

	/* Perform search using A* algorithm */
	RoutePlanner route_planner{ model, start_x, start_y, end_x, end_y };
	route_planner.a_star_search();
	std::cout << "Distance: " << route_planner.get_distance() << " meters." << std::endl;

	/* Render model */
	Render render{ model };

	/* Display results */
	auto display = io2d::output_surface{ 800, 800, io2d::format::argb32, io2d::scaling::none, io2d::refresh_style::fixed, 30 };
	display.size_change_callback([](io2d::output_surface & surface)
	{
		surface.dimensions(surface.display_dimensions());
	});
	display.draw_callback([&](io2d::output_surface & surface)
	{
		render.Display(surface);
	});
	display.begin_show();
}
