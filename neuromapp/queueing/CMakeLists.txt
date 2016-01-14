add_library (queueing
	     container.cpp
	     main.cpp
	     pool.cpp
	     thread.cpp)


install (TARGETS queueing DESTINATION lib)
install (FILES container.h pool.h queueing.h thread.h spinlock_queue.h DESTINATION include)
