/**
* implement a container like std::map
*/
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
   class Key,
   class T,
   class Compare = std::less <Key>
   > class map {
  public:
   typedef pair<const Key, T> value_type;

  private:
   enum Color { RED, BLACK };

   struct Node {
       value_type* data;
       Node* left;
       Node* right;
       Node* parent;
       Color color;

       Node(const value_type& val, Node* p = nullptr)
           : data(new value_type(val)), left(nullptr), right(nullptr),
             parent(p), color(RED) {}

       ~Node() {
           if (data) delete data;
       }
   };

   Node* root;
   size_t nodeCount;
   Compare comp;

   // Helper functions for RB tree operations
   void rotateLeft(Node* x) {
       Node* y = x->right;
       x->right = y->left;
       if (y->left != nullptr) {
           y->left->parent = x;
       }
       y->parent = x->parent;
       if (x->parent == nullptr) {
           root = y;
       } else if (x == x->parent->left) {
           x->parent->left = y;
       } else {
           x->parent->right = y;
       }
       y->left = x;
       x->parent = y;
   }

   void rotateRight(Node* x) {
       Node* y = x->left;
       x->left = y->right;
       if (y->right != nullptr) {
           y->right->parent = x;
       }
       y->parent = x->parent;
       if (x->parent == nullptr) {
           root = y;
       } else if (x == x->parent->right) {
           x->parent->right = y;
       } else {
           x->parent->left = y;
       }
       y->right = x;
       x->parent = y;
   }

   void fixInsert(Node* z) {
       while (z->parent != nullptr && z->parent->color == RED) {
           if (z->parent == z->parent->parent->left) {
               Node* y = z->parent->parent->right;
               if (y != nullptr && y->color == RED) {
                   z->parent->color = BLACK;
                   y->color = BLACK;
                   z->parent->parent->color = RED;
                   z = z->parent->parent;
               } else {
                   if (z == z->parent->right) {
                       z = z->parent;
                       rotateLeft(z);
                   }
                   z->parent->color = BLACK;
                   z->parent->parent->color = RED;
                   rotateRight(z->parent->parent);
               }
           } else {
               Node* y = z->parent->parent->left;
               if (y != nullptr && y->color == RED) {
                   z->parent->color = BLACK;
                   y->color = BLACK;
                   z->parent->parent->color = RED;
                   z = z->parent->parent;
               } else {
                   if (z == z->parent->left) {
                       z = z->parent;
                       rotateRight(z);
                   }
                   z->parent->color = BLACK;
                   z->parent->parent->color = RED;
                   rotateLeft(z->parent->parent);
               }
           }
       }
       root->color = BLACK;
   }

   void transplant(Node* u, Node* v) {
       if (u->parent == nullptr) {
           root = v;
       } else if (u == u->parent->left) {
           u->parent->left = v;
       } else {
           u->parent->right = v;
       }
       if (v != nullptr) {
           v->parent = u->parent;
       }
   }

   Node* minimum(Node* node) const {
       while (node->left != nullptr) {
           node = node->left;
       }
       return node;
   }

   Node* maximum(Node* node) const {
       while (node->right != nullptr) {
           node = node->right;
       }
       return node;
   }

   void fixDelete(Node* x, Node* xParent) {
       while (x != root && (x == nullptr || x->color == BLACK)) {
           if (x == xParent->left) {
               Node* w = xParent->right;
               if (w->color == RED) {
                   w->color = BLACK;
                   xParent->color = RED;
                   rotateLeft(xParent);
                   w = xParent->right;
               }
               if ((w->left == nullptr || w->left->color == BLACK) &&
                   (w->right == nullptr || w->right->color == BLACK)) {
                   w->color = RED;
                   x = xParent;
                   xParent = x->parent;
               } else {
                   if (w->right == nullptr || w->right->color == BLACK) {
                       if (w->left != nullptr) w->left->color = BLACK;
                       w->color = RED;
                       rotateRight(w);
                       w = xParent->right;
                   }
                   w->color = xParent->color;
                   xParent->color = BLACK;
                   if (w->right != nullptr) w->right->color = BLACK;
                   rotateLeft(xParent);
                   x = root;
                   break;
               }
           } else {
               Node* w = xParent->left;
               if (w->color == RED) {
                   w->color = BLACK;
                   xParent->color = RED;
                   rotateRight(xParent);
                   w = xParent->left;
               }
               if ((w->right == nullptr || w->right->color == BLACK) &&
                   (w->left == nullptr || w->left->color == BLACK)) {
                   w->color = RED;
                   x = xParent;
                   xParent = x->parent;
               } else {
                   if (w->left == nullptr || w->left->color == BLACK) {
                       if (w->right != nullptr) w->right->color = BLACK;
                       w->color = RED;
                       rotateLeft(w);
                       w = xParent->left;
                   }
                   w->color = xParent->color;
                   xParent->color = BLACK;
                   if (w->left != nullptr) w->left->color = BLACK;
                   rotateRight(xParent);
                   x = root;
                   break;
               }
           }
       }
       if (x != nullptr) x->color = BLACK;
   }

   Node* findNode(const Key& key) const {
       Node* current = root;
       while (current != nullptr) {
           if (comp(key, current->data->first)) {
               current = current->left;
           } else if (comp(current->data->first, key)) {
               current = current->right;
           } else {
               return current;
           }
       }
       return nullptr;
   }

   Node* copyTree(Node* node, Node* parent) {
       if (node == nullptr) return nullptr;
       Node* newNode = new Node(*(node->data), parent);
       newNode->color = node->color;
       newNode->left = copyTree(node->left, newNode);
       newNode->right = copyTree(node->right, newNode);
       return newNode;
   }

   void deleteTree(Node* node) {
       if (node == nullptr) return;
       deleteTree(node->left);
       deleteTree(node->right);
       delete node;
   }

   Node* successor(Node* node) const {
       if (node->right != nullptr) {
           return minimum(node->right);
       }
       Node* p = node->parent;
       while (p != nullptr && node == p->right) {
           node = p;
           p = p->parent;
       }
       return p;
   }

   Node* predecessor(Node* node) const {
       if (node->left != nullptr) {
           return maximum(node->left);
       }
       Node* p = node->parent;
       while (p != nullptr && node == p->left) {
           node = p;
           p = p->parent;
       }
       return p;
   }

  public:
   class const_iterator;
   class iterator {
      private:
       Node* node;
       const map* container;

       friend class map;
       friend class const_iterator;

       iterator(Node* n, const map* c) : node(n), container(c) {}

      public:
       iterator() : node(nullptr), container(nullptr) {}

       iterator(const iterator &other) : node(other.node), container(other.container) {}

       iterator operator++(int) {
           iterator temp = *this;
           ++(*this);
           return temp;
       }

       iterator &operator++() {
           if (node == nullptr) {
               throw invalid_iterator();
           }
           node = container->successor(node);
           return *this;
       }

       iterator operator--(int) {
           iterator temp = *this;
           --(*this);
           return temp;
       }

       iterator &operator--() {
           if (node == nullptr) {
               if (container == nullptr || container->root == nullptr) {
                   throw invalid_iterator();
               }
               node = container->maximum(container->root);
           } else {
               Node* prev = container->predecessor(node);
               if (prev == nullptr) {
                   throw invalid_iterator();
               }
               node = prev;
           }
           return *this;
       }

       value_type &operator*() const {
           if (node == nullptr) {
               throw invalid_iterator();
           }
           return *(node->data);
       }

       bool operator==(const iterator &rhs) const {
           return node == rhs.node && container == rhs.container;
       }

       bool operator==(const const_iterator &rhs) const {
           return node == rhs.node && container == rhs.container;
       }

       bool operator!=(const iterator &rhs) const {
           return !(*this == rhs);
       }

       bool operator!=(const const_iterator &rhs) const {
           return !(*this == rhs);
       }

       value_type *operator->() const noexcept {
           return node->data;
       }
   };

   class const_iterator {
      private:
       Node* node;
       const map* container;

       friend class map;
       friend class iterator;

       const_iterator(Node* n, const map* c) : node(n), container(c) {}

      public:
       const_iterator() : node(nullptr), container(nullptr) {}

       const_iterator(const const_iterator &other) : node(other.node), container(other.container) {}

       const_iterator(const iterator &other) : node(other.node), container(other.container) {}

       const_iterator operator++(int) {
           const_iterator temp = *this;
           ++(*this);
           return temp;
       }

       const_iterator &operator++() {
           if (node == nullptr) {
               throw invalid_iterator();
           }
           node = container->successor(node);
           return *this;
       }

       const_iterator operator--(int) {
           const_iterator temp = *this;
           --(*this);
           return temp;
       }

       const_iterator &operator--() {
           if (node == nullptr) {
               if (container == nullptr || container->root == nullptr) {
                   throw invalid_iterator();
               }
               node = container->maximum(container->root);
           } else {
               Node* prev = container->predecessor(node);
               if (prev == nullptr) {
                   throw invalid_iterator();
               }
               node = prev;
           }
           return *this;
       }

       const value_type &operator*() const {
           if (node == nullptr) {
               throw invalid_iterator();
           }
           return *(node->data);
       }

       bool operator==(const iterator &rhs) const {
           return node == rhs.node && container == rhs.container;
       }

       bool operator==(const const_iterator &rhs) const {
           return node == rhs.node && container == rhs.container;
       }

       bool operator!=(const iterator &rhs) const {
           return !(*this == rhs);
       }

       bool operator!=(const const_iterator &rhs) const {
           return !(*this == rhs);
       }

       const value_type *operator->() const noexcept {
           return node->data;
       }
   };

   map() : root(nullptr), nodeCount(0) {}

   map(const map &other) : root(nullptr), nodeCount(0), comp(other.comp) {
       root = copyTree(other.root, nullptr);
       nodeCount = other.nodeCount;
   }

   map &operator=(const map &other) {
       if (this != &other) {
           deleteTree(root);
           root = copyTree(other.root, nullptr);
           nodeCount = other.nodeCount;
           comp = other.comp;
       }
       return *this;
   }

   ~map() {
       deleteTree(root);
   }

   T &at(const Key &key) {
       Node* node = findNode(key);
       if (node == nullptr) {
           throw index_out_of_bound();
       }
       return node->data->second;
   }

   const T &at(const Key &key) const {
       Node* node = findNode(key);
       if (node == nullptr) {
           throw index_out_of_bound();
       }
       return node->data->second;
   }

   T &operator[](const Key &key) {
       Node* node = findNode(key);
       if (node != nullptr) {
           return node->data->second;
       }

       // Insert new node with default value
       value_type newValue(key, T());
       Node* newNode = new Node(newValue);

       if (root == nullptr) {
           root = newNode;
           root->color = BLACK;
       } else {
           Node* parent = nullptr;
           Node* current = root;
           while (current != nullptr) {
               parent = current;
               if (comp(key, current->data->first)) {
                   current = current->left;
               } else {
                   current = current->right;
               }
           }
           newNode->parent = parent;
           if (comp(key, parent->data->first)) {
               parent->left = newNode;
           } else {
               parent->right = newNode;
           }
           fixInsert(newNode);
       }

       nodeCount++;
       return newNode->data->second;
   }

   const T &operator[](const Key &key) const {
       return at(key);
   }

   iterator begin() {
       if (root == nullptr) return iterator(nullptr, this);
       return iterator(minimum(root), this);
   }

   const_iterator cbegin() const {
       if (root == nullptr) return const_iterator(nullptr, this);
       return const_iterator(minimum(root), this);
   }

   iterator end() {
       return iterator(nullptr, this);
   }

   const_iterator cend() const {
       return const_iterator(nullptr, this);
   }

   bool empty() const {
       return nodeCount == 0;
   }

   size_t size() const {
       return nodeCount;
   }

   void clear() {
       deleteTree(root);
       root = nullptr;
       nodeCount = 0;
   }

   pair<iterator, bool> insert(const value_type &value) {
       Node* node = findNode(value.first);
       if (node != nullptr) {
           return pair<iterator, bool>(iterator(node, this), false);
       }

       Node* newNode = new Node(value);

       if (root == nullptr) {
           root = newNode;
           root->color = BLACK;
       } else {
           Node* parent = nullptr;
           Node* current = root;
           while (current != nullptr) {
               parent = current;
               if (comp(value.first, current->data->first)) {
                   current = current->left;
               } else {
                   current = current->right;
               }
           }
           newNode->parent = parent;
           if (comp(value.first, parent->data->first)) {
               parent->left = newNode;
           } else {
               parent->right = newNode;
           }
           fixInsert(newNode);
       }

       nodeCount++;
       return pair<iterator, bool>(iterator(newNode, this), true);
   }

   void erase(iterator pos) {
       if (pos.node == nullptr || pos.container != this) {
           throw invalid_iterator();
       }

       Node* z = pos.node;
       Node* y = z;
       Node* x;
       Node* xParent;
       Color yOriginalColor = y->color;

       if (z->left == nullptr) {
           x = z->right;
           xParent = z->parent;
           transplant(z, z->right);
       } else if (z->right == nullptr) {
           x = z->left;
           xParent = z->parent;
           transplant(z, z->left);
       } else {
           y = minimum(z->right);
           yOriginalColor = y->color;
           x = y->right;
           if (y->parent == z) {
               xParent = y;
           } else {
               xParent = y->parent;
               transplant(y, y->right);
               y->right = z->right;
               y->right->parent = y;
           }
           transplant(z, y);
           y->left = z->left;
           y->left->parent = y;
           y->color = z->color;
       }

       delete z;
       nodeCount--;

       if (yOriginalColor == BLACK) {
           fixDelete(x, xParent);
       }
   }

   size_t count(const Key &key) const {
       return findNode(key) != nullptr ? 1 : 0;
   }

   iterator find(const Key &key) {
       Node* node = findNode(key);
       if (node == nullptr) {
           return end();
       }
       return iterator(node, this);
   }

   const_iterator find(const Key &key) const {
       Node* node = findNode(key);
       if (node == nullptr) {
           return cend();
       }
       return const_iterator(node, this);
   }
};

}

#endif
