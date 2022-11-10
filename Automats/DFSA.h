#ifndef DFSA_H
#define DFSA_H

#include "IMachine.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <iterator>

// конечный детерминированный автомат

class DFSA : public IMachine 
{
	private:
		std::map<Key, std::string> table;
		std::set<std::string> states;
		std::set<std::string> symbols;
		std::set<std::string> accessStates;
		std::vector<std::string> wordVector;

	public:
		DFSA() = default;
		DFSA(std::map<Key, std::string> _table, \
			std::set<std::string> _states, \
			std::set<std::string> _symbols, \
			std::set<std::string> _accessStates, \
			std::vector<std::string> _wordVector) : \
			table(_table), states(_states), symbols(_symbols), accessStates(_accessStates), wordVector(_wordVector) {}

		DFSA& operator=(const DFSA& kda)
		{
			table = kda.table;
			states = kda.states;
			symbols = kda.symbols;
			accessStates = kda.accessStates;

			return *this;
		}

		void Insert(void* arg) override
		{
			if (!arg) return;
			std::pair<Key, std::string>* castArg = (std::pair<Key, std::string>*)arg;
			states.insert(castArg->first.state);
			symbols.insert(castArg->first.symbol);
			table.insert(*castArg);
		}

		void Remove(void* arg) override 
		{
			if (!arg) return;
			Key* castArg = (Key *)arg;
			table.erase(*castArg);
		}

		void DisplayTable() override 
		{
			bprinter::TablePrinter tp(&std::cout);

			tp.AddColumn("state", 10);

			for (auto& symbol : symbols) 
			{
				tp.AddColumn(symbol, 10);
			}

			tp.PrintHeader();

			for (auto& state : states) 
			{
				if (state == *states.begin())
				{
					if (accessStates.count(state)) tp << "->*" + state;
					else tp << "->" + state;
				}
				else if (accessStates.count(state)) tp << "*" + state;
				else tp << state;

				Key key;
				for (auto& symbol : symbols) 
				{
					key = Key(state, symbol);
					tp << (table.count(key) ? table[Key(state, symbol)] : "-");
				}
			}

			tp.PrintFooter();
		}

		void Prepare(std::ifstream &in) override 
		{
			std::string statesString;
			std::getline(in, statesString);

			std::istringstream issStates(statesString);
			states = std::set<std::string>((std::istream_iterator<std::string>(issStates)), std::istream_iterator<std::string>());

			std::string symbolsString;
			std::getline(in, symbolsString);

			std::istringstream issSymbols(symbolsString);
			symbols = std::set<std::string>((std::istream_iterator<std::string>(issSymbols)), std::istream_iterator<std::string>());
			
			std::string accessStatesString;
			std::getline(in, accessStatesString);

			std::istringstream issAccessStates(accessStatesString);
			accessStates = std::set<std::string>((std::istream_iterator<std::string>(issAccessStates)), std::istream_iterator<std::string>());
		
			std::string wordString;
			std::getline(in, wordString);

			std::istringstream issWord(wordString);
			wordVector = std::vector<std::string>((std::istream_iterator<std::string>(issWord)), std::istream_iterator<std::string>());
		}

		void HandleWord() override 
		{
			std::string currentState = *states.begin();

			for (auto& wordElement : wordVector)
			{
				Key keyTable(currentState, wordElement);

				if (table.count(keyTable))
				{
					std::cout << "Move from state " << keyTable.state << " to " << table[keyTable] << " by " << keyTable.symbol << std::endl;
					currentState = table[keyTable];
					std::cout << "Current state: " << currentState << std::endl;
				}
				else 
				{
					std::cout << "Move from state " << keyTable.state << " by " << keyTable.symbol << std::endl;
					std::cout << "Bad word: devil mark" << std::endl;
					return;
				}
			}

			std::cout << (accessStates.count(currentState) ? "Good word" : "Bad word: current state isn`t access state") << std::endl;
		}
};

#endif