#include "apidemonstration.h"

#include <iostream>
#include <string>

#include <alcommon/albroker.h>
#include <alerror/alerror.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/alnavigationproxy.h>
#include <alproxies/alphotocaptureproxy.h>
#include <alproxies/alrobotpostureproxy.h>
#include <alproxies/alvideodeviceproxy.h>
#include <althread/alcriticalsection.h>
#include <alvalue/alvalue.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <qi/log.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

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

    functionName("onRightBumperPressed", getName(), "Method called when the right bumper is pressed. Makes Nao speak");
    BIND_METHOD(APIDemonstration::onRightBumperPressed);

    // If you had other methods, you could bind them here...
    /** Bound methods can only take const ref arguments of basic types,
    * or AL::ALValue or return basic types or an AL::ALValue.
    */
}

APIDemonstration::~APIDemonstration() {
    if (fCallbackMutex != 0) 
        unsubscribe_to_event();
}

void APIDemonstration::init() {
    /** Init is called just after construction.**/
    try {
        memory_proxy = ALMemoryProxy(getParentBroker());
        TTS_proxy = ALTextToSpeechProxy(getParentBroker());
    }
    catch(const AL::ALError&) {
        qiLogError("module.example") << "Proxy initialization failed." << std::endl;
    }
    //show_postures();
    //move_navigation(1.0);
    take_picture("lol");
    //disagree();
    //say_phrase("Here we go!", "English");
    //say_phrase("Bonjour! Un, deux, trois, Je ne sais pas", "French");
    //subscribe_to_event();
    //get_visual();
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
    const std::string folderPath = "/home/nao/recordings/cameras/";
    const std:: string fileName = filename + ".jpg";
    ALPhotoCaptureProxy cam_proxy(getParentBroker());
    try {
        AL::ALValue ret = cam_proxy.takePicture(folderPath, fileName);
        std::cout << ret << std::endl;
    }
    catch(const ALError& e) {
        qiLogError("module.example") << e.what() << std::endl;
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
        TTS_proxy.setLanguage(language);
        TTS_proxy.say(phrase);
    }
    catch(const AL::ALError&) {
        qiLogError("module.example") << "Could not get proxy to ALTextToSpeechProxy" << std::endl;
    }
}

void APIDemonstration::subscribe_to_event() {
    qiLogInfo("module.example") << "Subscribing to RightBumperPressed event." << std::endl;
    try {
        //fState = memory_proxy.getData("RightBumperPressed");

        // Do not forget to initialize this mutex.
        fCallbackMutex = AL::ALMutex::createALMutex();
        /** Subscribe to event LeftBumperPressed
        * Arguments:
        * - name of the event
        * - name of the module to be called for the callback
        * - name of the bound method to be called on event
        */
        memory_proxy.subscribeToEvent("RightBumperPressed", getName(), "onRightBumperPressed");
    }
    catch (const AL::ALError& e) {
        qiLogError("module.example") << e.what() << std::endl;
    }
}

void APIDemonstration::unsubscribe_to_event() {
    qiLogInfo("module.example") << "Unsubscribing to RightBumperPressed event." << std::endl;
    memory_proxy.unsubscribeToEvent("RightBumperPressed", getName());
}

void APIDemonstration::onRightBumperPressed() {
    qiLogInfo("module.example") << "Executing callback method on right bumper event" << std::endl;
    /**
    * As long as this is defined, the code is thread-safe.
    */
    ALCriticalSection section(fCallbackMutex);
    /**
    * Check that the bumper is pressed.
    */
    float fState =  memory_proxy.getData("RightBumperPressed");
    if (fState > 0.5f) {
        return;
    }
    try {
        TTS_proxy.say("Right bumper pressed");
    }
    catch (const AL::ALError& e) {
        qiLogError("module.example") << e.what() << std::endl;
    }
}

void APIDemonstration::get_visual() {
    ALVideoDeviceProxy cam_proxy(getParentBroker());
    const std::string clientName = cam_proxy.subscribe("test", kQVGA, kBGRColorSpace, 30);

    cv::Mat imgHeader = cv::Mat(cv::Size(320, 240), CV_8UC3);
    cv::namedWindow("images");

    /** Main loop. Exit when pressing ESC.*/
    while ((char) cv::waitKey(30) != 27)
    {
        /** Retrieve an image from the camera.
        * The image is returned in the form of a container object, with the
        * following fields:
        * 0 = width
        * 1 = height
        * 2 = number of layers
        * 3 = colors space index (see alvisiondefinitions.h)
        * 4 = time stamp (seconds)
        * 5 = time stamp (micro seconds)
        * 6 = image buffer (size of width * height * number of layers)
        */
        ALValue img = cam_proxy.getImageRemote(clientName);

        /** Access the image buffer (6th field) and assign it to the opencv image
        * container. */
        imgHeader.data = (uchar*) img[6].GetBinary();

        /** Tells to ALVideoDevice that it can give back the image buffer to the
        * driver. Optional after a getImageRemote but MANDATORY after a getImageLocal.*/
        cam_proxy.releaseImage(clientName);

        /** Display the iplImage on screen.*/
        cv::imshow("images", imgHeader);
    }

    /** Cleanup.*/
    cam_proxy.unsubscribe(clientName);
}