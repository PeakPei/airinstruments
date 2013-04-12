/*
  ==============================================================================

    DrumKit.cpp
    Created: 25 Mar 2013 1:39:50pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "GfxTools.h"
#include "DrumKit.h"

DrumKit::DrumKit()
: mTextureId(0)
{
}


DrumKit::~DrumKit()
{
}


DrumKit::Status DrumKit::LoadFromXml(XmlElement* element, File& directory)
{
	mSamples.clear();

	Status status = DrumItem::LoadFromXml(element);
	if (status != kNoError)
		return status;
    
    String imageFilename = element->getStringAttribute("image", "");
    if (imageFilename != "")
    {
        File imageFile = directory.getChildFile(imageFilename);
        if (imageFile.existsAsFile())
            mImage = ImageFileFormat::loadFrom(imageFile);
        else
            Logger::outputDebugString("Sample image " + imageFilename + " not found");
    }

	XmlElement* child = element->getChildByName("sample");
	while (child != nullptr)
	{
		SharedPtr<DrumSample> sample(new DrumSample);
		DrumSample::Status status = sample->LoadFromXml(child, directory);
		if (status != DrumSample::kNoError)
			return kItemLoadError;
		mSamples.push_back(sample);
		child = child->getNextElementWithTagName("sample");
	}

	if (GetSampleCount() == 0)
		return kItemEmptyError;
	return kNoError;
}


DrumKit::Status DrumKit::CreateFromMemory(std::vector<MemorySampleInfo>& samples, String name, Uuid uuid)
{
	mSamples.clear();

	SetName(name);
	SetUuid(uuid);

/*
	// TODO :  The built-in kit needs images ?  If so, the image data needs to be made binary and passed in to this method.
    String imageFilename = element->getStringAttribute("image", "");
    if (imageFilename != "")
    {
        File imageFile = directory.getChildFile(imageFilename);
        if (imageFile.existsAsFile())
            mImage = ImageFileFormat::loadFrom(imageFile);
        else
            Logger::outputDebugString("Sample image " + imageFilename + " not found");
    }
*/

	for (int i = 0; i < (int) samples.size(); ++i)
	{
		SharedPtr<DrumSample> sample(new DrumSample);
		MemorySampleInfo& info = samples.at(i);
		DrumSample::Status status = sample->CreateFromMemory(info.data, info.size, info.note, info.category);
		if (status != DrumSample::kNoError)
			return kItemLoadError;
		mSamples.push_back(sample);
	}

	if (GetSampleCount() == 0)
		return kItemEmptyError;
	return kNoError;
}


SharedPtr<DrumSample> DrumKit::GetSample(int index)
{
	jassert(index < (int) mSamples.size());
	jassert(index >= 0);

	return mSamples.at(index);
}


int DrumKit::GetSampleCount(void)
{
	return mSamples.size();
}


DrumKit::Status DrumKit::SaveToXml(String /*fileName*/, File& /*directory*/)
{
	return kItemSaveError;
}

void DrumKit::LoadTextures()
{
    for (Item drumSample : mSamples)
    {
        drumSample->LoadTextures();
    }
    
    if (mImage.isValid())
    {
        glGenTextures(1, &mTextureId);
        glBindTexture(GL_TEXTURE_2D, mTextureId);
        GfxTools::loadTextureFromJuceImage(mImage);
    }
}

GLuint DrumKit::GetTexture() const
{
    return mTextureId;
}

const Image& DrumKit::GetImage() const
{
    return mImage;
}
