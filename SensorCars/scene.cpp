#include "scene.h"
#include <string>
#include <fstream>

namespace car
{
	bool Scene::LoadEntity(ID3D11Device *device, LPCWSTR filename, gfx::Model *model, std::vector<mth::Triangle>* m_modelFrame)
	{
		std::ifstream infile;
		infile.open(filename, std::ios::in | std::ios::binary);
		if (!infile.good())
			return false;
		UINT vertexCount, indexCount, unused;
		std::vector<gfx::VerteyType> vertices;
		std::vector<UINT> indices;
		infile.read((char*)&unused, sizeof(UINT));
		infile.read((char*)&vertexCount, sizeof(UINT));
		infile.read((char*)&indexCount, sizeof(UINT));
		vertices.resize(vertexCount);
		indices.resize(indexCount);
		infile.read((char*)vertices.data(), vertices.size() * sizeof(gfx::VerteyType));
		infile.read((char*)indices.data(), indices.size() * sizeof(UINT));
		infile.close();
		model->Init(device, vertices.data(), sizeof(vertices[0])*vertices.size(), indices.data(), sizeof(indices[0])*indices.size());
		if (m_modelFrame)
		{
			m_modelFrame->reserve(indices.size() / 3);
			for (size_t i = 0; i < indices.size() / 3; i++)
				m_modelFrame->push_back(mth::Triangle(
					vertices[indices[i * 3 + 0]].position,
					vertices[indices[i * 3 + 1]].position,
					vertices[indices[i * 3 + 2]].position));
		}
		return true;
	}
	bool Scene::LoadPath(ID3D11Device *device)
	{
		return LoadEntity(device, L"resources/path.mdl", &m_mdlPath, &m_mfPath);
	}
	bool Scene::LoadPlain(ID3D11Device *device)
	{
		return LoadEntity(device, L"resources/plain.mdl", &m_mdlPlain, &m_mfPlain);
	}
	bool Scene::LoadSensor(ID3D11Device *device)
	{
		return LoadEntity(device, L"resources/sensor.mdl", &m_mdlSensor, nullptr);
	}
	bool Scene::LoadCar(ID3D11Device *device)
	{
		return LoadEntity(device, L"resources/car.mdl", &m_mdlCar, &m_mfCar);
	}
	HWND Scene::CreateGfxScreen(RECT position)
	{
		WNDCLASSEX wc{};
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = DefWindowProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszClassName = L"GfxWindow";
		wc.cbSize = sizeof(WNDCLASSEX);
		RegisterClassEx(&wc);
		HWND gfxScreen = CreateWindowEx(WS_EX_APPWINDOW, L"GfxWindow", L"GfxWindow", WS_CHILD | WS_VISIBLE,
			position.left, position.top, position.right - position.left, position.bottom - position.top,
			m_owner, nullptr, wc.hInstance, nullptr);
		ShowWindow(gfxScreen, SW_SHOW);
		return gfxScreen;
	}

	Hitbox Scene::CreateHitbox(std::vector<mth::Triangle> *modelFrame, gfx::Entity *entity)
	{
		Hitbox hitbox;
		hitbox.modelFrame = modelFrame;
		hitbox.position = entity;
		hitbox.shade = 0.299f*entity->getColor().x + 0.587f*entity->getColor().y + 0.114f*entity->getColor().z;
		return hitbox;
	}

	void Scene::ControlUserCarManual(float deltaTime)
	{
		float distanceTaken = 0.0f;
		float speed = 4.0f;
		float steering = 0.5f;
		if (m_forward)
		{
			m_userCar.MoveForward(-speed * deltaTime);
			distanceTaken += speed * deltaTime;
		}
		if (m_back)
		{
			m_userCar.MoveBackward(-speed * deltaTime);
			distanceTaken -= speed * deltaTime;
		}
		if (m_right)
			m_userCar.TurnRight(distanceTaken*steering);
		if (m_left)
			m_userCar.TurnLeft(distanceTaken*steering);
	}

