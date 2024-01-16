#include "../include/b_plus_tree.h"
#include <cmath>
#include <iostream>

/*
 * Helper function to decide whether current b+tree is empty
 * Returns false if not empty
 * Return true if empty
 */
bool BPlusTree::IsEmpty() const
{
    return !(root != nullptr && root->key_num > 0);
}

/*****************************************************************************
 * SEARCH
 *****************************************************************************/
/*
 * Return the only value that associated with input keyTp
 * This method is used for point query
 * @return : true means keyTp exists
 */
bool BPlusTree::GetValue(const KeyType &keyTp, RecordPointer &result)
{
    if (IsEmpty())
    {
        return false;
    }

    Node *currentNode = root;


    while (!currentNode->is_leaf)
    {
        for (int currIndex = 0; currIndex < currentNode->key_num; currIndex++)
        {
            if (keyTp < currentNode->keys[currIndex])
            {
                currentNode = ((InternalNode *)currentNode)->children[currIndex];
                break;
            }
            if (currIndex == currentNode->key_num - 1)
            {
                currentNode = ((InternalNode *)currentNode)->children[currIndex + 1];
                break;
            }
        }
    }
    for (int i = 0; i < currentNode->key_num; i++)
    {
        if (currentNode->keys[i] == keyTp)
        {
            result = ((LeafNode *)currentNode)->pointers[i];
            // cout << "got the node\n";
            return true;
        }
    }
    // cout << "did not get the node\n";

    return false;
}

/*****************************************************************************
 * INSERTION
 ******************************************************************************/
/*
 * Insert constant key & value pair into b+ tree
 * If current tree is empty, start new tree, otherwise insert into leaf Node.
 * @return: since we only support unique key, if user try to insert duplicate
 * keys return false, otherwise return true.
 */
bool BPlusTree::Insert(const KeyType &key, const RecordPointer &value)
{
    if (IsEmpty())
    {
        // first key to be inserted
        root = new LeafNode();
        root->key_num = 1;
        root->keys[0] = key;
        ((LeafNode *)root)->pointers[0] = value;
        return true;
    }
    else
    {
        Node *currNode = root;
        Node *parent;

        // Find the leaf node where the key should be inserted
        findLeafNodeToInsertNewKey(key, currNode, parent);

        // If CurrNode has empty key slots
        // Insert it into this current node without any slice or rearranging
        if (currNode->key_num < MAX_FANOUT - 1) {
            return insertInCurrNodeAvlSlot(key, value, currNode);
        }
        return insertInNewNodeAndRearrange(key, value, currNode, parent);
    }
}

/**
 * Inserts a new Node in the tree and rearranges by adding new nodes in bottom up fashion
 * @param key The key to be inserted
 * @param value The pointer to the key points to in database
 * @param currNode The currentNode pointer
 * @param parent The Parent pointer
 * @return true is successfully inserted else returns false
 */
bool BPlusTree::insertInNewNodeAndRearrange(const int &key, const RecordPointer &value, Node *currNode, Node *parent) {
    try {
        // creating new leaf node
        Node *newLeafNode = new LeafNode();

        vector<int> vectorOfNodes(MAX_FANOUT);
        vector<RecordPointer> vectorOfPointers(MAX_FANOUT);

        for (int index = 0; index < MAX_FANOUT - 1; index++)
        {
            vectorOfNodes[index] = currNode->keys[index];
            vectorOfPointers[index] = ((LeafNode *)currNode)->pointers[index];
        }
        int index = 0, currentKey;
        while (key > vectorOfNodes[index] && index < MAX_FANOUT - 1)
            index++;
        for (int keyCount = MAX_FANOUT - 1; keyCount > index; keyCount--)
        {
            vectorOfNodes[keyCount] = vectorOfNodes[keyCount - 1];
            vectorOfPointers[keyCount] = vectorOfPointers[keyCount - 1];
        }
        vectorOfNodes[index] = key;
        vectorOfPointers[index] = value;
        currNode->key_num = (MAX_FANOUT) / 2;
        newLeafNode->key_num = MAX_FANOUT - (MAX_FANOUT) / 2;

        // filling cur leaf node again
        for (index = 0; index < currNode->key_num; index++)
        {
            currNode->keys[index] = vectorOfNodes[index];
            ((LeafNode *)currNode)->pointers[index] = vectorOfPointers[index];
        }
        // filling newleaf node
        for (index = 0, currentKey = currNode->key_num; index < newLeafNode->key_num; index++, currentKey++)
        {
            newLeafNode->keys[index] = vectorOfNodes[currentKey];
            ((LeafNode *)newLeafNode)->pointers[index] = vectorOfPointers[currentKey];
        }
        if (currNode == root) {
            return insertInRootNode(currNode, newLeafNode);
        }
        return insertNodeInInternalTree(newLeafNode->keys[0], parent, newLeafNode);
    }  catch (const std::exception& e) {
        std::cout<<"Exception occurred while inserting "<<e.what()<<endl;
        return false;
    }
}


