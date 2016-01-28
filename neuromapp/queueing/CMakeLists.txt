add_library (queueing
	     queue.cpp
	     main.cpp)


install (TARGETS queueing DESTINATION lib)
install (FILES queue.h pool.h queueing.h spinlock_apple.h spinlock_queue.h thread.h DESTINATION include)
