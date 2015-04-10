#ifndef PIN_THREAD_H
#define PIN_THREAD_H

#include <vector>

namespace llc {
// (currently only Linux 2.6 API back-end)

void pin_thread();
void pin_thread(int cpu);
int thread_cpu();

typedef std::vector<int> affinity_set_t;
affinity_set_t thread_affinity();

} // namespace llc

#endif // ndef PIN_THREAD_H