/**
 * Insert in the root node
 * @param currNode current node where the value must be inserted
 * @param newLeafNode the new Leaf node which must be added
 * @return True if successfully inserted and false if any error encountered
 */
bool BPlusTree::insertInRootNode(Node *currNode, Node *newLeafNode) {
    try {
        Node *newRoot = new InternalNode();
        newRoot->key_num = 1;
        newRoot->keys[0] = newLeafNode->keys[0];
        ((InternalNode *)newRoot)->children[0] = currNode;
        ((InternalNode *)newRoot)->children[1] = newLeafNode;

        // Change the root of BPlusTree to new root with new value
        root = newRoot;
        ((LeafNode *)currNode)->next_leaf = (LeafNode *)newLeafNode;
        ((LeafNode *)newLeafNode)->prev_leaf = (LeafNode *)currNode;
//        printNode(root, 0);
        return true;
    } catch (const std::exception& e) {
        std::cout<<"Exception occurred while inserting "<<e.what()<<endl;
        return false;
    }
}

/**
 * Inserts the key in available slot of current node
 * @param key Key to insert
 * @param value The recordPointer the key points to
 * @param currNode The current Node to insert the key
 * @return Return true if successfully inserted else returns false
 */
bool BPlusTree::insertInCurrNodeAvlSlot(const int &key, const RecordPointer &value, Node *currNode) {
    try {
        int currIndex = 0;
        for(; key > currNode->keys[currIndex] && currIndex < currNode->key_num; currIndex++) {};
        for (int currentPointer = currNode->key_num; currentPointer > currIndex; currentPointer--)
        {
            currNode->keys[currentPointer] = currNode->keys[currentPointer - 1];
            ((LeafNode *)currNode)->pointers[currentPointer] = ((LeafNode *)currNode)->pointers[currentPointer - 1];
        }
        currNode->keys[currIndex] = key;
        ((LeafNode *)currNode)->pointers[currIndex] = value;
        currNode->key_num++;
//        printNode(root, 0);
        return true;
    } catch (const std::exception& e) {
        std::cout<<"Exception occurred while inserting "<<e.what()<<endl;
        return false;
    }

}

/**
 * Search for the Leaf Node where the new value must be inserted
 * @param key The key to be inserted
 * @param currNode The node where value must be inserted
 * @param parent Parent of that pointer
 */
void BPlusTree::findLeafNodeToInsertNewKey(const int &key, Node *&currNode, Node *&parent) {
    while (!currNode->is_leaf)
    {
        // going to the leaf node where the key needs to be inserted
        parent = currNode;
        for (int index = 0; index < currNode->key_num; index++)
        {
            if (key < currNode->keys[index])
            {
                currNode = ((InternalNode *)currNode)->children[index];
                break;
            }
            if (index == currNode->key_num - 1)
            {
                currNode = ((InternalNode *)currNode)->children[index + 1];
                break;
            }
        }
    }
}

/**
 * Inserts a node in Internal part of the Tree
 * @param keyTp The key to insert
 * @param parentNode The parentNode
 * @param childNode The childNode
 * @return Return true if successfully inserted else returns false
 */
