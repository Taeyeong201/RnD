#pragma once
class AndroidKeyListen
{
public:
	AndroidKeyListen();
	~AndroidKeyListen();

	int getKeyCode();
	int is_state();
	int is_mouse();

private:
	int keyCode;
	int is_state;

	int x, y;
};

