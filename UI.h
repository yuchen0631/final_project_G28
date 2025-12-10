#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include <allegro5/bitmap.h>
#include <vector>
#include <tuple>
#include "./shapes/Point.h"

class UI
{
public:
	UI() {}
	void init();
	void update();
	void draw();
private:
	ALLEGRO_BITMAP *love;
};

#endif
