#include <iostream>
#include <string>
#include "File.h"
using namespace std;

//  ----- Comparison Function to compare two strings
bool isH1GreaterThantH2(string h1, string h2)
{
    // string 2 is greater
    if (h2.length() > h1.length())
        return false;
    // string 1 is greater
    else if (h1.length() > h2.length())
        return true;

    // else if both are equal in lengths, then iterating to check for greater string
    for (int i = 0; i < h1.length() && i < h2.length(); i++)
    {
        if (h1[i] > h2[i])
        {
            return true;
        }
        else if (h2[i] > h1[i])
        {
            return false;
        }
    }
    // If the loop completes and lengths are different
    return (h1.length() > h2.length());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------    BTREE NODE CLASS (Responsible for searching,insertion and deletion within the node) -----//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
class BTREE_Node
{

public:
    File* btreeKey;
    int degree_node;
    BTREE_Node** child; // A child pointer's Array
    int counter;
    int check_node;
    bool IsChildLeaf;

    // Constructor for initializing a B-tree node with given parameters
    BTREE_Node(int degree, bool settingLeaf, int node)
    {
        if (node > 4)
        {
            if (node % 2 == 0)
            {
                check_node = node;
                counter = 0; // Initializing position counter

                IsChildLeaf = settingLeaf;
                degree_node = degree;
                //  degree_node--;
                // Initializing variables responsible for allocating memory
                int totalkeys = degree_node * 2 + 1;
                //  totalkeys--;

                int totalChild = degree_node * 2 + 2;

                // Allocating memory for keys and child pointers based on the degree
                btreeKey = new File[totalkeys];
                child = new BTREE_Node * [totalChild];

                for (int i = 0; i < totalChild; i++)
                {
                    child[i] = NULL;
                }
            }
            else if (node % 2 != 0)
            {
                check_node = node;
                counter = 0; // Initializing position counter

                IsChildLeaf = settingLeaf;
                degree_node = degree;
                //  degree_node--;
                // Initializing variables responsible for allocating memory
                int totalkeys = degree_node * 2;
                //  totalkeys--;

                int totalChild = degree_node * 2 + 1;

                // Allocating memory for keys and child pointers based on the degree
                btreeKey = new File[totalkeys];
                child = new BTREE_Node * [totalChild];

                for (int i = 0; i < totalChild; i++)
                {
                    child[i] = NULL;
                }
            }
        }
        else
        {

            check_node = node;
            counter = 0; // Initializing position counter

            IsChildLeaf = settingLeaf;
            degree_node = degree;
            //  degree_node--;
            // Initializing variables responsible for allocating memory
            int totalkeys = degree_node * 2;
            totalkeys--;

            int totalChild = degree_node * 2;

            // Allocating memory for keys and child pointers based on the degree
            btreeKey = new File[totalkeys];
            child = new BTREE_Node * [totalChild];

            for (int i = 0; i < totalChild; i++)
            {
                child[i] = NULL;
            }
        }
    }

    ~BTREE_Node()
    {
        delete[] btreeKey;
        delete[] child;
    }

    /////////////////////////////////////////////////////////
    /////----------------    INSERTION   -------////////////
    ///////////////////////////////////////////////////////

    // Function to insert a key into a non-full node
    void insertAtNode(File obj)
    {
        int position = counter - 1;

        if (IsChildLeaf == true)
        {
            // If the node is a leaf, find the correct position for the new key
            for (; position >= 0 && (isH1GreaterThantH2(btreeKey[position].getKey(), obj.getKey())); --position)
            {
                // Shift keys to make space for the new key
                btreeKey[position + 1] = btreeKey[position];
            }

            // Inserting the new key at the appropriate location and incrementing the count
            int app_loc = position + 1;
            btreeKey[app_loc] = obj;
            counter++;
            ;
        }
        else
        {
            // If the node is not a leaf, traversing to the appropriate child node to insert the file obj
            for (; position >= 0 && (isH1GreaterThantH2(btreeKey[position].getKey(), obj.getKey())); --position)
            {
            }

            // Check if the child is full and perform a split operation if needed
            int Isfull = degree_node * 2 - 1;
            if ((child[position + 1]->counter) == Isfull)
            {
                separatingnode(child[position + 1], position + 1); // Split the full child node

                // Adjust the index if necessary after splitting
                if ((isH1GreaterThantH2(obj.getKey(), btreeKey[position + 1].getKey())))
                {
                    position++;
                }
            }
            child[position + 1]->insertAtNode(obj); // Recursively insert the key into the appropriate child node
        }
    }

    // Function to separate a  node when it is full
    void separatingnode(BTREE_Node* fullNode, int position)
    {

        BTREE_Node* newNode = new BTREE_Node(fullNode->degree_node, fullNode->IsChildLeaf, this->check_node); // new node  to store keys from the full node
        newNode->counter = degree_node - 1;                                                 // Seting the counter for newNode node (median right)

        // Copying the second half of keys from full node to newNode
        for (int j = 0; j < degree_node - 1; j++)
        {
            int pos = j + degree_node;
            newNode->btreeKey[j] = fullNode->btreeKey[pos];
        }

        // If fullNode is an internal node (not a leaf), copy corresponding child pointers to newNode
        if (fullNode->IsChildLeaf == 0)
        {
            for (int j = 0; j < degree_node; j++)
            {
                int pos = j + degree_node;
                newNode->child[j] = fullNode->child[pos];
            }
        }

        fullNode->counter = degree_node - 1; // Update counter for fullnode

        // Rearrange child pointers of fullnode to make space for the newnode
        int j = counter;
        while (j >= position + 1)
        {
            child[j + 1] = child[j];
            j--;
        }

        child[position + 1] = newNode; // Insert the newNode into FullNode at the specified position

        // Shift keys in 'btreeKey' to make space for the key being moved from 'y' to its parent
        for (int j = counter - 1; j >= position; j--)
            btreeKey[j + 1] = btreeKey[j];

        // Move a key from fullNode i.e median key to its parent node's key array at the appropriate position
        int mediankey = degree_node - 1;
        btreeKey[position] = fullNode->btreeKey[mediankey];

        counter++; // Increment the counter of keys in the node
    }

    ///////////////////////////////////////////////////////
    //.----------------------   Deletion
    //////////////////////////////////////////////////////

    // A function to remove the key k

    void Delete_Object_FromNode(File obj)
    {
        int findingpos;

        // Find the position of the key to delete or the position for insertion
        for (findingpos = 0; findingpos < counter && (isH1GreaterThantH2(obj.getKey(), btreeKey[findingpos].getKey())); ++findingpos)
        {
            // Finding the appropriate position
        }

        // If the key to delete is found in the node
        if (findingpos < counter && btreeKey[findingpos].getKey() == obj.getKey())
        {
            // If the node is a leaf node, delete the key directly from the node
            if (IsChildLeaf == 1)
            {
                int left = findingpos + 1;
                while (left < counter)
                {
                    btreeKey[left - 1] = btreeKey[left];
                    left++;
                }
                counter--;
            }
            else
            {
                // If the node is not a leaf, delete the key using a different method
                Deletion_from_non_leafNode(findingpos);
            }
        }
        // If the key is not found in the current node, traverse to the appropriate child node
        else
        {
            // If the node is a leaf and the key isn't found, print a message and return
            if (IsChildLeaf == 1)
            {
                // cout << "The key " << obj.getKey() << "is not present in the tree to delete the file\n";
                return;
            }

            bool founded = 0;
            if (findingpos == counter)
                founded = 1;

            // If the child node where the key is expected to be deleted has less than 'degree_node' keys add keys to it from Right_siblingor other cases...
            if (child[findingpos]->counter < degree_node)
            {
                // Minimum Node Case
                settingNode(findingpos);
            }

            // Recursively delete the key from the appropriate child node
            int valid_pos = findingpos - 1;

            if (findingpos > counter && founded)
                child[valid_pos]->Delete_Object_FromNode(obj);
            else
                child[findingpos]->Delete_Object_FromNode(obj);
        }

        return;
    }

    void Deletion_from_non_leafNode(int findingpos)
    {

        string key = btreeKey[findingpos].getKey();

        // Finding successor and replacing and then deleting successor, but only if child[(pred position)] meets the minimum criteria
        bool Is_minimumcriteria = child[findingpos + 1]->counter >= degree_node;
        if (Is_minimumcriteria)
        {
            // this getsucc function returns the successor at findingpos
            File succ = FindingSucce(findingpos);
            btreeKey[findingpos] = succ;
            child[findingpos + 1]->Delete_Object_FromNode(succ);
        }
        // Else predecessor case
        else if (child[findingpos]->counter >= degree_node)

        {
            File predecessorr = FindingPred(findingpos);

            btreeKey[findingpos] = predecessorr;
            child[findingpos]->Delete_Object_FromNode(predecessorr);
        }

        // Third case, when nor left or right Right_siblinghas enough keys, merging both
        // Removing file from child[findingposition] by deleting child[findingposition+1]
        else
        {
            MergingNodes(findingpos);
            child[findingpos]->Delete_Object_FromNode(key);
        }
        return;
    }

    // A function to get predecessor of key[findingpos]
    File FindingPred(int findingpos)
    {
        // Move to the rightmost node until we get to a IsChildLeaf.
        BTREE_Node* temp = child[findingpos];
        while (!temp->IsChildLeaf)
        {
            temp = temp->child[temp->counter];
        }

        return temp->btreeKey[temp->counter - 1];
    }

    File FindingSucce(int findingpos)
    {
        int suuccPos = findingpos + 1;
        BTREE_Node* temp = child[suuccPos];
        while (!temp->IsChildLeaf)
        {
            temp = temp->child[0];
        }

        // Return the first key of the IsChildLeaf
        return temp->btreeKey[0];
    }

    // Function to set the node after deletion
    void settingNode(int findingpos)
    {

        // bool IS_minimum_criteria =
        //  If there is a previous node with enough keys to borrow from
        if (findingpos != 0 && child[findingpos - 1]->counter >= degree_node)
        {
            LeftBorrowSibling(findingpos); // Borrow a key from the previous node
        }

        // If there is a next node with enough keys to borrow from
        else if (findingpos != counter && child[findingpos + 1]->counter >= degree_node)
            RightBorrowSibling(findingpos); // Borrow a key from the next node

        else
        {
            // IF both cases doesn't work then merging that node with either right or left node
            if (findingpos != counter)
            {
                MergingNodes(findingpos);
            }
            else
                // If the current node is the last child node, merge with the previous node
                MergingNodes(findingpos - 1);
        }

        return;
    }

    // Function merges right sibling and node to replace or current node
    void MergingNodes(int findingpos)
    {
        // Assigning references to the nodes being merged
        BTREE_Node* Node_to_rep = child[findingpos];
        BTREE_Node* Right_sibling = child[findingpos + 1];

        // Moving the key from the current node to the merging node
        Node_to_rep->btreeKey[degree_node - 1] = btreeKey[findingpos];

        // Copying keys from the right sibling to the merging node
        int k = 0;
        while (k < Right_sibling->counter)
        {
            Node_to_rep->btreeKey[k + degree_node] = Right_sibling->btreeKey[k];
            k++;
        }

        // Copying child pointers from the right sibling to the merging node if it's not a leaf node
        if (!Node_to_rep->IsChildLeaf)
        {
            k = 0;
            while (k <= Right_sibling->counter)
            {
                Node_to_rep->child[k + degree_node] = Right_sibling->child[k];
                k++;
            }
        }

        // Shifting keys in the parent node to remove the merged key
        int i = findingpos + 1;
        while (i < counter)
        {
            btreeKey[i - 1] = btreeKey[i];
            i++;
        }

        // Shifting child pointers in the parent node to remove the merged child pointer
        int l = findingpos + 2;
        while (l <= counter)
        {
            child[l - 1] = child[l];
            l++;
        }

        // Updating counters for the merging node and the parent node
        Node_to_rep->counter += Right_sibling->counter + 1; // +1 for the key moved from parent
        counter--;

        // Deleting the right sibling node as it has been merged
        delete Right_sibling;
    }


    void LeftBorrowSibling(int findingpos)
    {
        int sibling_pos = findingpos - 1;
        BTREE_Node* Node_to_rep = child[findingpos];
        BTREE_Node* Left_sibling = child[sibling_pos];

        int first_poss = 0;
        // Shifting keys to make space for insertion
        int i = Node_to_rep->counter - 1;
        while (i >= 0)
        {
            Node_to_rep->btreeKey[i + 1] = Node_to_rep->btreeKey[i];
            i--;
        }

        // If Node_to_rep is not a leaf, shifting child pointers as well
        if (!(Node_to_rep->IsChildLeaf == 1))
        {
            i = Node_to_rep->counter;
            while (i >= 0)
            {
                Node_to_rep->child[i + 1] = Node_to_rep->child[i];
                i--;
            }
        }
        // inserting at first position file from sibling after we have make the space

        Node_to_rep->btreeKey[first_poss] = btreeKey[findingpos - 1];

        // Adjusting its child as well
        if (!(Node_to_rep->IsChildLeaf == 1))
        {
            Node_to_rep->child[first_poss] = Left_sibling->child[Left_sibling->counter];
        }

        int replaced_pos = Left_sibling->counter - 1;
        // keys will be one less after replacing it in parent
        btreeKey[findingpos - 1] = Left_sibling->btreeKey[replaced_pos];

        Node_to_rep->counter = Node_to_rep->counter + 1;
        Left_sibling->counter -= 1;

        return;
    }
    // A function that takes a key from C[findingpos+1] and stores it in C[findingpos].

    // Function to borrow a key from the right sibling
    void RightBorrowSibling(int findingpos)
    {

        // Getting references to the child node and its sibling
        BTREE_Node* Node_to_rep = child[findingpos];
        BTREE_Node* Right_sibling = child[findingpos + 1];

        // Borrowing the key from the Right_siblingto the Node_to_rep node
        Node_to_rep->btreeKey[(Node_to_rep->counter)] = btreeKey[findingpos];

        // Check if Node_to_rep node is an internal node (not a leaf)
        if (!(Node_to_rep->IsChildLeaf == 1))
        {
            Node_to_rep->child[(Node_to_rep->counter) + 1] = Right_sibling->child[0];
        }

        btreeKey[findingpos] = Right_sibling->btreeKey[0];

        // Shifting keys in the Right_siblingnode
        int k = 1;
        while (k < Right_sibling->counter)
        {
            Right_sibling->btreeKey[k - 1] = Right_sibling->btreeKey[k];
            k++;
        }

        // Shifting child pointers if the nodes are not leaf nodes
        if (!(Right_sibling->IsChildLeaf == 1))
        {
            k = 1;
            while (k <= Right_sibling->counter)
            {
                Right_sibling->child[k - 1] = Right_sibling->child[k];
                k++;
            }
        }

        Node_to_rep->counter++;   // File replaced to the node's counter will be incremented
        Right_sibling->counter--; // Borrow node --

        return;
    }

    void removeall()
    {
        if (!IsChildLeaf)
        {
            for (int i = 0; i < counter + 1; ++i)
            {
                if (child[i] != nullptr)
                {
                    child[i]->removeall();
                    delete child[i];
                    child[i] = NULL;
                }
            }
        }
        delete[] btreeKey;
        delete[] child;
        btreeKey = NULL;
        child = NULL;
    }
    ////////////////////////////////////////////////////////
    //---------------   PRINTING BTREE ////////////////////
    //////////////////////////////////////////////////////

    // A Function to traverse all nodes and Print the B-tree
    void printBtree()
    {
        int position;
        // Iterate through all keys in the current node
        for (position = 0; position < counter; position++)
        {
            // If the node is not a leaf node (it's an internal node)
            if (IsChildLeaf == 0)
            {
                // Recursively call printBtree for the child node
                child[position]->printBtree();
            }

            // Print the name of the file
            cout << btreeKey[position].getName() << "\n";
        }
        // Print keys of the last child node if it's not a leaf node
        if (IsChildLeaf == 0)
        {
            child[position]->printBtree();
        }
    }

    /////////////////////////////////////////////////////////////
    //--------------------  SEARCHING FUNCTIONS    -----------//
    ///////////////////////////////////////////////////////////

    // A function to search key k in btree
    // Function to search for a key  in the B-tree
    BTREE_Node* searchingKey(string key)
    {
        int position = 0;

        // Find the appropriate position to search for the key
        while (position < counter && (isH1GreaterThantH2(key, btreeKey[position].getKey())))
            position++;

        // Return this node if the detected key is equal to 'k'
        if (btreeKey[position].getKey() == key)
            return this;

        // If the key isn't found here and the node is a leaf (IsChildLeaf),
        if (IsChildLeaf == true)
            return NULL;

        // Go to the appropriate child node and continue the search recursively
        return child[position]->searchingKey(key);
    }

    // New functions to be added:
    File search(File myFile)
    {
        BTREE_Node* result = searchingKey(myFile.getKey());
        if (result != nullptr)
        {
            for (int i = 0; i < result->counter; ++i)
            {
                if (result->btreeKey[i].getKey() == myFile.getKey())
                {
                    return result->btreeKey[i];
                }
            }
        }
        return File(); // Return an empty File if not found
    }

    File searchingFile(string myFile)
    {
        BTREE_Node* result = searchingKey(myFile);
        if (result != nullptr)
        {
            for (int i = 0; i < result->counter; ++i)
            {
                if (result->btreeKey[i].getKey() == myFile)
                {
                    return result->btreeKey[i];
                }
            }
        }
        return File(); // Return an empty File if not found
    }

    bool searchkey(string key)
    {
        return (searchingKey(key) != NULL);
    }
};

////////////////////////////////////////////////////////////
////---------------     BTREE CLASS--------------//////////
//////////////////////////////////////////////////////////

class BTree
{

public:
    BTREE_Node* root;
    int degree;
    int previous_degree;
    // btree class Constructor
    BTree(int degree_node)
    {
        if (degree_node > 4)
        {
            previous_degree = degree_node;
            //     degree_node--;
            root = NULL;
            int deg;
            degree_node--;
            deg = degree_node / 2;

            degree = deg; // Setting the node's degree
        }

        else
        {
            previous_degree = degree_node;
            root = NULL;
            int deg;
            deg = degree_node / 2;
            deg++;
            degree = deg;
        }
    }



    // Function to insert a key into the B-tree
    void insertion(File obj)
    {

        if (root == NULL)
        {
            // If the tree is empty, create a new root node and insert the key
            root = new BTREE_Node(degree, true, previous_degree);
            root->btreeKey[0] = obj;

            // newNode 1 key has been inserted, so counter set =1
            root->counter = 1;
        }
        else if (root != NULL)
        {
            //
            int calculating_full = 2 * degree - 1;
            if (root->counter == calculating_full)
            {
                int position = 0;
                // If the node is full, then creating a new node and splitting
                // calling new node by setting its position with 0 and defining its degree
                BTREE_Node* newparent = new BTREE_Node(degree, 0, previous_degree);

                // Left will be old root
                newparent->child[position] = root;

                newparent->separatingnode(root, 0); // Split the full root node and adjust 's'

                // Determine the child where the key needs to be inserted
                if (isH1GreaterThantH2(obj.getKey(), newparent->btreeKey[position].getKey()))
                {
                    position++;
                }

                // Insert the key into the appropriate child node of 's'
                newparent->child[position]->insertAtNode(obj);
                root = newparent; // updating root
            }
            else
            {
                // If the root node is not full, insert the key into the root directly
                root->insertAtNode(obj);
            }
        }
    }

    void deletion(File obj)
    {
        if (root == NULL)
        {
            cout << "\nTREE is Empty\n";
            return;
        }

        // A function call to remove that particular object file from the node
        root->Delete_Object_FromNode(obj);

        if (root->counter == 0)
        {
            BTREE_Node* tmp = root;

            // check if root child is leaf
            if (root->IsChildLeaf)
            {
                root = NULL;
            }
            else
            {
                root = root->child[0];
            }
            delete tmp;
        }
        return;
    }

    BTREE_Node deleteFile()
    {
        BTREE_Node temp = *root;
        deleteAllObjects();
        return temp;

    }


    File deletionKey(string Key)
    {
        // Searching file containing that key
        File obj = root->searchingFile(Key);
        File temp = obj;

        // deleting that file
        this->deletion(obj);

        return temp;
    }

    // Function to delete all files from Btree
    void deleteAllObjects()
    {
        if (root != NULL)
        {
            root->removeall();
            delete root;
            root = NULL;
        }
    }

    ///////////////////////////////////////////////////
    ///----------- Printing BTREE  ---------------////
    /////////////////////////////////////////////////
    void displayTree()
    {
        if (root != NULL)
        {
            cout << "  \n ";
            cout << "-------\n";
            cout << "Files \n";
            cout << "-------\n";
            root->printBtree(); // Root calling the print function in Btree Node class
        }
        else
        {
            cout << "\nBTREE is Empty\n";
        }
    }

    /////////////////////////////////////////////////////////////
    ////------------  Searching Functions  --------/////////////
    ///////////////////////////////////////////////////////////
    // A function to search a key in this tree

    BTREE_Node* searchNode(string k)
    {
        if (root == NULL)
            return NULL;

        else
            root->search(k);
    }

    File search(File myFile)
    {
        if (this->root == NULL)
        {

            return File(); // Return an empty File if the tree is empty
        }
        else
            return root->search(myFile);
    }

    bool search(string key)
    {
        if (root == NULL)
            return false;
        else
            return root->searchingKey(key);
    }

    void displayFileContents(const string& fileName)
    {
        ifstream inFile(fileName);
        if (inFile.is_open())
        {
            string fileline;
            cout << "File Name: " << fileName << endl;
            cout << "File Content:\n";
            while (getline(inFile, fileline))
            {
                cout << fileline << endl;
            }
            inFile.close();
        }
        else
        {
            cout << "Error! File not opening " << fileName << endl;
        }
    }

    File FileSearchWithKey(string key)
    {
        if (root != nullptr) {

            return root->searchingFile(key);
        }
        else {

            return File(); // Return an empty File if tree is empty
        }
    }

    BTREE_Node* copyingTree(BTREE_Node* node, BTree& newTree)
    {
        if (node != nullptr)
        {
            for (int i = 0; i < node->counter; ++i)
            {
                newTree.insertion(node->btreeKey[i]); // Insert into the new tree
                if (!node->IsChildLeaf)
                {
                    copyingTree(node->child[i], newTree); // Recursively copy child nodes
                }
            }
            if (!node->IsChildLeaf)
            {
                copyingTree(node->child[node->counter], newTree); // Copy the last child
            }
        }
        return newTree.root;
    }

    BTREE_Node* removeAllAndCopy()
    {
        BTree newTree(previous_degree); // Creating a new B-tree object

        // Copying existing B-tree to the new one
        BTREE_Node* newRoot = copyingTree(root, newTree);

        // Delete all files from the old B-tree
        deleteAllObjects();

        // Returning the root of the new B-tree
        return newRoot;
    }

    void copyFiles(BTREE_Node* node, BTree& newBTree, bigint machineID)
    {
        if (node != NULL)
        {
            for (int i = 0; i < node->counter; ++i)
            {
                if (node->btreeKey[i].getMachineID() <= machineID)
                {
                    newBTree.insertion(node->btreeKey[i]);


                    node->Delete_Object_FromNode(node->btreeKey[i]);
                    --i; // adjusting counter after deletion
                }
            }
            if (!node->IsChildLeaf)
            {
                for (int i = 0; i <= node->counter; ++i)
                {
                    copyFiles(node->child[i], newBTree, machineID);
                }
            }
        }
    }

    BTree Copyingactualfiles(bigint machineID)
    {
        BTree newTree(previous_degree);
        copyFiles(root, newTree, machineID); // Copying files with specific machineID
        return newTree;
    }
};