bool BPlusTree::insertNodeInInternalTree(KeyType keyTp, Node *parentNode, Node *childNode)
{
    try {
        if (parentNode->key_num < MAX_FANOUT - 1) {
            return insertKeyInParentAvlSlot(keyTp, parentNode, childNode);
        }
        return insertInTreeByCreatingNewNode(keyTp, parentNode, childNode);
    } catch (const std::exception& e) {
        std::cout<<"Error in insertNodeInInternalTree "<<e.what()<<endl;
        return false;
    }

}


/**
 * Creates and Inserts a new Node in Internal part of the Tree
 * @param keyTp The key to insert
 * @param parentNode The parentNode
 * @param childNode The childNode
 * @return Return true if successfully inserted else returns false
 */
bool BPlusTree::insertInTreeByCreatingNewNode(int keyTp, Node *parentNode, Node *childNode) {
    try {
        Node *newIntNode = new InternalNode();
        vector<int> vectorOfKeys(MAX_FANOUT);
        vector<Node *> vtrOfChildPointers(MAX_FANOUT + 1);
        for (int index = 0; index < MAX_FANOUT - 1; index++)
        {
            vectorOfKeys[index] = parentNode->keys[index];
        }
        for (int index = 0; index < MAX_FANOUT; index++)
        {
            vtrOfChildPointers[index] = ((InternalNode *)parentNode)->children[index];
        }
        int index = 0, j;
        while (keyTp > vectorOfKeys[index] && index < MAX_FANOUT - 1)
            index++;
        for (int childIndex = MAX_FANOUT - 1; childIndex > index; childIndex--)
        {
            vectorOfKeys[childIndex] = vectorOfKeys[childIndex - 1];
        }
        vectorOfKeys[index] = keyTp;
        for (int childIndex = MAX_FANOUT; childIndex > index + 1; childIndex--)
        {
            vtrOfChildPointers[childIndex] = vtrOfChildPointers[childIndex - 1];
        }
        vtrOfChildPointers[index + 1] = childNode;
        
        if (childNode->is_leaf)
        {
            ((LeafNode *)childNode)->next_leaf = ((LeafNode *)(vtrOfChildPointers[index]))->next_leaf;
            ((LeafNode *)(vtrOfChildPointers[index]))->next_leaf = (LeafNode *)childNode;
            ((LeafNode *)childNode)->prev_leaf = ((LeafNode *)(vtrOfChildPointers[index]));
            if (((LeafNode *)childNode)->next_leaf != NULL)
            {
                ((LeafNode *)childNode)->next_leaf->prev_leaf = (LeafNode *)childNode;
            }
        }

        parentNode->key_num = (MAX_FANOUT) / 2;
        newIntNode->key_num = MAX_FANOUT - 1 - (MAX_FANOUT) / 2;
        for (index = 0, j = parentNode->key_num + 1; index < newIntNode->key_num; index++, j++)
        {
            newIntNode->keys[index] = vectorOfKeys[j];
        }
        for (index = 0, j = parentNode->key_num + 1; index < newIntNode->key_num + 1; index++, j++)
        {
            ((InternalNode *)newIntNode)->children[index] = vtrOfChildPointers[j];
        }

        if (parentNode == root)
        {
            Node *newRoot = new InternalNode();
            newRoot->keys[0] = parentNode->keys[parentNode->key_num];
            ((InternalNode *)newRoot)->children[0] = parentNode;
            ((InternalNode *)newRoot)->children[1] = newIntNode;
            newRoot->key_num = 1;
            root = newRoot;
            return true;
        }
        else
        {
            // changed 1st argument from parentNode->keys[parentNode->key_num] for 222
            return insertNodeInInternalTree(vectorOfKeys[parentNode->key_num],
                                            parentNodeSearch(root, parentNode),
                                            newIntNode);
        }
    } catch (exception& e) {
        cout<<"Error in insertInTreeByCreatingNewNode "<<e.what()<<endl;
        return false;
    }
}


/**
 * Creates and Inserts a new Key in available parent key slot
 * @param keyTp The key to insert
 * @param parentNode The parentNode
 * @param childNode The childNode
 * @return Return true if successfully inserted else returns false
 */
