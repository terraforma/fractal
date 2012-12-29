# include <version.h>

namespace
{
	enum version {
		VERSION_MAJOR = 0, VERSION_MINOR = 2, VERSION_PATCH = 0
	};
};

//
//	Return an structure that details the current version.
Fractal::SemVer Fractal::GetVersion()
{
	Fractal::SemVer ver = {
		VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH
	};

	return ver;
}
