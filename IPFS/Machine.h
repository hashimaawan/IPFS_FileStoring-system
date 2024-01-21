#pragma once

#include "bTree.h"
#include "File.h"
#include "cstdlib"
#include "string"
#include "sha1.hpp"
#include "bigint.h"
#include "DHT.h"

using namespace std;

class RoutingTable;

class Machine
{
private:

	// --- We dont need to make bool for active/inactive behaviour of Nodes as inactive Nodes will simply not exist in the System --- //

	// Machine Name
	string name;

	// Machine Position
	bigint index;

	// Unique Machine ID - Hash of the Machine Name
	// This can be a simple ID from 1 to no. of Machines
	bigint ID;

	// Number of Files that the Machine currently holds
	int noOfFiles;

	// Don't know if this should be a pointer or a non-pointer just yet


	// B-Tree that will store the Hash values of the file content & path
	BTree Btree;


	// ------------------------- //
	// --- Private Functions --- //
	// ------------------------- //


	// --- ASSIGNING ID --- //

	void automaticallyAssignID(Machine* machines, bigint maxMachines)
	{
		// --- Step 1 : Find the Smallest Unused ID --- //
		Machine* temp = machines;
		bigint x(1);
		bigint id(0);

		// Base Case
		if (machines->next == nullptr)
		{
			ID = x;
			return;
		}

		while (temp->next != machines)
		{
			if (x != temp->getID())
			{
				ID = x;
				return;
			}

			x++;
			temp = temp->next;
		}

		// Only the Last ID is available
		ID = x;
	}

	void manuallyAssignID(Machine* machines, int i, bigint maxMachines)
	{
		bool invalid = true;
		bigint* id = new bigint(0);
		bigint zero(0);

		// Input Validation
		while (invalid)
		{
			invalid = false;

			cout << "Machine " << i << "'s ID : ";
			cin >> *id;

			// Invalid DataType
			for (int i = 0; i < ID.str.length(); i++)
				if (id->str[i] != '0' && id->str[i] != '1' && id->str[i] != '2' && id->str[i] != '3' && id->str[i] != '4' && id->str[i] != '5' && id->str[i] != '6' && id->str[i] != '7' && id->str[i] != '8' && id->str[i] != '9' && id->str[i] != '\0')
				{
					cout << "\nInvalid Machine ID. Please enter a Positive Integer.\n\n";
					invalid = true;
					break;
				}

			if (invalid)
				continue;

			// ID > maxMachines or ID <= 0
			if (*id > maxMachines || *id <= zero)
			{
				cout << "\nID cannot be more than the Maximum Number of Machines and should be greater than 0.\n";
				invalid = true;
				continue;
			}

			// ID already exists
			if (searchID(machines, id) == true)
			{
				cout << "\nAnother Machine has the same ID.\nPlease enter some other ID.\n";
				invalid = true;
			}
		}

		ID = *id;
	}



	// --- SEARCHING --- // 

	bool searchID(Machine* machines, bigint* id)
	{
		Machine* temp = machines;

		// Dont check if only 1 Machine exists
		if (temp->next == nullptr)
			return false;

		while (temp->next != machines)
		{
			if (temp->getID() == *id)
				return true;

			temp = temp->next;
		}

		return false;
	}

	bool searchID(Machine* machines, string id)
	{
		Machine* temp = machines;

		// Dont check if only 1 Machine exists
		if (temp->next == nullptr)
			return false;

		while (temp->next != machines)
		{
			if (temp->getID() == id)
				return true;

			temp = temp->next;
		}

		return false;
	}



	// --- HELPERS --- //

	bool isH1GreaterThanH2(string h1, string h2)
	{
		// Compare ASCIIs
		for (int i = 0; i < h1.length(); i++)
			if (h1[i] > h2[i])
				return true;
			else if (h2[i] > h1[i])
				return false;
	}

public:


	// --- Table Node Structure --- //

	struct Node {
		bigint index;
		bigint machineID;
		Machine* ptr;
		Node* next;
		Node* prev;

		Node() {
			index = 0;
			machineID = 0;
			ptr = NULL;
			next = NULL;
			prev = NULL;
		}

		Node(bigint idx, bigint mID, Machine* Ptr) {
			index = idx;
			machineID = mID;
			ptr = Ptr;
			next = NULL;
			prev = NULL;
		}
	};



	// --- Routing Table Class --- // 

	class RoutingTable
	{
	public:

		//insert at end
		//delete 
		//search
		//print
		//constructor
		//parametrized constructor(int )

		Node* head;
		Node* tail;

		RoutingTable() {
			head = NULL;
			tail = NULL;
		}

		void empty() {

			if (head == NULL)
				return;
			while (head->next != NULL) {
				Node* temp;
				temp = head;
				head = head->next;
				delete temp;
				temp = NULL;
			}
			head = NULL;
			tail = NULL;
		}



		void insert(bigint idx, bigint mID, Machine* ptr) {
			Node* temp = new Node(idx, mID, ptr);
			Node* temp2 = head;
			if (head == NULL && tail == NULL) {
				head = temp;
				tail = head;
			}
			else {
				tail->next = temp;
				temp->prev = tail;
				tail = temp;
			}
		}

