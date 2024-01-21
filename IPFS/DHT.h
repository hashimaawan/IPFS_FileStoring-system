#pragma once
#include "Machine.h"
#include "BigInt.h"
#include <iostream>
#include <cmath>
#include <direct.h>
#include <io.h>
#include <fstream>

using namespace std;

Machine* machinesRoot;

class DHT
{
private:

	// No. of Bits the Hash Function will use
	int identifierSpace;

	// Order of the B-Tree
	int bTreeOrder;

	// Maximum no. of Machines
	bigint maxMachines;

	// Maximum no. of Files
	bigint maxFiles;

	// Current no. of Files
	bigint currFileNumber;

	// Current no. of Machines
	bigint currMachineNumber;

	// Circular Linked List
	Machine* machines;



	// ------------------------- //
	// --- Private Functions --- //
	// ------------------------- //

	// --- INITIAL USER INPUTS --- //

	void specifyIdentifierSpace()
	{
		cout << "\nMax Identifier Space : 160 Bits\nIdentifier Space     : ";
		cin >> identifierSpace;

		// Input Validation
		while (identifierSpace > 160 || identifierSpace < 0)
		{
			cout << "\nIdentifier Space cannot be greater than 160 bits or less than 0.\n" << "Identifier Space : ";
			cin >> identifierSpace;
		}

		// Only calculating correct maxMachines till 100 Bits
		bigint temp(identifierSpace);
		maxMachines = maxMachines._big_pow(maxMachines, temp);
		cout << "Max no. of Machines  : " << maxMachines << endl;

		// Only calculating correct maxFiles till 100 Bits
		maxFiles = maxMachines._big_pow(maxFiles, temp);
		cout << "Max no. of Files     : " << maxFiles << endl;
	}

	void specifyBtreeOrder()
	{
		int x = -1;

		// Input Validation
		while (x < 2)
		{
			cout << "\nOrder of the B-Tree : ";
			cin >> x;

			if (x < 2)
				cout << "\nPlease enter a Valid Btree order of greater then or equal to 2\n";
		}

		if (x == 4)
			x--;

		bTreeOrder = x;
	}



	// --- INSERTION --- //

	void addMachine()
	{
		// Dont allow more Machines than Max
		if (currMachineNumber < maxMachines)
		{
			currMachineNumber++;

			// No Machines are Present
			if (machines == nullptr)
			{
				machines = new Machine(bTreeOrder, currMachineNumber);
				machinesRoot = machines;
			}

			// Only 1 Machine is Present
			else if (machines->next == nullptr)
			{
				machines->next = new Machine(bTreeOrder, currMachineNumber);
				// To ensure Circular Structure
				machines->next->next = machines;
			}

			// >= 2 Machines are Present
			else
			{
				// Temp will make sure the 'next' of each Machine is correctly linked
				Machine* temp2 = machines;
				Machine* nodeToBeInserted = new Machine(bTreeOrder, currMachineNumber);

				// Get to the End of the List
				while (temp2->next != machines) {
					temp2 = temp2->next;
				}
				// Now, add the Machine at this Place
				temp2->next = nodeToBeInserted;
				nodeToBeInserted->next = machines;
			}
		}

		else
			cout << "\nThe system already has the maximum no. of machines.\nMore machines cannot be added.\n";
	}

