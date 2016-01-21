add_library (queueing
	     container.cpp
	     main.cpp)


install (TARGETS queueing DESTINATION lib)
install (FILES container.h pool.h queueing.h spinlock_apple.h spinlock_queue.h thread.h DESTINATION include)