		void deletion(bigint idx, bigint mID) {
			Node* pre = head;
			Node* curr = head;
			for (bigint i = 1; i < idx; i++) {
				pre = curr;
				curr = curr->next;
			}
			pre->next = curr->next;
			curr->next->prev = pre;
			delete curr;
			curr = NULL;
		}
	};


	// Unfortunately, we have to make this public so that it can be accessed in the DHT class
	// Next Pointer for Circular Linked List implementation
	Machine* next;
	RoutingTable* routingTable;



	// ------------------------ //
	// --- Public Functions --- //
	// ------------------------ //

	Machine(int bTreeOrder, bigint i = 0) : ID(0), Btree(bTreeOrder)
	{
		noOfFiles = 0;
		name = "";
		index = i;

		routingTable = new RoutingTable();
		next = nullptr;
	}

	~Machine()
	{
		// De-allocate Memory

		// --- Call Delete Functions of the Routing Table & B-Tree --- //
		delete next;
		delete routingTable;
		Btree.deleteAllObjects();
	}



	// --- ROUTING TABLE FUNCTIONS --- //

	bool findMachine(bigint id) {
		bool found = false;
		Node* temp = this->routingTable->head;
		if (temp == NULL) {
			return true;
		}

		while (temp->next != NULL) {
			if (temp->machineID == id) {
				found = true;
				break;
			}
			temp = temp->next;
		}
		return found;
	}

	Machine* diffMachine(bigint id) {
		Node* temp = this->routingTable->head;
		bool found = false;
		Machine* obj = new Machine(0);

		while (found == false || temp->next != NULL) {
			if (temp->machineID == temp->next->machineID)
				temp = temp->next;
			if (id > temp->machineID && id > temp->next->machineID)
				return temp->ptr;
			if (id > temp->machineID && id < temp->next->machineID) {
				found = true;
				return temp->ptr;
			}
			else
				temp = temp->next;
		}


		//machine not found


		return obj;
	}



	// --- ID --- //

	void specifyMachineID(Machine* machines, int i, bigint maxMachines)
	{
		int x = -1;
		string tempStr = "^";

		// Input Validation
		while (x != 1 && x != 2)
		{
			// It is totally upto the User whether he/she wants to Manually or Automatically assign Machine IDs
			cout << "\n-> Press '1' if you want to manually assign the ID for Machine " << i << ".\n-> Press '2' if you want the ID for Machine " << i << " to be automatically assigned.\n\nChoice : ";
			cin >> x;

			if (x == 1 || x == 2)
			{
				// Ensuring Machine Name Uniqueness
				while (true)
				{
					cout << "\nMachine " << i << "'s Name : ";
					cin >> tempStr;

					Machine* temp = machines;

					// First Machine
					if (machines->next == nullptr)
						break;

					// Second Machine
					if (temp->next->next == machines && temp->getName() == tempStr)
					{
						cout << "\nA Machine with the same name already exists.\n";
						continue;
					}

					if (temp->next->next == machines && temp->getName() != tempStr)
						break;

					// Else
					while (temp->next != machines)
					{
						if (temp->getName() == tempStr)
						{
							cout << "\nA Machine with the same name already exists.\n";
							break;
						}

						temp = temp->next;
					}

					if (temp->next == machines && temp->getName() != tempStr)
						break;
				}

				name = tempStr;
			}

			// Manual ID
			if (x == 1)
				manuallyAssignID(machines, i, maxMachines);

			// Automatic ID
			else if (x == 2)
				automaticallyAssignID(machines, maxMachines);

			// Invalid Input
			else
				cout << "\nPlease enter an valid choice.\n";
		}
	}



	// --- INSERTIONS --- //

	void insertFile(File myFile)
	{
		Btree.insertion(myFile);
		noOfFiles++;
	}



	// --- GETTERS --- //

	string getName()
	{
		return name;
	}

	bigint getID()
	{
		return ID;
	}

	BTree getBtree()
	{
		return Btree;
	}

	File getFile(string key)
	{
		return Btree.FileSearchWithKey(key);
	}

	int getNoOfFiles()
	{
		return noOfFiles;
	}

	bigint getIndex()
	{
		return index;
	}



	// --- SETTERS --- ///

	void setNoOfFiles(int x)
	{
		noOfFiles = x;
	}

	void setIndex(bigint i)
	{
		index = i;
	}



	// --- PRINTING --- //

	void printMachine()
	{
		cout << "\nName  : " << getName() << "\nID    : " << getID() << "\nFiles : " << getNoOfFiles() << endl;
	}

	void printBTree()
	{
		// B-Tree is Empty
		if (Btree.root == nullptr)
		{
			cout << "\nThe Machine does not have any Files to Display.\n";
			return;
		}

		// B-Tree is not Empty
		Btree.displayTree();
	}

	void printRoutingTable(Machine* machinesRoot) {

		cout << "\nROUTING TABLE OF THE MACHINE \n \n";

		Node* temp = this->routingTable->head;
		while (temp->next != machinesRoot->routingTable->head) {
			cout << "INDEX: " << temp->index << "   ";
			cout << "MACHINE ID: " << temp->machineID << endl;
			if (temp->next == nullptr)
				break;
			temp = temp->next;
		}
	}
};