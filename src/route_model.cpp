#include "route_model.h"

void RouteModel::Node::find_neighbors()
{
	/* Get all of the roads that the current node belongs to */
	for (auto& road : parent_model_->node_to_road[this->index_])
	{
		/* Access all the indices of the nodes on that road and find the closest one */
		auto* new_neighbor = this->find_neighbor(parent_model_->Ways()[road->way].nodes);

		/* We find a new neighbor, so add it to our neighbors*/
		if (new_neighbor)
		{
			this->neighbors.emplace_back(new_neighbor);
		}
	}
}

RouteModel::Node::Node(int idx, RouteModel* search_model, Model::Node node) : Model::Node(node),
parent_model_(search_model), index_(idx)
{

}

RouteModel::Node* RouteModel::Node::find_neighbor(std::vector<int> node_indices) const
{
	Node* closest_node = nullptr;

	/* Go through all node indices and find closest */
	for (auto node_idx : node_indices)
	{
		Node node = parent_model_->SNodes()[node_idx];

		/* Do not return the node itself and check it hasn't been visited so far */
		if ((this->distance(node) != 0) && (!node.visited))
		{
			/* If there is no closest node or current one is closer than previous */
			if ((closest_node == nullptr) || (this->distance(node) < this->distance(*closest_node)))
			{
				/* Set our closest node to the address of new closest node */
				closest_node = &parent_model_->SNodes()[node_idx];
			}
		}
	}

	return closest_node;
}

RouteModel::RouteModel(const std::vector<std::byte>& xml) : Model(xml)
{
	/* Add all nodes to the vector of nodes */
	int counter = 0;
	for (auto& node : this->Nodes())
	{
		this->m_nodes.emplace_back(counter, this, node);
		counter++;
	}

	/* Find all roads that correspond to those nodes */
	create_node_to_road_hashmap();
}

RouteModel::Node& RouteModel::find_closest_node(float x, float y)
{
	Node input;
	input.x = x;
	input.y = y;

	float min_dist = std::numeric_limits<float>::max();
	int closest_idx = 0;

	for (auto& road : Roads())
	{
		if (road.type != Road::Type::Footway)
		{
			for (auto node_idx : Ways()[road.way].nodes)
			{
				float dist = input.distance(SNodes()[node_idx]);
				if (dist < min_dist)
				{
					closest_idx = node_idx;
					min_dist = dist;
				}
			}
		}
	}

	return SNodes()[closest_idx];
}

void RouteModel::create_node_to_road_hashmap()
{
	/* Go through all roads */
	for (auto& road : Roads())
	{
		/* Exclude footways */
		if (road.type != Road::Type::Footway)
		{
			/* For each node along the road, get our way and his nodes as int's */
			for (auto node_idx : Ways()[road.way].nodes)
			{
				/* If our node to road does not have this node in it, create an empty road first */
				if (node_to_road.find(node_idx) == node_to_road.end())
				{
					node_to_road[node_idx] = std::vector<const Model::Road*>();
				}
				/* Afterwards push the address of that road to the hashmap */
				node_to_road[node_idx].push_back(&road);
			}
		}
	}
}
