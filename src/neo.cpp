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

uint16_t effectState = 0;  
unsigned long animationStartTime = 0;
lightState contextLightState = {true,RgbColor(255,255,255),220};

struct MyAnimationState
{
    RgbColor StartingColor;
    RgbColor EndingColor;
};

MyAnimationState animationState[AnimationChannels];


// simple blend function
void BlendAnimUpdate(const AnimationParam& param)
{
    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
    RgbColor updatedColor = RgbColor::LinearBlend(
        animationState[param.index].StartingColor,
        animationState[param.index].EndingColor,
        param.progress);

    // apply the color to the strip
    for (uint16_t pixel = 0; pixel < PixelCount; pixel++)
    {
        strip.SetPixelColor(pixel, updatedColor);
    }
}

void fadeToIdleColor() {
    // write_to_log("fading to idle color");
    uint16_t time = 200;
    RgbColor c = contextLightState.idleColor;
    c.Darken(255 - contextLightState.brightness);
    animationState[0].StartingColor = strip.GetPixelColor(0);
    animationState[0].EndingColor = c;
    animations.StartAnimation(0, time, BlendAnimUpdate);
}

void fadeToBlack() {
    // write_to_log("fading to black");
    const uint16_t time = 200;
    animationState[0].StartingColor = strip.GetPixelColor(0);
    animationState[0].EndingColor = RgbColor(0,0,0);
    animations.StartAnimation(0, time, BlendAnimUpdate);
}

void FadeInFadeOutRinseRepeat(float luminance)
{
    if (effectState == 0)
    {
        // Fade upto a random color
        // we use HslColor object as it allows us to easily pick a hue
        // with the same saturation and luminance so the colors picked
        // will have similiar overall brightness
        RgbColor target = HslColor(random(360) / 360.0f, 1.0f, luminance);
        uint16_t time = 500;

        animationState[0].StartingColor = strip.GetPixelColor(0);
        animationState[0].EndingColor = target;

        animations.StartAnimation(0, time, BlendAnimUpdate);
    }
    else if (effectState == 1)
    {
        // fade to black
        uint16_t time = random(50, 60);

        animationState[0].StartingColor = strip.GetPixelColor(0);
        animationState[0].EndingColor = RgbColor(0);

        animations.StartAnimation(0, time, BlendAnimUpdate);
    }

    // toggle to the next effect state
    effectState = (effectState + 1) % 2;
}




void setupNeo()
{
    strip.Begin();
    strip.ClearTo(RgbColor(0));
    strip.Show();
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
    if (!contextLightState.state) {
        return;
    }
    if (animationStartTime == 0) {
        return;
    }
    FadeInFadeOutRinseRepeat(0.5f); // 0.0 = black, 0.25 is normal, 0.5 is bright
}

void startCupAnimation() {
    if (animationStartTime > 0) {
        return;
    }
    effectState = 0;
    animationStartTime = millis();
}


void stopCupAnimation() {
    if (animationStartTime ==0) {
        return;
    }
    animations.StopAll();
    animationStartTime = 0;
    if (contextLightState.state) {
        fadeToIdleColor();
    } else {
        fadeToBlack();
    }
}


void refreshNeo() {
    animations.StopAll();
    animationStartTime = 0;
    effectState = 0;
    if (contextLightState.state) {
        fadeToIdleColor();
    } else {
        fadeToBlack();
    }
}

String idleColorAsJson() {
    char temp[100];
    RgbColor c = contextLightState.idleColor;
    const char* s = "OFF";
    if (contextLightState.state) {
        s = "ON";
    }
    sprintf(temp,LIGHT_JSON_TEMPLATE,contextLightState.brightness, c.R,c.G,c.B,s);
    return temp;    
}