#include <iostream>

int main()
{
    int N, lisible = 0;
    std::cin >> N;

    for (int i = 0; i < N; i++)
    {
        int width, height, scale, field;
        std::cin >> width >> height >> scale >> field;

        int realWidth = width * scale / 100;
        int realHeight = height * scale / 100;
        int ppd = (realWidth + field / 2) / field;

        if (ppd >= 15)
            lisible++;

        std::cout << realWidth << ' ' << realHeight << ' ' << ppd << '\n';
    }

    std::cout << "LISIBLE " << lisible << '\n';

    return 0;
}