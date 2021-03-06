/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/
#include "GL/glew.h"
#include <GLTools.h>
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include "HUD.h"

#include "Environment.h"
#include "GfxTools.h"
#include "AudioServer.h"
#include "Harp.h"
#include "MotionServer.h"
#include "FingerView.h"
#include "SkinManager.h"
#include "Main.h"

#include "MainComponent.h"

#include <algorithm>

#define BUFFER_SIZE 512
#define TUTORIAL_TIMEOUT 30000
#define SPLASH_FADE 1500
#define MIN_SPLASH_DURATION 3000

//RtAudioDriver driver(BUFFER_SIZE);

//==============================================================================
MainContentComponent::MainContentComponent()
: chordRegionsNeedUpdate(false)
, toolbar(NULL)
, statusBar(NULL)
, settingsScreen(NULL)
, leapDisconnectedView(NULL)
, sizeChanged(false)
, connected(false)
{
    startTime = Time::getCurrentTime();
    setSize (1280, 690);
    setWantsKeyboardFocus(true);

    File special = File::getSpecialLocation(File::currentApplicationFile);
#if JUCE_WINDOWS
	File resourcesFile = special.getChildFile("../");
#elif JUCE_MAC
	File resourcesFile = special.getChildFile("Contents/Resources");
#endif
    File bgImageFile = resourcesFile.getChildFile("splash_bg.png");
    File splashTitleImageFile = resourcesFile.getChildFile("splash_title.png");
    File splashLogoImageFile = resourcesFile.getChildFile("logotype.png");
    
    if (bgImageFile.exists())
        splashBgImage = ImageFileFormat::loadFrom(bgImageFile);
    else
        Logger::writeToLog("ERROR: splash_bg.png not found!");
    
    if (splashTitleImageFile.exists())
        splashTitleImage = ImageFileFormat::loadFrom(splashTitleImageFile);
    else
        Logger::writeToLog("ERROR: splash_title.png not found!");
    
    if (splashLogoImageFile.exists())
        splashLogoImage = ImageFileFormat::loadFrom(splashLogoImageFile);
    else
        Logger::writeToLog("ERROR: logotype.png not found!");
}

MainContentComponent::~MainContentComponent()
{
    openGLContext.detach();
    delete backgroundView;
    delete toolbar;
    delete statusBar;
    delete settingsScreen;
    delete leapDisconnectedView;
}

void MainContentComponent::go2d()
{
    Environment::instance().viewFrustum.SetOrthographic(0, Environment::instance().screenW, 0.0f, Environment::instance().screenH, 800.0f, -800.0f);
	Environment::instance().modelViewMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
}

void MainContentComponent::go3d()
{
    Environment::instance().viewFrustum.SetPerspective(10.0f, float(Environment::instance().screenW)/float(Environment::instance().screenH), 0.01f, 500.0f);
	Environment::instance().projectionMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    Environment::instance().modelViewMatrix.LoadIdentity();
}

void MainContentComponent::paint (Graphics& g)
{
    static bool firstTime = true;
    if (Environment::instance().ready)
        return;
    
    splashImage = Image(Image::ARGB, getWidth(), getHeight(), true);
    Graphics offscreen(splashImage);
    g.fillAll (Colour (0x000000));
    g.setFont (Font (16.0f));
    g.setColour (Colours::black);
    g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
    if (splashTitleImage.isValid()) {
        float aspectRatio = splashTitleImage.getHeight() / (float)splashTitleImage.getWidth();
        float w = getWidth() / 2.f;
        float h = w * aspectRatio;
        int x = (int)(getWidth() / 2.f - w / 2.f);
        int y = (int)(getHeight() / 2.f - h / 2.f);
        offscreen.drawImage(splashTitleImage, x, y, (int)w, (int)h, 0, 0, splashTitleImage.getWidth(), splashTitleImage.getHeight());
        offscreen.setColour (Colour (0xffffffff));
        Font f = Font(Environment::instance().getDefaultFont(), 16, Font::plain);
        f.setExtraKerningFactor(1.5);
        offscreen.setFont(f);
        offscreen.drawText("LOADING", x, y + (int)h + 20, (int)w, 12, Justification::centred, false);
        f.setExtraKerningFactor(0);
        offscreen.setFont(f);
        offscreen.drawText("v" + AirHarpApplication::getInstance()->getApplicationVersion(), x, y + (int)h + 20 + 35, (int)w, 12, Justification::centred, false);
    }
    if (splashLogoImage.isValid())
    {
        float aspectRatio = splashLogoImage.getHeight() / (float)splashLogoImage.getWidth();
        float w = getWidth() / 4.f;
        float h = w * aspectRatio;
        int x = getWidth() - w;
        int y = (int)(getHeight() - h);
        offscreen.setOpacity(0.7f);
        offscreen.drawImage(splashLogoImage, x, y, (int)w, (int)h, 0, 0, splashLogoImage.getWidth(), splashLogoImage.getHeight());
    }
    if (splashBgImage.isValid())
        g.drawImage(splashBgImage, 0, 0, getWidth(), getHeight(), 0, 0, splashBgImage.getWidth(), splashBgImage.getHeight());
    g.drawImage(splashImage, 0, 0, getWidth(), getHeight(), 0, 0, splashImage.getWidth(), splashImage.getHeight());
    
    if (firstTime) {
        Logger::writeToLog("painted once, sending InitGLMessage");
        postMessage(new InitGLMessage);
    }
    firstTime = false;
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    // Prevent a divide by zero
    int w = getWidth();
    int h = getHeight();
    if(h == 0)
        h = 1;
    
    // Set Viewport to window dimensions
    //glViewport(0, 0, w, h);
    
    Environment::instance().screenW = w;
    Environment::instance().screenH = h;
    
    sizeChanged = true;
}

