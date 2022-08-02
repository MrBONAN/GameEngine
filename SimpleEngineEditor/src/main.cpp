#include <iostream>
#include <memory>
#include "SimpleEngineCore/Application.hpp"
#include "SimpleEngineCore/Input.hpp"

#include "imgui/imgui.h"


class SimpleEngineEditor : public SimpleEngine::Application
{
public:
	virtual void on_update() override {
		using SimpleEngine::Input;
		using SimpleEngine::KeyCode;
		if (Input::IsKeyPressed(KeyCode::KEY_W)) camera_position[2] -= 0.01f;
		if (Input::IsKeyPressed(KeyCode::KEY_S)) camera_position[2] += 0.01f;
		if (Input::IsKeyPressed(KeyCode::KEY_A)) camera_position[0] -= 0.01f;
		if (Input::IsKeyPressed(KeyCode::KEY_D)) camera_position[0] += 0.01f;

		if (Input::IsKeyPressed(KeyCode::KEY_LEFT_SHIFT)) camera_position[1] -= 0.01f;
		if (Input::IsKeyPressed(KeyCode::KEY_SPACE)) camera_position[1] += 0.01f;

		if (Input::IsKeyPressed(KeyCode::KEY_LEFT)) camera_rotation[1] += 1;
		if (Input::IsKeyPressed(KeyCode::KEY_RIGHT)) camera_rotation[1] -= 1;

		if (Input::IsKeyPressed(KeyCode::KEY_UP)) camera_rotation[0] += 1;
		if (Input::IsKeyPressed(KeyCode::KEY_DOWN)) camera_rotation[0] -= 1;
		
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