
#ifndef APIDEMONSTRATION_H
#define APIDEMONSTRATION_H

#include <boost/shared_ptr.hpp>
#include <alcommon/almodule.h>

namespace AL{
  // This is a forward declaration of AL:ALBroker which
  // avoids including <alcommon/albroker.h> in this header
  class ALBroker;
}

/**
 * This class inherits AL::ALModule. This allows it to bind methods
 * and be run as a remote executable or as a plugin within NAOqi
 */
class APIDemonstration : public AL::ALModule{
  public:
    APIDemonstration(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName);

    virtual ~APIDemonstration();

    /** Overloading ALModule::init().
    * This is called right after the module has been loaded
    */
    virtual void init();
	
	void different_postures();


};
#endif // APIDEMONSTRATION_H