bool BPlusTree::insertKeyInParentAvlSlot(int keyTp, Node *parentNode, Node *childNode) {
    try {
        int childIndex = 0;
        while (keyTp > parentNode->keys[childIndex] && childIndex < parentNode->key_num)
            childIndex++;
        for (int j = parentNode->key_num; j > childIndex; j--)
        {
            parentNode->keys[j] = parentNode->keys[j - 1];
        }
        for (int j = parentNode->key_num + 1; j > childIndex + 1; j--)
        {
            ((InternalNode *)parentNode)->children[j] = ((InternalNode *)parentNode)->children[j - 1];
        }
        parentNode->keys[childIndex] = keyTp;
        parentNode->key_num++;
        ((InternalNode *)parentNode)->children[childIndex + 1] = childNode;

        if (childNode->is_leaf) {
            ((LeafNode *)childNode)->next_leaf = ((LeafNode *)(((InternalNode *)parentNode)->children[childIndex]))->next_leaf;
            ((LeafNode *)(((InternalNode *)parentNode)->children[childIndex]))->next_leaf = (LeafNode *)childNode;
            ((LeafNode *)childNode)->prev_leaf = (LeafNode *)(((InternalNode *)parentNode)->children[childIndex]);
            if (((LeafNode *)childNode)->next_leaf) {
                ((LeafNode *)childNode)->next_leaf->prev_leaf = (LeafNode *)childNode;
            }
        }
        return true;
    } catch (std::exception& e) {
        std::cout<<"Error occurred in insertKeyInParentAvlSlot "<<e.what()<<endl;
        return false;
    }

}


/**
 * Finds the parent Node and returns it
 * @param currentNode The current Node
 * @param childNode The child Node
 * @return Returns the parent node if found else returns NULL
 */
Node *BPlusTree::parentNodeSearch(Node *currentNode, Node *childNode)
{
    if (currentNode->is_leaf || (((InternalNode *)currentNode)->children[0])->is_leaf) {
        return NULL;
    }
    for (int currNodeIndex = 0; currNodeIndex < currentNode->key_num + 1; currNodeIndex++) {
        if (((InternalNode *)currentNode)->children[currNodeIndex] == childNode) {
            return currentNode;
        } else {
            Node *parent = parentNodeSearch(((InternalNode *) currentNode)->children[currNodeIndex], childNode);
            if (parent) return parent;
        }
    }
    return NULL;
}

/*****************************************************************************
 * REMOVE
 *****************************************************************************/
/*
 * Delete keyTp & value pair associated with input keyTp
 * If current tree is empty, return immediately.
 * If not, User needs to first find the right leaf node as deletion target, then
 * delete entry from leaf node. Remember to deal with redistribute or merge if
 * necessary.
 */
