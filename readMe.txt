/*
    Inter-Planetary File System in C++

    MIT License

    Created by Ali Asif, Arshaq Kirmani & Hashim Awan on 15-12-2023
    Copyright (c) 2023

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/



Welcome to the IPFS Ring DHT Implementation project! This project emulates a Distributed Hash Table (DHT) with a circular identifier space, specifically designed for the InterPlanetary File System (IPFS). The system allows efficient storage, retrieval, and management of files in a decentralized network.



CONTENTS:

-> Contains the source code files.
-> Stores the compiled executable files.



REQUIREMENTS:

-> C++ compiler (supporting C++)
-> Open a terminal and navigate to the project directory.



NOTE:

-> Move your .txt files to the directory with the .cpp, .h & .hpp files.



The project supports the following commands and options:

-> Specify the number of machines in the system.
-> Specify the size of the identifier space in bits (e.g., 160 bits).
-> Enable manual assignment of IDs to machines.
-> Insert files in the form of key-value pairs from any machine. Display the path taken for insertion and print the B-tree.
-> Remove data by specifying the key from any machine. Display the path taken for removal and print the updated B-tree.
-> Print the routing table of a specified machine.
-> Add a new machine to the system without disrupting the functionality of the Ring DHT.
-> Remove a machine from the system without disrupting the functionality of the Ring DHT.