void MainContentComponent::focusGained(FocusChangeType cause)
{
}

void MainContentComponent::focusLost(FocusChangeType cause)
{
}

void MainContentComponent::newOpenGLContextCreated()
{
   if (Environment::instance().ready)
      return;
    
    Logger::writeToLog("newOpenGLContextCreated()");
    Logger::writeToLog("glewInit");
    glewInit();
    if (GLEW_ARB_vertex_array_object || GLEW_APPLE_vertex_array_object)
        Logger::writeToLog("VAOs Supported");
    else
        Logger::writeToLog("VAOs Not Supported");

    MotionDispatcher::instance().setUseHandsAndFingers(true);

    Logger::writeToLog("loading skins");
    SkinManager::instance().loadResources();
    Logger::writeToLog("set skin 0");
    SkinManager::instance().setSelectedSkin("skin0");
    
    Logger::writeToLog("setting GL parameters");
    //glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    
    glEnable(GL_DEPTH_TEST);
    //glDepthMask(true);
    //glDepthFunc(GL_LESS);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glEnable(GL_MULTISAMPLE);
    
    //Environment::instance().cameraFrame.MoveForward(-15.0f);
    
    Logger::writeToLog("init finger views");
    for (auto iter : MotionDispatcher::instance().fingerViews)
        iter.second->setup();
    
    Logger::writeToLog("init bg");
    backgroundView = new View2d;
    backgroundView->setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("background_dark"));
    GLfloat bgColor[4] = { 0.7f, 0.7f, 0.7f, 1.f };
    backgroundView->setDefaultColor(bgColor);

    Logger::writeToLog("init harps");
    for (int i = 0; i < HarpManager::instance().getNumHarps(); ++i)
    {
        HarpView* hv = new HarpView(i);
        hv->loadTextures();
        hv->numHarps = HarpManager::instance().getNumHarps();
        harps.push_back(hv);
    }
    
    Logger::writeToLog("layout strings");
    layoutStrings();
    
    glEnable(GL_DEPTH_TEST);
    Logger::writeToLog("init shaders");
    Environment::instance().shaderManager.InitializeStockShaders();
    
    bool showTutorial = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getBoolValue("showTutorial", true);

    int w = getWidth();
    int h = getHeight();

    Logger::writeToLog("init splash");
    splashBgView = new View2d;
    splashBgView->setBounds(HUDRect(0,0,(GLfloat)w,(GLfloat)h));
    splashBgView->setDefaultTexture(GfxTools::loadTextureFromJuceImage(splashBgImage));
    if (!showTutorial)
        splashBgView->setVisible(false, SPLASH_FADE);
    splashTitleView = new View2d;
    splashTitleView->setBounds(HUDRect(0,0,(GLfloat)w,(GLfloat)h));
    splashTitleView->setDefaultTexture(GfxTools::loadTextureFromJuceImage(splashImage));
    splashTitleView->setVisible(false, SPLASH_FADE);
    
    Logger::writeToLog("init toolbar");
    HarpToolbar* tb = new HarpToolbar;
    views.push_back(tb);
    toolbar = tb;
    toolbar->updateButtons();
    toolbar->addActionListener(this);
    
    {
    MessageManagerLock mml;
    toolbar->addChangeListener(this);
    }
    
    Logger::writeToLog("init statusbar");
    StatusBar* sb = new StatusBar;
    sb->setIndicatorTextures(SkinManager::instance().getSelectedSkin().getTexture("leapStatus_on"), SkinManager::instance().getSelectedSkin().getTexture("leapStatus_off"));
    views.push_back(sb);
    statusBar = sb;

    Logger::writeToLog("init chord regions");
    for (int i = 0; i < 7; ++i)
    {
        ChordRegion* cr = new ChordRegion;
        cr->setId(i);
        cr->setVisible(false);
        chordRegions.push_back(cr);
        views.push_back(cr);
    }
    
    Logger::writeToLog("init settings screen");
    settingsScreen = new SettingsScreen;
    settingsScreen->addActionListener(this);
    settingsScreen->getScaleEditor()->addActionListener(this);
    settingsScreen->setVisible(false);
    views.push_back(settingsScreen);
    
    Logger::writeToLog("load view textures");
    for (HUDView* v : views)
        v->loadTextures();

    Logger::writeToLog("init tutorial");
    tutorial = new TutorialSlide;
    tutorial->loadTextures();
    tutorial->setButtonRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    tutorial->setDotTextures(SkinManager::instance().getSelectedSkin().getTexture("dot_white"),
                             SkinManager::instance().getSelectedSkin().getTexture("dot_black"));
    tutorial->addActionListener(this);
    tutorial->setVisible(false, 0);
    if (showTutorial)
        startTimer(kTimerShowTutorial, 500);

    toolbar->setBounds(HUDRect(0,h-70,w,70));
    statusBar->setBounds(HUDRect(0,0,w,35));

    float yPos = 20.f;
    float height = (h-90)/7.f;
    for (ChordRegion* cr : chordRegions)
    {
        cr->setBounds(HUDRect(0,yPos, w, height));
        yPos += height;
    }

