#ifndef _SPOTLIGHT_
#define _SPOTLIGHT_

#include "PointLight.h"

class SpotLight : public PointLight
{
public:
	SpotLight () {}

	struct Light
	{
		Light()
		{
			ZeroMemory(this, sizeof(Light));
		}

		float cone;
	};
};

#endif // !_SPOTLIGHT_
