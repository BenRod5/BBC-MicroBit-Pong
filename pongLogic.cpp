// source/main.cpp
#include "MicroBit.h"
MicroBit uBit; // The MicroBit object
// --- DECLARE GLOBAL VARIABLES, STRUCTURE(S) AND ADDITIONAL FUNCTIONS (if needed) HERE ---

class gameObject
{//gameObject acts as our base class for gameBall and gamePaddle its basically just a way of representing the relevant pixels for the ball and paddle without creating too many global variables
    private://for the sake of modularity we will keep these attributes private and access them through getter and setter methods
    int coordinateX;
    int coordinateY;
    public://obviously the constructor, the get methods, and the code to move the object must be public so that they can be accessed in the main code
    gameObject(int x,int y)
    {//a constructor for initialising our gameObjects to the values of int x and y
        coordinateX=x;
        coordinateY=y;
    }

    int getCoordinateX()
    {//both of these methods are simply get methods for retreiving the coordinates X and Y since they are private
        return coordinateX;
    }

    int getCoordinateY()
    {
        return coordinateY;
    }

    void moveGameObject(int x, int y)
    {//again to preserve modularity and allow for a common method between the ball and the paddle pieces, we create this moveGameObject method to change the coordinates so they cannot be accidentally changed in another class
        coordinateX+=x;
        coordinateY+=y;
    }
    
};

class gameBall:public gameObject
{//gameBall inherits from gameObject so that the ball can inherit the attributes and methods of its gameObject class
    private://again we create private variables for ballDirectionX and Y based for the sake of modularity. It's a cool detail to note that our assembly program doesn't actually care if we make things private or public since it deals with memory directly and ignores access control
    int ballDirectionX=1;
    int ballDirectionY=1;
    public:

    int getBallDirectionX()
    {//simple getter methods for the x and y ball direction
        return ballDirectionX;
    }
    int getBallDirectionY()
    {
        return ballDirectionY;
    }

    void invertBallDirectionX()
    {//nifty little 1 line inversion methods
        ballDirectionX*=-1;//the *=-1 will flip a 1 to -1 and a -1 to 1
    }
    void invertBallDirectionY()
    {
        ballDirectionY*=-1;
    }//notice that rather than make one method which takes an input to decide which direction, I've instead separated it into two methods. This made sense because it removes the possibility of any error with an input int that would determine which value to change, makes the code more modular, and removes unnecessary complexity

    gameBall():gameObject(0,0){}//cool little tidbit here, we are calling the gameObject constructor within the constructor for gameBall and instantiating the object coordinates of the gameBall to 0,0
};

class gamePaddle
{
    public ://paddlePieces needs to be public so it can be referenced in the C++ code to move and draw the paddle
    gameObject paddlePieces[2]={gameObject(0,4),gameObject(1,4)};//here we are creating an array of length 2, with the type gameObject in order to instantiate the two pixels which make up the paddle as gameObjects. I instantiate the two items in the array forcibly using the gameObject constructor
};

gameBall ball=gameBall();//creating a global instance of gameBall, within the gameBall class we call the gameObject constuctor with an input of 0,0 to start the ball at 0,0

gamePaddle paddle=gamePaddle();//here we create paddle, an instance of the gamePaddle class which should come with the paddlePieces array already instantiated to 0,4 and 1,4

int score=0;//leaving score as a global variable rather than assigning it inside draw_scene as if that were the case it would set score to 0 each time draw_scene runs


// extern "C" int updateBallPosAssembly(int* score);//this tells our compiler that there exists an external funciton that takes an int* input called score and returns an int, it is basically a function declaration
// extern gameBall ball; these were old attempts at including the ball and paddle as extern variables in ARM assembly, naturally they didn't work in extern C because they are objects and so are parts of a class which don't exist in C
// extern gamePaddle paddle;
// {
//     volatile int*  ballCoordinateX=&(ball.coordinateX);//establishing each of the values that we reference in the assembly as global variables
//     volatile int* ballCoordinateY=&(ball.coordinateY);
//     volatile int* ballDirectionX=&(ball.ballDirectionX);
//     volatile int* ballDirectionY=&(ball.ballDirectionY);
//     volatile int* paddle0CoordinateX=&(paddle.paddlePieces[0].coordinateX);
//     volatile int* paddle1CoordinateX=&(paddle.paddlePieces[1].coordinateX);
//     //we have opted to use the keyword volatile here so that the linker knows not to touch these values under any circumstances, it prevents name mangling and makes sure the assembler can find the correct c++ symbols
// }//this pointer approach to the extern global variables mean that we get to directly access the memory of the object attributes without having to bring classes into the mix which isn't a C concept and thus can't be messed with by assembly
//unfortunately I couldn't get this working in time so this code is unused

