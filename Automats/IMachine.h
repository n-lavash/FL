#ifndef IMACHINE_H
#define IMACHINE_H

#pragma comment(lib, "C:\\Users\\levas\\Downloads\\bprinter\\buil\\Debug\\bprinter.lib")

#include <fstream>
#include <string>
#include "bprinter/table_printer.h"

struct Key
{
	std::string state, symbol;
	Key() = default;
	Key(std::string _state, std::string _symbol) : state(_state), symbol(_symbol) {}
	bool operator<(const Key& key) const
	{
		return std::pair<std::string, std::string>(state, symbol) < std::pair<std::string, std::string>(key.state, key.symbol);
	}
};

class IMachine 
{
	public:
		virtual void Insert(void* arg) = 0;
		virtual void Remove(void* arg) = 0;
		virtual void DisplayTable() = 0;
		virtual void Prepare(std::ifstream &in) = 0;
		virtual void HandleWord() = 0;

		virtual ~IMachine(){};
};

#endif