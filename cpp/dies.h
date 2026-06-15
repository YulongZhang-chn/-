#pragma once
#include <iostream>

class die
{
public:
	int capacity = 0; 
	int list[5000];//list of nodes' index 
	int lut = 0, ff = 0, carry = 0, dsp = 0, ram = 0, io = 0;
	void add_to_die(int a);//add node to die
	void del_from_die(int a);//delet node from die
};

void die::add_to_die(int a)
{
	capacity++;
	if (capacity > 5000)
	{
		std::cout << "Die's capacity is overflow!" << std::endl;
	}
	else
	{
		list[capacity - 1] = a;
	}
}
void die::del_from_die(int a)
{
	int j;//a's index
	for (int i = 0; i < capacity; i++)
	{
		if (list[i] == a)
		{
			j = i;
			break;
		}
	}
	for (int i = j; i < capacity; i++)
	{
		list[i] = list[i + 1];
	}
	capacity--;
}