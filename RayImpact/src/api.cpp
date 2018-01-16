#include "api.hpp"
#include "error.hpp"
#include "parallel.hpp"
#include "Spectrum.hpp"

namespace Impact {
namespace RayImpact {

enum class APIState {Uninitialized, Configuration, SceneGeneration};
	
static APIState current_API_state = APIState::Uninitialized; // State of the Ray Impact API

Options RIMP_OPTIONS; // Global rendering options

// API macros

#define verify_initialized(function_name) \
	if (current_API_state == APIState::Uninitialized) \
	{ \
		printErrorMessage("API system must be initialized before calling \"%s()\". Ignoring call.", function_name); \
		return; \
	} \
	else // required so that the trailing semicolon doesn't cause syntax error

#define verify_in_config_state(function_name) \
	if (current_API_state == APIState::Configuration) \
	{ \
		printErrorMessage("API system must be in the configuration state when calling \"%s()\". Ignoring call.", function_name); \
		return; \
	} \
	else // required so that the trailing semicolon doesn't cause syntax error

#define verify_in_scene_gen_state(function_name) \
	if (current_API_state == APIState::SceneGeneration) \
	{ \
		printErrorMessage("API system must be in the scene generation state when calling \"%s()\". Ignoring call.", function_name); \
		return; \
	} \
	else // required so that the trailing semicolon doesn't cause syntax error

// API function implementations

// Performs required initializations for the rendering system
void RIMP_Initialize(const Options& options)
{
	RIMP_OPTIONS = options;

	if (current_API_state != APIState::Uninitialized)
		printErrorMessage("API system has already been initialized");

	current_API_state = APIState::Configuration;

	initializeParallel(options.n_threads);

	SampledSpectrum::initialize();
}

// Performs cleanup for the rendering system
void RIMP_Cleanup()
{
	if (current_API_state == APIState::Uninitialized)
		printErrorMessage("API cleanup requested before initialization");
	else if (current_API_state == APIState::SceneGeneration)
		printErrorMessage("API cleanup requested before finishing configuration");

	current_API_state = APIState::Uninitialized;

	cleanupParallel();
}

} // RayImpact
} // Impact

