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
#include "init.h"

using namespace std;

void check(string file_name, cell *cell_list, node *node_list, die *die_list, int &bridge, float &average_cell_using)
//caculate the number of bridges and the average cell using of the partitioning result
{
	bridge = 0;

	ifstream file1(file_name + ".nets");

	if (!file1.is_open())
	{
		cout << "Cannot open file: " << file_name + ".nets" << endl;
		return;
	}
	//caculate the average cell using
	int c_M = 0;
	int c_m = 5000;
	for (int i = 0; i < 4; i++)
	{
		if (die_list[i].capacity > c_M) c_M = die_list[i].capacity;
		if (die_list[i].capacity < c_m) c_m = die_list[i].capacity;
	}
	average_cell_using = (c_M - c_m) * 1.0 / c_m;
	//caculate the SLL using
	int d_M = 0, d_m = 3;
	string line;
	while (getline(file1, line))
	{
		if (line.empty()) continue;

		stringstream iss(line);
		string token;
		if (!(iss >> token)) continue;

		if (token == "net") continue;
		if (token == "endnet")
		{
			bridge += (d_M - d_m);
			d_M = 0;
			d_m = 3;
			continue;
		}

		int found_node = -1;
		for (int r = 0; r < 10000; r++)
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
	file1.close();
}