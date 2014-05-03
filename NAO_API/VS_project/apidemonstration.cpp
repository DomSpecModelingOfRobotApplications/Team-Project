#include "apidemonstration.h"

#include <iostream>
#include <string>
#include <vector>

#include <alcommon/albroker.h>
#include <alerror/alerror.h>
#include <althread/alcriticalsection.h>
#include <alvalue/alvalue.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <qi/log.hpp>
#include <qi/os.hpp>

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

    functionName("move", getName(), "Nao walks 2 metres.");
    BIND_METHOD(APIDemonstration::move);

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

    functionName("reverence", getName(), "Makes a reverence");
    BIND_METHOD(APIDemonstration::reverence);

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
    }
    catch(const AL::ALError&) {
        qiLogError("module.example") << "Memory proxy initialization failed." << std::endl;
    }
    try {
        TTS_proxy = ALTextToSpeechProxy(getParentBroker());
    }
    catch(const AL::ALError&) {
        qiLogError("module.example") << "TTS proxy initialization failed." << std::endl;
    }
    try {
        navigation_proxy = ALNavigationProxy(getParentBroker());
    }
    catch(const AL::ALError&) {
        qiLogError("module.example") << "Navigation proxy initialization failed." << std::endl;
    }
    try {
        video_proxy = ALVideoDeviceProxy(getParentBroker());
    }
    catch(const AL::ALError&) {
        qiLogError("module.example") << "Video initialization failed." << std::endl;
    }
    qiLogInfo("module.example") << "Initialization complete!" << std::endl;
    haste = 0.7f;
    script();
}

void APIDemonstration::rest() {
    posture_proxy.goToPosture("Crouch", haste);
    motion_proxy.rest();
}

void APIDemonstration::wake_up() {
    motion_proxy.wakeUp();
    posture_proxy.goToPosture("StandInit", haste);
}

void APIDemonstration::show_postures() {
    posture_proxy.goToPosture("StandInit", haste);
    posture_proxy.goToPosture("StandZero", haste);
    posture_proxy.goToPosture("Crouch", haste);
    posture_proxy.goToPosture("Sit", haste);
    posture_proxy.goToPosture("SitRelax", haste);
    posture_proxy.goToPosture("LyingBelly", haste);
    posture_proxy.goToPosture("LyingBack", haste);
    posture_proxy.goToPosture("Stand", haste);
    qiLogInfo("module.example") << "Done !" << std::endl;
}

void APIDemonstration::posture(const std::string& posture_name) {
    std::string known_postures[] = {
        "Sit", 
        "SitRelax",
        "Stand",
        "StandInit",
        "StandZero",
        "Crouch",
        "LyingBelly",
        "LyingBack"
    };
    for (int i = 0; i < 8; ++i)
        if (known_postures[i] == posture_name) {
            posture_proxy.goToPosture(posture_name, haste);
            return;
        }
    qiLogInfo("module.example") << "Unknown posture!" << std::endl;
}

void APIDemonstration::move(const float& x, const float& y, const float& theta) {
    posture_proxy.goToPosture("StandInit", 1.0);

    motion_proxy.moveInit();
    motion_proxy.moveTo(x, y, theta, ALValue::array(ALValue::array("MaxStepX", 0.2f)));
}

bool APIDemonstration::move_navigation(const float& x, const float& y, const float& theta) {
    posture_proxy.goToPosture("StandInit", 1.0);

    bool obstacle_met = !navigation_proxy.moveTo(x, y, theta);
    std::cout << (obstacle_met ? "Met an obstacle" : "Navigation complete") << std::endl;
    return obstacle_met;
}

void APIDemonstration::take_picture(const std::string& filename) {
    /** HINT: 
    * Webots does not have this module and the programm crashes if
    * this photo_proxy is declared as a static variable.
    */
    AL::ALPhotoCaptureProxy photo_proxy;
    try {
        ALPhotoCaptureProxy photo_proxy = ALPhotoCaptureProxy(getParentBroker());
    }
    catch(const AL::ALError&) {
        qiLogError("module.example") << "Photo proxy initialization failed." << std::endl;
    }

    const std::string folderPath = "/home/nao/recordings/cameras/";
    const std:: string fileName = filename + ".jpg";
    
    try {
        AL::ALValue ret = photo_proxy.takePicture(folderPath, fileName);
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
        AL::ALValue stiffness = 1.0f;
        AL::ALValue time = 1.0f;
        motion_proxy.stiffnessInterpolation(jointName, stiffness, time);

        AL::ALValue targetAngles = AL::ALValue::array(-1.5f, 1.5f, 0.0f);
        AL::ALValue targetTimes = AL::ALValue::array(3.0f, 6.0f, 9.0f);
        bool isAbsolute = true;
        motion_proxy.angleInterpolation(jointName, targetAngles, targetTimes, isAbsolute);

        stiffness = 0.0f;
        time = 1.0f;
        motion_proxy.stiffnessInterpolation(jointName, stiffness, time);
    }
    catch (const AL::ALError& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }
}

