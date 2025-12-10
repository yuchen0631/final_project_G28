#ifndef DIALOG_MANAGER_H_INCLUDED
#define DIALOG_MANAGER_H_INCLUDED

#include <string>

class DialogManager
{
public:
    bool active = false;
    std::string text;

    void show(const std::string& t);
    void close();
    void draw();
};

#endif
