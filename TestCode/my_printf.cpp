#include <cstdarg>
#include <iostream>
using namespace ::std;

int cprintf(const char *fmt, ...);

int main(void)
{
    int num = 42;
    char str[] = "String";
    cprintf("%d,%s", num, str);
}

int cprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char ch;
    int cnt;

    while (ch = *(fmt++))
    {
        cnt++;
        if (ch == '%')
        {
            ch = *(fmt++);
            switch (ch)
            {
            case 'i':
            case 'd':
            {
                int num = va_arg(args, int);
                cout << num;
                break;
            }
            case 'u':
            {
                unsigned uns = va_arg(args, unsigned);
                cout << uns;
                break;
            }
            case 's':
            {
                char *str = va_arg(args, char *);
                cout << str;
                break;
            }
            }
        }
        else
        {
            cout << ch;
        }
    }
    cout << endl;
    va_end(args);
    return cnt;
}