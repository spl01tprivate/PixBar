#include <iostream>

using namespace std;

long map(long, long, long, long, long);

int main()
{
    for (int i = 0; i < 256; i++)
    {
        if (i == 255)
            cout << i << " | " << "4" << endl;
        else
            cout << i << " | " << map(i, 0, 255, 1, 5) << endl;
    }

    return 1;
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    const long run = in_max - in_min;
    if(run == 0){
        return -1; // AVR returns -1, SAM returns 0
    }
    const long rise = out_max - out_min;
    const long delta = x - in_min;
    return (delta * rise) / run + out_min;
}