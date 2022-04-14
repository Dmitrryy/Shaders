

#include <Sample.hpp>


#ifndef ROOT_SHADERS_PATH
#define ROOT_SHADERS_PATH "Shaders\\"
#endif


int main(int argc, char *argv[])
{
    std::string root = ROOT_SHADERS_PATH;
    if (argc > 1) 
        root = argv[1];

    sh::Sample sample(1000, 800, root);
    sample.run();
    return 0;
}