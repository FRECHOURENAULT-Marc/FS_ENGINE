#include "pch.h"

UIManager::UIManager()
{
	m_ecs = &ECS::Get();
	TextureManager::LoadTexture("RedTex", L"../../res/Textures/Red.dds");
}

void UIManager::Update()
{
	for(auto* ui : m_uiList)
		ui->Update();
}

UIBar* UIManager::CreateBar()
{
	UIBar* bar = new UIBar();
	AddUI(bar);
	return bar;
}

UIString* UIManager::CreateText()
{
	UIString* text = new UIString();
	AddUI(text);
	return text;
}

UIImage* UIManager::CreateImage(std::string texName)
{
	UIImage* img = new UIImage(texName);
	AddUI(img);
	return img;
}
