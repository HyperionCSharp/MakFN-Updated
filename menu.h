#pragma once

#include <Windows.h>
#include "settings.h"
#include <stdio.h>
#include <string.h>
#include "dx_renderer.h"
#include <strsafe.h>

bool show_menu = true;
int current;

/*
wchar_t test1[] = { ' ', ' ', ' ', 'R', 'M', 'B', 0 };
wchar_t test2[] = { ' ', ' ', ' ', 'L', 'M', 'B', 0 };
wchar_t test3[] = { ' ',' ', 'L', 'A', 'L', 'T', 0 };
wchar_t test4[] = { ' ','L', 'C', 'T', 'R', 'L', 0 };
wchar_t test5[] = { 'L','S', 'H', 'I', 'F', 'T', 0 };


wchar_t test6[] = { ' ', ' ', 'H', 'e', 'a', 'd', 0};
wchar_t test7[] = { ' ', ' ', 'N', 'e', 'c', 'k', 0 };
wchar_t test8[] = { ' ','C', 'h', 'e', 's', 't', 0 };
wchar_t test9[] = { 'P','e', 'l', 'v', 'i', 's', 0 };
wchar_t test0[] = { 'R','a', 'n', 'd', 'o', 'm', 0 };
*/

int index1 = 1;
int index2 = 0;

int shotgun_index = 0;
int ar_index = 1;
int smg_index = 1;
int other_index = 0;

int thekey = VK_RBUTTON; //default?
DWORD thehitbox = 68;


bool find6(const char* name)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	auto snapshot = LI_FN(CreateToolhelp32Snapshot).forwarded_safe_cached()(TH32CS_SNAPPROCESS, NULL);


	if (LI_FN(Process32First).forwarded_safe_cached()(snapshot, &entry) == TRUE)
	{
		while (LI_FN(Process32Next).forwarded_safe_cached()(snapshot, &entry) == TRUE)
		{
			if (strcmp((const char*)entry.szExeFile, name))
			{
				return true;
			}
		}
	}

	LI_FN(CloseHandle).forwarded_safe_cached()(snapshot);
	return false;

}