void APIDemonstration::agree()
{
    const AL::ALValue jointName = "HeadPitch";
    try {
        AL::ALValue stiffness = 1.0f;
        AL::ALValue time = 1.0f;
        motion_proxy.stiffnessInterpolation(jointName, stiffness, time);

        AL::ALValue targetAngles = AL::ALValue::array(-0.5f, 1.5f, 0.0f);
        AL::ALValue targetTimes = AL::ALValue::array(3.0f, 6.0f, 9.0f);
        bool isAbsolute = true;
        motion_proxy.angleInterpolation(jointName, targetAngles, targetTimes, isAbsolute);

        stiffness = 0.0f;
        time = 1.0f;
        motion_proxy.stiffnessInterpolation(jointName, stiffness, time);
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
    const std::string clientName = video_proxy.subscribe("test", kQVGA, kBGRColorSpace, 30);

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
        try {
            ALValue img = video_proxy.getImageRemote(clientName);

            /** Access the image buffer (6th field) and assign it to the opencv image
            * container. */
            imgHeader.data = (uchar*) img[6].GetBinary();

            /** Tells to ALVideoDevice that it can give back the image buffer to the
            * driver. Optional after a getImageRemote but MANDATORY after a getImageLocal.*/
            video_proxy.releaseImage(clientName);

            /** Display the iplImage on screen.*/
            cv::imshow("images", imgHeader);
        }
        catch (const AL::ALError& e) {
            qiLogError("module.example") << e.what() << std::endl;
        }
    }

    /** Cleanup.*/
    cv::destroyWindow("images");
    video_proxy.unsubscribe(clientName);
}

void APIDemonstration::not_these_droids() {
    AL::ALValue joints = AL::ALValue::array(
        "LShoulderPitch", 
        "LElbowRoll", 
        "LElbowYaw", 
        "LWristYaw",
        "LHand"
    );

    bool useSensors = false;
    std::vector<float> angles_before = motion_proxy.getAngles(joints, useSensors);
    std::vector<float> stiffness_before = motion_proxy.getStiffnesses(joints);

    motion_proxy.setStiffnesses(joints, AL::ALValue::array(1.0, 1.0, 1.0, 1.0, 1.0));

    TTS_proxy.setLanguage("English");
    TTS_proxy.post.say("These aren't the droids you're looking for.");

    AL::ALValue target_angles = AL::ALValue::array(
        AL::ALValue::array(0.8, 0.5),
        AL::ALValue::array(-1.3, -1.3),
        AL::ALValue::array(-0.5, -1.5),
        AL::ALValue::array(0.9, 1.4),
        AL::ALValue::array(0.5, 0.8)
    );
    float time = 5.0;
    AL::ALValue target_times = AL::ALValue::array(
        AL::ALValue::array(2.0, time),
        AL::ALValue::array(2.0, time),
        AL::ALValue::array(2.0, time),
        AL::ALValue::array(2.0, time),
        AL::ALValue::array(2.0, time)
    );
    bool isAbsolute = true;
    motion_proxy.angleInterpolation(joints, target_angles, target_times, isAbsolute);

    //qi::os::sleep(1.0f);

    motion_proxy.angleInterpolation(joints, angles_before, AL::ALValue::array(1.0, 1.0, 1.0, 1.0, 1.0), true);
    motion_proxy.setStiffnesses(joints, stiffness_before);
}

void APIDemonstration::reverence() {
    AL::ALValue joints = AL::ALValue::array(
        "HeadPitch",
        "LShoulderPitch", 
        "LElbowRoll", 
        "LElbowYaw",
        "LHipYawPitch"
    );

    bool useSensors = false;
    posture_proxy.goToPosture("Stand", haste);
    std::vector<float> angles_before = motion_proxy.getAngles(joints, useSensors);
    std::vector<float> stiffness_before = motion_proxy.getStiffnesses(joints);

    motion_proxy.setStiffnesses(joints, AL::ALValue::array(1.0, 1.0, 1.0, 1.0, 1.0));
   
    TTS_proxy.setLanguage("English");
    TTS_proxy.post.say("Good evening.");

    AL::ALValue target_angles = AL::ALValue::array(
        AL::ALValue::array(0.5),
        AL::ALValue::array(-0.04),
        AL::ALValue::array(-1.3),
        AL::ALValue::array(0.07),
        AL::ALValue::array(-0.7)
    );

    float time = 5.0;
    AL::ALValue target_times = AL::ALValue::array(
        AL::ALValue::array(time),
        AL::ALValue::array(time),
        AL::ALValue::array(time),
        AL::ALValue::array(time),
        AL::ALValue::array(time)
    );

    bool isAbsolute = true;
    motion_proxy.angleInterpolation(joints, target_angles, target_times, isAbsolute);
    motion_proxy.angleInterpolation(joints, angles_before, AL::ALValue::array(1.0, 1.0, 1.0, 1.0, 1.0), true);
    motion_proxy.setStiffnesses(joints, stiffness_before);
}