void BPlusTree::Remove(const KeyType &keyTp)
{
    if (IsEmpty()) return;

    Node *currNode = root, *parentNode;
    int lSiblingValue, rSiblingValue, pointerPos ;
    bool foundTheKey = false;

    findNodeWhichHasGivenKey(keyTp, currNode, parentNode, lSiblingValue, rSiblingValue);

    for (pointerPos = 0; pointerPos < currNode->key_num; pointerPos++) {
        if (currNode->keys[pointerPos] == keyTp)  {
            foundTheKey = true;
            break;
        }
    }
    if (foundTheKey) {
        for (int currPosition = pointerPos; currPosition < currNode->key_num; currPosition++) {
            currNode->keys[currPosition] = currNode->keys[currPosition + 1];
            ((LeafNode *)currNode)->pointers[currPosition] = ((LeafNode *)currNode)->pointers[currPosition + 1];
        }
        currNode->key_num--;
        if (currNode == root) {
            for (int currIndex = 0; currIndex < MAX_FANOUT - 1; currIndex++) {
                ((InternalNode *)currNode)->children[currIndex] = NULL;
            }
            if (currNode->key_num == 0) {
                delete currNode;
                root = NULL;
            }
//            printNode(root, 0);
            return;
        }
        if (currNode->key_num >= (MAX_FANOUT - 1) / 2) {
            // First Remove the KeyTp and then remove the parentNode
            // leave if the node is half empty else merge the node
            removeTheNodeWhichIsHalfFilled(keyTp, currNode);
//            printNode(root, 0);
            return;
        }
        if (lSiblingValue >= 0)
        {
            Node *leftChild = ((InternalNode *)parentNode)->children[lSiblingValue];
            if (leftChild->key_num >= (MAX_FANOUT - 1) / 2 + 1) {
                removeMoreThanHalfFilledLSibling(currNode, parentNode, lSiblingValue, leftChild);
//                printNode(root, 0);
                return;
            }
        }
        if (rSiblingValue <= parentNode->key_num)
        {
            Node *rightChild = ((InternalNode *)parentNode)->children[rSiblingValue];
            if (rightChild->key_num >= (MAX_FANOUT) / 2 + 1)
            {
                removeMoreThanHalfFilledRSibling(currNode, parentNode, rSiblingValue, rightChild);
//                printNode(root, 0);
                return;
            }
        }
        if (lSiblingValue >= 0)
        {
            Node *leftNode = ((InternalNode *)parentNode)->children[lSiblingValue];
            for (int index = leftNode->key_num, j = 0; j < currNode->key_num; index++, j++)
            {
                leftNode->keys[index] = currNode->keys[j];
            }
            ((InternalNode *)leftNode)->children[leftNode->key_num] = NULL;
            leftNode->key_num += currNode->key_num;
            ((InternalNode *)leftNode)->children[leftNode->key_num] = ((InternalNode *)currNode)->children[currNode->key_num];
            removeNodeInInternalTree(parentNode->keys[lSiblingValue], parentNode, currNode);
            delete currNode;
        }
        else if (rSiblingValue <= parentNode->key_num)
        {
            Node *rightNode = ((InternalNode *)parentNode)->children[rSiblingValue];
            for (int index = currNode->key_num, j = 0; j < rightNode->key_num; index++, j++)
            {
                currNode->keys[index] = rightNode->keys[j];
            }
            ((InternalNode *)currNode)->children[currNode->key_num] = NULL;
            currNode->key_num += rightNode->key_num;
            ((InternalNode *)currNode)->children[currNode->key_num] = ((InternalNode *)currNode)->children[rightNode->key_num];
            removeNodeInInternalTree(parentNode->keys[rSiblingValue - 1], parentNode, rightNode);
            delete rightNode;
        }
//        printNode(root, 0);
    } else {
        cout << "Element not foundTheKey" << endl;
        return;
    }
}

/**
 * Remove the Half filled Right sibling
 * @param currNode currNode
 * @param parentNode parentNode
 * @param rSiblingValue rSiblingValue
 * @param rightChild rightChild
 */
void BPlusTree::removeMoreThanHalfFilledRSibling(Node *currNode, Node *parentNode, int rSiblingValue,
                                                 Node *rightChild) const {
    currNode->key_num++;
    ((InternalNode *)currNode)->children[currNode->key_num] = ((InternalNode *)currNode)->children[currNode->key_num - 1];
    ((InternalNode *)currNode)->children[currNode->key_num - 1] = NULL;
    currNode->keys[currNode->key_num - 1] = rightChild->keys[0];
    rightChild->key_num--;
    ((InternalNode *)rightChild)->children[rightChild->key_num] = ((InternalNode *)rightChild)->children[rightChild->key_num + 1];
    ((InternalNode *)rightChild)->children[rightChild->key_num + 1] = NULL;
    for (int index = 0; index < rightChild->key_num; index++)
    {
        rightChild->keys[index] = rightChild->keys[index + 1];
    }
    parentNode->keys[rSiblingValue - 1] = rightChild->keys[0];
}

/**
 * Remove the Half filled Left sibling
 * @param currNode currNode
 * @param parentNode parentNode
 * @param rSiblingValue lSiblingValue
 * @param rightChild leftChild
 */
void BPlusTree::removeMoreThanHalfFilledLSibling(Node *currNode, Node *parentNode, int lSiblingValue,
                                                 Node *leftChild) const {
    for (int index = currNode->key_num; index > 0; index--) {
        currNode->keys[index] = currNode->keys[index - 1];
    }
    currNode->key_num++;
    ((InternalNode *)currNode)->children[currNode->key_num] = ((InternalNode *)currNode)->children[currNode->key_num - 1];
    ((InternalNode *)currNode)->children[currNode->key_num - 1] = NULL;
    currNode->keys[0] = leftChild->keys[leftChild->key_num - 1];
    leftChild->key_num--;
    ((InternalNode *)leftChild)->children[leftChild->key_num] = currNode;
    ((InternalNode *)leftChild)->children[leftChild->key_num + 1] = NULL;
    parentNode->keys[lSiblingValue] = currNode->keys[0];
}

