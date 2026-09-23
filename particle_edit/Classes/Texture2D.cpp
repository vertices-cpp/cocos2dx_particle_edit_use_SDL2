

#include "Texture2D.h" 
#include <cassert>


const BlendFunc BlendFunc::DISABLE = { BlendFactor::ONE, BlendFactor::ZERO };
const BlendFunc BlendFunc::ALPHA_PREMULTIPLIED = { BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA };
const BlendFunc BlendFunc::ALPHA_NON_PREMULTIPLIED = { BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA };
const BlendFunc BlendFunc::ADDITIVE = { BlendFactor::SRC_ALPHA, BlendFactor::ONE };
 
Texture2D::Texture2D() :mTexture(nullptr), _imageSize(Size::ZERO), mPitch(0), mPixels(nullptr)
{
}


Texture2D::~Texture2D()
{
	free();
}

void Texture2D::free()
{
	if (mTexture != nullptr)
		SDL_DestroyTexture(mTexture);
}
void Texture2D::setRGBA(Color4B color)
{
	//调制纹理rgb
	SDL_SetTextureColorMod(mTexture, color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(mTexture, color.a);
}

void Texture2D::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//调制纹理rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}
void Texture2D::SetTextureAlphaMod(Uint8 a) {
	SDL_SetTextureAlphaMod(mTexture, a);
}

void Texture2D::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(mTexture,blending);
}
 
 

bool Texture2D::loadMemData(SDL_Renderer * renderer, unsigned char * data, int len)
{
	free();

	SDL_RWops *memp = SDL_RWFromMem((void*)data, len);
	SDL_Surface *load = IMG_Load_RW(memp,0);
    printf("%s", SDL_GetError());
	assert(load != NULL);

	SDL_Surface *surfaceFormat = SDL_ConvertSurfaceFormat(load, SDL_PIXELFORMAT_RGBA8888, 0);

	mTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		surfaceFormat->w, surfaceFormat->h);

	SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);

	SDL_LockTexture(mTexture, &surfaceFormat->clip_rect, &mPixels, &mPitch);

	memcpy(mPixels, surfaceFormat->pixels, mPitch * surfaceFormat->h);

	_imageSize = Size((float)surfaceFormat->w, (float)surfaceFormat->h );
//  
//     Uint32 *pixels = (Uint32*)mPixels;
//     Uint32 colorKey = SDL_MapRGB(surfaceFormat->format, 0, 0, 0);
//     Uint32 transParent = SDL_MapRGBA(surfaceFormat->format, 0, 0, 0, 0);
//     Uint32 pixelCount = mPitch / 4 * surfaceFormat->h;
//     for (Uint32 i = 0; i < pixelCount; i++)
//     {
//         Uint8 r, g, b, a;
//         SDL_GetRGBA(pixels[i], surfaceFormat->format, &r, &g, &b, &a);
// 
//         r = (r * a) / 255;
//         g = (g * a) / 255;
//         b = (b * a) / 255;
// 
//         pixels[i] = SDL_MapRGBA(surfaceFormat->format, r, g, b, a);
//
//     }

#undef TRANSPRENT_BLACK
#ifdef TRANSPRENT_BLACK
	Uint32 *pixels = (Uint32*)mPixels;
	Uint32 colorKey = SDL_MapRGB(surfaceFormat->format, 0, 0, 0);
	Uint32 transParent = SDL_MapRGBA(surfaceFormat->format, 0, 0, 0, 0);

	Uint32 pixelCount = mPitch / 4 * surfaceFormat->h;

	for (Uint32 i = 0; i < pixelCount; ++i)
	{
		if (pixels[i] == colorKey)
			pixels[i] = transParent;
	}

#endif

	SDL_UnlockTexture(mTexture);

	SDL_FreeSurface(surfaceFormat);
	SDL_FreeSurface(load);
	SDL_FreeRW(memp);
	return true;
}
// Texture2D.cpp
bool Texture2D::createTarget(SDL_Renderer* renderer, int w, int h)
{
    free();   // 释放旧的

    mTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,   // 关键
        w, h);

    if (!mTexture) return false;

   // SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
    _imageSize.w = (float)w;
    _imageSize.h = (float)h;
    return true;
}
void Texture2D::createTexture(SDL_Renderer * renderer,   const Rect & rect)
{ 

    float w = rect.w, h = rect.h;

    SDL_Texture *tex = SDL_GetRenderTarget(renderer);

    mTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)w, (int)h);
    
   

    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);

    //SDL_SetRenderTarget(renderer, mTexture);
    //创建目标纹理
    Rect dest = { 0,0,w,h };
    Size size = { w,h };

    
    SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);

    render(renderer ,rect, dest, Vec2::ZERO,
        size / 2, Color4B(255, 255, 255, 255), false, false);
    // 	SDL_Rect rrr = { 0,0,w,h };
    // 	SDL_Rect ddd = { 0,0,w,h };
    //	SDL_RenderCopyEx(SDLView::getInstance()->getRender(), getTexture.get()->getTexture(),  &rrr, &ddd, 0, 0,SDL_FLIP_NONE);
    _imageSize.w = w;
    _imageSize.h = h;
     
    // 4. 切到新目标，把源纹理渲染上来
   SDL_SetRenderTarget(renderer, oldTarget);
}

void Texture2D::render(SDL_Renderer* renderer, const Rect& clip, const Rect &dest,Vec2 rotate, const Vec2 &pointer, const Color4B &color, bool flipX, bool flipY)
{
	quad.TextureCvRenderer(renderer,mTexture, _imageSize, clip, dest, rotate, pointer, color,flipX, flipY);
}
