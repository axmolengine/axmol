/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated July 28, 2023. Replaces all prior versions.
 *
 * Copyright (c) 2013-2023, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software or
 * otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THE
 * SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <spine/Extension.h>
#include <spine/spine-axmol.h>

USING_NS_AX;
using namespace spine;

AxmolAtlasAttachmentLoader::AxmolAtlasAttachmentLoader(Atlas *atlas) : AtlasAttachmentLoader(atlas) {
}

AxmolAtlasAttachmentLoader::~AxmolAtlasAttachmentLoader() {}

void AxmolAtlasAttachmentLoader::configureAttachment(Attachment *attachment) {
}

static Texture2D::TexParams chooseTexParams(spine::AtlasPage& page)
{
    Texture2D::TexParams texParams{};
    switch (page.minFilter)
    {
    case TextureFilter_Linear:
        texParams.minFilter = rhi::SamplerFilter::MIN_LINEAR;
        break;
    case TextureFilter_Nearest:
        texParams.minFilter = rhi::SamplerFilter::MIN_NEAREST;
        break;
    case TextureFilter_MipMap:
        texParams.minFilter = rhi::SamplerFilter::MIN_LINEAR;
        break;
    case TextureFilter_MipMapNearestNearest:
        texParams.minFilter = rhi::SamplerFilter::MIN_NEAREST;
        texParams.mipFilter = rhi::SamplerFilter::MIP_NEAREST;
        break;
    case TextureFilter_MipMapLinearNearest:
        texParams.minFilter = rhi::SamplerFilter::MIN_NEAREST;
        texParams.mipFilter = rhi::SamplerFilter::MIP_LINEAR;
        break;
    case TextureFilter_MipMapNearestLinear:
        texParams.minFilter = rhi::SamplerFilter::MIN_LINEAR;
        texParams.mipFilter = rhi::SamplerFilter::MIP_NEAREST;
        break;
    case TextureFilter_MipMapLinearLinear:
        texParams.minFilter = rhi::SamplerFilter::MIN_LINEAR;
        texParams.mipFilter = rhi::SamplerFilter::MIP_LINEAR;
        break;
    }
    switch (page.magFilter)
    {
    case TextureFilter_Linear:
        texParams.magFilter = rhi::SamplerFilter::MAG_LINEAR;
        break;
    case TextureFilter_Nearest:
        texParams.magFilter = rhi::SamplerFilter::MAG_NEAREST;
        break;
    }

    texParams.sAddressMode = page.uWrap == TextureWrap_ClampToEdge ? rhi::SamplerAddressMode::CLAMP_TO_EDGE
                                                                   : rhi::SamplerAddressMode::REPEAT;
    texParams.tAddressMode = page.vWrap == TextureWrap_ClampToEdge ? rhi::SamplerAddressMode::CLAMP_TO_EDGE
                                                                   : rhi::SamplerAddressMode::REPEAT;
    return texParams;
}

AxmolTextureLoader::AxmolTextureLoader() : TextureLoader() {}
AxmolTextureLoader::~AxmolTextureLoader() {}

void AxmolTextureLoader::load(AtlasPage &page, const spine::String &path) {
	Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(path.buffer());
	AXASSERT(texture != nullptr, "Invalid image");
	if (texture) {
		texture->retain();
		Texture2D::TexParams texParams = chooseTexParams(page);
		texture->setTexParameters(texParams);

		page.texture = texture;
		page.width = texture->getPixelsWide();
		page.height = texture->getPixelsHigh();
	}
}

void AxmolTextureLoader::unload(void *texture) {
	if (texture) {
		((Texture2D *) texture)->release();
	}
}


AxmolExtension::AxmolExtension() : DefaultSpineExtension() {}

AxmolExtension::~AxmolExtension() {}

char *AxmolExtension::_readFile(const spine::String &path, int *length) {
	Data data = FileUtils::getInstance()->getDataFromFile(path.buffer());
	if (data.isNull()) return nullptr;

		// avoid buffer overflow (int is shorter than ssize_t in certain platforms)
	ssize_t tmpLen;
	char *ret = (char *) data.takeBuffer(&tmpLen);
	*length = static_cast<int>(tmpLen);
	return ret;
}

SpineExtension *spine::getDefaultExtension() {
	return new AxmolExtension();
}