	void addMachines()
	{
		bigint one(1);
		bigint x(maxMachines + one);
		bigint x2(0);
		bigint zero(0);
		Machine* temp = machines;
		bool invalid;

		// - operator is not working properly
		// This number should be < Maximum no. of Machines allowed
		while (x > maxMachines - currMachineNumber)
		{
			invalid = false;

			cout << "\nEnter the no. of Machines that you want to add to the System right now : ";
			cin >> x;

			// Invalid DataType 
			for (int i = 0; i < x.str.length(); i++)
				if (x.str[i] != '0' && x.str[i] != '1' && x.str[i] != '2' && x.str[i] != '3' && x.str[i] != '4' && x.str[i] != '5' && x.str[i] != '6' && x.str[i] != '7' && x.str[i] != '8' && x.str[i] != '9')
				{
					cout << "\nPlease enter an Integer >= 0.\n";
					invalid = true;
					break;
				}

			if (invalid)
				continue;

			if (x >= zero && x <= maxMachines - currMachineNumber)
				break;
			else
				cout << "\nYour input cannot be more than the Maximum no. of Machines specified\nPlease Try Again.\n";
		}

		// Get Temp to the last Machine
		if (currMachineNumber >= one)
		{
			temp = machines;

			if (temp->next != nullptr)
				while (temp->next != machines)
					temp = temp->next;
		}

		// Add x no. of Machines to the System
		int j = 0;

		for (bigint i = 0; i < x; j++, i++)
		{
			// Now add that Machine
			addMachine();

			if (currMachineNumber == one)
				temp = machines;
			else
			{
				// Get Temp to the last Machine becuase there would be an error if the machineRoot was changed in this Loop
				while (temp->next != machines) {
					temp = temp->next;
				}
			}

			// Input IDs of the Machine
			if (i == zero)
				cout << "\nNote : ID cannot be more than the Maximum Number of Machines (" << maxMachines << ") and should be greater than 0.\n";

			temp->specifyMachineID(machines, j + 1, maxMachines);

			// Put it in its Correct Place
			sortMachines();

			// Make its Folder
			makeFolder(temp->getID());

			// Assign its Files to it
			if (temp->next != nullptr)
				inheritFiles(temp);
		}
	}

	void insertFile()
	{
		// Base Case
		if (machines == nullptr)
		{
			cout << "\nFiles can only be inserted in the presence of a Machine.\nPlease add a Machine before inserting a File.\n\n";
			return;
		}

		string name, path;

		// Check if currFiles <= maxFiles
		// --- Step 1 : Input File --- //
		cout << "\nFile Name : ";
		cin >> name;

		cout << "File Path : ";
		cin >> path;

		File myFile(name, path);


		// --- Step 2 : Find Machine to store it in --- // 
		bigint id = myFile.hashFile(maxMachines);
		Machine* temp = getMachine(id);


		// --- Step 3 : Store it in that Machine --- //
		temp->insertFile(myFile);
		cout << "\nThe File has successfully been stored in Machine " << temp->getID() << ".\n";
		currFileNumber++;


		// --- Step 4 : Move File to that Machine's Folder --- //
		moveFile(myFile, temp);
	}



	// --- DELETION --- //

	void removeMachine()
	{
		bigint x(0);

		cout << "\nID of the Machine that you want to Remove : ";
		cin >> x;

		// Machine doesn't Exist
		if (searchMachine(x) == false)
		{
			cout << "\nThe specified Machine does not Exist.\n";
			return;
		}


		// Machine Exists -> Remove it
		Machine* tempPrev = machines;
		Machine* temp = machines->next;

		// If Machine is at the Front
		if (x <= machines->getID())
		{
			// Distribute its Files
			distributeFiles(machines);

			// Delete Machine Folder
			deleteMachineFolder(x);

			if (temp == nullptr)
			{
				machines = nullptr;
				currMachineNumber = 0;
			}

			else
			{
				// Get TempPrev to the End
				while (tempPrev->next != machines)
					tempPrev = tempPrev->next;

				// Link Pointers
				tempPrev->next = machines->next;
				machines = tempPrev->next;
				currMachineNumber--;
				machinesRoot = machines;
			}

			updateIndexes();
			return;
		}

		// Machine is not at the Front
		// --- Step 1 : Get to the Machine --- //

		while (true)
		{
			if (x <= temp->getID())
				break;

			tempPrev = tempPrev->next;
			temp = temp->next;
		}

		// --- Step 2 : Distribute Files --- //
		distributeFiles(temp);


		// --- Step 3 : Delete Machine Folder --- //
		deleteMachineFolder(x);


		// --- Step 4 : Free Memory & Link Pointers --- //

		// Machine is at the End
		if (temp->next == machines)
		{
			temp = nullptr;
			tempPrev->next = machines;
			currMachineNumber--;
			updateIndexes();

			return;
		}

		// Machine is somewhere b/w Front & End
		tempPrev->next = temp->next;
		temp = nullptr;
		currMachineNumber--;
		updateIndexes();
	}

	void removeAllMachines()
	{
		Machine* temp = machines;

		for (int i = 0; i < currMachineNumber; i++)
		{
			temp = temp->next;
			//delete machines;
			machines = temp;
		}

		// delete machinesRoot
		machinesRoot = nullptr;
	}

	void removeFile()
	{
		string key;

		cout << "\nFile Key : ";
		cin >> key;

		deleteFile(key);
	}

