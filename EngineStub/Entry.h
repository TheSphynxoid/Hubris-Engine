#ifdef STUB_MAIN
#else
#define STUB_MAIN
#include <iostream>
#include <Engine.h>
extern int run(int argc, char** argv);

int main(int argc, char** argv){
    std::cout << "Sphynx Engine has started." << std::endl;
    return run(argc, argv);
}
#endif 
