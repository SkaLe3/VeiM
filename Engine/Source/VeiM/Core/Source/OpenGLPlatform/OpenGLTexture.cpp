#include "OpenGLPlatform/OpenGLObjects.h"

namespace VeiM
{
	OpenGLTexture::OpenGLTexture(const OpenGLTextureCreateSpecs& specs)
		: TextureRI(specs)
		, Target(specs.Target)
		, Attachment(specs.Attachment)
		, bIsPowerOfTwo(specs.bIsPowerOfTwo)
		, bIsCubeMap(specs.bIsCubeMap)
		, bArray(specs.bArray)
		, bDepthStencil(specs.bDepthStencil)
	{

		// INITIALIZE
	}
}