	bool Scene::Init(HWND owner, RECT position)
	{
		m_owner = owner;
		int width = position.right - position.left;
		int height = position.bottom - position.top;
		m_gfxScreen = CreateGfxScreen(position);
		if (!m_gfx.Init(m_gfxScreen, width, height))
			return false;
		if (!m_material.Init(m_gfx.getDevice()))
			return false;
		if (!LoadPath(m_gfx.getDevice()) || !LoadPlain(m_gfx.getDevice()) ||
			!LoadSensor(m_gfx.getDevice()) || !LoadCar(m_gfx.getDevice()))
			return false;

		m_path.Init(&m_mdlPath, &m_mfPath, { 0.1f, 0.1f, 0.1f, 1.0f });
		m_path.position.y += 0.01f;
		m_plain.Init(&m_mdlPlain, &m_mfPlain, { 0.2f, 1.0f, 0.35f, 1.0f });
		for (Car& c : m_cars)
		{
			c.Init(&m_mdlCar, &m_mfCar, 0.0f, 0.0f);
			c.InitSensors(&m_mdlSensor);
			m_envCars.push_back(CreateHitbox(&m_mfCar, &c));
		}
		m_autoControl = false;
		m_userCar.Init(&m_mdlCar, &m_mfCar, 0.0f, 0.0f, { 0.2f, 0.4f, 1.0f, 1.0f });
		m_userCar.InitSensors(&m_mdlSensor);
		m_envCars.push_back(CreateHitbox(&m_mfCar, &m_userCar));
		Restart();

		m_envFloor.push_back(CreateHitbox(&m_mfPlain, &m_plain));
		m_envFloor.push_back(CreateHitbox(&m_mfPath, &m_path));

		m_camera.setProjection((float)width / (float)height);
		m_camera.position = { 14.2f, 9.4f, -14.0f };
		m_camera.rotation = { 0.46f, -0.8f, 0.0f };

		return true;
	}

	void Scene::Update(float deltaTime)
	{
		m_camera.Update(deltaTime);
		for (Car& c : m_cars)
		{
			c.UpdateDistanceSensors(m_envCars);
			c.UpdateLightSensors(m_envFloor);
			c.Control_Auto(deltaTime);
		}
		m_userCar.UpdateDistanceSensors(m_envCars);
		m_userCar.UpdateLightSensors(m_envFloor);
		if (m_autoControl)
			m_userCar.Control_Auto(deltaTime);
		else
			ControlUserCarManual(deltaTime);
	}

	void Scene::Render()
	{
		auto context = m_gfx.getContext();
		mth::matrix matrixBuffer[2];
		matrixBuffer[1] = m_camera.getCameraMatrix();
		float lightBuffer[8] = { 1.0f,1.0f,1.0f,1.0f,
			m_camera.position.x, m_camera.position.y, m_camera.position.z,0.5f };
		mth::float4 colorBuffer;
		m_material.WriteLightBuffer(context, lightBuffer);

		m_gfx.BeginFrame(0.5f, 0.75f, 0.96f);

		m_plain.Render(context, matrixBuffer, m_material);
		m_path.Render(context, matrixBuffer, m_material);

		for (Car& c : m_cars)
		{
			c.Render(context, matrixBuffer, m_material);
			c.RenderSensors(context, matrixBuffer, m_material);
		}
		m_userCar.Render(context, matrixBuffer, m_material);
		m_userCar.RenderSensors(context, matrixBuffer, m_material);

		m_gfx.EndFrame();
	}

	void Scene::Restart()
	{
		mth::float3 pos;
		for (Car& c : m_cars)
		{
			c.position = pos;
			c.rotation = 0.0f;
			pos.z += 3.0f;
		}
		m_userCar.position = { -3.0f, 0.0f, 0.0f };
		m_userCar.rotation = { 0.0f, -mth::pi / 2.0f, 0.0f };
	}

	void Scene::setCarSpeed(int s)
	{
		m_forward = s > 0;
		m_back = s < 0;
	}
	void Scene::setCarSteering(int s)
	{
		m_right = s > 0;
		m_left = s < 0;
	}
	void Scene::SwitchCarPilotAutoManual()
	{
		m_autoControl = !m_autoControl;
	}
	void Scene::Frame(float deltaTime)
	{
		Update(deltaTime);
		Render();
	}

	void Scene::MessageHandler(MSG& msg)
	{
		RECT rect;
		GetClientRect(m_gfxScreen, &rect);
		ScreenToClient(m_gfxScreen, &msg.pt);
		if (msg.pt.x < rect.left || msg.pt.x > rect.right || msg.pt.y < rect.top || msg.pt.y > rect.bottom)
		{
			return;
		}
		m_camera.MessageHandler(msg);
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.wParam)
			{
			case VK_UP:
				m_forward = true;
				break;
			case VK_DOWN:
				m_back = true;
				break;
			case VK_LEFT:
				m_left = true;
				break;
			case VK_RIGHT:
				m_right = true;
				break;
			case 'R':
				Restart();
				break;
			case 'C':
				m_autoControl = !m_autoControl;
				break;
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			}
			break;
		case WM_KEYUP:
			switch (msg.wParam)
			{
			case VK_UP:
				m_forward = false;
				break;
			case VK_DOWN:
				m_back = false;
				break;
			case VK_LEFT:
				m_left = false;
				break;
			case VK_RIGHT:
				m_right = false;
				break;
			}
			break;
		}
	}
}
