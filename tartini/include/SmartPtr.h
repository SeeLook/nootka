#ifndef SMART_PTR_H
#define SMART_PTR_H

#include "safe_bool.h"

/** Smart Pointer class
 *  This can be used in place of normal pointers.
 *  It performs reference counting, and deletes objects automatically when done
 *  Note: One map per type is used to store the number of references
 *
 *  Authors: Philip McLeod, Koen Herms
 *  Created: 13 Feb 2006
 *
 *  Example Use:
 *
 *  SmartPtr<TestClass> p1 = new TestClass();
 *  SmartPtr<TestClass> p2 = p1;
 *  p1->x = 4; //or (*p1).x = 4;
 *  p1 = NULL; //Here the object is not deleted, because it still has a reference
 *  cout << p2->x; //output: 4
 *  p2 = NULL; //Here the object is deleted
 *
 *  Note: You don't normally need to set to NULL, as they are destroyed (if not needed) when they go out of scope
 */ 

#include <map>

template <class T>
class SmartPtr {
    SPTR_SAFE_BOOL_DEFINE;

 public:
    typedef T                                  value_type;
    typedef std::map<const T*, unsigned int>   map_type;
    typedef T*                                 pointer_type;
    typedef unsigned int                       map_data_type;

 private:
    T *_ptr; //Pointer to the actual object, or NULL
    map_data_type *_ref_counter; //Pointer to the reference counter in the map

    static map_type _smartPtrs;

 public:
    /** Default constructor
	Speed: Constant time
     */
    SmartPtr() :  _ptr(NULL), _ref_counter(NULL) { }
    /** Construct from pointer
	Speed: Logarithmic to the size of the map
     */
    SmartPtr(T *ptr) : _ptr(ptr) {
	if(_ptr) _ref_counter = &(++(_smartPtrs[_ptr]));
	else _ref_counter = NULL;
    }
    /** Construct from SmartPtr
	Speed: Constant time
     */
    SmartPtr(SmartPtr<T> const &r) : _ptr(r.ptr()) {
	if(_ptr) ++(*(_ref_counter = r.getRefCounter())); //no need to search the map here
	else _ref_counter = NULL;
    }
    /** Destructor
	Speed: Logarithmic if last reference, else Constant time
     */
    ~SmartPtr() {
	if(_ptr) {
	    if(--(*_ref_counter) == 0) {
		_smartPtrs.erase(_ptr); //we need to search the map here to erase the element
		delete _ptr;
		_ptr = NULL;
		_ref_counter = NULL;
	    }
	}
    }
    /** Assignment of a smart pointer (Shallow copy)
	Note: Creates the new pointer before deleting the old pointer
	Speed: Constant time, unless previous value was a last reference, then logarithmic
    */
    SmartPtr<T> &operator = (SmartPtr<T> const &r) {
	if(_ptr != r.ptr()) {
	    T *old_ptr = _ptr;
	    map_data_type *old_ref_counter = _ref_counter;

	    if(r.ptr()) {
		_ptr = r.ptr();
		++(*(_ref_counter = r.getRefCounter()));
	    } else {
		_ptr = NULL;
		_ref_counter = NULL;
	    }
	    if(old_ptr) {
		if(--(*old_ref_counter) == 0) {
		    _smartPtrs.erase(old_ptr);
		    delete old_ptr;
		}
	    }
	}
	return *this;
    }
    /** Assignment of a raw pointer (Shallow copy)
	Speed: Logarithmic to the size of the map
     */
    SmartPtr<T> &operator = (T *p) {
	if(_ptr != p) {
	    T *old_ptr = _ptr;
	    map_data_type *old_ref_counter = _ref_counter;

	    if(p) {
		_ref_counter = &(++(_smartPtrs[p])); //this is the magic!
		_ptr = p;
	    } else {
		_ptr = NULL;
		_ref_counter = NULL;
	    }
	    if(old_ptr) {
		if(--(*old_ref_counter) == 0) {
		    _smartPtrs.erase(old_ptr);
		    delete old_ptr;
		}
	    }
	}	
	return *this;
     }
    
    /** Return a (deep) copy of this object
	Speed: Logarithmic
     */
    SmartPtr<T> copy() {
	T* t = new T();
	*t = *_ptr;
	return SmartPtr<T>(t);
    }

    /** boolean (cast) operator. e.g. if(smartPtr) ... */
    operator safe_bool () const { return (_ptr != (T*)NULL) ? SPTR_SAFE_BOOL_TRUE : false; }
    /** boolean not operator. e.g. if(!smartPtr) ... */
    bool operator !() const { return (_ptr != (T*)NULL) ? false : true; }
    /** Dereferencing the pointer */
    T &operator * () const { return *_ptr; }
    /** Return the raw/dumb pointer. */
    T *operator -> () const { return _ptr; }
    /** Return the raw/dumb pointer. */
    T *ptr() const { return _ptr; }
    //operator T *() const { return _ptr; } //People should explicitly get the pointer using ptr() 

   /** Do they point to the same object. (shallow equality) */
   bool operator == (T const *p) const { return _ptr == p; }
 
   /** Do the smart pointers not point to the same object. (shallow inequality) */
   bool operator != (T const *p) const { return _ptr != p; }
 
   /** Do they point to the same object. (shallow equality) */
    bool operator == (SmartPtr<T> const &r) const { return _ptr == r.ptr(); }

    /** Do the smart pointers not point to the same object. (shallow inequality) */
    bool operator != (SmartPtr<T> const &r) const { return _ptr != r.ptr(); }
    bool operator <  (SmartPtr<T> const &r) const { return _ptr <  r.ptr(); }
    bool operator >  (SmartPtr<T> const &r) const { return _ptr >  r.ptr(); }
    bool operator <= (SmartPtr<T> const &r) const { return _ptr <= r.ptr(); }
    bool operator >= (SmartPtr<T> const &r) const { return _ptr >= r.ptr(); }

    /* Returns the number of references to this object
       Speed: Constant time
     */
    unsigned int getNumRef() { return (_ptr) ? *_ref_counter : 0; }
    /** This allows read access to the map, to see what memory is being used
	Mainly for debugging purposes
     */
    static const map_type &getMap() { return _smartPtrs; }

protected:
    unsigned int *getRefCounter() const { return _ref_counter; }
};


/** Is p and r pointing to the same object */
//: Comparison of pointer with smart-pointer (cannot be a member function)
template <class T>
inline bool operator == (T const* p, SmartPtr<T> const& r)
{
    return r.ptr() == p;
}

/** Is p and r pointing to the same object */
template <class T>
inline bool operator!= (T const* p, SmartPtr<T> const& r)
{
    return r.ptr() != p;
}

/** This inforces instatiation of the static variable _smartPtrs */
template <class T> std::map<const T*, unsigned int> SmartPtr<T>::_smartPtrs = std::map<const T*, unsigned int>();

/** Test whether a pointer has been turn into a smart pointer
    Speed: Logarithmic
 */
template <class T>
inline bool isSmart(const T* ptr) {
    return (ptr) ? SmartPtr<T>::getMap().count(ptr) : false;
}

#endif
