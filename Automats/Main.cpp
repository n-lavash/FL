#include "DFSA.h"
#include "NFA.h"
#include "ENFA.h"
#include <iostream>
#include <fstream>
#include <memory>

void CallDFSA() {
	DFSA automat;

	std::ifstream in("inputDFSA.txt");

	automat.Prepare(in);

	int numRecords;
	in >> numRecords;


	for (int i = 0; i < numRecords; ++i)
	{
		std::string stateKey, symbolKey, stateValue;
		in >> stateKey >> symbolKey >> stateValue;
		auto pairTable = std::make_pair(Key(stateKey, symbolKey), stateValue);
		std::shared_ptr<std::pair<Key, std::string>> arg(new std::pair<Key, std::string>(pairTable));
		automat.Insert(arg.get());
	}

	automat.HandleWord();
	automat.DisplayTable();
}

void CallNFA() 
{
	NFA automat;

	std::ifstream in("inputNFA.txt");

	automat.Prepare(in);

	int numRecords;
	in >> numRecords;


	for (int i = 0; i < numRecords; ++i)
	{
		std::string stateKey, symbolKey, stateValue;
		in >> stateKey >> symbolKey >> stateValue;
		auto pairTable = std::make_pair(Key(stateKey, symbolKey), stateValue);
		std::shared_ptr<std::pair<Key, std::string>> arg(new std::pair<Key, std::string>(pairTable));
		automat.Insert(arg.get());
	}

	automat.HandleWord();
	automat.DisplayTable();
}

void CallENFA() {
	ENFA automat;

	std::ifstream in("inputENFA.txt");

	automat.Prepare(in);

	int numRecords;
	in >> numRecords;


	for (int i = 0; i < numRecords; ++i)
	{
		std::string stateKey, symbolKey, stateValue;
		in >> stateKey >> symbolKey >> stateValue;
		auto pairTable = std::make_pair(Key(stateKey, symbolKey), stateValue);
		std::shared_ptr<std::pair<Key, std::string>> arg(new std::pair<Key, std::string>(pairTable));
		automat.Insert(arg.get());
	}

	automat.HandleWord();
	automat.DisplayTable();

	NFA kna = automat.Eclose();
	kna.HandleWord();
	kna.DisplayTable();

	DFSA kda = kna.AlgorithmTompson();
	kda.HandleWord();
	kda.DisplayTable();
}

int main()
{
	CallDFSA();
}