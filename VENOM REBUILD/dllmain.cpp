#include "plugin.h"

HMODULE m_Module;

void Dark::Mod(HMODULE m_Mod)
{
	m_Module = m_Mod;
}

HMODULE Dark::Mod(void)
{
	return m_Module;
}

std::string Dark::Dir()
{
	static std::string dir = "";
	if (dir == "")
	{
		char buffer[MAX_PATH];
		getcwd(buffer, MAX_PATH);
		dir = buffer;
	}
	return dir;
}

void CALLBACK Dark::Log(char *text, ...)
{
#ifdef PLUGIN_OUTPUT
	static int output_mode = std::fstream::out;

	va_list li;
	char	tmp[512];
	memset(tmp, 0, 512);

	va_start(li, text);
	vsnprintf(tmp, sizeof(tmp) - 1, text, li);
	va_end(li);

	std::fstream out(Dark::Dir() + "\\" + PLUGIN_OUTPUT_FILE, output_mode);
	out << tmp << std::endl;

	if (output_mode == 2) output_mode = std::fstream::out | std::fstream::app;
#endif
	return;
}

bool Dark::Match(const char *text, char *buf, const char *begin)
{
	return Dark::Match(text, buf, begin, "");
}

bool Dark::Match(const char *text, char *buf, const char *begin, const char *end)
{
	if (text == NULL) return false;

	int text_len = strlen(text);

	int write_begin = 0;
	int write_end = text_len;

	bool iswrite = false;

	int begin_len = strlen(begin);
	if (begin_len == NULL) iswrite = true;
	else
	{
		if (begin_len == 1)
		{
			for (int i = 0; i <= text_len; i++)
			{
				if (text[i] == begin[0])
				{
					write_begin = i + 1;
					iswrite = true;
					break;
				}
			}
		}
		else
		{
			int begin_num = 0;

			for (int i = 0; i <= text_len; i++)
			{
				if (text[i] == begin[begin_num]) begin_num++;
				else begin_num = 0;

				if (begin_num == begin_len)
				{
					write_begin = i + 1;
					iswrite = true;
					break;
				}
			}
		}
	}

	if (!iswrite) return false;
	iswrite = false;

	int end_len = strlen(end);
	if (end_len == NULL) iswrite = true;
	else
	{
		if (end_len == 1)
		{
			for (int i = write_begin; i <= text_len; i++)
			{
				if (text[i] == end[0])
				{
					write_end = i - 1;
					iswrite = true;
					break;
				}
			}
		}
		else
		{
			int end_num = 0;

			for (int i = 0; i <= text_len; i++)
			{
				if (text[i] == end[end_num])
				{
					if (end_num == 0) write_end = i - 1;
					end_num++;
				}
				else end_num = 0;

				if (end_num == end_len)
				{
					iswrite = true;
					break;
				}
			}
		}
	}

	if (!iswrite) return false;

	int write = 0;
	for (int i = write_begin; i <= write_end; i++)
	{
		buf[write] = text[i];
		write++;
	}
	return true;
}

std::string ser_key;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
	switch (dwReasonForCall)
	{
	case DLL_PROCESS_ATTACH:
	{
		Dark::Mod(hModule);
		Dark::Render::CreateHook();
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
