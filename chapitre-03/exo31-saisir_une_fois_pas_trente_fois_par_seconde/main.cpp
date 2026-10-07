#include <iostream>
#include <string>

int main()
{
    int N, sansGarde = 0;
    std::cin >> N;

    for (int i = 0; i < N; i++)
    {
        std::string evenement;
        std::cin >> evenement;

        if (evenement == "enfonce")
        {
            std::cout << "SAISIR\n";
            sansGarde++;
        }
        else if (evenement == "repete")
        {
            std::cout << "RIEN\n";
            sansGarde++;
        }
        else
        {
            std::cout << "LACHER\n";
        }
    }

    std::cout << "SANS_GARDE " << sansGarde << '\n';
    return 0;
}