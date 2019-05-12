#pragma once

#include <limits>
#include <cmath>
#include <unordered_map>
#include "model.h"

class RouteModel : public Model 
{

public:

	class Node : public Model::Node 
	{
	public:
		Node* parent = nullptr;
		float h_value = std::numeric_limits<float>::max();
		float g_value = 0.0;
		bool visited = false;
		std::vector<Node*> neighbors;
		void find_neighbors();

		Node() = default;
		Node(int idx, RouteModel* search_model, Model::Node node);

		float distance (Node other) const
		{
			return std::sqrt(std::pow((x - other.x), 2) + std::pow((y - other.y), 2));
		}

	private:
		int index_{};
		Node* find_neighbor(std::vector<int> node_indices) const;
		RouteModel* parent_model_ = nullptr;
	};



	RouteModel(const std::vector<std::byte>& xml);
	Node& find_closest_node(float x, float y);

	auto& SNodes()
	{
		return m_nodes;
	}

	auto& get_node_to_roadmap()
	{
		return node_to_road;
	}

	std::vector<Node> path;

private: 

	void create_node_to_road_hashmap();
	std::unordered_map<int, std::vector<const Model::Road*>> node_to_road;
	std::vector<Node> m_nodes;

};
