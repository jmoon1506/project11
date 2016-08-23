#include <stdio.h>
#include "app.h"
//#include "yaml-cpp/yaml.h"

int main(int args, char* argc[])
{
    //YAML::Node test = YAML::LoadFile("data/test1.yaml");
    //YAML::Node node  = YAML::Load("[1, 2, 3]");
    //YAML::Node config = YAML::LoadFile("data/sample.yaml");
    App app;
    app.Init();
    app.ProgramLoop();
    app.Close();
    //printf("close main\n");
    return 0;
}