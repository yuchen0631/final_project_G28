#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <string>
#include <vector>
#include "monsters/Monster.h"

class Level {
public:
    Level() { init(); }

    void init();                 
    void load_level(int lvl);   
    void update();             
    void draw();              

    std::string get_background() const { return background_path; }

private:
    int level;
    std::string background_path; 
};

#endif
