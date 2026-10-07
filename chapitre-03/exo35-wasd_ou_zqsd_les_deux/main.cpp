#include <iostream>
#include <string>
#include <set>

int main()
{
    int N;
    std::cin >> N;
    std::cin.ignore();

    for (int i = 0; i < N; i++)
    {
        std::string ligne;
        std::getline(std::cin, ligne);

        int avant = 0, cote = 0;
        std::set<std::string> touches;
        std::string mot;

        for (char c : ligne + " ")
        {
            if (c == ' ')
            {
                if (!mot.empty()) touches.insert(mot);
                mot.clear();
            }
            else
                mot += c;
        }

        if (touches.count("W") || touches.count("Z")) avant++;
        if (touches.count("S")) avant--;
        if (touches.count("A") || touches.count("Q")) cote--;
        if (touches.count("D")) cote++;

        std::cout << avant << ' ' << cote << '\n';
    }

    return 0;
}