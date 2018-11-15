#include "neo.h"
#include "pumps.h"
#include <Arduino.h>
#include "common.h"
#include <NeoPixelAnimator.h>

#define LIGHT_JSON_TEMPLATE "{\"brightness\" : %d, \"color\" : {\"r\":%d, \"g\":%d, \"b\":%d}, \"state\" : \"%s\"}"
const uint16_t PixelCount = 13;
const uint8_t PixelPin = 2;
const uint8_t AnimationChannels = 1; // we only need one as all the pixels are animated at once

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
NeoPixelAnimator animations(AnimationChannels);

unsigned long animationStartTime = 0;
lightState contextLightState = {true, RgbColor(255, 255, 255), 220};

struct MyAnimationState
{
    RgbColor StartingColor;
    RgbColor EndingColor;
};

MyAnimationState animationState[AnimationChannels];

void blendAnimUpdate(const AnimationParam &param)
{
    RgbColor updatedColor = RgbColor::LinearBlend(
        animationState[param.index].StartingColor,
        animationState[param.index].EndingColor,
        param.progress);

    for (uint16_t pixel = 0; pixel < PixelCount; pixel++)
    {
        strip.SetPixelColor(pixel, updatedColor);
    }
}

void fadeToIdleColor()
{
    uint16_t time = 200;
    RgbColor c = contextLightState.idleColor;
    c.Darken(255 - contextLightState.brightness);
    animationState[0].StartingColor = strip.GetPixelColor(0);
    animationState[0].EndingColor = c;
    animations.StartAnimation(0, time, blendAnimUpdate);
}

void fadeToBlack()
{
    const uint16_t time = 200;
    animationState[0].StartingColor = strip.GetPixelColor(0);
    animationState[0].EndingColor = RgbColor(0, 0, 0);
    animations.StartAnimation(0, time, blendAnimUpdate);
}

void fadeToRandomColor(float luminance)
{
    RgbColor target = HslColor(random(360) / 360.0f, 1.0f, luminance);
    uint16_t time = 500;

    animationState[0].StartingColor = strip.GetPixelColor(0);
    animationState[0].EndingColor = target;

    animations.StartAnimation(0, time, blendAnimUpdate);
}

void setupNeo()
{
    strip.Begin();
    fadeToIdleColor();
}

void loopNeo()
{
    if (animations.IsAnimating())
    {
        animations.UpdateAnimations();
        strip.Show();
        return;
    }
    if (!contextLightState.state)
    {
        return;
    }
    if (animationStartTime == 0)
    {
        return;
    }
    fadeToRandomColor(0.5f);
}

void startCupAnimation()
{
    if (animationStartTime > 0)
    {
        return;
    }
    animationStartTime = millis();
}

void stopCupAnimation()
{
    if (animationStartTime == 0)
    {
        return;
    }
    animations.StopAll();
    animationStartTime = 0;
    if (contextLightState.state)
    {
        fadeToIdleColor();
    }
    else
    {
        fadeToBlack();
    }
}

void refreshNeo()
{
    animations.StopAll();
    animationStartTime = 0;
    if (contextLightState.state)
    {
        fadeToIdleColor();
    }
    else
    {
        fadeToBlack();
    }
}

String idleColorAsJson()
{
    char temp[100];
    RgbColor c = contextLightState.idleColor;
    const char *s = "OFF";
    if (contextLightState.state)
    {
        s = "ON";
    }
    sprintf(temp, LIGHT_JSON_TEMPLATE, contextLightState.brightness, c.R, c.G, c.B, s);
    return temp;
}