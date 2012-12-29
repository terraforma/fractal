# ifndef   __FRACTAL_INCLUDE_VERSION_H_
#  define  __FRACTAL_INCLUDE_VERSION_H_ 1

namespace Fractal
{
	class SemVer;
	
	SemVer GetVersion();
};

class Fractal::SemVer
{
public :
	unsigned m_major, m_minor, m_patch;
};

# endif /* __FRACTAL_INCLUDE_VERSION_H_ */
