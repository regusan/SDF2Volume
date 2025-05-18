#include "Volume.hpp"

Volume::~Volume()
{
}

void Volume::SetSDF(SDFPtr sdf, float valueScale, float coordScale)
{
    for (size_t i = 0; i < data.size(); i++)
    {
        for (size_t j = 0; j < data[i].size(); j++)
        {
            for (size_t k = 0; k < data[i][j].size(); k++)
            {
                data[i][j][k] = sdf(
                                    (static_cast<float>(i) / size - 0.5f) * 2 * coordScale,
                                    (static_cast<float>(j) / size - 0.5f) * 2 * coordScale,
                                    (static_cast<float>(k) / size - 0.5f) * 2 * coordScale) *
                                valueScale;
            }
        }
    }
}

std::ostream &Volume::Print(std::ostream &os, size_t depth, float threshold)
{
    size_t step = this->size / 30 + 1;

    for (size_t j = 0; j < this->data[depth].size(); j += step)
    {
        for (size_t k = 0; k < this->data[depth][j].size(); k += step)
        {
            os << (this->data[depth][j][k] > threshold ? '#' : ' ');
        }
        os << std::endl;
    }
    return os;
}