/**
 * Removes the node which are half filled
 * @param keyTp The keyTp to remove
 * @param currNode The currNode to remove
 */
void BPlusTree::removeTheNodeWhichIsHalfFilled(const int &keyTp, const Node *currNode) const {
    Node *parentContainTheKey = root;
    bool parentFoundContainKey = false;
    while (!parentFoundContainKey && !parentContainTheKey->is_leaf)
    {
        for (int i = 0; i < parentContainTheKey->key_num; i++)
        {
            if (keyTp < parentContainTheKey->keys[i])
            {
                parentContainTheKey = ((InternalNode *)parentContainTheKey)->children[i];
                break;
            }
            if (keyTp == parentContainTheKey->keys[i])
            {
                parentContainTheKey->keys[i] = currNode->keys[0];
                parentFoundContainKey = true;
                break;
            }
            if (i == parentContainTheKey->key_num - 1)
            {
                parentContainTheKey = ((InternalNode *)parentContainTheKey)->children[i + 1];
                break;
            }
        }
    }
}

/**
 * Finds the Node which has the given key and returns the currNode
 * @param keyTp keyTp
 * @param currNode The current Node
 * @param parentNode The parent Node
 * @param lSiblingValue The left sibling
 * @param rSiblingValue The right sibling
 */
void BPlusTree::findNodeWhichHasGivenKey(const int &keyTp, Node *&currNode,
                                         Node *&parentNode, int &lSiblingValue, int &rSiblingValue) const {
    while (!currNode->is_leaf) {
        for (int currIndex = 0; currIndex < currNode->key_num; currIndex++) {
            parentNode = currNode;
            lSiblingValue = currIndex - 1;
            rSiblingValue = currIndex + 1;
            if (keyTp < currNode->keys[currIndex]) {
                currNode = ((InternalNode *) currNode)->children[currIndex];
                break;
            }
            if (currIndex == currNode->key_num - 1) {
                lSiblingValue = currIndex;
                rSiblingValue = currIndex + 2;
                currNode = ((InternalNode *)currNode)->children[currIndex + 1];
                break;
            }
        }
    }
}



/**
 * Remove a node in internal Tree for given key
 * @param keyTp The key to delete
 * @param currNode The curr Node which have to be deleted
 * @param childNode The child node to be deleted
 */
void BPlusTree::removeNodeInInternalTree(KeyType keyTp, Node *currNode, Node *childNode) {
    if (currNode == root && currNode->key_num == 1) {
        removeRootNodeWith1Key(keyTp, currNode, childNode);
        return;
    }
    int currentPosition;
    extractCurrNodePosOfTheKey(keyTp, currNode, childNode);
    if (currNode->key_num >= (MAX_FANOUT - 1) / 2 - 1) return;
    if (currNode == root) return;
    Node *parent = parentNodeSearch(root, currNode);
    int lSibling = 0, rSibling = 0;
    for (currentPosition = 0; currentPosition < parent->key_num + 1; currentPosition++) {
        if (((InternalNode *)parent)->children[currentPosition] == currNode) {
            lSibling = currentPosition - 1;
            rSibling = currentPosition + 1;
            break;
        }
    }
    if (lSibling >= 0) {
        Node *leftChild = ((InternalNode *)parent)->children[lSibling];
        if (leftChild->key_num >= (MAX_FANOUT - 1) / 2) {
            removeLSiblingOfLChild(currNode, parent, lSibling, leftChild);
            return;
        }
    }
    if (rSibling <= parent->key_num) {
        Node *rightChild = ((InternalNode *)parent)->children[rSibling];
        if (rightChild->key_num >= (MAX_FANOUT - 1) / 2) {
            removeRSiblingOfRChild(currNode, currentPosition, parent, rightChild);
            return;
        }
    }
    if (lSibling >= 0) {
        traverseTheLChildToRemove(currNode, parent, lSibling);
        removeNodeInInternalTree(parent->keys[lSibling], parent, currNode);
    }
    else if (rSibling <= parent->key_num) {
        Node *rightChild = traverseRSiblinginRChild(currNode, parent, rSibling);
        removeNodeInInternalTree(parent->keys[rSibling - 1], parent, rightChild);
    }
}

