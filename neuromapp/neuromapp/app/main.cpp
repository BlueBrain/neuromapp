#include "app/miniapp.h" // the list of the miniapp API
#include "app/driver.h"

int main(int argc, char * argv[]){
     mapp::driver d;
     d.insert("hello",hello_execute);
     d.insert("cnkernel",cnkernel_execute);
     d.execute(argc, argv);
     return 0;
}
