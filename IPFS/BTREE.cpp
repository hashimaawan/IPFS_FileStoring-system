
//--------- Libraries
#include <iostream>
#include <string>
#include "File.h"


//-----
using namespace std;

//------ helper function to compare two strings... Here comparing key of file

bool isH1GreaterThantH2(string h1, string h2)
{
    // if h2 is greater in length then h2 is greater
    if (h2.length() > h1.length())
        return false;

    else if (h1.length() > h2.length())
        return true;

    for (int i = 0; i < h1.length() && i < h2.length(); i++)
    {
        if (h1[i] > h2[i])
            return true;
        else if (h2[i] > h1[i])
            return false;
    }
    // If the loop completes and lengths are different
    return (h1.length() > h2.length());
}

bool Isequal(string h1, string h2)
{
    if (h1.length() != h2.length())
        return false;
    else
    {
        for (int i = 0; i < h1.length() && i < h2.length(); i++)
        {
            if (h1[i] != h2[i])
                return false;
        }
    }
    return true;
}
////////////////////////////////////////////////////////////////////////
//------------------------- Btree node class ------------------------//
//////////////////////////////////////////////////////////////////////

class BTREE_Node
{
public:
    // Value Array
    File* btreeKey;
    // Dynamically making child arrays depending upon the OrderBtree
    BTREE_Node** child;

    // --  Will count how many values has been insertioned in btree right now, and will work accordingly
    int counter;

    //  -- Maximum and minimum no of keys
    int maximum_key;
    int minimum_key;

    // Constructor for B-tree node class initializing member variables and arrays
    BTREE_Node(int OrderBtree) : counter(0), minimum_key(OrderBtree / 2), maximum_key(OrderBtree - 1)
    {
        btreeKey = new File[maximum_key + 1];
        child = new BTREE_Node * [maximum_key + 2];

        // Setting child initially NULL
        for (int i = 0; i < maximum_key + 2; ++i)
        {
            child[i] = nullptr;
        }
    }
    //-- Destuctor
    ~BTREE_Node()
    {
        delete[] btreeKey;
        delete[] child;
    }
};

////////////////////////////////////////////////////
//--------------    BTREE CLASS  //////////////////
//////////////////////////////////////////////////

class BTree
{
private:
    int OrderBtree;

public:
    BTREE_Node* root;
    BTree(int OrderBtree) : root(nullptr), OrderBtree(OrderBtree) {}

    //// Function to insert a file object based on its key into the B-tree
    void insertion(File file)
    {
        // Value upon which splitting will happen
        File splitting_index;

        // Right node after splitting
        BTREE_Node* child_node;

        bool to_split = ValueSettingInNode(root, &child_node, file, &splitting_index);

        // This condition will run whenever splitting happens and in case of root == NULL (in start)
        if (to_split != 0)
        {
            BTREE_Node* n = new BTREE_Node(OrderBtree);

            n->btreeKey[1] = splitting_index; // value at which spliting will happen
            n->counter = 1;                  // it is root node after splitting so it's counter will be one
            n->child[0] = root;               // When split happens, left side at [0] will be old root
            n->child[1] = child_node;         // right side [1] child node it will be the node of values greater then that index value

            root = n;
        }
        return;
    }

    bool ValueSettingInNode(BTREE_Node* rootnode, BTREE_Node** child, File obj, File* pvalue)
    {
        int current_pos;

        // if right is empty or it is rootNode
        if (rootnode == nullptr)
        {

            *pvalue = obj;
            // Child will be Null which is right
            *child = nullptr;

            return 1;
        }
        else
        {
            bool checking_duplicate = searchingValueInNode(obj.getKey(), rootnode, &current_pos);

            if (checking_duplicate == 1) // Value exists already in BTREE
            {
                cout << "\nDuplicate, Key already!\n";
            }
            if (ValueSettingInNode(rootnode->child[current_pos], child, obj, pvalue) == 1)
            {
                // Splitting when counter has reached maximum counter                         // return 0 in case of not splitting
                return (rootnode->maximum_key > rootnode->counter) ? (placingInNode(*pvalue, *child, rootnode, current_pos), 0) : (Node_Split(*pvalue, rootnode, current_pos, pvalue, child, *child), 1);
            }
        }
        return 0;
    }