	void deleteFile(string key)
	{
		// --- Step 1 : Convert Hexadecimal Key to an Integer --- //
		bigint max(maxMachines.str);
		bigint id(max.hexToInt(key));


		// --- Step 2 : Find which Machine the File is stored at --- //
		id = id % max;
		id++;


		// --- Step 3 : Traverse to the Machine using the Routing Table --- //
		Machine* x = new Machine(2); // = machines.routingTable.findMachine(id)


		// --- Step 4 : Delete File from B-Tree--- //
		File temp;

		if (x->getBtree().search(key) == true)
			temp = x->getBtree().deletionKey(key);
		else
			cout << "\nThe File does not exist.\n\n";


		// --- Step 5 : Delete File from Directory --- //
		deleteFileFromFolder(temp);
	}



	// --- SORTING --- //

	void sortMachines()
	{
		Machine* node = machines;
		Machine* nodePrev = machines;

		// Only 1 Machine Exists
		if (node->next == nullptr)
			return;

		// Only 2 Machines Exist
		if (node->next->next == machines)
		{
			if (node->next->getID() < node->getID())
			{
				machines = node->next;
				machinesRoot = machines;
			}

			return;
		}

		// More than 2 Machines Exist

		// --- Step 1 : Get node to the newly Inserted Machine
		node = machines->next;

		while (node->next != machines)
		{
			nodePrev = nodePrev->next;
			node = node->next;
		}

		// --- Step 2 : Find where to Place it --- //

		// If it needs to be Placed at the Front of the List
		if (node->getID() < machines->getID())
		{
			machines = node;
			machinesRoot = machines;
			return;
		}

		// Find its Place
		Machine* xPrev = machines;
		Machine* x = machines->next;

		while (x != node)
		{
			if (x->getID() > node->getID())
			{
				xPrev->next = node;
				node->next = x;
				nodePrev->next = machines;
				break;
			}

			x = x->next;
			xPrev = xPrev->next;
		}

		machinesRoot = machines;
	}

	void updateIndexes()
	{
		bigint i = 1;
		Machine* temp = machines;

		while (temp->next != machines)
		{
			temp->setIndex(i);
			i++;
			temp = temp->next;
		}
	}



	// --- PRINTING --- //

	void printDHT()
	{
		bigint t(0);

		if (currMachineNumber == t)
		{
			cout << "\nThere are currently no Machines in the System.\n\n";
			return;
		}

		Machine* temp = machines;

		for (int i = 0; i < currMachineNumber; i++)
		{
			cout << "\n\nMachine : " << i + 1 << "\nName    : " << temp->getName() << "\nID      : " << temp->getID() << "\nFiles   : " << temp->getNoOfFiles() << endl;
			temp = temp->next;
		}
	}

	void printBTree()
	{
		bigint id(0), zero(0);

		while (id <= zero)
		{
			cout << "\nMachine ID : ";
			cin >> id;

			if (id <= zero)
				cout << "\nPlease enter a Positive Integer.\n";
		}

		// Machine does not Exist
		if (searchMachine(id) == false)
		{
			cout << "\nNo Machine with ID " << id << " exists.\n";
			return;
		}

		// Machine Exists
		getMachine(id)->printBTree();
	}

	void printFileDetails(File myFile)
	{
		cout << "\n\nName    : " << myFile.getName();
		cout << "\nPath    : " << myFile.getPath();
		cout << "\nKey     : " << myFile.getKey();
		cout << "\nMachine : " << myFile.getMachineID();
		cout << "\n\nContent :\n" << myFile.getFileContent() << endl;
	}

	void printRoutingTable()
	{
		bigint id(0);

		cout << "\nMachine ID : ";
		cin >> id;

		printRoutingTable(id);
	}

	void printRoutingTable(bigint id)
	{
		if (searchMachine(id) == false)
		{
			cout << "\nMachine does not exist.\n";
			return;
		}

		Machine* temp = getMachine(id);
		temp->printRoutingTable(machinesRoot);

	}



	// --- SEARCHING --- //

