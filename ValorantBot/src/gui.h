#pragma once

#include "imgui_custom.h"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <nav_elements.h>
#include <etc_elements.h>

#include <string>
#include <vector>

struct Setting
{
	std::string Name;

	Setting() = default;
	Setting(const std::string& name)
		: Name(name) {}

	virtual void Render() = 0;
};

struct BooleanSetting : public Setting
{
	bool* Value;

	BooleanSetting() = default;
	BooleanSetting(const std::string& name, bool* value)
		: Setting(name), Value(value) {}

	virtual void Render() override { ImGui::Checkbox(Name.c_str(), Value); }
};

struct FloatSetting : public Setting
{
	float* Value;
	float MinValue;
	float MaxValue;

	FloatSetting() = default;
	FloatSetting(const std::string& name, float* value, float minValue, float maxValue)
		: Setting(name), Value(value), MinValue(minValue), MaxValue(maxValue) {}

	virtual void Render() override { ImGui::SliderFloat(Name.c_str(), Value, MinValue, MaxValue); }
};

struct IntSetting : public Setting
{
	int* Value;
	int MinValue;
	int MaxValue;

	IntSetting() = default;
	IntSetting(const std::string& name, int* value, int minValue, int maxValue)
		: Setting(name), Value(value), MinValue(minValue), MaxValue(maxValue) {}

	virtual void Render() override { ImGui::SliderInt(Name.c_str(), Value, MinValue, MaxValue); }
};

struct KeybindSetting : public Setting
{
	Hotkey* Key;

	KeybindSetting() = default;
	KeybindSetting(const std::string& name, Hotkey* key)
		: Setting(name), Key(key) {}

	virtual void Render() override { ImGui::HotkeySelection(Name.c_str(), Key); }
};

struct ChoiceSetting : public Setting
{
	std::vector<const char*>* ComboItems;
	int* Choice;

	ChoiceSetting() = default;
	ChoiceSetting(const std::string& name, int* choice, std::vector<const char*>* comboItems)
		: Setting(name), Choice(choice), ComboItems(comboItems) {}

	virtual void Render() override { ImGui::Combo(Name.c_str(), Choice, ComboItems->data(), ComboItems->size()); }

	const std::string GetChoice() const { return (*ComboItems)[*Choice]; }
};

struct SubMenu
{
	std::string Name;
	std::vector<std::unique_ptr<Setting>> Settings;

	BooleanSetting& AddSetting(const std::string& name, bool* value)
	{
		Settings.push_back(std::make_unique<BooleanSetting>(name, value));
		return (BooleanSetting&)*Settings.back();
	}

	FloatSetting& AddSetting(const std::string& name, float* value, float minValue = 0.0f, float maxValue = 100.0f)
	{
		Settings.push_back(std::make_unique<FloatSetting>(name, value, minValue, maxValue));
		return (FloatSetting&)*Settings.back();
	}

	IntSetting& AddSetting(const std::string& name, int* value, int minValue = 0.0f, int maxValue = 100.0f)
	{
		Settings.push_back(std::make_unique<IntSetting>(name, value, minValue, maxValue));
		return (IntSetting&)*Settings.back();
	}

	KeybindSetting& AddSetting(const std::string& name, Hotkey* hotkey)
	{
		Settings.push_back(std::make_unique<KeybindSetting>(name, hotkey));
		return (KeybindSetting&)*Settings.back();
	}

	ChoiceSetting& AddSetting(const std::string& name, int* choice, std::vector<const char*>* comboItems)
	{
		Settings.push_back(std::make_unique<ChoiceSetting>(name, choice, comboItems));
		return (ChoiceSetting&)*Settings.back();
	}
};

struct Menu
{
	std::string Icon;
	std::string Name;
	int SelectedSubMenuIndex;
	std::vector<SubMenu> SubMenus;

	Menu(const std::string& icon, const std::string& name)
		: Icon(icon), Name(name), SelectedSubMenuIndex(-1) {}

	SubMenu& AddSubMenu(const std::string& name)
	{
		SubMenus.push_back({ name });
		if (SelectedSubMenuIndex == -1)
			SelectedSubMenuIndex = 0;
		return SubMenus.back();
	}
};

class Gui
{
public:
	Gui();
	~Gui() = default;

	Menu& AddTab(const std::string& icon, const std::string& name) 
	{
		m_Menus.push_back(Menu(icon, name));
		if (m_SelectedMenuIndex == -1)
			m_SelectedMenuIndex = 0;
		return m_Menus.back();
	}

	void Render();
private:
	int m_SelectedMenuIndex = -1;
	std::vector<Menu> m_Menus;
};
