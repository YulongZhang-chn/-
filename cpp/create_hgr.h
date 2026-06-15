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

using namespace std;

void create_hgr(string file_name, node *node_list,int n_nodes, int n_nets, int n_pins)
//create a .hgr file for partitioning
{
	ofstream file;
	file.open(file_name + ".hgr", ios::out);
	ifstream File(file_name + ".nets");

	if (!File.is_open())
	{
		cout << "Cannot open nodes file: " << file_name + ".nets" << endl;
		return;
	}

	file << "0" << " " << n_nodes << " " << n_nets << " " << n_pins << endl;

	string line;
    while (getline(File, line))
    {
        if (line.empty()) continue;

        istringstream iss(line);
        string token1;
        if (!(iss >> token1)) continue;

        if (token1 == "net") continue;
        if (line == "endnet") { file << endl; continue; }

        int found_pin = -1;
        for (int r = 0; r < n_pins; ++r)
        {
            if (node_list[r].name == token1)
            {
                found_pin = r;
                break;
            }
        }
        file << found_pin << " ";
    }
    File.close();
    file.close();
}