	void searchMachine()
	{
		bigint id(0);
		cout << endl;

		cout << "\nid of the machine to be searched : ";
		cin >> id;
		
		Machine* temp = machines;
		
		// If only 1 Machine Exists
		if (temp->next == nullptr && id == temp->getID())
		{
			cout << "The Machine already exists in the System.\n";
			return;
		}

		if (temp->next == nullptr && id != temp->getID())
		{
			cout << "The Machine does not exist in the System.\n";
			return;
		}
			
		while (temp->next != machines)
		{
			if (id == temp->getID())
			{
				temp->printMachine();
				return;
			}

			temp = temp->next;
		}

		// For the Last Machine
		if (id == temp->getID())
		{
			temp->printMachine();
			return;
		}

		cout << "The Machine does not exist in the System.\n";

		//Machine* x;
		//bigint one(1);

		//// one machine exists in whole DHT
		//if (currMachineNumber == one) {
		//	x = machines;
		//	if (machines->findMachine(id) == true) {
		//		x = machines;
		//	}

		//}

		//// > 1 machines exists in whole DHT
		//else {

		//	// file is stored on the same machine (case 1)
		//	if (machines->findMachine(id) == true) {
		//		x = machines;
		//	}

		//	// search query forwared to the first node of current machine's routing table (case 2)
		//	else if (id > machines->getID() && id <= machines->routingTable->head->machineID) {
		//		x = machines->routingTable->head->ptr;
		//	}

		//	// same machine
		//	else if (machines->getID() == id) {
		//		x = machines;
		//	}

		//	// search query forwarded to node returned by diffMachine function (case 3)
		//	else {
		//		x = machines->diffMachine(id);
		//	}
		//}

		//if (id == x->getID()) {
		//	x->printMachine();
		//	return;
		//}

		//cout << "The Machine does not exist in the System! \n";
	}

	bool searchMachine(bigint id)
	{
		Machine* temp = machines;

		// If only 1 Machine Exists
		if (temp->next == nullptr && id == temp->getID())
			return true;

		if (temp->next == nullptr && id != temp->getID())
			return false;

		while (temp->next != machines)
		{
			if (id == temp->getID())
				return true;

			temp = temp->next;
		}

		// For the Last Machine
		if (id == temp->getID())
			return true;

		return false;
	}

	Machine* getMachine(bigint id)
	{
		Machine* temp = machines;

		// If only 1 Machine Exists
		if (temp->next == nullptr)
			return machines;

		while (temp->next != machines)
		{
			// Machine Found
			if (temp->getID() == id)
				return temp;

			// Machine not Found, return its Successor
			if (temp->getID() > id)
				return temp;

			temp = temp->next;
		}

		// For the Last Machine
		if (temp->getID() == id)
			return temp;

		// Worst Case : Return the Last Machine
		temp = machines;

		while (temp->next != machines)
			temp = temp->next;

		return temp;
	}

	void searchFile()
	{
		string key;
		cout << "\nFile Key : ";
		cin >> key;

		searchFile(key);
	}

	void searchFile(string key)
	{
		// --- Step 1 : Convert Hexadecimal Key to an Integer --- //
		bigint max(maxMachines.str);
		bigint id(max.hexToInt(key));


		// --- Step 2 : Find which Machine the File is stored at --- //
		id = id % max;
		id++;

		// --- Step 3 : Traverse to the Machine using the Routing Table --- //
		Machine* x;
		bigint one(1);

		// one machine exists in whole DHT
		if (currMachineNumber == one) {
			x = machines;
			if (machines->findMachine(id) == true) {
				x = machines;

			}

		}
		// > 1 machines exists in whole DHT
		else {

			// file is stored on the same machine (case 1)
			if (machines->findMachine(id) == true) {
				x = machines;
			}

			// search query forwared to the first node of current machine's routing table (case 2)
			else if (id > machines->getID() && id <= machines->routingTable->head->machineID) {
				x = machines->routingTable->head->ptr;
			}

			// same machine
			else if (machines->getID() == id) {
				x = machines;
			}

			// search query forwarded to node returned by diffMachine function (case 3)
			else {
				x = machines->diffMachine(id);
			}
		}

		// --- Step 4 : Display File Information --- //	
		if (x->getBtree().search(key) == true)
			x->getFile(key).displayFileInformation();
		else
			cout << "\nThe File does not exist.\n\n";
	}

	Machine* findSuccessor(Machine* x)
	{
		return x->next;
	}

	Machine* findPredecessor(Machine* x)
	{
		if (x->next == nullptr)
			return nullptr;
		else
		{
			Machine* temp = x;

			while (temp->next != x)
				temp = temp->next;

			return temp;
		}
	}