// Event handler for button A
void onButtonA(MicroBitEvent e)
{
    // DEVELOP CODE HERE

        if(paddle.paddlePieces[0].getCoordinateX()>0)//notice we use 0 in the paddlePieces part of the if statement rather than a for loop with n because otherwise the pixel not adjacent to the edge would be moved inside of the other pixel and the pixel adjacent to the edge would be unable to move due to our if statements
        {
        paddle.paddlePieces[0].moveGameObject(-1,0);//moving the game object one pixel to the left, as long as it doesn't go off screen and again leaving the y axis unchanged
        paddle.paddlePieces[1].moveGameObject(-1,0);
        }
}
    // Event handler for button B

void onButtonB(MicroBitEvent e)
{
    // DEVELOP CODE HERE
        if(paddle.paddlePieces[1].getCoordinateX()<4)
        {//only execute the move if the pixel won't go off the screen, again the same logic of an if statement over a for loop was used here for aforementioned reasons
        paddle.paddlePieces[0].moveGameObject(1,0);//moving the game object one pixel to the right and leaving the y axis unchanged
        paddle.paddlePieces[1].moveGameObject(1,0);//notice that we get to use the moveGameObject method here, which is cool because we also get to use it in the c++ updateBallPosition code showing how we saved time writing two separate methods by using inheritance
        }
    
 
}


int updateBallPosition(int* score)
{
    int potentialCoordinateX=ball.getBallDirectionX()+ball.getCoordinateX();//here we establish potential coordinates for x and y which are where the ball will be if this updateBallPosition would go through unchecked
    int potentialCoordinateY=ball.getBallDirectionY()+ball.getCoordinateY();//we use these values rather than the actual values because if we only went off the ball touching the outer walls or paddle the ball would be able to go into the paddle or off the screen which is unintended, so we use this method of prevention instead
    int invertY=1;//if invert is set to 0 then we invert the y axis because the paddle was about to collide with the ball
    if(potentialCoordinateX==-1||potentialCoordinateX==5)//so if the ball is about to touch either the right or left side of the screen,
    {//it's worth noting that more than one of these functions can happen at once, the ball should be able to touch the paddle and a wall simultaneously so I shouldn't make these methods if elses 
        ball.invertBallDirectionX();
    }
    if(potentialCoordinateY==-1)
    {//if the ball is about to touch the top wall
        ball.invertBallDirectionY();
    }

    for(int n=0;n<2;n++)
    {//if the ball is about to touch the paddle
        if(paddle.paddlePieces[n].getCoordinateX()==potentialCoordinateX&&paddle.paddlePieces[n].getCoordinateY()==potentialCoordinateY)//if the ball is about to touch either piece of the paddle
        {//I actually coded this slightly differently in the assembly version, instead opting to check if the ball was even about to be at the y coordinate 4 or not and only checking the x values if it was
            invertY=0;
        }
    }
    if(invertY==0)
    {
        ball.invertBallDirectionY();
        (*score)++;//this should increment the actual value of score, rather than the address
    }
    //now that we have gotten all the none game breaking cases out of the way, e.g the walls and the paddle, we need to account for the game over condition, the floor
    if(ball.getCoordinateY()==4)//this way regardless of the ball direction, even if it gets changed by the paddle once the ball has gone past the paddle, if the ball touches the floor and it hasn't already made contact with the paddle this statement will trigger. We know it didn't make contact with the paddle because if it did it would have been reflected by the last round of ball position updates.
    {
        return 0;
    }

       //here is the code to change the balls direction and return a value of 1 to inply the game is still running, again we get to use the moveGameObject method which continues to prove that using a common class was a good choice
    ball.moveGameObject(ball.getBallDirectionX(),ball.getBallDirectionY());
    return 1;
}

void draw_scene()
{
    while(1)
    {
        int gameOver=1;
        gameOver=updateBallPosition(&score);
        if(gameOver==1)
        {
        uBit.display.clear();
        //draw ball
        uBit.display.image.setPixelValue(ball.getCoordinateX(),ball.getCoordinateY(),255);
        //draw paddle pixels, should be fine to leave the y values as 4 because the paddle can never move from the 4 position
        uBit.display.image.setPixelValue(paddle.paddlePieces[0].getCoordinateX(),4,255);//here is the movement of the paddle
        uBit.display.image.setPixelValue(paddle.paddlePieces[1].getCoordinateX(),4,255);
        fiber_sleep(1000);
        }
        else if(gameOver==0){
        uBit.display.clear();//this is the ending of the game
        uBit.display.print(score);
        fiber_sleep(10000);
        }
        // uBit.display.print(5);//basically just a code saying I GOT HERE I know the error has to be in the assembly code one way or another because when I remove it the paddle and ball are able to function as they did before, the question is if the assembly code is straight crashing the program or whether it's just not returning a 1

        fiber_sleep(1000);//fiber_sleep sleeps the program so it only updates once every second
    }
}


int main()
{
    // Initialise the micro:bit
    uBit.init();
    // setupPointers();
    // Ensure that different levels of brightness can be displayed
    uBit.display.setDisplayMode(DISPLAY_MODE_GREYSCALE);
    // Set up listeners for button A, B and the combination A and B.
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButtonA);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButtonB);
    create_fiber(draw_scene);
    release_fiber();
}
