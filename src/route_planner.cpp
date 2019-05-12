#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel& model, float start_x, float start_y, float end_x, float end_y) : m_model_(model)
{
	/* Scale values to be between 0 and 1*/
	start_x *= 0.01;
	start_y *= 0.01;
	end_x *= 0.01;
	end_y *= 0.01;

	/* Find our closest start and end node for the coordinates */
	start_node_ = &m_model_.find_closest_node(start_x, start_y);
	end_node_ = &m_model_.find_closest_node(end_x, end_y);
}

void RoutePlanner::a_star_search()
{
	/* Start with starting node */
	start_node_->visited = true;
	open_list_.emplace_back(start_node_);

	/* Our algorithm is running here */
	while(!open_list_.empty())
	{
		/* Find best next node for exploration */
		RouteModel::Node* current_node = next_node();

		/* Check if we have reached our goal already */
		if(current_node->distance(*end_node_) == 0)
		{
			m_model_.path = construct_final_path(end_node_);
			return;
		}

		/* Otherwise, add our current node to the neighbors */
		add_neighbors(current_node);
	}
}

void RoutePlanner::add_neighbors(RouteModel::Node* current_node)
{
	/* Populate current node's neighbors vector */
	current_node->find_neighbors();

	/* Go through each neighbor, calc its attributes and then add it to the open list */
	for(auto neighbor : current_node->neighbors)
	{
		neighbor->parent = current_node;
		neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);
		neighbor->h_value = calc_h_value(neighbor);

		open_list_.emplace_back(neighbor);
		neighbor->visited = true;
	}
}

RouteModel::Node* RoutePlanner::next_node()
{
	/* Sort our list of open nodes by their sum of h and g value */
	std::sort(open_list_.begin(), open_list_.end(), [](const auto & _1st, const auto & _2nd)
	{
		return (_1st->h_value + _1st->g_value) < (_2nd->h_value + _2nd->g_value);
	});

	/* Get the node with lowest value and remove it from the list */
	RouteModel::Node* lowest_node = open_list_.front();
	open_list_.erase(open_list_.begin());
	return lowest_node;
}

float RoutePlanner::calc_h_value(RouteModel::Node* node) const
{
	return node->distance(*end_node_);
}

std::vector<RouteModel::Node> RoutePlanner::construct_final_path(RouteModel::Node* current_node)
{
	distance_ = 0.0f;
	std::vector<RouteModel::Node> path_found;

	/* Repeat as long as we are not at the starting point */
	while(current_node->parent != nullptr)
	{
		path_found.emplace_back(*current_node);
		const RouteModel::Node parent = *(current_node->parent);
		distance_ += current_node->distance(parent);
		current_node = current_node->parent;
	}

	/* Add start node and multiply with metric scale to convert to meters */
	path_found.emplace_back(*current_node);
	distance_ *= m_model_.MetricScale();
	return path_found;
}