	// --- DIRECTORY FUNCTIONS --- //

	void deleteFileFromFolder(File myFile)
	{
		if (remove(myFile.getPath().c_str()) == 0)
			cout << "File deleted successfully.\n";
		else
			cerr << "Error deleting file.\n";
	}

	void makeFolder(bigint id)
	{
		string x = id.str;
		string folderName = "Machine" + x;

		// Check if the folder already exists
		if (_mkdir(folderName.c_str()) == 0)
			cout << "Folder created successfully.\n";
		else
			cerr << "Error creating folder.\n";
	}

	void moveFile(File myFile, Machine* temp)
	{
		string x = temp->getID().str;
		string destinationFolder = "Machine" + x;
		string destinationFilePath = destinationFolder + "/" + myFile.getName();
		string sourceFilePath = myFile.getPath();

		// Move File
		if (rename(sourceFilePath.c_str(), destinationFilePath.c_str()) == 0)
		{
			cout << "File moved successfully.\n";
			myFile.setPath(destinationFilePath);
		}

		else
			cerr << "Error moving file.\n";
	}

	void deleteMachineFolder(bigint x)
	{
		// Specify the folder path
		string folderPath = "C:\\Users\\92334\\Desktop\\IPFS\\DHT\\Machine";
		folderPath += x.str;

		// Use _rmdir to delete the empty folder on Windows
		if (_rmdir(folderPath.c_str()) == 0)
			cout << "Folder deleted successfully.\n";
		else
			cerr << "Error deleting folder.\n";
	}

	void transferFile(File myFile, Machine* destination, Machine* source)
	{
		// Create an ofstream object to write to the file
		ofstream outputFile(myFile.getName());

		// Check if the file is successfully opened
		if (outputFile.is_open())
		{
			// Write a string to the file
			outputFile << myFile.getFileContent();

			// Close the file
			outputFile.close();

			// Construct the destination path
			string destinationPath = "Machine" + destination->getID().str + "\\" + myFile.getName();

			// Move the file to the folder
			if (rename(myFile.getName().c_str(), destinationPath.c_str()) == 0)
				std::cout << "File created, text written, and moved to the folder successfully." << endl;
			else
				cerr << "Error moving the file." << endl;

			// Specify the file name and folder name
			string filename = "Machine" + source->getID().str + "/" + myFile.getName();

			// Check if the file exists before attempting to delete
			if (_access(filename.c_str(), 0) == 0) {
				// Remove the file
				if (_unlink(filename.c_str()) == 0)
					std::cout << "File deleted successfully." << endl;
				else
					std::cerr << "Error deleting the file." << endl;
			}

			else
				cerr << "File not found: " << filename << endl;
		}

		else
			cerr << "Unable to open the file: " << myFile.getName() << endl;
	}



	// --- ROUTING TABLE ---//

	bigint power(int i) {
		bigint x = 2;
		if (i == 0)
			return 1;

		return x * power(i - 1);
	}

	Machine* succ(int i, bigint mID, Machine* ptr) {
		bigint machineID = mID + power(i - 1);


		// usually used for last machine's successor
		if (machineID > maxMachines) {
			machineID = machineID - maxMachines;
			ptr = machinesRoot;
		}

		// when last node's last index value is less than first machine's ID 
		if (machineID < ptr->getID()) {
			return ptr;
		}

		// finding correct successor
		while (machineID > ptr->getID() && machineID > ptr->next->getID()) {
			ptr = ptr->next;
			if (ptr == machinesRoot) {
				return machinesRoot;
			}
		}
		return ptr->next;
	}

	void createTable() {

		// to create routing table at every machine
		Machine* temp = machinesRoot;

		// machine data to be added in each machine's routing table
		Machine* temp2;

		// identifier space in bigint
		bigint idSpace(identifierSpace);


		if (temp->next == NULL) {
			for (int i = 1; i <= identifierSpace; i++) {
				temp->routingTable->insert(i, temp->getID(), temp);
			}
			return;
		}

		// creates routing table at each machine except for last machine
		while (temp->next != machinesRoot) {
			for (int i = 1; i <= identifierSpace; i++) {
				temp2 = succ(i, temp->getID(), temp);
				temp->routingTable->insert(i, temp2->getID(), temp2);
			}
			temp = temp->next;
		}

		// creation of last machine's routing table
		for (int i = 1; i <= identifierSpace; i++) {
			temp2 = succ(i, temp->getID(), temp);
			temp->routingTable->insert(i, temp2->getID(), temp2);
		}


	}