/**
 * Extract the current node position of the key
 * @param keyTp keyTp
 * @param currNode current Node
 * @param childNode child Node
 */
void BPlusTree::extractCurrNodePosOfTheKey(int keyTp, Node *currNode, const Node *childNode) const {
    int currentPosition = 0;
    while(currentPosition < currNode->key_num) {
        if (currNode->keys[currentPosition] == keyTp) {
            break;
        }
        currentPosition++;
    }
    for (int currIndex = currentPosition; currIndex < currNode->key_num; currIndex++) {
        currNode->keys[currIndex] = currNode->keys[currIndex + 1];
    }
    for (currentPosition = 0; currentPosition < currNode->key_num + 1; currentPosition++) {
        if (((InternalNode *)currNode)->children[currentPosition] == childNode) {
            break;
        }
    }
    for (int currIndex = currentPosition; currIndex < currNode->key_num + 1; currIndex++) {
        ((InternalNode *)currNode)->children[currIndex] = ((InternalNode *)currNode)->children[currIndex + 1];
    }
    currNode->key_num--;
}

Node *BPlusTree::traverseRSiblinginRChild(Node *currNode, const Node *parent, int rSibling) const {
    Node *rightChild = ((InternalNode *)parent)->children[rSibling];
    currNode->keys[currNode->key_num] = parent->keys[rSibling - 1];
    for (int index = currNode->key_num + 1, j = 0; j < rightChild->key_num; j++) {
        currNode->keys[index] = rightChild->keys[j];
    }
    for (int index = currNode->key_num + 1, j = 0; j < rightChild->key_num + 1; j++) {
        ((InternalNode *)currNode)->children[index] = ((InternalNode *)rightChild)->children[j];
        ((InternalNode *)rightChild)->children[j] = NULL;
    }
    currNode->key_num += rightChild->key_num + 1;
    rightChild->key_num = 0;
    return rightChild;
}

void BPlusTree::traverseTheLChildToRemove(Node *currNode, const Node *parent, int lSibling) const {
    try {
        Node *leftChild = ((InternalNode *)parent)->children[lSibling];
        leftChild->keys[leftChild->key_num] = parent->keys[lSibling];
        for (int currIndex = leftChild->key_num + 1, currNodeIndex = 0; currNodeIndex < currNode->key_num; currNodeIndex++) {
            leftChild->keys[currIndex] = currNode->keys[currNodeIndex];
        }
        for (int currIndex = leftChild->key_num + 1, currNodeIndex = 0; currNodeIndex < currNode->key_num + 1; currNodeIndex++) {
            ((InternalNode *)leftChild)->children[currIndex] = ((InternalNode *)currNode)->children[currNodeIndex];
            ((InternalNode *)currNode)->children[currNodeIndex] = NULL;
        }
        leftChild->key_num += currNode->key_num + 1;
        currNode->key_num = 0;
    } catch(std::exception& e){
        cout<<"Error in traverseTheLChildToRemove method"<<e.what()<<endl;
    }
}

void BPlusTree::removeRSiblingOfRChild(Node *currNode, int currentPosition, Node *parent, Node *rightChild) const {
    currNode->keys[currNode->key_num] = parent->keys[currentPosition];
    parent->keys[currentPosition] = rightChild->keys[0];
    for (int currIndex = 0; currIndex < rightChild->key_num - 1; currIndex++) {
        rightChild->keys[currIndex] = rightChild->keys[currIndex + 1];
    }
    ((InternalNode *)currNode)->children[currNode->key_num + 1] = ((InternalNode *)rightChild)->children[0];
    for (int currIndex = 0; currIndex < rightChild->key_num; ++currIndex) {
        ((InternalNode *)rightChild)->children[currIndex] = ((InternalNode *)rightChild)->children[currIndex + 1];
    }
    currNode->key_num++;
    rightChild->key_num--;
}

