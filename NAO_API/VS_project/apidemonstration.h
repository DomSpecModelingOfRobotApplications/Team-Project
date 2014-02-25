#ifndef APIDEMONSTRATION_H
#define APIDEMONSTRATION_H

#include <boost/shared_ptr.hpp>
#include <alcommon/almodule.h>

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
        void move_basic();
        /** IMPORTANT: 
        * If you want to make a method publicly available
        * through BIND_METHOD, all arguments should be typed
        * according to "const T&".
        */
        bool move_navigation(const float& dist);
        void take_picture(const std::string& filename);
        void disagree();
        void say_phrase(const std::string& phrase, const std::string& language = "English");
};
#endif // APIDEMONSTRATION_H
