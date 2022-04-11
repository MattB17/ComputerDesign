# JumpGame Written In Jack
This directory implements a jump game in which the avatar needs to jump from
platform to platform without falling off. The platforms continuously scroll
down the screen, and as time progresses, falling obstacles are added to
increase the difficulty.

The screen is 256 x 512 bits and memory can be accessed in segments of 16 bits.

### Building the game
The following are the steps to build the game.
1. Clone this directory.
2. Run the `JackCompiler` on the files in `source`
3. Load the directory containing the `.vm` files into the `VMEmulator`

### File Descriptions
* `Main.jack` handles the running of the game
  * all of the game methods are called from the function in this file
* `GameController.jack` is the main game controller
  * it handles the platforms, objects, and jumper through appropriate controllers
* `PlatformController` is the controller for handling platforms
  * it handles the platforms on screen, scrolling them downwards as the game progresses
  * the platforms are implemented in `Platform.jack` representing the surface the Jumper stands on between jumps
  * `PlatformList.jack` implements a version of a DoublyLinkedList to store the Platforms on screen.
  * `PlatformNode.jack` implements the nodes of the PlatformList, containing the platform as well as previous and next pointers
* `ObstacleController.jack` is the controller for handling obstacles
  * it manages obstacles as they fall down the board
  * `Obstacle.jack` implements the Obstacle objects that drop down the board
* `JumperController.jack` is the controller for handling the jumper
  * it controls the jumper's movement and object collision detection
  * `Jumper.jack` implements the Jumper avatar
