// RingBuffer.h: interface for the RingBuffer class.
// A fast efficient queue (FIFO)
// Wraping around inside preset max buffer size
//
//////////////////////////////////////////////////////////////////////

#ifndef RingBuffer_h
#define RingBuffer_h

//#include <assert.h>
#include <algorithm>
#include "myassert.h"
#include "useful.h"

template<class T> class RingBuffer
{
protected:
	T *buf;
  int buf_size, cur_size, start;
  bool autoGrow;

  int end() { return (start + cur_size) % buf_size; }
public:
	RingBuffer() { buf = NULL; buf_size=0; autoGrow = false; clear(); }
	RingBuffer(int buf_size_) { buf = NULL; buf_size=0; autoGrow = false; resize_clear(buf_size_); }
  virtual ~RingBuffer() { if(buf) delete buf; }

	void resize_clear(int buf_size_ = 0)
	{
    if(buf) delete buf;
		buf_size = std::max(buf_size_, 0);
		buf = new T[buf_size];
		clear();
	}

	int size() { return cur_size; }
  int capacity() { return buf_size; }
	int available() { return buf_size - cur_size; }
  void setAutoGrow(bool value) { autoGrow = value; }
  int nextGrowSize() { return std::max(2, cur_size * 2); }

  T &operator[](int x) {
	    myassert(x >= 0 && x < cur_size);
	    return buf[(start+x) % buf_size];
  }

  //Empty all the element from the buffer
	void clear() {
		cur_size = start = 0;
	}

  //ignore - read past num elements without returning them
	int ignore(int num) {
    num = bound(num, 0, cur_size);
    start = (start + num) % buf_size;
    cur_size -= num;
		return num;
	}

  //put item on the back of the queue
  //returns false in no room on the queue
	bool put(const T &item)
	{
		if(cur_size == buf_size) {
      if(autoGrow) resize(nextGrowSize());
      else return false;
    }
		buf[end()] = item;
		cur_size++;
		return true;
	}

  //put a number of items on the queue
	//returns the number of items successfully put in
	int put(const T *items, int num)
	{
		if(num <= 0) return 0;
    if(num > available()) {
      if(autoGrow) resize(nextPowerOf2(cur_size+num));
      else num = available();
    }
		if(end() + num <= buf_size) { //do in one go
			//memcpy((void *)(buf+end()), (void *)items, num * sizeof(T));
      std::copy(items, items+num, buf+end());
		} else { //split into two bits
			int chunk1 = buf_size - end();
			//int chunk2 = num - chunk1;
			//memcpy((void *)(buf+end()), (void *)items, chunk1 * sizeof(T));
      std::copy(items, items+chunk1, buf+end());
			//memcpy((void *)buf, (void *)(items+chunk1), chunk2 * sizeof(T));
      std::copy(items+chunk1, items+num, buf);
		}
		cur_size += num;
		return num;
	}

	//always put item, may lose the oldest item from the buffer
	void force_put(const T &item)
	{
		if(autoGrow || cur_size < buf_size) put(item);
		else {
			buf[end()] = item;
      start = (start+1) % buf_size;
		}
	}

	//always put items, may lose some older items from the buffer
	void force_put(const T *items, int num)
	{
    if(autoGrow || num < available()) put(items, num);
		if(num > buf_size) { //just fill the whole buffer with the last buf_size elements
			//memcpy((void *)buf, (void *)(items+num-buf_size), buf_size * sizeof(T));
      std::copy(items+num-buf_size, items+num, buf);
			cur_size = buf_size;
			start = 0;
		} else {
			int overlap = num - available();
			if(overlap > 0) ignore(overlap);
			put(items, num);
		}
	}

  //retrieves 1 item without removing it
	bool peek(T *item)
	{
		if(cur_size < 1) return false;
		*item = buf[start];
		return true;
	}

	//returns the number of items returned, max of num items
	//peek doesn't remove the item from the list like get does
	int peek(T *items, int num)
	{
    num = bound(num, 0, cur_size);
		if(start + num < buf_size) { //do in one go
			//memcpy((void *)items, (void *)(buf+start), num * sizeof(T));
      std::copy(buf+start, buf+start+num, items);
		} else { //split into two bits
			int chunk1 = buf_size - start;
			int chunk2 = num - chunk1;
			//memcpy((void *)items, (void *)(buf+start), chunk1 * sizeof(T));
      std::copy(buf+start, buf+buf_size, items);
			//memcpy((void *)(items + chunk1), (void *)buf, chunk2 * sizeof(T));
      std::copy(buf, buf+chunk2, items+chunk1);
		}
		return num;
	}

	bool get(T *item)
	{
		if(cur_size < 1) return false;
		*item = buf[start];
    start = (start+1) % buf_size;
		cur_size--;
		return true;
	}

	//returns the number of items returned, max of num items
	int get(T *items, int num)
	{
    num = peek(items, num);
    start = (start+num) % buf_size;
    cur_size -= num;
    return num;
		/*if(num <= 0) return 0;
		num = min(num, cur_size);
		if(start + num < buf_size) { //do in one go
			memcpy((void *)items, (void *)(buf+start), num * sizeof(T));
			start += num;
		} else { //split into two bits
			int chunk1 = buf_size - start;
			int chunk2 = num - chunk1;
			memcpy((void *)items, (void *)(buf+start), chunk1 * sizeof(T));
			memcpy((void *)(items + chunk1), (void *)buf, chunk2 * sizeof(T));
			start = chunk2;
		}
		cur_size -= num;
		return num;*/
	}

  //keeps the current data intacted
  //Note: if resizing smaller, items will be lost from the front
  void resize(int new_size)
  {
    if(cur_size == 0) { resize_clear(new_size); return; }
    if(new_size < buf_size) ignore(buf_size - new_size);
    T *new_buf = new T[new_size];
    if(start+cur_size > buf_size) { //do in one go
      std::copy(buf+start, buf+start+cur_size, new_buf);
    } else {
      int chunk1 = buf_size - start;
      int chunk2 = cur_size - chunk1;
      std::copy(buf+start, buf+buf_size, new_buf);
      std::copy(buf, buf+chunk2, new_buf+chunk1);
    }
    start = 0;
    if(buf) delete buf;
    buf = new_buf;
    buf_size = new_size;
  }

/*
	void resize(int new_size)
	{
		new_size = max(new_size, 1); //minimum size = 1
		//CString str;
		if(new_size == buf_size) return; //already correct size
		if(cur_size == 0) { //no items to worry about
			clear();
			buf = (T *)realloc(buf, new_size * sizeof(T));
		} else if(new_size < buf_size) { //new buffer is smaller
			if(start + cur_size >= new_size) { //unwrap some
				int diff = buf_size - new_size;
				//str.Format("diff = %d", diff);
				//MessageBox(NULL, str, "Caption", MB_OK);
				int overlap = diff - available();
				if(overlap > 0) ignore(overlap); // lose some items to make it fit
				if(start >= new_size) {
					diff = buf_size - start;
					start = 0;
					memmove((void *)(buf+diff), (void *)buf, end()*sizeof(T));
					memcpy((void *)buf, (void *)(buf+buf_size-diff), diff*sizeof(T));
					//end += diff; if(end == new_size) end = 0;
				} else {
					memcpy((void *)buf, (void *)(buf+new_size), (end()-new_size)*sizeof(T));
					//end -= new_size;
				}
			}
			buf = (T *)realloc(buf, new_size * sizeof(T));
		} else { //new buffer is bigger
			buf = (T *)realloc(buf, new_size * sizeof(T));
			if(start + cur_size >= buf_size) { //unwrap some
				int diff = new_size - buf_size;
				if(diff <= end()) {
					memcpy((void *)(buf+buf_size), (void *)buf, diff*sizeof(T));
					memmove((void *)buf, (void *)(buf+diff), (end()-diff)*sizeof(T));
					//end -= diff;
				} else {
					memcpy((void *)(buf+buf_size), (void *)buf, end()*sizeof(T));
					//end = start + cur_size;
				}
			}
		}
		buf_size = new_size;
	}
*/
};


#endif