//    SkinManager::instance().getSkin();
//    toolbar->setButtonTextures(SkinManager::instance().getSelectedSkin().getTexture("button_on0"), SkinManager::instance().getSelectedSkin().getTexture("button_off0"));
    
    // Load shaders for finger rendering
//    File special = File::getSpecialLocation(File::currentApplicationFile);
//#if JUCE_WINDOWS
//    File resources = special.getChildFile("..");
//#elif JUCE_MAC
//    File resources = special.getChildFile("Contents/Resources");
//#endif
//   File vsFile = resources.getChildFile("testShader.vs");
//   File fsFile = resources.getChildFile("testShader.fs");

    Logger::writeToLog("load shader test");
    shaderId = Environment::instance().shaderManager.LoadShaderPairSrcWithAttributes("test", BinaryData::testShader_vs, BinaryData::testShader_fs, 2,
                                                                                     GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");
    //jassert(shaderId != 0);
    if (shaderId == 0) {
        AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Shader Error", "failed to load testShader");
        Logger::writeToLog("ERROR: failed to load testShader");
    }
//    vsFile = resources.getChildFile("bloom.vs");
//    fsFile = resources.getChildFile("bloom.fs");

    Logger::writeToLog("load shader bloom");
    bloomShaderId = gltLoadShaderPairSrcWithAttributes(BinaryData::bloom_vs, BinaryData::bloom_fs, 2,
                                                       GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");
    //jassert(bloomShaderId != 0);
    if (bloomShaderId == 0) {
        AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Shader Error", "failed to load bloom shader");
        Logger::writeToLog("ERROR: failed to load bloom shader");
    }

    Logger::writeToLog("setup offscreen");
    // setup the offscreen finger texture
    int imageW = 512;
    int imageH = 512;
    Image im(Image::PixelFormat::ARGB, imageW, imageH, true);
    TextureDescription td = GfxTools::loadTextureFromJuceImage(im);
    td.texX = 0.f;
    td.texY = 1.f;
    td.texW = 1.f;
    td.texH = -1.f;
    fingersImage.setDefaultTexture(td);
    
    Logger::writeToLog("init disconnect message");
    leapDisconnectedView = new HUDView;
    leapDisconnectedView->setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("LeapDisconnected"));
    leapDisconnectedView->setVisible(false, 0);
    startTimer(kTimerWaitingForConnection, 1000);
    
    Logger::writeToLog("init fullscreen tip");
    fullscreenTipView = new HUDView;
    fullscreenTipView->setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("fullscreenTip"));
    fullscreenTipView->setVisible(false, 0);
    showFullscreenTip();
    
    Logger::writeToLog("set matrix stacks");
    Environment::instance().transformPipeline.SetMatrixStacks(Environment::instance().modelViewMatrix, Environment::instance().projectionMatrix);
    Environment::instance().ready = true;
    
    Logger::writeToLog("enable gestures");
    MotionDispatcher::instance().addListener(*this);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);

    Logger::writeToLog("setSwapInterval");
    openGLContext.setSwapInterval(1);

    MessageManagerLock mml;
    grabKeyboardFocus();
    
    MotionDispatcher::instance().cursor->setEnabled(false);
    
    actionListenerCallback("scaleChanged");

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f );
}