	void emptyTable() {
		Machine* temp = machinesRoot;

		//root is deleted
		if (temp == NULL)
			return;

		if (temp->next == NULL) {
			temp->routingTable->empty();
			return;
		}

		//deletes routing table of every machine
		while (temp->next != machinesRoot) {
			temp->routingTable->empty();
			temp = temp->next;
		}

		temp->routingTable->empty();
	}



	// --- HELPERS --- //

	void inheritFiles(Machine* x)
	{
		if (machines == nullptr)
			return;

		// Inherit appropriate Files
		BTree newBtree = findSuccessor(x)->getBtree().Copyingactualfiles(x->getID());

		if (newBtree.root != nullptr)
			transferBtree(x, newBtree.root);
	}

	void transferBtree(Machine* abc, BTREE_Node* temp)
	{
		int position;
		// Iterate through all keys in the current node
		for (position = 0; position < temp->counter; position++)
		{
			// If the node is not a leaf node (it's an internal node)
			if (temp->IsChildLeaf == 0)
			{
				// Recursively call transferBtree for the child node
				transferBtree(abc, temp->child[position]);
			}

			// Transfer File
			abc->insertFile(temp->btreeKey[position]);
			findSuccessor(abc)->setNoOfFiles(findSuccessor(abc)->getNoOfFiles() - 1);
			transferFile(temp->btreeKey[position], abc, findSuccessor(abc));
		}

		// Print keys of the last child node if it's not a leaf node
		if (temp->IsChildLeaf == 0)
			transferBtree(abc, temp->child[position]);
	}

	void distributeFiles(Machine* x)
	{
		// No Files to Distribute
		if (x->getBtree().root == nullptr)
			return;

		// Distribute all Files
		BTREE_Node* newRoot = x->getBtree().removeAllAndCopy();
		BTree newBtree(bTreeOrder);
		newBtree.root = newRoot;
		transferBtree(findPredecessor(x), newBtree.root);
	}




public:

	DHT() : maxMachines(2), maxFiles(2)
	{
		// Only add Machines to the System when the User specifies to do so
		machines = nullptr;

		// Hence, there will be no Machines & Files in the beginning
		currMachineNumber = 0;
		currFileNumber = 0;
	}

	// This Function will be run in Main
	void driverCode()
	{
		cout << "\n--------------------------------------------\n Welcome to the Inter-Planetary File System\n--------------------------------------------\n";

		specifyBtreeOrder();
		specifyIdentifierSpace();
		addMachines();
		createTable();
		// User Interface
		int x = -1;

		while (true)
		{
			cout << "\n\n------\n Menu\n------\n\n1. Add Machines\n2. Search Machine\n3. Remove Machine\n4. Add File\n5. Search File\n6. Remove File\n7. Display a B-Tree\n8. Display a Routing Table\n9. Display the IPFS\n10. Exit\n\nChoice : ";
			cin >> x;

			if (x == 1)
			{
				//if (currMachineNumber == maxMachines)
					//cout << "\nYou have already added the Maximum number of Machines to the System.\n";
				//else

				//empty routing table of each machine
				emptyTable();

				addMachines();

				//create routing table after the machine is added
				createTable();


			}

			if (x == 2)
				searchMachine();

			if (x == 3) {

				//empty routing table of each
				emptyTable();

				removeMachine();
				cout << "REMOVED SUCCESSFULLY!" << endl;
				//create routing table after the machine is removed
				createTable();
				cout << "TABLELS CREATED" << endl;
			}
			if (x == 4)
				insertFile();

			if (x == 5)
				searchFile();

			if (x == 6)
				removeFile();

			if (x == 7)
				printBTree();

			if (x == 8)
				printRoutingTable();

			if (x == 9)
				printDHT();

			// Exit
			if (x == 10)
			{
				cout << "\n\n-------------------------------\nThank You for using the IPFS!\n-------------------------------\n\n";
				break;
			}

			// Invalid Input
			if (x < 1 || x > 10)
				cout << "\nInvalid Choice\n";
		}
	}

	~DHT()
	{
		// Deallocate Memory
		removeAllMachines();
	}
};