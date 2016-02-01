/*
 * lock.h
 *
 */

#ifndef LOCK_H_
#define LOCK_H_

#include <omp.h>


class MyLock {
public:
	MyLock() {}
	virtual ~MyLock() {}
	virtual void lock() = 0;
	virtual void unlock() = 0;
};



class MyDummyLock : public MyLock {
public:
	MyDummyLock() {}
	virtual ~MyDummyLock() {}
	inline void lock() {}
	inline void unlock() {}
};



class MyOMPLock : public MyLock {
private:
	omp_lock_t	_lock;
public:
	MyOMPLock() { omp_init_lock(&_lock); }
	virtual ~MyOMPLock() { omp_destroy_lock(&_lock); }
	inline void lock() { omp_set_lock(&_lock); }
	inline void unlock() { omp_unset_lock(&_lock); }
};



#if 0
#include <mutex>
class MyCppLock : public MyLock {
private:
	std::mutex _lock;
public:
	MyCppLock() : _lock() {}
	virtual ~MyCppLock() {}
	inline void lock() { _lock.lock(); }
	inline void unlock() { _lock.unlock(); }
};
#endif

#endif
