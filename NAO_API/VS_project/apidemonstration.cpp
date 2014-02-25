#include "apidemonstration.h"

#include <iostream>
#include <string>

#include <alcommon/albroker.h>
#include <qi/log.hpp>
#include <alproxies/alrobotpostureproxy.h>
#include <alproxies/alnavigationproxy.h>
#include <alproxies/almotionproxy.h>
#include <alvalue/alvalue.h>
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <alerror/alerror.h>
#include <alproxies/alphotocaptureproxy.h>
#include <alproxies/altexttospeechproxy.h>

using namespace AL;

APIDemonstration::APIDemonstration(boost::shared_ptr<ALBroker> broker, const std::string& name):
        ALModule(broker, name) {
    /** Describe the module here. This will appear on the webpage*/
    setModuleDescription("An API demonstration module.");

    /** Define callable methods with their descriptions:
    * This makes the method available to other cpp modules
    * and to python.
    * The name given will be the one visible from outside the module.
    * This method has no parameters or return value to describe
    */
    functionName("show_postures", getName(), "Shows some postures.");
    BIND_METHOD(APIDemonstration::show_postures);

    functionName("move_basic", getName(), "Nao walks 2 metres.");
    BIND_METHOD(APIDemonstration::move_basic);

    functionName("move_navigation", getName(),
                "Nao walks given distance or stops before an obstacle.");
    addParam("dist", "The distance to walk.");

    functionName("say_basic", getName(), "Nao says a phrase.");
    BIND_METHOD(APIDemonstration::say_phrase);
    addParam("phrase", "The phrase to say.");
    ///**
    //* This enables to document the return of the method.
    //* It is not compulsory to write this line.
    //*/
    setReturn("obstacle_met", "Is true if Nao encountered an obstacle.");
    BIND_METHOD(APIDemonstration::move_navigation);

    // If you had other methods, you could bind them here...
    /** Bound methods can only take const ref arguments of basic types,
    * or AL::ALValue or return basic types or an AL::ALValue.
    */
}

APIDemonstration::~APIDemonstration() {}

void APIDemonstration::init() {
    /** Init is called just after construction.**/
    //show_postures();
    //move_navigation(1.0);
    //take_picture("lol");
    //disagree();
    //say_phrase("Here we go!", "English");
    //say_phrase("Bonjour! Un, deux, trois, Je ne sais pas", "French");
}

void APIDemonstration::show_postures() {
    try {
        ALRobotPostureProxy robotPosture(getParentBroker());
        robotPosture.goToPosture("StandInit", 0.5f);
        robotPosture.goToPosture("StandZero", 0.5f);
        robotPosture.goToPosture("Crouch", 0.5f);
        robotPosture.goToPosture("Sit", 0.5f);
        robotPosture.goToPosture("SitRelax", 0.5f);
        robotPosture.goToPosture("LyingBelly", 0.5f);
        robotPosture.goToPosture("LyingBack", 0.5f);
        robotPosture.goToPosture("Stand", 0.5f);
    }
    catch(const AL::ALError&) {
        qiLogError("module.example") << "Could not get proxy to ALRobotPostureProxy" << std::endl;
    }
    qiLogInfo("module.example") << "Done !" << std::endl;
}

void APIDemonstration::move_basic() {
    ALRobotPostureProxy robotPosture(getParentBroker());
    robotPosture.goToPosture("StandInit", 1.0);

    ALMotionProxy motion(getParentBroker());
    motion.moveInit();
    motion.moveTo(2.0f, 0.0f, 0.0f, ALValue::array(ALValue::array("MaxStepX", 0.2f)));
}

bool APIDemonstration::move_navigation(const float& dist) {
    ALRobotPostureProxy robotPosture(getParentBroker());
    robotPosture.goToPosture("StandInit", 1.0);

    ALNavigationProxy motion(getParentBroker());
    bool obstacle_met = !motion.moveTo(dist, 0.0, 0);
    std::cout << (obstacle_met ? "Met an obstacle" : "Navigation complete") << std::endl;
    return obstacle_met;
}

void APIDemonstration::take_picture(const std::string& filename) {
    const std::string folderPath = "D:";
    const std:: string fileName = "NAO_picture.jpg";
    ALPhotoCaptureProxy camProxy(getParentBroker());
    try {
        AL::ALValue ret = camProxy.takePicture(folderPath, fileName);
        std::cout << ret << std::endl;
    }
    catch(const ALError&) {
        qiLogError("module.example") << "IMmna no photographer" << std::endl;
    }

}

void APIDemonstration::disagree()
{
    const AL::ALValue jointName = "HeadYaw";
    try {
        ALMotionProxy motion(getParentBroker());

        AL::ALValue stiffness = 1.0f;
        AL::ALValue time = 1.0f;
        motion.stiffnessInterpolation(jointName, stiffness, time);

        AL::ALValue targetAngles = AL::ALValue::array(-1.5f, 1.5f, 0.0f);
        AL::ALValue targetTimes = AL::ALValue::array(3.0f, 6.0f, 9.0f);
        bool isAbsolute = true;
        motion.angleInterpolation(jointName, targetAngles, targetTimes, isAbsolute);

        stiffness = 0.0f;
        time = 1.0f;
        motion.stiffnessInterpolation(jointName, stiffness, time);
    }
    catch (const AL::ALError& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }
}

void APIDemonstration::say_phrase(const std::string& phrase, const std::string& language) {
    try {
        ALTextToSpeechProxy robotSpeech(getParentBroker());
        robotSpeech.setLanguage(language);
        robotSpeech.say(phrase);
    }
    catch(const AL::ALError&) {
        qiLogError("module.example") << "Could not get proxy to ALTextToSpeechProxy" << std::endl;
    }
}