void MainContentComponent::renderOpenGL()
{
    if (sizeChanged)
    {
        const float tutorialWidth = 800.f;
        const float tutorialHeight = 500.f;

        if (backgroundView)
            backgroundView->setBounds(HUDRect(0,0,Environment::instance().screenW,Environment::instance().screenH));

        if (tutorial)
            tutorial->setBounds(HUDRect((GLfloat) (Environment::instance().screenW / 2 - tutorialWidth / 2),
                                        (GLfloat) (Environment::instance().screenH / 2 - tutorialHeight / 2),
                                        tutorialWidth,
                                        tutorialHeight));
        
        if (splashBgView)
            splashBgView->setBounds(HUDRect(0.f,0.f,(GLfloat)Environment::instance().screenW,(GLfloat)Environment::instance().screenH));
        
        if (splashTitleView)
            splashBgView->setBounds(HUDRect(0.f,0.f,(GLfloat)Environment::instance().screenW,(GLfloat)Environment::instance().screenH));

        if (toolbar)
            toolbar->setBounds(HUDRect(0,Environment::instance().screenH-100,Environment::instance().screenW,100));
        if (statusBar)
            statusBar->setBounds(HUDRect(0,0,Environment::instance().screenW,35));
        if (settingsScreen)
            settingsScreen->setBounds(HUDRect(0,
                                              20,
                                              Environment::instance().screenW,
                                              Environment::instance().screenH - 70 - 20));
        if (leapDisconnectedView) {
            TextureDescription td = SkinManager::instance().getSelectedSkin().getTexture("LeapDisconnected");
            float aspectRatio = td.imageH / (float)td.imageW;
            float w = Environment::instance().screenW / 2.f;
            float h = w * aspectRatio;
            leapDisconnectedView->setBounds(HUDRect(Environment::instance().screenW / 2.f - w / 2.f,
                                                    Environment::instance().screenH / 2.f - h / 2.f,
                                                    w,
                                                    h));
        }
        if (fullscreenTipView) {
            TextureDescription td = SkinManager::instance().getSelectedSkin().getTexture("fullscreenTip");
            float aspectRatio = td.imageH / (float)td.imageW;
            float w = 327;
            float h = w * aspectRatio;
            fullscreenTipView->setBounds(HUDRect(20,
                                                 Environment::instance().screenH - h - 20.f,
                                                 w,
                                                 h));
        }
        layoutStrings();
        chordRegionsNeedUpdate = true;

        fingersImage.setBounds(HUDRect(0,0,Environment::instance().screenW,Environment::instance().screenH));
        
        sizeChanged = false;
    }
    if (chordRegionsNeedUpdate) {
        layoutChordRegions();
        chordRegionsNeedUpdate = false;
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glClearColor(0.f, 0.f, 0.f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	go3d();

    // Draw fingers to offscreen texture
    const TextureDescription& td = fingersImage.getDefaultTexture();
    glViewport(0,0,td.imageW,td.imageH);
    glUseProgram((GLuint)shaderId);
    for (auto iter : MotionDispatcher::instance().fingerViews)
        if (iter.second->inUse)
            iter.second->drawWithShader(shaderId);

    glBindTexture(GL_TEXTURE_2D,td.textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, td.imageW, td.imageH, 0);
    glViewport(0,0,Environment::instance().screenW,Environment::instance().screenH);

    go2d();

    backgroundView->draw();
    
	go3d();
    
    glEnable(GL_DEPTH_TEST);

    for (HarpView* hv : harps)
        hv->draw();
    
	go2d();

    glDisable(GL_DEPTH_TEST);

    for (HUDView* v : views)
        v->draw();

    splashBgView->setDefaultBlendMode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    splashBgView->draw();
    splashTitleView->setDefaultBlendMode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    splashTitleView->draw();

    glEnable(GL_DEPTH_TEST);
    
    go3d();
    
    for (auto iter : MotionDispatcher::instance().handViews)
        if (iter.second->inUse)
            ;//iter.second->draw();

    go2d();
    
    glDisable(GL_DEPTH_TEST);

    tutorial->draw();
    
    leapDisconnectedView->setDefaultBlendMode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    leapDisconnectedView->draw();
    fullscreenTipView->setDefaultBlendMode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    fullscreenTipView->draw();
    
    // Render the offscreen fingers image using bloom shader
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    fingersImage.setDefaultBlendMode(GL_SRC_ALPHA,GL_ONE);
    glBindTexture(GL_TEXTURE_2D, fingersImage.getDefaultTexture().textureId);
    glUseProgram((GLuint)bloomShaderId);
    GLint iModelViewMatrix = glGetUniformLocation(bloomShaderId, "mvpMatrix");
    glUniformMatrix4fv(iModelViewMatrix, 1, GL_FALSE, Environment::instance().transformPipeline.GetModelViewMatrix());
    GLfloat imageColor[4] = { 1.f, 1.f, 1.f, 1.f };
    GLint iColor = glGetUniformLocation(bloomShaderId, "vColor");
    glUniform4fv(iColor, 1, imageColor);
    GLint iTextureUnit = glGetUniformLocation(bloomShaderId, "textureUnit0");
    glUniform1i(iTextureUnit, 0);
    fingersImage.defaultBatch.Draw();
    
    go3d();
    
    for (HarpView* hv : harps)
        hv->update();

    //openGLContext.triggerRepaint();
}

void MainContentComponent::openGLContextClosing()
{
    MotionDispatcher::instance().removeListener(*this);
}

void MainContentComponent::layoutStrings()
{
    for (HarpView* hv : harps)
    {
        hv->height = 2.f / harps.size();
        hv->layoutStrings();
    }
}

void MainContentComponent::layoutChordRegions()
{
    if (!Environment::instance().ready)
        return;
    
    Harp* h = HarpManager::instance().getHarp(0);
    if (!h->getChordMode())
        return;
    std::vector<ChordRegion*> activeChordRegions;
    for (ChordRegion* cr : chordRegions)
    {
        if (h->isChordSelected(cr->getId())) {
            activeChordRegions.push_back(cr);
            cr->setVisible(true);
        }
        else
            cr->setVisible(false);
    }
    
    int numActiveChords = std::max<int>(1, activeChordRegions.size());
    float height = (getHeight()-90) / (float)numActiveChords;
    float yPos = 20.f;
    
    for (int i = 0; i < activeChordRegions.size(); ++i)
    {
        ChordRegion* cr = activeChordRegions.at(i);
        GLfloat color[] = {.3, .3, .3, .1};
        if (i % 2 != 0) {
            color[0] = .4;
            color[1] = .4;
            color[2] = .4;
        }
        cr->setDefaultColor(color);
        cr->setBounds(HUDRect(0,yPos, getWidth(), height));
        yPos += height;
    }
    
}

void MainContentComponent::mouseMove(const MouseEvent& e)
{
    for (HUDView* v : views)
        v->passiveMotion(e.getPosition().x, e.getPosition().y);
    
    float leapSpaceX = e.getPosition().x / (float)Environment::instance().screenW;
    leapSpaceX *= 400;
    leapSpaceX -= 200;
    float leapSpaceY = 1 - (e.getPosition().y / (float)Environment::instance().screenH);
    leapSpaceY *= 300;
    leapSpaceY += 100;
    //MotionDispatcher::instance().spoof(leapSpaceX, leapSpaceY, -5);
}

void MainContentComponent::mouseDown(const MouseEvent& e)
{
    if (!Environment::instance().ready)
        return;

    for (HUDView* v : views)
        v->mouseDown(e.getPosition().x, Environment::instance().screenH - e.getPosition().y);
    
    if (tutorial)
        tutorial->mouseDown((float) e.getPosition().x, (float)Environment::instance().screenH - e.getPosition().y);
}

void MainContentComponent::mouseDrag(const MouseEvent& e)
{
    for (HUDView* v : views)
        v->motion(e.getPosition().x, e.getPosition().y);
}

bool MainContentComponent::keyPressed(const KeyPress& kp)
{
    bool ret = false;
    
    printf("%d\n", kp.getTextDescription().getIntValue());
    if (kp.getTextCharacter() == 'h')
    {
        showTutorial();
        ret = true;
    }
    else if (kp.getTextCharacter() == 'f')
    {
        if (!AirHarpApplication::getInstance()->isFullscreen()) {
             AirHarpApplication::getInstance()->enterFullscreenMode();
        }
        else {
           AirHarpApplication::getInstance()->exitFullscreenMode();
        }
        ret = true;
    }
    else if (kp.isKeyCode(KeyPress::escapeKey))
    {
        AirHarpApplication::getInstance()->quit();
        ret = true;
    }
    else if (kp.getTextCharacter() == 'a')
    {
//        for (HarpView* hv : harps)
//        {
//            hv->addString();
//        }
        ret = true;
    }
    else if (kp.getTextCharacter() == 'z')
    {
//        for (HarpView* hv : harps)
//        {
//            hv->removeString();
//        }
        ret = true;
    }
    else if (kp.getTextCharacter() == 'c')
    {
//        Harp* h = HarpManager::instance().getHarp(0);
//        h->setChordMode(!h->getChordMode());
//        toolbar->updateButtons();
//        if (h->getChordMode()) {
//            chordRegionsNeedUpdate = true;
//        }
//        else {
//            for (ChordRegion* cr : chordRegions)
//                cr->setVisible(false);
//        }
//        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("chordMode", h->getChordMode());
        
        
        ret = true;
    }
    else if (kp.getTextDescription().getIntValue() > 0)
    {
//        SkinManager::instance().setSkinIndex(kp.getTextDescription().getIntValue()-1);
//        toolbar->setButtonTextures(SkinManager::instance().getSkin().buttonOn, SkinManager::instance().getSkin().buttonOff);
//        statusBar->setIndicatorTextures(SkinManager::instance().getSkin().buttonOn, SkinManager::instance().getSkin().buttonOff);
        ret = true;
    }

    return ret;
}

void MainContentComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    chordRegionsNeedUpdate = true;
}

void MainContentComponent::onFrame(const Leap::Controller& controller)
{
    lastFrame = Time::getCurrentTime();

    if (!Environment::instance().ready)
        return;
    
    const Leap::Frame frame = controller.frame();
    const Leap::GestureList& gestures = frame.gestures();
    Leap::GestureList::const_iterator i = gestures.begin();
    Leap::GestureList::const_iterator end = gestures.end();
    for ( ; i != end; ++i)
    {
        const Leap::Gesture& g = *i;
        switch (g.type())
        {
            case Leap::Gesture::TYPE_SWIPE:
                break;
                
            case Leap::Gesture::TYPE_KEY_TAP:
            {
                const Leap::KeyTapGesture keyTap(g);
                handleTapGesture(keyTap.pointable());
            }
                break;
                
            case Leap::Gesture::TYPE_SCREEN_TAP:
            {
                const Leap::ScreenTapGesture screenTap(g);
                handleTapGesture(screenTap.pointable());
            }
                break;
                
            case Leap::Gesture::TYPE_CIRCLE:
            {
                const Leap::CircleGesture circle(g);
                
                bool isClockwise = circle.normal().z < 0;
                
//                if (isClockwise && circle.state() == Leap::Gesture::STATE_STOP)
//                    slide->end();
//                else if (!slide->isDone() && circle.state() == Leap::Gesture::STATE_STOP)
//                    slide->back();
            }
                break;
                
            default:
                break;
        }
    }
}


void MainContentComponent::onConnect(const Leap::Controller&)
{
    if (!leapDisconnectedView)
        return;
    leapDisconnectedView->setVisible(false);
    connected = true;
}

void MainContentComponent::onDisconnect(const Leap::Controller&)
{
    if (!leapDisconnectedView)
        return;
    leapDisconnectedView->setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("LeapDisconnected"));
    leapDisconnectedView->setVisible(true);
    connected = false;
}

