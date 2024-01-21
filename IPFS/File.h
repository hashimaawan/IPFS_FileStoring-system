#pragma once

#include "sha1.hpp"
#include <fstream>
#include "string"
#include "bigint.h"

using namespace std;

class File
{
private:
	string name;
	string key;
    string path;
    string itsContent;
    bigint machineID;


public:
    
    // ------------------------ // 
    // --- PUBLIC FUNCTIONS --- //
    // ------------------------ //
     
    File(string Name = "^", string Path = "-") : machineID(0)
    {
        name = Name;
        path = Path;
    }



    // --- GETTERS --- //

    string getName()
    {
        return name;
    }

    string getKey()
    {
        return key;
    }

    string getPath()
    {
        return path;
    }

    bigint getMachineID()
    {
        return machineID;
    }

    string getFileContent()
    {
        return itsContent;
    }



    // --- SETTERS --- //
    
    void setPath(string p)
    {
        path = p;
    }
    
    

    // --- HASHING -- //
    bigint hashFile(bigint maxMachines)
    {
        // --- Step 1 : Read File --- //
        ifstream inputFile(name);

        // Check if the file is open
        if (!inputFile.is_open())
        {
            cerr << "\nThe File does not exist!\n" << endl;
            return 0;
        }

        stringstream buffer;
        buffer << inputFile.rdbuf();
        string content = path;
        content += '\n';
        content += buffer.str();
        itsContent = buffer.str();



        // --- Step 2 : Hash File Content --- //
        SHA1 sha1;
        sha1.update(content);
        key = sha1.final();


        // --- Step 3 : Close File --- //
        inputFile.close();


        // --- Step 4 : Convert Hexadecimal to Integer
        bigint max(maxMachines.str);
        bigint val(max.hexToInt(key));

        // --- Step 5 : Find which Machine to store the File in --- //
        val = val % max;
        val++;

        machineID = val;
        
        return machineID;
    }
 


    // --- PRINTING --- //
    void displayFileInformation()
    {
        cout << "\nFile Name : " << getName();
        cout << "\nFile Key  : " << getKey();
        cout << "\nFile Path : " << getPath();
        cout << "\nMachine : " << getMachineID();
        cout << "\nContent : \n\n" << getFileContent();
        cout << endl << endl;
    }
};