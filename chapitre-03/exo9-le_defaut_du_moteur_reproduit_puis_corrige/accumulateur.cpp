struct Accumulateur
{
    int totalX = 0;
    int totalY = 0;

    void SurMouvement(int dx, int dy)
    {
        totalX += dx;
        totalY += dy;
    }

    void Consommer(int& dx, int& dy)
    {
        dx = totalX;
        dy = totalY;

        totalX = 0;
        totalY = 0;
    }
};