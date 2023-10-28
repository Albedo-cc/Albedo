#pragma once

#include <string>

namespace Albedo{
namespace APP
{
	
	class APPConfig final
	{
	public:
		struct Parameters
		{
			struct APP
			{
				std::string name;
			}app;

			struct Window
			{
				std::string title;
				std::string icon;
				int width, height;
				struct Options
				{
					bool maximize;
					bool resizable;
				}options;
			}window;

			struct Editor
			{
				std::string layout;
				struct Font
				{
					std::string name;
					float size;
				}font;
				struct Options
				{
					bool save_layout;
				}options;
			}editor;

			struct Path
			{
				struct Asset
				{
					operator const char*() { return "asset/"; }
					const char* font	= "asset/font/";
					const char* icon	= "asset/icon/";
					const char* image	= "asset/image/";
					const char* shader	= "asset/shader/";
					const char* texture = "asset/texture/";
				}asset;
				struct Cache
				{
					operator const char*() { return "cache/"; }
				}cache;
				struct Config
				{
					operator const char*() { return "config/"; }
				}config;
			}path;
		};
		static auto GetView() -> const Parameters&
		{
			static APPConfig instance;
			return instance.m_parameters;
		}

	private:
		APPConfig();
		virtual ~APPConfig() noexcept;
		APPConfig(const APPConfig&)             = delete;
        APPConfig(APPConfig&&)                  = delete;
        APPConfig& operator=(const APPConfig&)  = delete;
        APPConfig& operator=(APPConfig&&)       = delete;

	private:
		Parameters m_parameters;
	};


}} // namespace Albedo::APP