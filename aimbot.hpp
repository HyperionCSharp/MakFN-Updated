#pragma once
#include <Windows.h>
#include <iostream>
#include <cmath>
#include "settings.h"
#include "driver.h"
#include "mouse_interface.h"
#include "dx9.h"

#include <random>

#include <strsafe.h>



static int shoot_downed = 0;
double numberx;
double numbery;
void random_aim()
{
	while (true)
	{

		std::random_device rdx;
		std::default_random_engine generatorx(rdx()); // rd() provides a random seed
		std::uniform_real_distribution<double> distributionx(-3.03, 3.06);
		numberx = distributionx(generatorx);

		std::random_device rdy;
		std::default_random_engine generatory(rdy()); // rd() provides a random seed
		std::uniform_real_distribution<double> distributiony(-3.04, 3.01);

		numbery = distributiony(generatory);

		if (nig) ExitThread(-1);

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}


void aimbot(float xscreen, float yscreen)
{
	float screen_center_x = (Width / 2);
	float screen_center_y = (Height / 2);
	float target_x = 0;
	float target_y = 0;


	if (xscreen != 0)
	{
		if (xscreen > screen_center_x)
		{
			target_x = -(screen_center_x - xscreen);
			target_x /= 1.0f;
			if (target_x + screen_center_x > screen_center_x * 2) target_x = 0;
		}

		if (xscreen < screen_center_x)
		{
			target_x = xscreen - screen_center_x;
			target_x /= 1.0f;
			if (target_x + screen_center_x < 0) target_x = 0;
		}
	}

	if (yscreen != 0)
	{
		if (yscreen > screen_center_y)
		{
			target_y = -(screen_center_y - yscreen);
			target_y /= 1.0f;
			if (target_y + screen_center_y > screen_center_y * 2) target_y = 0;
		}

		if (yscreen < screen_center_y)
		{
			target_y = yscreen - screen_center_y;
			target_y /= 1.0f;
			if (target_y + screen_center_y < 0) target_y = 0;
		}
	}
	//i removed these things idk if needed no not need

	int x = floor(target_x / settings::aimspeed);
	int y = floor(target_y / settings::aimspeed);





	Hijacked_Move((x + numberx) , (y - numbery));
}