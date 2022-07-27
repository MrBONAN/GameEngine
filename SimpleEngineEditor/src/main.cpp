#include <iostream>
#include <memory>
#include "SimpleEngineCore/Application.hpp"


class MyApp : public SimpleEngine::Application
{
public:
	virtual void on_update() override {
		//std::cout << "Update frame: " << ++frame << std::endl;
	}
	int frame = 0;
};

int main() {
	auto myApp = std::make_unique<MyApp>();

	int returnCode = myApp->start(1200, 800, "SimpleGameEngine");

	/*MyApp myApp;
	int returnCode =  myApp.start(800, 400, "SimpleGameEngine");*/

	return returnCode;
}