void MainContentComponent::onFocusGained (const Leap::Controller &)
{
    if (!leapDisconnectedView)
        return;
    if (connected)
        leapDisconnectedView->setVisible(false);
    else
        leapDisconnectedView->setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("LeapDisconnected"));
}

void MainContentComponent::onFocusLost (const Leap::Controller &)
{
    if (!leapDisconnectedView)
        return;
    leapDisconnectedView->setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("AppInBackground"));
    leapDisconnectedView->setVisible(true);
}


void MainContentComponent::handleTapGesture(const Leap::Pointable &p)
{
//    if (!slide->isDone())
//        slide->next();
}

void MainContentComponent::timerCallback(int timerId)
{
//    Harp* h = HarpManager::instance().getHarp(0);
//    if (h->checkIdle())
//        slide->begin();
//    else {
//        stopTimer();
//        startTimer(TUTORIAL_TIMEOUT);
//    }
    switch (timerId) {
        case kTimerShowTutorial:
            tutorial->setVisible(true, 2000);
            stopTimer(kTimerShowTutorial);
            for (HUDView* v : views)
                v->setVisible(false);
            for (HarpView* hv : harps)
                hv->setVisible(false);
            HarpManager::instance().getHarp(0)->setEnabled(false);
            break;
        case kTimerWaitingForConnection:
            if (!connected)
                leapDisconnectedView->setVisible(true);
            else
                stopTimer(kTimerWaitingForConnection);
            break;
        case kFullscreenTipTimer:
            fullscreenTipView->setVisible(false, 2000);
            stopTimer(kFullscreenTipTimer);
            break;
        default:
            break;
    }
}

