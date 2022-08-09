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

		glm::vec3 movement_delta{ 0, 0, 0 };
		glm::vec3 rotation_delta{ 0, 0, 0 };

		bool move_camera = false;

		if (Input::IsKeyPressed(KeyCode::KEY_W)) {
			movement_delta.x += 0.01f;
			move_camera = true;
		}

		if (Input::IsKeyPressed(KeyCode::KEY_S)) {
			movement_delta.x -= 0.01f;
			move_camera = true;
		}

		if (Input::IsKeyPressed(KeyCode::KEY_A)) {
			movement_delta.y -= 0.01f;
			move_camera = true;
		}

		if (Input::IsKeyPressed(KeyCode::KEY_D)) {
			movement_delta.y += 0.01f;
			move_camera = true;
		}

		if (Input::IsKeyPressed(KeyCode::KEY_LEFT_SHIFT)) {
			movement_delta.z -= 0.01f;
			move_camera = true;
		}

		if (Input::IsKeyPressed(KeyCode::KEY_SPACE)) {
			movement_delta.z += 0.01f;
			move_camera = true;
		}

		if (Input::IsKeyPressed(KeyCode::KEY_LEFT)) {
			rotation_delta.z += 1;
			move_camera = true;
		}

		if (Input::IsKeyPressed(KeyCode::KEY_RIGHT)) {
			rotation_delta.z -= 1;
			move_camera = true;
		}

		if (Input::IsKeyPressed(KeyCode::KEY_UP)) {
			rotation_delta.y -= 1;
			move_camera = true;
		}

		if (Input::IsKeyPressed(KeyCode::KEY_DOWN)) {
			rotation_delta.y += 1;
			move_camera = true;
		}

		if (Input::IsKeyPressed(KeyCode::KEY_Q)) {
			rotation_delta.x += 1;
			move_camera = true;
		}

		if (Input::IsKeyPressed(KeyCode::KEY_E)) {
			rotation_delta.x -= 1;
			move_camera = true;
		}
		if(move_camera) camera.add_movement_and_rotation(movement_delta, rotation_delta);
	}

	virtual void on_ui_draw() override {
		camera_position[0] = camera.get_position().x;
		camera_position[1] = camera.get_position().y;
		camera_position[2] = camera.get_position().z;
		camera_rotation[0] = camera.get_rotation().x;
		camera_rotation[1] = camera.get_rotation().y;
		camera_rotation[2] = camera.get_rotation().z;
		ImGui::Begin("Editor");
		ImGui::SliderFloat3("camera position", camera_position, -1, 1);
		ImGui::SliderFloat3("camera rotation", camera_rotation, 0, 360);
		ImGui::Checkbox("Perspective camera", &is_perspective_mode);
		ImGui::End();
		camera.set_position_rotation({ camera_position[0], camera_position[1], camera_position[2] },
									 { camera_rotation[0], camera_rotation[1], camera_rotation[2] });
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