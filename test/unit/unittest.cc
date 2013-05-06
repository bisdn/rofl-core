#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
//#include "fipv6frametest.h"
#include <stdlib.h>

//using namespace rofl;

int main( int argc, char **argv)
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  bool wasSuccessful = runner.run( "", false );

  int rc = (wasSuccessful) ? EXIT_SUCCESS : EXIT_FAILURE;
  return rc;
}

