/*
  ==============================================================================

    DrumPattern.cpp
    Created: 30 Mar 2013 12:38:28pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "DrumPattern.h"
#include "Drums.h"
#include "DrumKit.h"
#include "PatternManager.h"
#include "KitManager.h"


DrumPattern::DrumPattern() :
	mMidiBuffer(new MidiBuffer)
{
}


DrumPattern::~DrumPattern()
{
}


DrumPattern::Status DrumPattern::LoadFromXml(XmlElement* element, File& /*directory*/)
{
	mMidiBuffer->clear();

	Status status = DrumItem::LoadFromXml(element);
	if (status != kNoError)
		return status;
	double patternRate = element->getDoubleAttribute("sampleRate", 0.0);
	if (patternRate == 0.0)
		return kSampleRateError;
	Drums& drums = Drums::instance();
	double sampleRate = drums.getSampleRate();
	double rateAdj = sampleRate / patternRate;

	XmlElement* kitElement = element->getChildByName("kit");
	if (kitElement == nullptr)
		return kNoKitError;
	DrumKit kitInfo;
	status = kitInfo.DrumItem::LoadFromXml(kitElement);
	if (status != kNoError)
		return status;
	KitManager& mgr = KitManager::GetInstance();
	SharedPtr<DrumKit> kit = mgr.GetItem(kitInfo.GetUuid());
	if (kit.get() == nullptr)
		return kNoKitError;
	mDrumKit = kit;

	XmlElement* child = element->getChildByName("event");
	while (child != nullptr)
	{
		int samplePosition = child->getIntAttribute("position", -1);
		if (samplePosition < 0)
			return kSamplePositionError;
		String midi = child->getStringAttribute("data", "");
		if (midi == "")
			return kMidiDataError;
		int midiLength = midi.length();
		if ((midiLength & 1) != 0)	// Must be an even number of characaters, 2 per byte
			return kMidiDataError;
		midiLength /= 2;

		samplePosition = (int) (samplePosition * rateAdj);

		UniquePtr<char> midiBytes(new char[midiLength]);
		for (int i = 0; i < midiLength; ++i)
		{
			String midiByte = midi.substring(i * 2, i * 2 + 2);
			int value = midiByte.getHexValue32();
			jassert(value < 256);
			jassert(value >= 0);
			midiBytes.get()[i] = (char) value;
		}

		mMidiBuffer->addEvent(midiBytes.get(), midiLength, samplePosition);

		child = child->getNextElementWithTagName("event");
	}

	return kNoError;
}


DrumPattern::Status DrumPattern::SaveToXml(String fileName, File& directory)
{
	SetName(fileName);

	Drums& drums = Drums::instance();
	double sampleRate = drums.getSampleRate();

	XmlElement main("pattern");
	DrumItem::SaveToXml(&main);
	main.setAttribute("sampleRate", sampleRate);

	SharedPtr<DrumKit> kit = drums.getDrumKit();
	jassert(kit.get() != nullptr);
	XmlElement* kitElement = main.createNewChildElement("kit");
	jassert(kitElement != nullptr);
	kit->DrumItem::SaveToXml(kitElement);

	MidiBuffer::Iterator it(*mMidiBuffer.get());
	const uint8* eventData;
	int eventSize;
	int samplePosition;
	while (it.getNextEvent(eventData, eventSize, samplePosition))
	{
		String eventString = String::toHexString(eventData, eventSize, 0);
		XmlElement* eventElement = main.createNewChildElement("event");
		jassert(eventElement != nullptr);
		eventElement->setAttribute("position", samplePosition);
		eventElement->setAttribute("data", eventString);
	}

	fileName += ".xml";
	File file = directory.getChildFile(fileName);

	if (!main.writeToFile(file, ""))
		return kItemSaveError;

	return kNoError;
}


MidiBuffer& DrumPattern::GetMidiBuffer(void)
{
	return *mMidiBuffer.get();
}


SharedPtr<DrumKit> DrumPattern::GetDrumKit(void)
{
	return mDrumKit;
}