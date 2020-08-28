#ifndef _MAINPAGE_H_
#define _MAINPAGE_H_

#include "Menu/Menus.h"

class MainPage : public Page {
    private:
    int lastCounter = -1;

    Text uiPanelText = { "0" };

    public:
    UIObject* localObjects[MAX_OBJECTS_ON_PAGE] = {
        new UIButton("X-", { 0, 5, 115, 115 }, { 25, 25 }, "X-", BLACK, WHITE_L_80, WHITE_L_5, [] {
            ChangeCounter(false);
        }),
        new UITextPanel("X+", { 125, 5, 225, 115 }, { 25, 25 }, uiPanelText, BLACK, WHITE_L_80),
        new UIButton("X+", { 360, 5, 115, 115 }, { 25, 25 }, "X+", BLACK, WHITE_L_80, WHITE_L_5, [] {
            ChangeCounter(true);
        }),

        new UIPanel("Y+", { 0, 130, 115, 115 }, BLUE),
        new UIPanel("Y-", { 360, 130, 115, 115 }, BLUE),
        new UIPanel("Text", { 125, 130, 225, 115 }, BLUE),
        NULL,
    };

    MainPage(char* getName) {
        Name = getName;
        
        for (int i = 0; i < MAX_OBJECTS_ON_PAGE; i++) {
            UIObjects[i] = localObjects[i];
        }
    }

    void Awake() override {
        
    }

    void Start() override {
        RepaintAll();
        UpdateText();
    }

    void Update() override {
        if (lastCounter != Counter) UpdateText();

        lastCounter = Counter;

        for (int i = 0; i < MAX_OBJECTS_ON_PAGE; i++) {
            if (UIObjects[i] == NULL) break;
            UIObjects[i]->Update();
        }
    }

    void UpdateText() {
        uiPanelText.SetText(Counter);
        UIObjects[2]->Repaint();
    }
};

#endif