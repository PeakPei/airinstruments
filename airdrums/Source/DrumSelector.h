#ifndef h_DrumSelector
#define h_DrumSelector

#include "HUD.h"


class DrumSelector : public HUDView
                   , public Timer
{
public:
    DrumSelector();
    ~DrumSelector();
    
    // HUDView overrides
    void setup();
    void draw();
    void loadTextures();
    void setBounds(const HUDRect& b);
    void fingerMotion(float x, float y, FingerView* fv);
    void fingerEntered(float x, float y, FingerView* fv);
    void fingerExited(float x, float y, FingerView* fv);

    // Juce::Timer override
    void timerCallback();

    void setSelection(int sel);
    int getSelection() const { return selection; }

    class Icon : public HUDView
    {
    public:
        Icon(int id);
        ~Icon();
        void setup();
        void draw();
        void loadTextures();
        void setBounds(const HUDRect& b);
    private:
        void updateBounds();
        
        GLBatch batch;
        int id;
        HUDRect targetBounds;
        HUDRect tempBounds;
        float xStep, yStep, wStep, hStep;
    };

    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void drumSelectorChanged(DrumSelector* selector) = 0;
    };
    
    void addListener(Listener* listener);
    void removeListener(Listener* listener);

private:
    void layoutIcons();

    GLBatch batch;
    GLuint textureID;
    std::vector<Icon*> icons;
    int selection;
    bool needsLayout;
    float prevFingerX;
    float prevFingerY;
    FingerView* trackedFinger;
    std::vector<Listener*> listeners;
};

#endif // h_DrumSelector