    bool searchingValueInNode(string key, BTREE_Node* node, int* curr_pos)
    {
        if (isH1GreaterThantH2(node->btreeKey[1].getKey(), key))
        {
            *curr_pos = 0;
            return 0;
        }
        else
        {
            // setting current position to the total number of values already been in node
            *curr_pos = node->counter;

            // This iteration will make sure the correct place of the value coming to be counter
            for (; *curr_pos > 1 && isH1GreaterThantH2(node->btreeKey[*curr_pos].getKey(), key); (*curr_pos)--)
            {
            }
            // if(Isequal(key,node->btreeKey[*curr_pos].key))
            // return 1;
            if (key == node->btreeKey[*curr_pos].getKey())
            {
                return 1;
            }
            else
                return 0;
        }
    }

    void placingInNode(File val, BTREE_Node* child, BTREE_Node* node, int curr_pos)
    {
        // This function helps in inserting a value and its associated child node
        // into the given node at a specified position.

        // Start from the end of the node's keys and children arrays
        int i = node->counter;

        // Shift elements to the right until reaching the position for the new value and child
        // This loop moves the existing keys and children to make space for the new one
        while (i > curr_pos)
        {
            node->btreeKey[i + 1] = node->btreeKey[i];
            node->child[i + 1] = node->child[i];
            i--;
        }

        // Place the new value and child in their appropriate positions
        node->btreeKey[curr_pos + 1] = val;
        node->child[curr_pos + 1] = child;

        // Increment the counter of keys/children in the node after insertion
        node->counter++;
    }

    void Node_Split(File obj, BTREE_Node* Node, int curr_pos, File* objy, BTREE_Node** NewNode, BTREE_Node* child)
    {
        // Function responsible for splitting a node 'Node' whenever it reaches max insertion,
        // and distributing the keys and children to maintain B-tree properties.

        int NewNode_index, mid;

        // Determining the midpoint position based on the current position and minimum keys
        mid = (curr_pos <= Node->minimum_key) ? Node->minimum_key : Node->minimum_key + 1;

        // Create a new node to hold the keys and children to be moved out of the current node
        *NewNode = new BTREE_Node(OrderBtree);

        // Moving keys and children from the 'Node'  to the newly created 'NewNode'
        NewNode_index = mid + 1;
        while (NewNode_index <= Node->maximum_key)
        {
            int j = NewNode_index - mid;

            (*NewNode)->btreeKey[j] = Node->btreeKey[NewNode_index];
            (*NewNode)->child[j] = Node->child[NewNode_index];

            NewNode_index++;
        }

        // Set the counter for the 'NewNode' based on the number of keys moved to it
        int newNode_counter = Node->maximum_key - mid;
        (*NewNode)->counter = newNode_counter;
        Node->counter = mid;

        // Determine where the new object should be placed after the split
        if (curr_pos <= Node->minimum_key)
        {
            // If the current position is within the minimum key range,
            // insert the new object into the original node 'Node'
            placingInNode(obj, child, Node, curr_pos);
        }
        else
        {
            // Calculate the position within the 'NewNode' to insert the new object
            int In_node2 = curr_pos - mid;

            // Insert the new object into the 'NewNode' at the calculated position
            placingInNode(obj, child, *NewNode, In_node2);
        }

        // Update the 'objy' value to the last key in node 'Node'
        *objy = Node->btreeKey[Node->counter];

        // Update the child pointer of 'NewNode' to link with the last child of node 'Node'
        (*NewNode)->child[0] = Node->child[Node->counter];

        // Decrement the counter in node 'n' as a key has been moved out
        Node->counter--;
    }

