#include <iostream>
#include <string>

int main()
{
    int N;
    std::cin >> N;

    int dx = 0, dy = 0;
    int bx = 0, by = 0;

    for (int i = 0; i < N; i++)
    {
        std::string action;
        std::cin >> action;

        if (action == "bouge")
        {
            int x, y;
            std::cin >> x >> y;

            dx += x;
            dy += y;

            bx = x;
            by = y;
        }
        else if (action == "image")
        {
            std::cout << dx << ' ' << dy << ' '
                      << bx << ' ' << by << '\n';

            dx = 0;
            dy = 0;
        }
    }

    return 0;
}