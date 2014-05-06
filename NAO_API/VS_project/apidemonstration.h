#ifndef APIDEMONSTRATION_H
#define APIDEMONSTRATION_H

#include <boost/shared_ptr.hpp>
#include <alcommon/almodule.h>
#include <string>

#include <alproxies/almemoryproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/alnavigationproxy.h>
#include <alproxies/alphotocaptureproxy.h>
#include <alproxies/alrobotpostureproxy.h>
#include <alproxies/alvideodeviceproxy.h>
#include <althread/almutex.h>

namespace AL {
    // This is a forward declaration of AL:ALBroker which
    // avoids including <alcommon/albroker.h> in this header
    class ALBroker;
}

/**
 * This class inherits AL::ALModule. This allows it to bind methods
 * and be run as a remote executable or as a plugin within NAOqi
 */
class APIDemonstration : public AL::ALModule {
public:
    APIDemonstration(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName);

    virtual ~APIDemonstration();

    /** Overloading ALModule::init().
    * This is called right after the module has been loaded
    */
    virtual void init();

    void show_postures();
    void move(const float& x, const float& y, const float& theta);
    /** IMPORTANT: 
    * If you want to make a method publicly available
    * through BIND_METHOD, all arguments should be typed
    * according to "const T&".
    */
    bool move_navigation(const float& x, const float& y, const float& theta);
    void rest();
    void wake_up();
    void posture(const std::string& posture_name);
    void take_picture(const std::string& filename);
    void disagree();
    void agree();
    void say_phrase(const std::string& phrase, const std::string& language = "English");
    void move_joints(const AL::ALValue& joints,
                     const AL::ALValue& target_angles,
                     const AL::ALValue& target_times,
                     const bool &restore_pos = false,
                     const std::string& phrase = "",
                     const float& phrase_lag = 0.0f);
    void not_these_droids();
    void bow();

    void script();

    void on_right_bumper_pressed();
    void subscribe_to_event();
    void unsubscribe_to_event();
    void onRightBumperPressed();
    void get_visual();

private:
    AL::ALMemoryProxy       memory_proxy;
    AL::ALTextToSpeechProxy TTS_proxy;
    AL::ALRobotPostureProxy posture_proxy;
    AL::ALMotionProxy       motion_proxy;
    AL::ALNavigationProxy   navigation_proxy;
    //AL::ALPhotoCaptureProxy photo_proxy;
    AL::ALVideoDeviceProxy  video_proxy;

    float haste;

    boost::shared_ptr<AL::ALMutex> fCallbackMutex;
};
#endif // APIDEMONSTRATION_H
