#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "Volume.hpp"
#include "SDFLib.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <output_file>" << std::endl;
        return 1;
    }

    size_t size = 256;

    Volume volume1(size);
    Volume volume2(size);
    Volume volume3(size);

    volume1.SetSDF(RepeatingTransformSDF);
    volume2.SetSDF(BoxFrameSDF);
    volume3.SetSDF(TorusSDF);

    Volume finalVolume = !(volume1.min(0)); // + volume2.min(0) + volume3.min(0));

    std::cout << finalVolume;
    // return 0;

    for (size_t i = 0; i < finalVolume.size; i++)
    {
        finalVolume.Print(std::cout, i);
        std::flush(std::cout);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000 / finalVolume.size));
        [[maybe_unused]] int result = system("clear");
    }

    std::ofstream file(argv[1], std::ios::binary);
    finalVolume.ExportBinary(file);

    return 0;
}
