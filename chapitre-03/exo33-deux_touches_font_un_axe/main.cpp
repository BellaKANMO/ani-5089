#include <iostream>
#include <vector>
#include <cmath>

struct Binding
{
    int scale;
    int threshold;
};

int main()
{
    int C;
    std::cin >> C;

    std::vector<Binding> bindings(C);
    std::string name;

    for (int i = 0; i < C; i++)
        std::cin >> name >> bindings[i].scale >> bindings[i].threshold;

    int T;
    std::cin >> T;

    for (int t = 0; t < T; t++)
    {
        int axis = 0;

        for (int i = 0; i < C; i++)
        {
            int raw;
            std::cin >> raw;

            int contribution = raw * bindings[i].scale / 1000;

            if (std::abs(contribution) >= bindings[i].threshold)
                axis += contribution;
        }

        std::cout << axis << '\n';
    }

    return 0;
}