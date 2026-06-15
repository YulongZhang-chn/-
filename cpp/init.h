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

void init(string file_name, die *die_list, node *node_list, cell *cell_list, int &n_cells, int &n_nets, int &n_pins, int &n_nodes)
//input: the name of file; 
//output: 4 dies, 10000 nodes, the number of cells, nets and pins in the file, the net list
{
	int i_cell = -1, j_opin = 0;
    int i_pin = 0;
	int i_net = -1;
    n_cells = 0;
    n_nets = 0;
    n_pins = 0;
	n_nodes = 0;
	//initialize or clear the node list
    for (int i = 0; i < 10000; i++)
    {
        node_list[i].name = "";
		node_list[i].cell = -1;
		node_list[i].net = -1;
		node_list[i].die = -1;
	}
	//initialize cell list this research needs
    cell_list[0].name = "LUT1";
    cell_list[1].name = "LUT2";
    cell_list[2].name = "LUT3";
    cell_list[3].name = "LUT4";
    cell_list[4].name = "LUT5";
    cell_list[5].name = "LUT6";
    cell_list[6].name = "FDRE";
    cell_list[7].name = "CARRY8";
    cell_list[8].name = "DSP48E2";
    cell_list[9].name = "RAMB36E2";
    cell_list[10].name = "IBUF";
    cell_list[11].name = "OBUF";
    cell_list[12].name = "BUFGCE";
	n_cells = 13;
	//open the files
    ifstream file1(file_name + ".lib");
    ifstream file2(file_name + ".nets");
    ifstream file3(file_name + ".nodes");
    ifstream file4(file_name + ".scl");
	//check if the files are opened successfully
    if(!file1.is_open())
    {
        cout << "Cannot open file: " << file_name << endl;
        return;
    }
    else if(!file2.is_open())
    {
        cout << "Cannot open file: " << file_name << endl;
        return;
    }
    else if(!file3.is_open())
    {
        cout << "Cannot open file: " << file_name << endl;
        return;
    }
    else if(!file4.is_open())
    {
        cout << "Cannot open file: " << file_name << endl;
        return;
    }

    string line;
	//complete the cell list based on the .lib file
    while (getline(file1, line))
    {
		if (line.empty()) continue;

        istringstream iss(line);
        string token1, token2, token3;
        if (!(iss >> token1)) continue;
		if (!(iss >> token2)) continue;
		if (!(iss >> token3)) continue;

        if (token1 == "CELL")
        {
            for (int r = 0; r < n_cells; r++)
            {
                if (cell_list[r].name == token2)
                {
                    i_cell = r;
                    break;
				}
            }
			continue;
        }
        if (token1 == "END")
        {
            i_cell = -1;
            j_opin = 0;
			continue;
        }

        if (token3 == "OUTPUT" && i_cell != -1)
        {
			cell_list[i_cell].output_pin[j_opin] = token2;
			j_opin++;
        }
    }
    file1.close();
    file4.close();
	//initialize the node list and the cell list based on the .nodes file
    while (getline(file3, line))
    {
        if (line.empty()) continue;

        istringstream iss(line);
        string token1, token2;
        if (!(iss >> token1)) continue;
        if (!(iss >> token2)) continue;
        //write down the name of the node
		node_list[i_pin].name = token1;
		//find and write down the cell index of the node
        int found_cell = -1;
        if (!token2.empty())
        {
            for (int r = 0; r < n_cells; ++r)
            {
                if (cell_list[r].name == token2)
                {
                    found_cell = r;
                    break;
                }
            }
        }
        node_list[i_pin].cell = found_cell;
		n_nodes++;


        ++i_pin;
		if (i_pin > 10000) break;
    }
	file3.close();
	//initialize the node list and based on the .nets file
    while (getline(file2, line))
    {
        if (line.empty()) continue;

        istringstream iss(line);
        string token1, token2;
        if (!(iss >> token1)) continue;
        if (!(iss >> token2)) continue;

		if (line == "endnet") continue;
        if (token1 == "net") 
        { i_net++; n_nets++; continue; }
		//write down the net index of the node
		n_pins++;
        int found_pin = -1;
        for (int r = 0; r < n_pins; ++r)
        {
            if (node_list[r].name == token1)
            {
                found_pin = r;
                break;
            }
		}
		node_list[found_pin].net = i_net;

		if (i_net > 10000) break;
    }
    file2.close();
}
