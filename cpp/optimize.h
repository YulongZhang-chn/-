#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "nodes.h"
#include "dies.h"
#include "cell.h"
#include "net.h"
#include "check.h"

using namespace std;

void sort_die(die *die_list, int *d_sorting, int n_dies)
{
	int i_d_c_m = -1, d_c_m = 5000;
	int is_sorted[10];
	for (int i = 0; i < n_dies; i++) is_sorted[i] = 0;
	for (int i = n_dies; i > 0; i--)
	{
		for (int j = 0; j < n_dies; j++)
		{
			if (die_list[j].capacity < d_c_m && is_sorted[j] ==0)
			{
				i_d_c_m = j;
				d_c_m = die_list[j].capacity;
			}
		}
		d_sorting[i - 1] = i_d_c_m;
		is_sorted[i_d_c_m] = 1;
	}
}
void update_die_list(node *node_list, die *die_list, int n_dies, int n_nodes)
{
	//clear the die list
	for (int i = 0; i < n_dies; i++)
	{
		for (int j = 0; j < die_list[i].capacity; j++)
		{
			die_list[i].list[j] = -1;
		}
		die_list[i].capacity = 0;
	}

	for (int i = 0; i < n_nodes; i++)
	{
		die_list[node_list[i].die].add_to_die(i);
	}
}

