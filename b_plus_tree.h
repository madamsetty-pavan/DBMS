//===----------------------------------------------------------------------===//
//
//                         Rutgers CS539 - Database System
//                         ***DO NO SHARE PUBLICLY***
//
// Identification:   include/b_plus_tree.h
//
// Copyright (c) 2023, Rutgers University
//
//===----------------------------------------------------------------------===//
#pragma once

#include <queue>
#include <string>
#include <vector>
#include "para.h"

using namespace std;

// Value structure we insert into BPlusTree
struct RecordPointer
{
    int page_id;
    int record_id;
    RecordPointer() : page_id(0), record_id(0){};
    RecordPointer(int page, int record) : page_id(page), record_id(record){};
};

// BPlusTree Node
class Node
{
public:
    Node(bool leaf) : key_num(0), is_leaf(leaf){};
    bool is_leaf;
    int key_num;
    KeyType keys[MAX_FANOUT - 1];
};

// internal b+ tree node
class InternalNode : public Node
{
public:
    InternalNode() : Node(false){};
    Node *children[MAX_FANOUT];
};

class LeafNode : public Node
{
public:
    LeafNode() : Node(true){};
    RecordPointer pointers[MAX_FANOUT - 1];
    // pointer to the next/prev leaf node
    LeafNode *next_leaf = NULL;
    LeafNode *prev_leaf = NULL;
};

/**
 * Main class providing the API for the Interactive B+ Tree.
 *
 * Implementation of simple b+ tree data structure where internal pages direct
 * the search and leaf pages contain record pointers
 * (1) We only support (and test) UNIQUE key
 * (2) Support insert & remove
 * (3) Support range scan, return multiple values.
 * (4) The structure should shrink and grow dynamically
 */

class BPlusTree
{
public:
    BPlusTree()
    {
        // Initialising root to NULL
        root = NULL;
    };

    // Returns true if this B+ tree has no keys and values
    bool IsEmpty() const;

    // Insert a key-value pair into this B+ tree.
    bool Insert(const KeyType &key, const RecordPointer &value);

    // Remove a keyTp and its value from this B+ tree.
    void Remove(const KeyType &keyTp);

    // return the value associated with a given keyTp
    bool GetValue(const KeyType &keyTp, RecordPointer &result);

    // return the values within a key range [key_start, key_end) not included key_end
    void RangeScan(const KeyType &key_start, const KeyType &key_end,
                   std::vector<RecordPointer> &result);


    // pointer to the root node.
    Node *root;

    // Below all are my Helper Functions
    bool insertNodeInInternalTree(KeyType keyTp, Node *parentNode, Node *childNode);
    Node* parentNodeSearch(Node *currentNode, Node *childNode);


    void removeNodeInInternalTree(KeyType keyTp, Node *currNode, Node *childNode);


    void printNode(Node *node, int level);

    static void findLeafNodeToInsertNewKey(const int &key, Node *&currNode, Node *&parent);

    bool insertInCurrNodeAvlSlot(const int &key, const RecordPointer &value, Node *currNode);

    bool insertInNewNodeAndRearrange(const int &key, const RecordPointer &value, Node *currNode, Node *parent);

    bool insertInRootNode(Node *currNode, Node *newLeafNode);

    static bool insertKeyInParentAvlSlot(int keyTp, Node *parentNode, Node *childNode) ;

    bool insertInTreeByCreatingNewNode(int keyTp, Node *parentNode, Node *childNode);

    void
    findNodeWhichHasGivenKey(const int &keyTp, Node *&currNode, Node *&parentNode, int &lSiblingValue,
                             int &rSiblingValue) const;

    void removeTheNodeWhichIsHalfFilled(const int &keyTp, const Node *currNode) const;

    void removeMoreThanHalfFilledLSibling(Node *currNode, Node *parentNode, int lSiblingValue, Node *leftChild) const;

    void removeMoreThanHalfFilledRSibling(Node *currNode, Node *parentNode, int rSiblingValue, Node *rightChild) const;

    void removeRootNodeWith1Key(int keyTp, const Node *currNode, const Node *childNode);

    void removeLSiblingOfLChild(Node *currNode, Node *parent, int lSibling, Node *leftChild) const;

    void removeRSiblingOfRChild(Node *currNode, int currentPosition, Node *parent, Node *rightChild) const;

    void traverseTheLChildToRemove(Node *currNode, const Node *parent, int lSibling) const;

    Node *traverseRSiblinginRChild(Node *currNode, const Node *parent, int rSibling) const;

    void extractCurrNodePosOfTheKey(int keyTp, Node *currNode, const Node *childNode) const;
};
