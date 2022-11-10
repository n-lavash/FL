#ifndef ENFA_H
#define ENFA_H

#include "IMachine.h"
#include "DFSA.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include "NFA.h"

// конечный недетерминированный автомат с эпсилон переходом

class ENFA : public IMachine
{
	private:
		std::map<Key, std::vector<std::string>> table;
		std::set<std::string> states;
		std::set<std::string> symbols;
		std::set<std::string> accessStates;
		std::vector<std::string> wordVector;

		bool isFoundGoodWord = false;

	public:
		ENFA() = default;

		void Insert(void* arg) override
		{
			if (!arg) return;
			std::pair<Key, std::string>* castArg = (std::pair<Key, std::string>*)arg;
			states.insert(castArg->first.state);
			symbols.insert(castArg->first.symbol);
			if (!table.count(castArg->first))
			{
				std::vector<std::string> tableValue;
				table[castArg->first] = tableValue;
			}

			table[castArg->first].push_back(castArg->second);
		}

		void Remove(void* arg) override
		{
			if (!arg) return;
			Key* castArg = (Key*)arg;
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

				for (auto& symbol : symbols)
				{
					if (!table.count(Key(state, symbol)))
					{
						tp << "-";
						continue;
					}

					std::string tableValue = "";
					for (auto& vecElement : table[Key(state, symbol)])
					{
						tableValue += vecElement;
						tableValue += ',';
					}

					tp << tableValue;
				}
			}

			tp.PrintFooter();
		}

		void AddEpsTransitions()
		{
			Key key;
			for (auto& state : states)
			{
				key = Key(state, "eps");
				if (table.count(key))
				{
					table[key].push_back(state);
				}
				else
				{
					std::vector<std::string> value{ state };
					table.insert(std::make_pair(key, value));
				}
			}
		}

		void Prepare(std::ifstream& in) override
		{
			std::string statesString;
			std::getline(in, statesString);

			std::istringstream issStates(statesString);
			states = std::set<std::string>((std::istream_iterator<std::string>(issStates)), std::istream_iterator<std::string>());

			AddEpsTransitions();

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

		bool CheckWord(std::string state, int index)
		{
			if (index >= wordVector.size())
			{
				if (accessStates.count(state))
				{
					std::cout << "Good word" << std::endl;
					isFoundGoodWord = true;
				}
				else
				{
					std::cout << "Bad word: current state isn`t access" << std::endl;
				}

				return true;
			}

			return false;
		}

		void HandleWordReccursion(std::string currentState, int index)
		{
			if (CheckWord(currentState, index)) return;
			Key key(currentState, wordVector[index]);

			if (table.count(key))
			{
				for (auto& state : table[key])
				{
					if (isFoundGoodWord) return;
					std::cout << "Move from state " << currentState << " to " << state << " by " << wordVector[index] << std::endl;
					std::cout << "Current state: " << state << std::endl;
					HandleWordReccursion(state, index + 1);
				}
			}
			else if (table[Key(currentState, "eps")].size() > 1)
			{
				key = Key(currentState, "eps");

				for (auto& state : table[key])
				{
					if (isFoundGoodWord) return;
					if (state == currentState) continue;

					std::cout << "Move from state " << currentState << " to " << state << " by eps" << std::endl;
					std::cout << "Current state: " << state << std::endl;
					HandleWordReccursion(state, index);
				}
			}
			else
			{
				std::cout << "Move from state " << currentState << " by " << wordVector[index] << std::endl;
				std::cout << "Devil mark" << std::endl;
				return;
			}
		}

		void HandleWord() override
		{
			std::string currentState = *states.begin();
			int index = 0;
			isFoundGoodWord = false;
			HandleWordReccursion(currentState, index);
		}

		std::string SetToString(std::set<std::string> _states)
		{
			std::string res = "";
			for (auto& state : _states)
			{
				res += state;
				res += ',';
			}

			return res;
		}

		std::vector<std::string> SetToVector(std::set<std::string> _states)
		{
			std::vector<std::string> res;
			for (auto& state : _states)
			{
				res.push_back(state);
			}

			return res;
		}

		void EcloseOne(std::set<std::string>& stateSet, std::set<std::string>& resOne)
		{
			for (auto& state : stateSet)
			{
				Key key(state, "eps");
				if (table.count(key))
				{
					std::set<std::string> argumentState(table[key].begin(), table[key].end());
					argumentState.erase(state);
					resOne.insert(table[key].begin(), table[key].end());
					if (table[key].size() >= 1) EcloseOne(argumentState, resOne);
				}
			}
		}

		std::set<std::string> EcloseAll(std::set<std::string> stateSet)
		{
			std::set<std::string> resAll;
			std::set<std::string> resOne;
			std::set<std::string> argumentState;

			for (auto& state : stateSet)
			{
				argumentState.insert(state);
				EcloseOne(argumentState, resOne);
				argumentState.clear();
				resAll.insert(resOne.begin(), resOne.end());
			}

			return resAll;
		}

		NFA Eclose()
		{
			std::set<std::string> startState{ *states.begin() };
			std::map<Key, std::vector<std::string>> newTable;
			std::set<std::string> newSymbols = symbols;
			newSymbols.erase("eps");

			std::set<std::string> ecloseArgument;
			std::set<std::string> ecloseCurrentStates;

			for (auto& currentState : states)
			{
				ecloseArgument.insert(currentState);
				EcloseOne(ecloseArgument, ecloseCurrentStates);

				std::set<std::string> translateCurrentStatesBySymbol;
				for (auto& newSymbol : newSymbols)
				{
					Key key;
					for (auto& ecloseCurrenState : ecloseCurrentStates)
					{
						key = Key(ecloseCurrenState, newSymbol);
						
						if (!table.count(key)) continue;
						translateCurrentStatesBySymbol.insert(table[key].begin(), table[key].end());
					}

					// не учитываются недостижимые переходы из начального состояния
					if (translateCurrentStatesBySymbol.empty()) continue;

					key = Key(currentState, newSymbol);
					auto vec = SetToVector(translateCurrentStatesBySymbol);

					if (!newTable.count(key))
					{
						newTable.insert(std::make_pair(key, vec));
					}
					else
					{
						for (auto& vecElement : vec)
						{
							newTable[key].push_back(vecElement);
						}
					}

					translateCurrentStatesBySymbol.clear();
				}

				ecloseArgument.clear();
				ecloseCurrentStates.clear();
			}

			std::set<std::string> accessNewStates = EcloseAll(accessStates);

			return NFA(newTable, states, newSymbols, accessNewStates, wordVector);
		}
};

#endif