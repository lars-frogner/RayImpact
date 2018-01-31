#include "api.hpp"
#include <vector>
#include <string>

using namespace Impact;
using namespace RayImpact;

int main(int argc, char *argv[])
{
	std::vector<std::string> filenames;
	filenames.push_back(std::string("scene.txt"));

	parseSceneDescriptionFiles(filenames);
}