    // Function to delete a file object from the B-tree
    void deletion(File obj)
    {
        // Check if the file object to delete is found in the B-tree
        bool checkIfpresent = deletionhelper(root, obj);
        if (!checkIfpresent)
        {
            // If the file object is not found, display a message and return
            cout << "\nGiven key has not been found in the Btree\n";
            return;
        }
        else
        {
            // If the file object was found

            // Check if the root node has become empty after deletion
            if (root->counter == 0)
            {
                // If the root node has become empty

                // Create a temporary pointer to the current root
                BTREE_Node* temp = root;

                // Set the root to the child node (promoting the child as the new root)
                root = root->child[0];

                // Deleting the previous root node
                delete temp;

                // Set the temporary pointer to nullptr for safety
                temp = nullptr;
            }
        }
    }

    void copypred(BTREE_Node* node, int pos)
    {
        // Traverse to the rightmost child of the left subtree to find the predecessor
        BTREE_Node* temp = node->child[pos - 1];
        while (temp->child[temp->counter])
        {
            temp = temp->child[temp->counter];
        }

        // Copy the predecessor to the node position
        node->btreeKey[pos] = temp->btreeKey[temp->counter];
    }
    bool deletionhelper(BTREE_Node*& root, File obj)
    {
        // Check if the root is nullptr, indicating the end of the search path
        if (root == nullptr)
        {
            return false;
        }

        int pos;
        // Search for the value in the current node
        bool obj_present = searchingValueInNode(obj.getKey(), root, &pos);

        if (obj_present)
        {
            if (root->child[pos] != nullptr)
            {
                // If a child node exists, find the successor
                successorbtree(root, pos);
                obj_present = deletionhelper(root->child[pos], root->btreeKey[pos]);

                if (!obj_present)
                {
                    // If successor not found, try finding predecessor
                    copypred(root, pos);
                    obj_present = deletionhelper(root->child[pos], root->btreeKey[pos]);
                    if (!obj_present)
                    {
                        // If neither successor nor predecessor found, handle differently or perform other action
                        cout << "\nbtreeKey not found";
                    }
                }
            }
            else
            {
                // If no child node, perform deletion in the current node
                remove(root, pos);
            }
        }
        else
        {
            // Continue searching in the child node
            obj_present = deletionhelper(root->child[pos], obj);
        }

        // Check for B-tree properties restoration after deletion
        if (root->child[pos] != nullptr && root->child[pos]->counter < root->minimum_key)
        {
            Making_Correct(root, pos);
        }

        //  Empty root node after deletion
        if (pos == 0 && !obj_present && root->counter == 0)
        {
            BTREE_Node* temp = root;
            root = root->child[0];
            delete temp;
            temp = nullptr;
        }

        return obj_present;
    }

    // Function to clear the node after deletion at a given index
    void remove(BTREE_Node* node, int position)
    {
        // Initializing the iteration
        int i = position + 1;

        // While loop to remove the element at index 'k' in the node
        while (i <= node->counter)
        {
            // Shift the elements and child pointers in the node
            node->btreeKey[i - 1] = node->btreeKey[i];
            node->child[i - 1] = node->child[i];
            i++;
        }

        // Reduce the counter of elements in the node
        node->counter--;
    }

    // Function to copy the successor value for deletion
    void successorbtree(BTREE_Node* node, int i)
    {
        // Traverse to the successor value
        BTREE_Node* temp = node->child[i];
        while (temp->child[0])
        {
            temp = temp->child[0];
        }

        // Replace the current value with the successor value
        node->btreeKey[i] = temp->btreeKey[1];
    }

    void Making_Correct(BTREE_Node* node, int position)
    {

        if (position == 0)
        {
            // Checking if the right child has more than the minimum number of keys
            bool rightchild = node->child[1]->counter > node->minimum_key;

            if (rightchild)
            {
                Shiftingleft_operation(node, 1); // Perform left shift with the next child
            }
            else
            {
                Merging(node, 1); // Merging current node with the next child
            }
        }
        // If index 'position' is the last element in the node
        else if (position == node->counter)
        {
            // Check if the left child has more than the minimum number of keys
            if (node->minimum_key > node->child[position - 1]->counter)
            {
                shiftingRight(position, node); // Performing right shift with the previous child
            }
            else
            {
                Merging(node, position); // Merging current node with the previous child
            }
        }
        // If position is between the first and last element in the node
        else
        {
            // Check if the left child has more than the minimum number of keys
            if (node->minimum_key < node->child[position - 1]->counter)
            {
                shiftingRight(position, node); // Perform right shift with the previous child
            }
            // Check if the right child has more than the minimum number of keys
            else if (node->child[position + 1]->counter > node->minimum_key)
            {
                Shiftingleft_operation(node, position + 1); // Perform left shift with the next child
            }
            else
            {
                Merging(node, position); // Merge current node with adjacent children
            }
        }
    }

