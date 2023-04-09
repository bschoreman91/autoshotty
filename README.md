# autoshotty

autoshotty (windows only) automates the process of manually screenshotting hundreds or thousands of pages or items at a time.

## Features

- Save files as combined PDF, jpgs, or pngs
- Configurable save directory (defaults to application folder)
- Quality slider - choose the output quality of your images to keep clarity or reduce filesize
- Keyboard mode - use one of four keyboard buttons to advance pages
- Mouse mode - define a position on the screen to advance the page
- Selectable dimensions - eliminate the need for cropping by choosing the top left and bottom right regions of your photos
- Delay - add a delay for the amount of time it takes to load a page so you don't get duplicates or buffering
- Full screen coordinate capture - application will "dissapear" and allow you to click anywhere on a page to specify coordinates 
- Collapsible window - collapse the application to a small size with gui buttons to increase your screen real estate when capturing

## Usage instructions

1. Run autoshotty.exe
2. Select options such as filetype, quality, number of pages, keyboard or mouse mode, filename, etc
3. Click "set top left" and then click the top left region you want to capture
4. Click "set bottom right" and then click the bottom right region you want to capture
5. If you are using keyboard mode, click "set target" and select the application you want to capture
6. If you are using mouse mode, click "set next page" and select the next page button on the application you want to capture (if you need to you can go back one page after doing this)
7. Move the window out of the way and click the play arrow to begin the capture

Once the program starts it will collapse and begin to capture your screenshots. You will know it's done once the window expands again.

## When to use this

autoshotty should be used as a last resort. Screenshotted files will always be larger than the source material for the same or less quality. If you really cannot get the files straight from the source and *need* to screenshot, use autoshotty to automate the process.

## Notes

- autoshotty is windows only due to using windows system libraries to click buttons and press keys
- PDFs are created from images; currently the quality slider changes the quality of both PDF and images used to create it which likely creates extra lossiness
- autoshotty is developed using default Qt libraries where applicable with a cmake build system
- autoshotty has only been tested on a 1080p monitor though i believe coordinate getting should scale
