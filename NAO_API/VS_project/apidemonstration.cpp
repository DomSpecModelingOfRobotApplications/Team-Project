
#include "apidemonstration.h"
#include <iostream>
#include <alcommon/albroker.h>
#include <qi/log.hpp>
#include <alproxies/altexttospeechproxy.h>

using namespace AL;

APIDemonstration::APIDemonstration(boost::shared_ptr<ALBroker> broker, const std::string& name):
  ALModule(broker, name)
{
  /** Describe the module here. This will appear on the webpage*/
  setModuleDescription("An API demonstration module.");

  /** Define callable methods with their descriptions:
  * This makes the method available to other cpp modules
  * and to python.
  * The name given will be the one visible from outside the module.
  * This method has no parameters or return value to describe
  */
//  functionName("sayHello", getName(), "Say hello to the world");
// BIND_METHOD(HelloWorld::sayHello);

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

void APIDemonstration::init()
{
  /** Init is called just after construction.
  *
  * Here we call sayHello, so that the module does something
  * without us having to explicitly call sayHello from somewhere else.
  */
//  sayHello();
}

