#include "../Utils/utils.hpp"

static const char alphanum[] = "0123456789" "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int stringLengthh = sizeof(alphanum) - 1;

inline bool hide_thread(HANDLE thread);

char genRandomn()
{
    return alphanum[LI_FN(rand).forwarded_safe_cached()() % stringLengthh];
}

std::string title_randomizer()
{
    std::string string = skCrypt("1234567890QWERTYUIOPASDFGHJKLZXCVMBNMqwertyuiopasdfghjklzxcvmbnm!@#$%^&*()_+{}:'><?~`").decrypt();
    std::string varstring;
    int title;
    while (varstring.size() != 115)
    {
        title = ((LI_FN(rand).forwarded_safe_cached()() % (string.size() + 1)));
        varstring += string.substr(title, 1);
    }
    return varstring + " ";
}

void random_console_title()
{
   LI_FN(SetConsoleTitleA).forwarded_safe_cached()(title_randomizer().c_str());
}

void printf_slow(char* strprint) 
{
    while (*strprint) {
        std::cout << *strprint++;
        LI_FN(Sleep).forwarded_safe_cached()(75);
    }
}

void clear_console()
{
    COORD topLeft = { 0, 0 };
    HANDLE console = LI_FN(GetStdHandle).forwarded_safe_cached()(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    LI_FN(GetConsoleScreenBufferInfo).forwarded_safe_cached()(console, &screen);
    LI_FN(FillConsoleOutputCharacterA).forwarded_safe_cached()(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    LI_FN(FillConsoleOutputAttribute).forwarded_safe_cached()(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    LI_FN(SetConsoleCursorPosition).forwarded_safe_cached()(console, topLeft);
}

void hide_console() 
{
    LI_FN(ShowWindow).safe_cached()(LI_FN(GetConsoleWindow).safe_cached()(), SW_HIDE);
}

void random_exe()
{
    LI_FN(srand).forwarded_safe_cached()(LI_FN(time).forwarded_safe_cached()(0));
    std::string Str;
    for (UINT i = 0; i < 10; i++)
    {
        Str += genRandomn();

    }

    std::string rename = Str + skCrypt(".exe").decrypt();

    char filename[MAX_PATH];
    DWORD size = LI_FN(GetModuleFileNameA).forwarded_safe_cached()(NULL, filename, MAX_PATH);
    if (size) std::filesystem::rename(filename, rename);
}



void utils_loop()
{
    random_exe();
    hide_thread(LI_FN(GetCurrentThread).get());
        while (true)
        {
            random_console_title();

        
                if (rape)
                {
                    ExitThread(-1);
                }

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    
}