void BPlusTree::removeLSiblingOfLChild(Node *currNode, Node *parent, int lSibling, Node *leftChild) const {
    for (int currIndex = currNode->key_num; currIndex > 0; currIndex--) {
        currNode->keys[currIndex] = currNode->keys[currIndex - 1];
    }
    currNode->keys[0] = parent->keys[lSibling];
    parent->keys[lSibling] = leftChild->keys[leftChild->key_num - 1];
    for (int currIndex = currNode->key_num + 1; currIndex > 0; currIndex--) {
        ((InternalNode *)currNode)->children[currIndex] = ((InternalNode *)currNode)->children[currIndex - 1];
    }
    ((InternalNode *)currNode)->children[0] = ((InternalNode *)leftChild)->children[leftChild->key_num];
    currNode->key_num++;
    leftChild->key_num--;
}

/**
 * Remove the Root node which has only 1 key
 * @param keyTp keyTp
 * @param currNode The current Node
 * @param childNode The child Node
 */
void BPlusTree::removeRootNodeWith1Key(int keyTp, const Node *currNode, const Node *childNode) {
    try {
        if (((InternalNode *)currNode)->children[1] == childNode)
        {
            delete childNode;
            root = ((InternalNode *)currNode)->children[0];
            delete currNode;
        } else if (((InternalNode *)currNode)->children[0] == childNode) {
            delete childNode;
            root = ((InternalNode *)currNode)->children[1];
            delete currNode;
        } else if (keyTp == currNode->keys[0]) {
            delete childNode;
            root = ((InternalNode *)currNode)->children[1];
            delete currNode;
        }
    } catch (std::exception& e) {
        cout<<"Exception occurred in removeRootNodeWith1Key "<<e.what()<<endl;
    }

}

/*****************************************************************************
 * RANGE_SCAN
 *****************************************************************************/
/*
 * Return the values that within the given key range
 * First find the node large or equal to the key_start, then traverse the leaf
 * nodes until meet the key_end position, fetch all the records.
 */
void BPlusTree::RangeScan(const KeyType &key_start, const KeyType &key_end,
                          std::vector<RecordPointer> &result)
{
    if (IsEmpty()) return;

    // find node large or equal to key_start
    Node *currentNode = root;
    while (!currentNode->is_leaf) {
        for (int currIndex = 0; currIndex < currentNode->key_num; currIndex++) {
            if (key_start < currentNode->keys[currIndex]) {
                currentNode = ((InternalNode *)currentNode)->children[currIndex];
                break;
            }
            if (currIndex == currentNode->key_num - 1) {
                currentNode = ((InternalNode *)currentNode)->children[currIndex + 1];
                break;
            }
        }
    }
    if (currentNode->is_leaf) {
        while (currentNode != NULL) {
            for (int currentIndex = 0; currentIndex < currentNode->key_num; currentIndex++) {
                if (currentNode->keys[currentIndex] >= key_start and currentNode->keys[currentIndex] <= key_end) {
                    result.push_back(((LeafNode *)currentNode)->pointers[currentIndex]);
                }
            }
            currentNode = ((LeafNode *)currentNode)->next_leaf;
        }
    }
}

/*Printing the tree*/
void BPlusTree::printNode(Node *node, int level)
{
    if (node == NULL) {
        return;
    }

    if (node->is_leaf) {
        LeafNode *leaf = (LeafNode *)node;
        for (int i = 0; i < leaf->key_num; i++) {
            cout << leaf->keys[i] << " ";
        }
        cout << endl;
    }
    else {
        InternalNode *internal = (InternalNode *)node;
        for (int i = 0; i < internal->key_num; i++)
        {
            if (internal->children[i] != NULL)
            {
                printNode(internal->children[i], level + 1);
            }
            cout << internal->keys[i] << " ";
        }

        // if the internal node is not the last internal node, print the last child
        if (internal->children[internal->key_num] != NULL)
        {
            printNode(internal->children[internal->key_num], level + 1);
        }
    }
}