
#include "apidemonstration.h"
#include <iostream>
#include <alcommon/albroker.h>
#include <qi/log.hpp>
#include <alproxies/alrobotpostureproxy.h>

using namespace AL;

APIDemonstration::APIDemonstration(boost::shared_ptr<ALBroker> broker, const std::string& name):
  ALModule(broker, name){
  /** Describe the module here. This will appear on the webpage*/
  setModuleDescription("An API demonstration module.");

  /** Define callable methods with their descriptions:
  * This makes the method available to other cpp modules
  * and to python.
  * The name given will be the one visible from outside the module.
  * This method has no parameters or return value to describe
  */
  functionName("different_postures", getName(), "Different postures");
  BIND_METHOD(APIDemonstration::different_postures);

//  functionName("sayText", getName(), "Say a given sentence.");
  /**
  * This enables to document the parameters of the method.
  * It is not compulsory to write this line.
  */
//  addParam("toSay", "The sentence to be said.");
//  BIND_METHOD(HelloWorld::sayText);

//  functionName("sayTextAndReturnLength", getName(),
//               "Say a given sentence, and return its length");
//  addParam("toSay", "The sentence to be said.");
  /**
  * This enables to document the return of the method.
  * It is not compulsory to write this line.
  */
//  setReturn("sentenceLength", "Length of the said sentence.");
//  BIND_METHOD(HelloWorld::sayTextAndReturnLength);

  // If you had other methods, you could bind them here...
  /** Bound methods can only take const ref arguments of basic types,
  * or AL::ALValue or return basic types or an AL::ALValue.
  */
}

APIDemonstration::~APIDemonstration() {}

void APIDemonstration::init(){
  /** Init is called just after construction.**/
  different_postures();
}

void APIDemonstration::different_postures(){
	 try{
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
	catch(const AL::ALError&){
		qiLogError("module.example") << "Could not get proxy to ALRobotPostureProxy" << std::endl;
		}
	qiLogInfo("module.example") << "Done !" << std::endl;
}