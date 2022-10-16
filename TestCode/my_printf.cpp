#include <cstdarg>
#include <iostream>
#include <cstdio>
using namespace ::std;

int cprintf(const char *fmt, ...);
void trans(int num, int base, int offset, char *result);
void dec2oct(int num, char *result);

int main(void)
{
    int num = 42;
    char str[] = "String";
    cprintf("%o,%s", num, str);
    char *str2 = (char *)malloc(sizeof(char));
    dec2oct(10, str2);
    printf("%s", str2);
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
            case 'o':
            {
                char *value = (char *)malloc(sizeof(char));
                dec2oct(va_arg(args, int), value);
                cout << value;
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

void trans(int num, int base, int offset, char *result)
{
    if (num == 0)
    {
        *result = '\0';
        return;
    }
    char serials[] = {
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        'A',
        'B',
        'C',
        'D',
        'E',
        'F'};
    char buffer[32];
    int position = sizeof(buffer) / sizeof(char);
    while (num != 0)
    {
        int temp = num & base;
        buffer[--position] = serials[temp];
        num = num >> offset;
    }
    for (int i = position; i < sizeof(buffer) / sizeof(char); i++)
    {
        *(result++) = buffer[i];
    }
    return;
}

void dec2oct(int num, char *result)
{
    trans(num, 7, 3, result);
}
