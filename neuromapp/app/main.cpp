#include "app/miniapp.h" // the list of the miniapp API
#include "app/driver.h"


#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>


char *convert(const std::string & s) {
    return strdup(s.c_str());
}

int main(int argc, char * const argv[]){
     mapp::driver d;
     d.insert("hello",hello_execute);
     d.insert("kernel",coreneuron10_kernel_execute);
     d.insert("solver",coreneuron10_solver_execute);
     d.insert("fullComputationStep",coreneuron10_fullComputationStep_execute);
     std::cout << "Welcome to NeuroMapp! Please enter "
               << "the name of the miniapp you wish to execute "
               << "followed by any arguments you wish to pass to it."
               << std::endl
               << "Please avoid preceding and trailing whitespace!"
               << std::endl
               << "To finish type quit"
               << std::endl
               << ">? ";

     while(1) {
         std::string command;
         std::getline (std::cin, command);
         // I need to split the string into an array of strings to pass it
         // in an argv style
         std::vector<std::string> command_v;
         command_v.push_back(std::string(argv[0]));
         size_t end_of_next_word = 0;
         size_t beginning_of_next_word;
         while(1) {
             beginning_of_next_word = command.find_first_not_of(" ", end_of_next_word);
             if( beginning_of_next_word != std::string::npos) {
                 end_of_next_word = command.find_first_of(" ", beginning_of_next_word);
                 if ( end_of_next_word != std::string::npos) {
                     command_v.push_back(command.substr(beginning_of_next_word, end_of_next_word - beginning_of_next_word));
                 } else { // we are at the very end of the string
                          // and there is no trailing whitespace
                          // but we still want to keep the last argument
                     command_v.push_back(command.substr(
                                beginning_of_next_word, command.length() - beginning_of_next_word));
                 }
             } else {
                 break;
             }
         }

         if( command_v[1].compare("quit") == 0 ) break;

         std::vector<char*> command_vc;
         std::transform(command_v.begin(), command_v.end(), std::back_inserter(command_vc), convert);

         try {
             d.execute(command_v.size(), &command_vc[0] );
         } catch(std::exception & e) {
             std::cerr << "caught exception: " << e.what() << "\n";
	 }
         std::cout << std::endl << ">? ";
    }
     return 0;
}