void MainContentComponent::handleMessage(const juce::Message &m)
{
    Message* inMsg = const_cast<Message*>(&m);
    
    InitGLMessage* initGLMessage = dynamic_cast<InitGLMessage*>(inMsg);
    if (initGLMessage)
    {
        if ((Time::getCurrentTime() - startTime).inMilliseconds() < MIN_SPLASH_DURATION) {
            postMessage(new InitGLMessage);
        }
        else {
            Logger::writeToLog("Got InitGLMessage");
            openGLContext.setRenderer (this);
            openGLContext.setComponentPaintingEnabled (true);
            openGLContext.attachTo (*this);
        }
    }
}


void MainContentComponent::actionListenerCallback(const String& message)
{
    if (message == "playMode")
    {
        settingsScreen->setVisible(false);
    }
    else if (message == "settingsMode")
    {
        settingsScreen->setVisible(true);
    }
    else if (message == "tutorialDone")
    {
        splashBgView->setVisible(false, 1000);
        for (HUDView* v : views)
            v->setVisible(true);
        for (ChordRegion* cr : chordRegions)
            cr->setVisible(false);
        chordRegionsNeedUpdate = true;
        settingsScreen->setVisible(false, 0);
        for (HarpView* hv : harps)
            hv->setVisible(true);
        HarpManager::instance().getHarp(0)->setEnabled(true);
    }
    else if (message == "disableTutorial")
    {
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("showTutorial", false);
    }
    else if (message == "showHelp")
    {
        showTutorial();
    }
    else if (message == "chordMode")
    {
        Harp* h = HarpManager::instance().getHarp(0);
        h->setChordMode(true);
        toolbar->updateButtons();
        chordRegionsNeedUpdate = true;
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("chordMode", true);
        harps.at(0)->setNumStrings(h->suggestedStringCount());
    }
    else if (message == "scaleMode")
    {
        Harp* h = HarpManager::instance().getHarp(0);
        h->setChordMode(false);
        int selectedScale = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("selectedScale", 0);
        h->SetScale(selectedScale);
        toolbar->updateButtons();
        for (ChordRegion* cr : chordRegions)
            cr->setVisible(false);
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("chordMode", false);
        harps.at(0)->setNumStrings(h->suggestedStringCount());
    }
    else if (message == "scaleChanged")
    {
        harps.at(0)->setNumStrings(HarpManager::instance().getHarp(0)->suggestedStringCount());
    }
}

void MainContentComponent::showTutorial()
{
    HarpManager::instance().getHarp(0)->setEnabled(false);
    AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("showTutorial", true);
    tutorial->setVisible(true);
    splashBgView->setVisible(true);
    for (HUDView* v : views)
        v->setVisible(false);
    for (HarpView* hv : harps)
        hv->setVisible(false);
    showFullscreenTip();
}

void MainContentComponent::showFullscreenTip()
{
    fullscreenTipView->setVisible(true, 2000);
    startTimer(kFullscreenTipTimer, 2000 + 2000);
}