void optimize(string file_name, cell *cell_list, die *die_list, node *node_list, int n_dies, int n_nets, int n_nodes, float average_cell_using, int bridge)
{
	ifstream file(file_name + ".nets");
	
	if (!file.is_open())
	{
		cout << "Cannot open file: " << file_name + ".nets" << endl;
		return;
	}
	file.close();
	//select the optimization mode
	int mode = -1;
	float a_c_u;
	int b;
	check(file_name, cell_list, node_list, die_list, b, a_c_u);
	if (b > bridge) mode = 1;
	else
	{
		if (a_c_u > average_cell_using) mode = 2;
		else mode = 0;
	}
	if (mode == -1) mode = 0;
	//optimization
	switch (mode)
	{
	case 1://optimize the partitioning result based on the limited SLL
	{
		//find the net with the largest SLL using
		int d_M = 0, d_m = 3, i_net = 0;
		int n_b_M = 0;
		int i_n_b_M = -1;
		int i_node = 0, n_c_M = 0;
		string line;
		file.open(file_name + ".nets");
		while (getline(file, line))
		{
			if (line.empty()) continue;

			stringstream iss(line);
			string token;
			if (!(iss >> token)) continue;

			if (token == "net") continue;
			if (token == "endnet")
			{
				if (d_M - d_m > n_b_M)
				{
					if (n_c_M < i_node)
					{
						n_b_M = d_M - d_m;
						i_n_b_M = i_net;
						n_c_M = i_node;
					}
				}
				i_net++;
				i_node = 0;
				d_M = 0;
				d_m = 3;
				continue;
			}

			int found_node = -1;
			for (int r = 0; r < n_nodes; r++)
			{
				if (node_list[r].name == token)
				{
					found_node = r;
					break;
				}
			}
			if (node_list[found_node].die > d_M) d_M = node_list[found_node].die;
			if (node_list[found_node].die < d_m) d_m = node_list[found_node].die;
			i_node++;
		}
		file.close();
		if (n_b_M == 0)//if the SLL using of every net is 0, then the partitioning result can't be optimized any more
		{
			cout << "Can't optimize the partitioning result any more. " << endl;
			return;
		}
		else
		{
			//move the nodes
			file.open(file_name + ".nets");
			d_M = 0, d_m = 3, i_net = 0;
			while (getline(file, line))
			{
				if (line.empty()) continue;

				stringstream iss(line);
				string token;
				if (!(iss >> token)) continue;

				if (token == "net") continue;
				if (token == "endnet")
				{
					if (i_net == i_n_b_M) break;
					else
					{
						i_net++;
						continue;
					}
				}

				if (i_net == i_n_b_M)
				{
					int found_node = -1;
					for (int r = 0; r < n_nodes; r++)
					{
						if (node_list[r].name == token)
						{
							found_node = r;
							break;
						}
					}
					if (node_list[found_node].die > d_M) d_M = node_list[found_node].die;
					if (node_list[found_node].die < d_m) d_m = node_list[found_node].die;
				}
				else continue;
			}
			file.close();
			file.open(file_name + ".nets");
			i_net = 0;
			while (getline(file, line))
			{
				if (line.empty()) continue;

				stringstream iss(line);
				string token;
				if (!(iss >> token)) continue;

				if (token == "net") continue;
				if (token == "endnet")
				{
					if (i_net == i_n_b_M) break;
					else
					{
						i_net++;
						continue;
					}
				}

				if (i_net == i_n_b_M)
				{
					int found_node = -1;
					for (int r = 0; r < n_nodes; r++)
					{
						if (node_list[r].name == token)
						{
							found_node = r;
							break;
						}
					}
					if (node_list[found_node].die == d_M)
						node_list[found_node].die = d_M - 2;
					else continue;
				}
				else continue;
			}
			file.close();
		}
		update_die_list(node_list, die_list, n_dies, n_nodes);
		break;
	}
	case 2://optimize the partitioning result based on the limited average cell using
	{
		int d_sort[4];
		sort_die(die_list, d_sort, n_dies);
		int i_net = 0, i_node = 0, i_net_selected = -1;
		int has_die[10], is_in_die[10];
		int net_capacity_M = 0;
		int S = -1;
		for (int i = 0; i < n_dies; i++)
		{
			has_die[i] = 0;
			is_in_die[i] = 0;
		}
		int sum_is_in_die;
		string line;
		//select a net to move its nodes (or the whole net)
		while (getline(file, line))
		{
			if (line.empty()) continue;

			stringstream iss(line);
			string token;
			if (!(iss >> token)) continue;

			if (token == "net") continue;
			if (token == "endnet")
			{
				for (int i = 0; i < n_dies; i++) sum_is_in_die += is_in_die[i];
				if (is_in_die[d_sort[0]] == 0)
				{
					i_net++;
					i_node = 0;
					for (int i = 0; i < n_dies; i++)
					{
						has_die[i] = 0;
						is_in_die[i] = 0;
					}
					sum_is_in_die = 0;
					continue;
				}
				else
				{
					if (sum_is_in_die == 1)
					{
						i_net++;
						i_node = 0;
						for (int i = 0; i < n_dies; i++)
						{
							has_die[i] = 0;
							is_in_die[i] = 0;
						}
						sum_is_in_die = 0;
						continue;
					}
					else
					{
						if (i_node > net_capacity_M)
						{
							net_capacity_M = i_node;
							i_net_selected = i_net;
							S = 0;
						}
						i_net++;
						i_node = 0;
						for (int i = 0; i < n_dies; i++)
						{
							has_die[i] = 0;
							is_in_die[i] = 0;
						}
						sum_is_in_die = 0;
						continue;
					}
				}
			}

			int found_node = -1;
			for (int r = 0; r < n_nodes; r++)
			{
				if (node_list[r].name == token)
				{
					found_node = r;
					break;
				}
			}
			is_in_die[node_list[found_node].die] = 1;
			has_die[node_list[found_node].die]++;
		}
		file.close();
		if (i_net_selected == -1)//special situation
		{
			S = 1;
			i_net = 0;
			i_node = 0;
			for (int i = 0; i < n_dies; i++)
			{
				has_die[i] = 0;
				is_in_die[i] = 0;
			}
			file.open(file_name + ".nets");
			while (getline(file, line))
			{
				if (line.empty()) continue;

				stringstream iss(line);
				string token;
				if (!(iss >> token)) continue;

				if (token == "net") continue;
				if (token == "endnet")
				{
					if (is_in_die[d_sort[0]] == 1)
					{
						i_net_selected = i_net;
						break;
					}
					else
					{
						i_net++;
						i_node = 0;
						for (int i = 0; i < n_dies; i++)
						{
							has_die[i] = 0;
							is_in_die[i] = 0;
						}
						continue;
					}
				}

				int found_node = -1;
				for (int r = 0; r < n_nodes; r++)
				{
					if (node_list[r].name == token)
					{
						found_node = r;
						break;
					}
				}
				is_in_die[node_list[found_node].die] = 1;
			}
			file.close();
		}
		if (S == -1)
		{
			cout << "Can't optimize the partitioning result any more. " << endl;
			return;
		}
		//move nodes (or net)
		i_net = 0;
		i_node = 0;
		for (int i = 0; i < n_dies; i++)
		{
			has_die[i] = 0;
			is_in_die[i] = 0;
		}
		file.open(file_name + ".nets");
		while (getline(file, line))
		{
			if (line.empty()) continue;

			stringstream iss(line);
			string token;
			if (!(iss >> token)) continue;

			if (token == "net") continue;
			if (token == "endnet")
			{
				if (i_net != i_net_selected)
				{
					i_net++;
					continue;
				}
				else break;
			}

			if (i_net != i_net_selected) continue;
			else
			{
				int found_node = -1;
				for (int r = 0; r < n_nodes; r++)
				{
					if (node_list[r].name == token)
					{
						found_node = r;
						break;
					}
				}
				is_in_die[node_list[found_node].die] = 1;
				has_die[node_list[found_node].die]++;
			}
		}
		file.close();
		i_net = 0;
		file.open(file_name + ".nets");
		while (getline(file, line))
		{
			if (line.empty()) continue;

			stringstream iss(line);
			string token;
			if (!(iss >> token)) continue;

			if (token == "net") continue;
			if (token == "endnet")
			{
				if (i_net != i_net_selected)
				{
					i_net++;
					continue;
				}
				else break;
			}

			if (i_net != i_net_selected) continue;
			else
			{
				int found_node = -1;
				for (int r = 0; r < n_nodes; r++)
				{
					if (node_list[r].name == token)
					{
						found_node = r;
						break;
					}
				}
				if (node_list[found_node].die != d_sort[0]) continue;
				else
				{
					if (S == 1) node_list[found_node].die = d_sort[3];
					else
					{
						if (is_in_die[d_sort[3]] == 1) node_list[found_node].die = d_sort[3];
						else if (is_in_die[d_sort[2]] == 1) node_list[found_node].die = d_sort[2];
						else node_list[found_node].die = d_sort[1];
					}
				}
			}
		}
		file.close();
		update_die_list(node_list, die_list, n_dies, n_nodes);
		break;
	}
	default:
		return;
	}
	return;
}

void create_result_file(string file_name, node *node_list, int n_nodes, int n_dies)
{
	ofstream file;
	file.open(file_name + ".hgr.part." + to_string(n_dies) + ".optimized");
	if (!file.is_open())
	{
		cout << "Cannot open file: " << file_name + ".hgr.part." + to_string(n_dies) + ".optimized" << endl;
		return;
	}
	for (int i = 0; i < n_nodes; i++)
	{
		file << node_list[i].die << endl;
	}
	file.close();
}