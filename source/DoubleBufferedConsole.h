#pragma once
#include <Windows.h>

template<typename char_t = wchar_t>
class DoubleBufferedConsole
{
	HANDLE  buffer[2]{ nullptr, nullptr };
	SMALL_RECT writeRect;
	PCHAR_INFO charInfoBuffer{ nullptr };
	COORD cursor{ 0,0 }, bufferSize;
	int bufferIndex{ 0 };
	short width, height;

	void handlerSetup(const LPCWSTR title) 
	{
		CONSOLE_CURSOR_INFO cursorInfo;

		for (auto i = 0; i < 2; ++i)
		{
			
			buffer[i] = CreateConsoleScreenBuffer(
				GENERIC_WRITE,
				0,
				NULL,
				CONSOLE_TEXTMODE_BUFFER,
				NULL);
			SetConsoleScreenBufferSize(buffer[i], bufferSize);
			SetConsoleWindowInfo(buffer[i], TRUE, &writeRect);

			if (!buffer[i])
				throw "DoubleBufferedConsole: Handler creation failed!";

			//remove cursor from both handlers
			GetConsoleCursorInfo(buffer[i], &cursorInfo);
			cursorInfo.bVisible = false;
			SetConsoleCursorInfo(buffer[i], &cursorInfo);
		}
		SetConsoleTitle(title);
	}

public:
	DoubleBufferedConsole(LPCWSTR title,const short width,const short height)
		:
		writeRect{ 0,0,width,height }, 
		charInfoBuffer(new CHAR_INFO[width*height]),
		bufferSize{ width,height }, 
		width(width), 
		height(height)
	{
		handlerSetup(title);
	}

	//use the default console size if no width and height are specified
	DoubleBufferedConsole(const LPCWSTR title, short height = 100)
		:height(height)
	{
		CONSOLE_SCREEN_BUFFER_INFO consoleBufferInfo;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleBufferInfo);
		width = consoleBufferInfo.dwSize.X - 1;

		bufferSize = { width,height };
		writeRect  = { 0,0,width,height };
		charInfoBuffer = new CHAR_INFO[width*height];
		handlerSetup(title);
	}

	~DoubleBufferedConsole()
	{
		delete[] charInfoBuffer;
		charInfoBuffer = nullptr;
	}

	//flip the background buffer to the console
	void flip()
	{
		WriteConsoleOutput(
			buffer[++bufferIndex & 0x01], 
			charInfoBuffer,    
			bufferSize,  
			{ 0, 0 },   
			&writeRect);
		SetConsoleActiveScreenBuffer(buffer[bufferIndex & 0x01]);
	}

	//clear the background buffer
	void clear(const char_t filling, unsigned short attribute)
	{
		for (auto y = 0; y < height; ++y)
		{
			for (auto x = 0; x < width; ++x)
			{
				charInfoBuffer[x + width * y].Attributes = attribute;
				if constexpr (std::is_same<char_t, wchar_t>::value)
					charInfoBuffer[x + width * y].Char.UnicodeChar = filling;
				else
					charInfoBuffer[x + width * y].Char.AsciiChar = filling;
			}
		}
	}

	//write to the background buffer
	void write(const int x, const int y, const char_t c, unsigned short attribute)
	{
		const auto coord = x + y * width;
		if constexpr (std::is_same<char_t,wchar_t>::value)
			charInfoBuffer[x + width * y].Char.UnicodeChar = c;
		else
			charInfoBuffer[x + width * y].Char.AsciiChar = c;

		charInfoBuffer[coord].Attributes = attribute;
	}

	std::tuple<short,short> getSize() const
	{
		return { width,height};
	}
};