#include <iostream>
#include <vector>
#include <string>

struct Callback
{
    int id;
    std::string type;
};

int main()
{
    int N;
    std::cin >> N;

    std::vector<Callback> registre;

    for (int i = 0; i < N; i++)
    {
        std::string commande;
        std::cin >> commande;

        if (commande == "poser")
        {
            int id;
            std::string type;
            std::cin >> id >> type;

            for (auto it = registre.begin(); it != registre.end(); ++it)
            {
                if (it->id == id)
                {
                    registre.erase(it);
                    break;
                }
            }

            registre.push_back({id, type});
        }
        else if (commande == "retirer")
        {
            int id;
            std::cin >> id;

            for (auto it = registre.begin(); it != registre.end(); ++it)
            {
                if (it->id == id)
                {
                    registre.erase(it);
                    break;
                }
            }
        }
        else if (commande == "envoyer")
        {
            std::string type;
            std::cin >> type;

            bool trouve = false;

            for (const auto& cb : registre)
            {
                if (cb.type == type)
                {
                    if (trouve) std::cout << ' ';
                    std::cout << cb.id;
                    trouve = true;
                }
            }

            if (!trouve)
                std::cout << "AUCUN";

            std::cout << '\n';
        }
    }

    return 0;
}