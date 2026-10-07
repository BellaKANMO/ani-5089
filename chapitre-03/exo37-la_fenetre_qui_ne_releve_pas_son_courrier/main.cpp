#include <iostream>
#include <string>

int main()
{
    int P, F;
    std::cin >> P >> F;

    int compteur = 0;
    int premier = 0;

    for (int i = 1; i <= F; i++)
    {
        std::string action;
        std::cin >> action;

        if (action == "releve")
            compteur = 0;
        else
            compteur++;

        bool morte = (compteur >= P);

        if (morte && premier == 0)
            premier = i;

        std::cout << compteur << ' '
                  << (morte ? "MORTE" : "VIVANTE")
                  << '\n';
    }

    std::cout << "PREMIER " << premier << '\n';

    return 0;
}