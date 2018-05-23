#include "api.hpp"
#include <vector>
#include <string>

using namespace Impact;
using namespace RayImpact;

int main(int argc, char *argv[])
{
    std::vector<std::string> filenames;

	if (argc < 2)
		filenames.push_back("default.impScene");
	else
		for (int i = 1; i < argc; i++)
		    filenames.push_back(std::string(argv[i]));

    parseSceneDescriptionFiles(filenames);
}