    // Right shift operation in the B-tree node
    void shiftingRight(int position, BTREE_Node* node)
    {
        // Get the target node and its sibling
        int target_pos = position - 1;
        BTREE_Node* sibling = node->child[target_pos];
        BTREE_Node* target_node = node->child[position];

        // Increment the counter of the target node
        target_node->counter++;

        // Updating keys and children references based on the sibling node
        int keyfirst = 1;
        int childfirst = 0;

        target_node->btreeKey[keyfirst] = node->btreeKey[position];

        target_node->child[childfirst] = sibling->child[sibling->counter];

        node->btreeKey[position] = sibling->btreeKey[sibling->counter];

        sibling->counter--;

        // Shift keys and child references in the target node
        int i = target_node->counter - 1;
        while (i > 1)
        {
            target_node->btreeKey[i] = target_node->btreeKey[i - 1];
            target_node->child[i] = target_node->child[i - 1];
            i--;
        }

        // Update specific keys and child references in the target node
        target_node->child[1] = target_node->child[0];

        target_node->btreeKey[2] = node->btreeKey[position + 1];
        target_node->child[0] = sibling->child[sibling->counter];
        sibling->child[sibling->counter] = nullptr;
    }

    // Left shift operation in the B-tree node
    void Shiftingleft_operation(BTREE_Node* node, int position)
    {
        // Get the target node and its sibling
        int target_pos = position - 1;
        BTREE_Node* target_node = node->child[position];
        BTREE_Node* sibling = node->child[target_pos];

        // Increment the counter of the target node
        target_node->counter++;
        // Shift child references in the target node
        target_node->child[target_node->counter] = target_node->child[target_node->counter - 1];

        // Shift keys and child references in the target node
        int i = target_node->counter - 1;
        while (i > 1)
        {
            target_node->btreeKey[i] = target_node->btreeKey[i - 1];
            target_node->child[i] = target_node->child[i - 1];
            i--;
        }

        // Update keys and child references based on the sibling node
        int firstpos = 0;
        int keyfirst = 1;

        target_node->btreeKey[keyfirst] = node->btreeKey[position];
        target_node->child[firstpos] = sibling->child[sibling->counter];
        node->btreeKey[position] = sibling->btreeKey[sibling->counter];
        sibling->counter--;
    }

    // Merging operation in the B-tree node
    void Merging(BTREE_Node* node, int position)
    {
        int target_pos = position - 1;
        // Get the target node and its sibling
        BTREE_Node* target_node = node->child[position];
        BTREE_Node* sibling = node->child[target_pos];

        // Increment the counter of the sibling node
        sibling->counter++;

        // Move the key and child from the current node to the sibling node
        int firstpos = 0;
        sibling->btreeKey[sibling->counter] = node->btreeKey[position];
        sibling->child[sibling->counter] = target_node->child[firstpos];

        // Move keys and child references from the target node to the sibling node
        int i = 1;
        while (i <= target_node->counter)
        {
            sibling->counter++;

            sibling->btreeKey[sibling->counter] = target_node->btreeKey[i];
            sibling->child[sibling->counter] = target_node->child[i];
            i++;
        }

        // Remove the key and child reference from the current node
        i = position;
        while (i < node->counter)
        {
            node->btreeKey[i] = node->btreeKey[i + 1];
            node->child[i] = node->child[i + 1];
            i++;
        }

        // Decrement the counter of the current node and perform cleanup
        node->counter--;
        delete target_node;
        target_node = NULL;
    }

    void display(BTREE_Node* root)
    {
        if (root != NULL)
        {
            for (int i = 0; i < root->counter; i++)
            {
                display(root->child[i]);
                cout << root->btreeKey[i + 1].getName() << " ";
            }
            display(root->child[root->counter]);
        }
    }