void keySystem() {

		//hide_thread(LI_FN(GetCurrentThread).get()());
		//owWindow_Spoofed(LI_FN(GetConsoleWindow).get()(), SW_HIDE);
		while (true) {
			
			if (nig) ExitThread(-1);
			if (GetAsyncKeyState_Spoofed(VK_INSERT) & 1)
			{
				show_menu = !show_menu;
				LI_FN(Sleep).get()(200);
			}

		
			if (!find6(skCrypt("FortniteClient-Win64-Shipping.exe"))) { *(uintptr_t*)(0) = 0; }
		
		


			if (settings::box)
			{
				settings::corner_box = false;
				settings::threed_box = false;
			}

			else if (settings::corner_box)
			{
				settings::box = false;
				settings::threed_box = false;
			}

			else if (settings::threed_box)
			{
				settings::corner_box = false;
				settings::box = false;
			}


			if (other_index == 0) {
				thehitbox = 68;
				//	current2 = test6;
			}
			else if (other_index == 1) {
				thehitbox = 67; //put neck hitbox id
			//	current2 = test7;
			}
			else if (other_index == 2) {
				thehitbox = 7; //put chest hitbox id
				//current2 = test8;
			}
			else if (other_index == 3) { //just do alt first my ocd :pray: what tf means ocd just do alt first
				thehitbox = 2; // pelvis hitbox
				//current2 = test9;
			}
			else if (other_index == 4) { //just do alt first my ocd :pray: what tf means ocd just do alt first

			//	thehitbox = 0;
				srand((unsigned)time(0));

				int ran = 1 + (rand() % 15);

				switch (ran) {
				case 1:
					thehitbox = 98;
					break;
				case 2:
					thehitbox = 67;
					break;
				case 3:
					thehitbox = 36;
					break;
				case 4:
					thehitbox = 2;
					break;
				case 5:
					thehitbox = 62;
					break;
				case 6:
					thehitbox = 11;
					break;
				case 7:
					thehitbox = 73;
					break;
				case 8:
					thehitbox = 10;
					break;
				}
				//current2 = test0;
			}



			if (smg_index == 0) {
				thehitbox = 68;
				//	current2 = test6;
			}
			else if (smg_index == 1) {
				thehitbox = 67; //put neck hitbox id
			//	current2 = test7;
			}
			else if (smg_index == 2) {
				thehitbox = 7; //put chest hitbox id
				//current2 = test8;
			}
			else if (smg_index == 3) { //just do alt first my ocd :pray: what tf means ocd just do alt first
				thehitbox = 2; // pelvis hitbox
				//current2 = test9;
			}
			else if (smg_index == 4) { //just do alt first my ocd :pray: what tf means ocd just do alt first

			//	thehitbox = 0;
				srand((unsigned)time(0));

				int ran = 1 + (rand() % 15);

				switch (ran) {
				case 1:
					thehitbox = 98;
					break;
				case 2:
					thehitbox = 67;
					break;
				case 3:
					thehitbox = 36;
					break;
				case 4:
					thehitbox = 2;
					break;
				case 5:
					thehitbox = 62;
					break;
				case 6:
					thehitbox = 11;
					break;
				case 7:
					thehitbox = 73;
					break;
				case 8:
					thehitbox = 10;
					break;
				}
				//current2 = test0;
			}


			if (ar_index == 0) {
				thehitbox = 68;
				//	current2 = test6;
			}
			else if (ar_index == 1) {
				thehitbox = 67; //put neck hitbox id
			//	current2 = test7;
			}
			else if (ar_index == 2) {
				thehitbox = 7; //put chest hitbox id
				//current2 = test8;
			}
			else if (ar_index == 3) { //just do alt first my ocd :pray: what tf means ocd just do alt first
				thehitbox = 2; // pelvis hitbox
				//current2 = test9;
			}
			else if (ar_index == 4) { //just do alt first my ocd :pray: what tf means ocd just do alt first

			//	thehitbox = 0;
				srand((unsigned)time(0));

				int ran = 1 + (rand() % 15);

				switch (ran) {
				case 1:
					thehitbox = 98;
					break;
				case 2:
					thehitbox = 67;
					break;
				case 3:
					thehitbox = 36;
					break;
				case 4:
					thehitbox = 2;
					break;
				case 5:
					thehitbox = 62;
					break;
				case 6:
					thehitbox = 11;
					break;
				case 7:
					thehitbox = 73;
					break;
				case 8:
					thehitbox = 10;
					break;
				}
				//current2 = test0;
			}


			if (shotgun_index == 0) {
				thehitbox = 68;
				//	current2 = test6;
			}
			else if (shotgun_index == 1) {
				thehitbox = 67; //put neck hitbox id
			//	current2 = test7;
			}
			else if (shotgun_index == 2) {
				thehitbox = 7; //put chest hitbox id
				//current2 = test8;
			}
			else if (shotgun_index == 3) { //just do alt first my ocd :pray: what tf means ocd just do alt first
				thehitbox = 2; // pelvis hitbox
				//current2 = test9;
			}
			else if (shotgun_index == 4) { //just do alt first my ocd :pray: what tf means ocd just do alt first

			//	thehitbox = 0;
				srand((unsigned)time(0));

				int ran = 1 + (rand() % 15);

				switch (ran) {
				case 1:
					thehitbox = 98;
					break;
				case 2:
					thehitbox = 67;
					break;
				case 3:
					thehitbox = 36;
					break;
				case 4:
					thehitbox = 2;
					break;
				case 5:
					thehitbox = 62;
					break;
				case 6:
					thehitbox = 11;
					break;
				case 7:
					thehitbox = 73;
					break;
				case 8:
					thehitbox = 10;
					break;
				}
				//current2 = test0;
			}



			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}

	}


