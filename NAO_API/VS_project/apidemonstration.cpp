#include "apidemonstration.h"
#include <iostream>
#include <alcommon/albroker.h>
#include <qi/log.hpp>
#include <alproxies/alrobotpostureproxy.h>
#include <alproxies/alnavigationproxy.h>
#include <alproxies/almotionproxy.h>
#include <alvalue/alvalue.h>

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
    show_postures();
    move_navigation(2.0);
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
    bool obstacle_met = motion.moveTo(dist, 0.0, 0);
    std::cout << (obstacle_met ? "Met an obstacle" : "Navigation complete") << std::endl;
    return obstacle_met;
}