    void displayTree()
    {
        cout << "B-tree:\n";
        display(root);
        cout << endl;
    }

    //-------------------------  Searching Functions

    // 1--) This function will return whether file present or not
    bool search(string key)
    {
        return searchFileHelper(key, root);
    }

    bool searchFileHelper(string key, BTREE_Node* root)
    {
        if (root == nullptr)
        {
            return false;
        }

        int pos;
        if (searchingValueInNode(key, root, &pos))
        {
            return true;
        }
        else
        {
            return searchFileHelper(key, root->child[pos]);
        }
    }

    File search(File myfile)
    {
        File foundFile;
        if (searchFileHelper(myfile.getKey(), root))
        {
            foundFile = searchFileWithKey(myfile.getKey(), root);
        }
        return foundFile;
    }

    File searchFileWithKey(string key, BTREE_Node* root)
    {
        if (root == nullptr)
        {
            File file; // Return an empty file if not found
            return file;
        }

        int pos;
        if (searchingValueInNode(key, root, &pos))
        {
            if (root->btreeKey[pos].getKey() == key)
            {
                return root->btreeKey[pos];
            }
        }
        else
        {
            return searchFileWithKey(key, root->child[pos]);
        }

        File file; // Return an empty file if not found
        return file;
    }

    File FileSearchWithKey(string key)
    {
        File obj = searchFileWithKey(key, root);
        return obj;

    }
    // Function to read file content and display it
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

    void deleteAll(BTREE_Node* node)
    {
        if (node == nullptr)
        {
            return;
        }

        // Delete all children first
        for (int i = 0; i <= node->counter; ++i)
        {
            deleteAll(node->child[i]);
        }

        // Delete current node
        delete node;
        node = nullptr;
    }

    // Function to delete all objects stored in the B-tree
    void deleteAllObjects()
    {
        deleteAll(root);
        root = nullptr; // Reset the root after deleting all nodes
    }

    //----- Machine adding and removing, Btree functions


    void copyFiles(BTREE_Node* node, BTree& newBTree, bigint machineID)
    {
        if (node != nullptr)
        {
            for (int i = 0; i <= node->counter; ++i)
            {
                if (node->child[i] != nullptr)
                {
                    copyFiles(node->child[i], newBTree, machineID); // Recursively copy child nodes
                }

                if (i < node->counter && node->btreeKey[i + 1].getMachineID() == machineID)
                {
                    newBTree.insertion(node->btreeKey[i + 1]); // Insert into the new tree
                    // deletion(node->btreeKey[i + 1]);           // Remove the file from the old B-tree
                }
            }
        }
    }

    void copyingTree(BTREE_Node* node, BTree& newTree)
    {

        if (node != NULL)
        {
            for (int i = 0; i < node->counter; ++i)
            {
                copyingTree(node->child[i], newTree);     // Recursively copy child nodes
                newTree.insertion(node->btreeKey[i + 1]); // Insert into the new tree
            }
            copyingTree(node->child[node->counter], newTree); // Copy the last child
        }
    }

    BTREE_Node* removeAllAndCopy()
    {
        BTree newTree(OrderBtree); // Create a new B-tree object

        // Copy existing B-tree to the new one
        copyingTree(root, newTree);

        // Setting the root of the new BTree to the copied root
        BTREE_Node* newRoot;
        // newTree.root = newRoot;
        cout << "inside:\n";
        newTree.displayTree();
        newRoot = newTree.root;
        // Delete all files from the old B-tree
        deleteAllObjects();

        return newRoot; // Return the root of the new B-tree
    }

    BTree Copyingactualfiles(bigint machineID)
    {
        BTree newTree(OrderBtree);           // Create a new B-tree object
        copyFiles(root, newTree, machineID); // Start copying files with the specific machineID

        return newTree; // Return the root of the new B-tree
    }

    File deletionKey(string Key)
    {
        // Searching file containing that key
        File obj = searchFileWithKey(Key, root);
        // deleting that file
      //  File obj = root->searchingFile(Key);
        File temp = obj;

        // deleting that file
        this->deletion(obj);

        return temp;
     //   this->deletion(obj);

    } 


};


