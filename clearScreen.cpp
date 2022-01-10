#include "clearScreen.hpp"
#ifdef WIN_32
#else
#include <unistd.h>
#include <term.h>

void clearScreen(){
    if(!cur_term){ //what cur_term does is it holds the terminal state
        int result;
        setupterm(NULL, STDOUT_FILENO, &result);
        if(result<=0){
            return;
        }
    }
    putp(tigetstr((char *)"clear"));
}
#endif
