#include <iostream>
#include <memory>
#include "SimpleEngineCore/Application.hpp"

#include "imgui/imgui.h"


class SimpleEngineEditor : public SimpleEngine::Application
{
public:
	virtual void on_update() override {
		//std::cout << "Update frame: " << ++frame << std::endl;
	}

	virtual void on_ui_draw() override {
		ImGui::Begin("Editor");
		ImGui::SliderFloat3("camera position", camera_position, -1, 1);
		ImGui::SliderFloat2("camera rotation", camera_rotation, 0, 360);
		ImGui::Checkbox("Perspective camera", &is_perspective_mode);
		ImGui::End();
	}

	int frame = 0;
};

int main() {
	auto pSimpleEngineEditor = std::make_unique<SimpleEngineEditor>();

	int returnCode = pSimpleEngineEditor->start(1200, 800, "SimpleGameEngine");

	/*MyApp myApp;
	int returnCode =  myApp.start(800, 400, "SimpleGameEngine");*/

	return returnCode;
}