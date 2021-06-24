#ifndef BINARYPQ_H
#define BINARYPQ_H


#include <algorithm>
#include "Eecs281PQ.h"

// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

// A specialized version of the 'heap' ADT implemented as a binary heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Description: Construct an empty heap with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
            
            // push back a dummy value
            TYPE initial;
            data.push_back(initial);
            
    } // BinaryPQ


    // Description: Construct a heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
    BaseClass{ comp } {
        // TODO: Implement this function.
        TYPE dummy;
        data.push_back(dummy); // NOT 100% on this
        while (start != end)
        {
            data.push_back(*start);
            start++;
        }
        updatePriorities();
    } // BinaryPQ


    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automaticslly.
    virtual ~BinaryPQ() {
    } // ~BinaryPQ()


    // Description: Assumes that all elements inside the heap are out of order and
    //              'rebuilds' the heap by fixing the heap invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        
        // GO OVER THIS
        for (size_t i = size(); i >= 1; i--)
        {
            fixDown(static_cast<int>(i));
        }
    } // updatePriorities()


    // Description: Add a new element to the heap.
    // Runtime: O(log(n))
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE & val) {
        data.push_back(val);
        fixUp(static_cast<int>(size())); // pushed to back, last index
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: O(log(n))
    virtual void pop() {
        std::swap(data[1], data[size()]); // HAVE TO GO OVER THIS
        data.pop_back(); // HAVE TO GO OVER THIS
        fixDown(1);
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE & top() const {
        // TODO: Implement this function.
        return data[1];
    } // top()


    // Description: Get the number of elements in the heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        // TODO: Implement this function.  Might be very simple,
        return data.size() - 1; // HAVE TO CHECK THIS LATER
    } // size()


    // Description: Return true if the heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        // TODO: Implement this function.  Might be very simple,
        if (data.size() == 1)
        {
            return true;
        }
        return false;
    } // empty()


private:
    // Note: This vector *must* be used your heap implementation.
    // data has a dummy value of default initialization at index 0
    std::vector<TYPE> data;
    

    // TODO: Add any additional member functions or data you require here.
    // For instance, you might add fixUp() and fixDown().
    
    void fixUp(int index)
    {
        // do i need to do anything with comparator, COMPARATOR IS LESS SO i/2 > i IS CONDITION
        // maybe swap i and i/2, big tests to tell
        while (index > 1 && this->compare(data[index/2], data[index]))
        {
            std::swap(data[index], data[index/2]);
            index /= 2;
        }
    }
    
    void fixDown(int index)
    {
        while (2 * index <= static_cast<int>(size()))
        {
        // do i need to do anything else with comparator? i/2 < i IS CONDITION, COMPARATOR IS LESS
            // <= maybe
            int k = 2*index;
            if (k < static_cast<int>(size()) && this->compare(data[k], data[k+1]))
                ++k;
            
            if (this->compare(data[k], data[index]))
                break;
            
            std::swap(data[index], data[k]);
            index = k;
        }
    }

}; // BinaryPQ


#endif // BINARYPQ_H
