#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

// A specialized version of the 'priority_queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure to
            // initialize it here.
            explicit Node(const TYPE &val)
            : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
            {}

            // Description: Allows access to the element at that Node's position.
			// There are two versions, getElt() and a dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data members of this
            // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            // TODO: Add one extra pointer (parent or previous) as desired.
            Node *parent;
    }; // Node


    // Description: Construct an empty priority_queue with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        // TODO: Implement this function.
        root = nullptr;
        sizeofdata = 0;
        
    } // PairingPQ()


    // Description: Construct a priority_queue out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        // TODO: Implement this function.
            root = nullptr;
            sizeofdata = 0;
            // have to iterate over this differently, ISSUES HERE, UNDEFINED BEHAVIOR, END ISNT GETTING PASSED CORRECTLY
            while (start != end)
            {
                addNode(*start);
                start++;
            }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ& other) :
        BaseClass{ other.compare } {
        // TODO: Implement this function.
        // SOMETHING MESSY GOING ON IN HERE
        std::deque<Node*> meldingdeque;
        root = nullptr;
        sizeofdata = 0;
        if (other.root == nullptr)
        {
            return;
        }
         /*
        else if (*this == &other)
        {
            
        }
         */
        else
        {
            // COULD JUST PUSH ROOT
            Node* temp = other.root;
            meldingdeque.push_back(temp);
            
            while (!meldingdeque.empty())
            {
                // do i need to sever relationships of every node i push in?
                if (meldingdeque[0]->sibling != nullptr)
                {
                    meldingdeque.push_back(meldingdeque[0]->sibling);
                }
                if (meldingdeque[0]->child != nullptr)
                {
                    meldingdeque.push_back(meldingdeque[0]->child);
                }
                meldingdeque[0]->parent = nullptr;
                TYPE value = meldingdeque[0]->getElt();
                push(value); // IS THIS SIGSEV??
                meldingdeque.pop_front();
            }
            
        }
        
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    // TODO: when you implement this function, uncomment the parameter names.
    PairingPQ& operator=(const PairingPQ& rhs) {
        // TODO: Implement this function.
        // FIX THIS, WILL MOST LIKELY BE ERRORS HERE
        
        PairingPQ temp(rhs);
        std::swap(root, temp.root);
        std::swap(sizeofdata, temp.sizeofdata);
        
        // temp gets nullptr in root and destructor doesn't know how to deal with that
        
        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        if (root == nullptr)
        {
            return;
        }
        
        std::deque<Node*> poppingdeque;
        poppingdeque.push_back(root);
        // while there are still nodes to delete
        while (!poppingdeque.empty())
        {
            // add child of node
            if (poppingdeque[0]->child != nullptr)
            {
                poppingdeque.push_back(poppingdeque[0]->child);
            }
            // add sibling of node
            if (poppingdeque[0]->sibling != nullptr)
            {
                poppingdeque.push_back(poppingdeque[0]->sibling);
            }
            // cut all ties (wish i could add an emoji here)
            poppingdeque[0]->sibling = nullptr;
            poppingdeque[0]->child = nullptr;
            poppingdeque[0]->parent = nullptr;
            // delete node
            delete poppingdeque[0];
            poppingdeque[0] = nullptr;
            // pop node off the deque
            poppingdeque.pop_front();
        }
        
        sizeofdata = 0;
        
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the priority_queue are out of order and
    //              'rebuilds' the priority_queue by fixing the priority_queue invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        
        std::deque<Node*> meldingdeque;
        Node* temp = root;
        meldingdeque.push_back(temp);
        root = nullptr; // does this overwrite temp?
        // cycle thru all pointers, pushing their siblings and children, eventually run into nullptr relations and pushing will stop, meldingdeque will empty and things will be good
        while (!meldingdeque.empty())
        {
            // DOES ORDERING OF SIBLING VERSUS CHILD HERE MAKE ANY DIFFERENCE
            
            // add top node's sibling, if not null, and break relation
            if (meldingdeque[0]->sibling != nullptr)
            {
                meldingdeque.push_back(meldingdeque[0]->sibling);
                meldingdeque[0]->sibling = nullptr;
            }
            
            // add top node's child, if not null, and break relation
            if (meldingdeque[0]->child != nullptr)
            {
                meldingdeque.push_back(meldingdeque[0]->child);
                meldingdeque[0]->child = nullptr;
            }
            
            // meld root and first node
            root = meld(root, meldingdeque[0]);
            // do i want to push the resulting meld back in??? i dont think so
            // meldingdeque.push_back(tempnode);
            meldingdeque.pop_front();
        }
        
    } // updatePriorities()


    // Description: Add a new element to the priority_queue. This is almost done,
    //              in that you should implement push functionality in the addNode()
    //              function, and this function should call addNode().
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE & val) {
        // TODO: Implement this function.
        // call addNode here
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the priority_queue.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the priority_queue is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        
        sizeofdata--;
        // pop the top element
        std::deque<Node*> meldingdeque;
        // check if we're popping root only, emptying pq
        if (root->child == nullptr)
        {
            delete root;
            root = nullptr;
        }
        // not a single element PQ, pop regularly
        else
        {
            // delete root and store its child in deque
            Node* temp = root->child;
            temp->parent = nullptr;
            delete root;
            root = nullptr;
            // if sibling isnt null, we push it back and add its sibling to container
            while (temp != nullptr)
            {
                Node* temp2 = temp->sibling;
                temp->sibling = nullptr;
                meldingdeque.push_back(temp);
                temp = temp2;
            }
            
            while (meldingdeque.size() != 1)
            {
                Node* node = meld(meldingdeque[0], meldingdeque[1]);
                meldingdeque.push_back(node);
                // needed?
                meldingdeque.pop_front();
                meldingdeque.pop_front();
            }
            root = meldingdeque.front();
        }
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE & top() const {
        
        return root->getElt();
        
    } // top()


    // Description: Get the number of elements in the priority_queue.
    // Runtime: O(1)
    virtual std::size_t size() const {
        
        return sizeofdata;
        
    } // size()

    // Description: Return true if the priority_queue is empty.
    // Runtime: O(1)
    virtual bool empty() const {
       
        if (sizeofdata == 0)
            return true;
        
        else
            return false;
        
    } // empty()


    // Description: Updates the priority of an element already in the priority_queue by
    //              replacing the element refered to by the Node with new_value.
    //              Must maintain priority_queue invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    // TODO: when you implement this function, uncomment the parameter names.
    void updateElt(Node* node, const TYPE & new_value) {
        // TODO: Implement this function
        // gotta call addnode in here for some reason, GO OVER THIS?
        
        // new value is higher priority, making node higher priority than parent, have to detach and meld
        if (this->compare(new_value, node->parent->getElt()))
        {
            // updated node is parent's child, do special case update
            if (node->parent->child == node)
            {
                node->parent->child = node->sibling;
                node->parent = nullptr;
            }
            // otherwise find if there's any nodes that have updated node as a sibling
            else
            {
                Node* temp = node->parent->child;
                while (temp->sibling != node)
                {
                    temp = temp->sibling;
                }
                // temp->sibling is our node to change, update relationships
                temp->sibling = node->sibling;
                node->sibling = nullptr; // does this ruin temp's relationship? WILL CHECK LATER
                node->parent = nullptr;
            }
            // should have severed all relationships to tree at this point, remeld in edited node.
            node->elt = new_value;
            meld(root, node);
        }
        // new node is NOT higher priority, no relationship changes, just change its elt
        else
        {
            // priority doesn't really change, so we good
            node->elt = new_value;
        }
        
    } // updateElt()


    // Description: Add a new element to the priority_queue. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    // NOTE: Whenever you create a node, and thus return a Node *, you must be sure to
    //       never move or copy/delete that node in the future, until it is eliminated
    //       by the user calling pop().  Remember this when you implement updateElt() and
    //       updatePriorities().
    Node* addNode(const TYPE & val) {
        // TODO: Implement this function
        // make one node, meld it
        Node* node = new Node(val);
        meld(root, node);
        // increase data size by one
        sizeofdata++;
        
        return node; // TODO: Delete or change this line
    } // addNode()


private:
    // TODO: Add any additional member functions or data you require here.
    // is data even needed here
    Node* root;
    size_t sizeofdata;
    
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap papers).
    // nodes cannot have siblings, remember
    // RETURN A NODE
    Node* meld(Node* left, Node* right)
    {
        // GO OVER THIS
        // adding to an empty pq, root must be set
        if (left == nullptr)
        {
            root = right;
            return right;
        }
        // right is higher priority, priority(left) < priority(right) true
        else if (this->compare(left->getElt(), right->getElt()))
        {
            left->sibling = right->child;
            right->child = left;
            left->parent = right;
            root = right;
            return right;
        }
        // left is higher priority, priority(left) > priority(right)
        else if (this->compare(right->getElt(), left->getElt()))
        {
            right->sibling = left->child;
            left->child = right;
            right->parent = left;
            root = left;
            return left;
        }
        // eh doesnt matter, theyre equal, do first branch by default, if needed can optimize this
        else
        {
            left->sibling = right->child;
            right->child = left;
            left->parent = right;
            root = right;
            return right;
        }
    }

};


#endif // PAIRINGPQ_H
