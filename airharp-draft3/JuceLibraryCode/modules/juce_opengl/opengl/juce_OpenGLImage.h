/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#ifndef __JUCE_OPENGLIMAGE_JUCEHEADER__
#define __JUCE_OPENGLIMAGE_JUCEHEADER__


//==============================================================================
/**
    A type of ImagePixelData that stores its image data in an OpenGL
    framebuffer, allowing a JUCE Image object to wrap a framebuffer.

    By creating an Image from an instance of an OpenGLFrameBufferImage,
    you can then use a Graphics object to draw into the framebuffer using normal
    JUCE 2D operations.

    @see Image, ImageType, ImagePixelData, OpenGLFrameBuffer
*/
class JUCE_API  OpenGLImageType     : public ImageType
{
public:
    OpenGLImageType();
    ~OpenGLImageType();

    ImagePixelData::Ptr create (Image::PixelFormat, int width, int height, bool shouldClearImage) const;
    int getTypeID() const;

    static OpenGLFrameBuffer* getFrameBufferFrom (const Image&);
};

#endif   // __JUCE_OPENGLIMAGE_JUCEHEADER__
