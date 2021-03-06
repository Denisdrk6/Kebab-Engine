#include "TextureLoader.h"

#include "Globals.h"

#define ASSETS_DIR "Assets/3D Models/"

TextureLoader* TextureLoader::instance = nullptr;

TextureLoader* TextureLoader::GetInstance()
{
	if (!instance) instance = new TextureLoader();
	return instance;
}

TextureLoader::~TextureLoader()
{
	RELEASE(instance);
	for (int i = 0; i < textures.size(); ++i)
	{
		RELEASE(textures[i]);
	}
}

Texture* TextureLoader::LoadTexture(const char* fileName)
{
	Texture* ret = nullptr;

	ILuint tmp;
	tmp = ilGenImage();
	ilBindImage(tmp);

	/*std::string dir = ASSETS_DIR;
	dir.append(fileName);*/

	if (!ilLoadImage(fileName))
	{
		LOG_CONSOLE("Could not load Image from %s", fileName);
	}
	else
	{
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		ret = new Texture(ilGetData(), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), fileName);
		textures.push_back(ret);

		ilDeleteImage(tmp);

		LOG_CONSOLE("\nLoaded image from %s", fileName);
	}

	ilBindImage(0);
	
	return ret;
}
