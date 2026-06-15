#include <iostream>
#include <string>
#include "nodes.h"
#include "dies.h"
#include "init.h"
#include "net.h"
#include "check.h"
#include "create_hgr.h"
#include "optimize.h"
//#include "patoh.h"

using namespace std;

void place(string f_name, int n_dies, node *node_list)
{
	//clear the die information in the node list
	for (int r = 0; r < 10000; r++)
	{
		node_list[r].die = -1;
	}

	ifstream file(f_name + ".hgr.part." + to_string(n_dies) + ".optimized");
	if (!file.is_open())
	{
		cout << "Cannot open nodes file: " << f_name + ".hgr.part." + to_string(n_dies) << endl;
		return;
	}
	string line;
	int i_node = 0;
	while (getline(file, line))
	{
		if (line.empty()) continue;

		istringstream iss(line);
		string d;
		while (iss >> d)
		{
			node_list[i_node].die = stoi(d);
			i_node++;
		}
	}
	file.close();
}

int main()
{
	int m_s;
	//mode selection: 
	//0 for preparation, 1 for partitioning, 2 for placement, 3 for checking, 4 for optimization
	string file_name;
	die dies[4];
	node nodes[10000];
	cell cells[13];
	int n_c, n_n, n_p, n_no;
	int bridge, _b;
	float a_cell_using, _a_c_u;
	cin >> file_name;
	init(file_name, dies, nodes, cells, n_c, n_n, n_p, n_no);
	while (true)
	{
		cout << "Please select the mode:\n0 for preparation, 1 for partitioning, 2 for placement, 3 for checking, 4 for optimization, others for exit. " << endl;
		cin >> m_s;
		switch (m_s)
		{
		case 0:
			create_hgr(file_name, nodes, n_no, n_n, n_p);
			break;
		case 1:
			//part(file_name + ".hgr", 4); /*using PaToH*/
			break;
		case 2:
			place(file_name, 4, nodes);
			for (int r = 0; r < n_no; r++)
			{
				dies[nodes[r].die].add_to_die(r);
				if (nodes[r].cell == 0) dies[nodes[r].die].lut++;
				else if (nodes[r].cell == 1) dies[nodes[r].die].lut++;
				else if (nodes[r].cell == 2) dies[nodes[r].die].lut++;
				else if (nodes[r].cell == 3) dies[nodes[r].die].lut++;
				else if (nodes[r].cell == 4) dies[nodes[r].die].lut++;
				else if (nodes[r].cell == 5) dies[nodes[r].die].lut++;
				else if (nodes[r].cell == 6) dies[nodes[r].die].ff++;
				else if (nodes[r].cell == 7) dies[nodes[r].die].carry++;
				else if (nodes[r].cell == 8) dies[nodes[r].die].dsp++;
				else if (nodes[r].cell == 9) dies[nodes[r].die].ram++;
				else if (nodes[r].cell == 10) dies[nodes[r].die].io++;
				else if (nodes[r].cell == 11) dies[nodes[r].die].io++;
				else if (nodes[r].cell == 12) dies[nodes[r].die].io++;
			}
			break;
		case 3:
			check(file_name, cells, nodes, dies, bridge, a_cell_using);
			cout << "The average cell using is: " << a_cell_using << endl;
			cout << "The SLL using is: " << bridge << endl;
			break;
		case 4:
			cout << "Please input the requested average cell using and SLL: " << endl;
			cin >> _a_c_u >> _b;
			optimize(file_name, cells, dies, nodes, 4, n_n, n_no, _a_c_u, _b);
			create_result_file(file_name, nodes, n_no, 4);
			break;
		case 5:
			cout << "die0: LUT " << dies[0].lut << " FF " << dies[0].ff << " CARRY " << dies[0].carry << " DSP " << dies[0].dsp << " RAM " << dies[0].ram << " IO " << dies[0].io << endl;
			cout << "die1: LUT " << dies[1].lut << " FF " << dies[1].ff << " CARRY " << dies[1].carry << " DSP " << dies[1].dsp << " RAM " << dies[1].ram << " IO " << dies[1].io << endl;
			cout << "die2: LUT " << dies[2].lut << " FF " << dies[2].ff << " CARRY " << dies[2].carry << " DSP " << dies[2].dsp << " RAM " << dies[2].ram << " IO " << dies[2].io << endl;
			cout << "die3: LUT " << dies[3].lut << " FF " << dies[3].ff << " CARRY " << dies[3].carry << " DSP " << dies[3].dsp << " RAM " << dies[3].ram << " IO " << dies[3].io << endl;
			break;
		default:
			return 0;
		}
	}
}
