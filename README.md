# Double buffered Windows Console
C ++ class with which you can selectively output characters to the console depending on X and Y coordinates.

## Features
- Double buffering, which means no flickering
- Easy to use (only 3 functions) and to copy & paste
- Unicode UTF-16 support
- Place characters freely on the console

## How to use
```
...
//init class
const auto attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY; //text attributes (color, intensity..)
DoubleBufferedConsole<wchar_t> myConsole(L"MyTitle",width,height);

//printing stuff
myConsole.clear(' ',FOREGROUND_GREEN); // clear background buffer and set foreground to green color
...
myConsole.write(x,y,L'Ʃ',attributes); // write the unicode character 'Ʃ' to (x|y) in the background buffer
...
myConsole.flip();  // flip the background buffer with the foreground buffer. This will show the new screen.
```

