.syntax unified

.text
_start:
.extern ballCoordinateX//import ballCoordinateX which is really the address of the real ballCoordinateX, the same approach goes for the other extern variables defined hereeeee
.extern ballCoordinateY//import all extern global variables from the c++ code
.extern ballDirectionX
.extern ballDirectionY
.extern paddle0CoordinateX
.extern paddle1CoordinateX
//rather than importing ball and paddle since they are c++ types not c types I would have to use a c wrapper and make things more confusing so I instead opted to extern all of the variables we needed separately and address each of them independently rather than use an offset because I was having trouble with crashes which I assume came from incorrect ldr and str calls


.global updateBallPosAssembly
.global invertCoordinateX//here we establish the names of each assembly function that we will use
.global invertCoordinateY



updateBallPosAssembly:

//saving all the pushing and popping that would be done with every function call by pushing once at the start and popping once at the end
push {lr}
//import all global variables, ball coordinateX,coordinateY, directionX, and directionY, it's also worth noting that our passed in pointer value, score, will be stored as r0 here which is why I haven't overwritten that register
ldr r7,=ballCoordinateX //address of the address of the ball object
//because we couldn't have class types in an extern C block I have instead used pointers, but this does mean we have to dereference each pointer twice before we get the acutal value at the address
ldr r7,[r7]//the address of the ball object
ldr r1,[r7]//value of coordinateX// so now load the value at the address of the ballCoordinateX address
ldr r7,=ballCoordinateY//Notice I'm never trusting an offset above 0, always directly loading the value at the specific address of the global extern value from c++ because I don't want to risk any segmentation errors from the code accessing the wrong value of part of memory
ldr r7,[r7]
ldr r2,[r7]//value of coordinateY
ldr r7,=ballDirectionX
ldr r7,[r7]
ldr r3,[r7]//value of directionX
ldr r7,=ballDirectionY
ldr r7,[r7]
ldr r4,[r7,#0]//value of directionY
mov r12,#1//this needs to be placed here because you can't use immediate values with a str function

//create potential coordinates x and y from ball coordinate + ball direction
mov r5,#0//good housekeeping to make sure that r5 and r6 don't have erroneous values
mov r6,#0
add r5,r1,r3//coordinateX+directionX = potentialCoordinateX
add r6,r2,r4//coordinateY+directionY = potentialCoordinateY

//run the if checking statements to check that the ball isn't about to crash into a wall if we move it 

//if potential coordinate X ==-1 || potential coordinate X==5 invert ball direction X
//this is for the right and left walls
movs r11,#1//moves 1 into r8 and sets condition flags
negs r11,r11//negates the 1 so that I can have a -1 to compare with, this will serve as our surrogate #-1 since we can't do that with the microbit
cmp r5,r11//comparing potentialCoordinateX and -1
beq invertX
cmp r5,#5
beq invertX//if the potentialCoordinateX isn't -1 or 5 then skip the inversion
b skipInvertX
invertX:
//no need to push any registers because the function only changes memory locations not registers and the original value of the link register has already been pushed onto the stack, to be popped off at the end of the function
bl invertCoordinateX//branch and link to the invertCoordinateX function

skipInvertX://if neither of the two statements triggerred then skip here and don't invert X


//if potential coordinate Y==-1, invertBallDirectionY, this is for the top wall of the pong game
cmp r6,r11//cmp potentialCoordinateY with -1 
bne skipY//if the ball isn't about to hit the roof then skip the invertY, otherwise invert ballDirectionY

bl invertCoordinateY

skipY:


cmp r6,#4//compare potentialCoordinateY with the bottom of the board, if they are equal then the ball is about to be on the bottom row and it's worth checking if it will collide with the paddle, otherwise skip this step
bne skipPaddle
//check to see if the paddle pieces coordinateX's are equal to the potentialCoordinates X and Y, if so then add 1 to score, also invert the ball Y direction 
ldr r8,=paddle0CoordinateX//the address of the address of the paddle object,notice we are allowed to use r8 and r9 which are later used for the ball movement because they are set back to 0 later
ldr r8,[r8]//load into r8 the value at the address of the address of the paddle0XoordinateX
ldr r9,[r8] //paddlePieces[0].coordinateX
ldr r8,=paddle1CoordinateX//the address of the address of the second paddle object, notice we double dereference here because we are using apoitners to a pointer
ldr r8,[r8]
ldr r10,[r8] //paddlePieces[1].coordinateX
//notice that we only check the x coordinates because the previous cmp statement verifies that the ball is about to be on the paddles level, the only question is whether it's about to hit it or not. This method saves register space and execution time as opposed to checking the coordinateY of both paddle pieces as well
cmp r9,r5//cmp paddlePieces[0].coordinateX with potentialCoordinateX to see if the ball is about to collide with the paddle
beq paddleInvert//we are effectively implementing an or statement with these double beq statements 
cmp r10,r5//cmp paddlePieces[1].coordinateX with potentialCoordinateX
beq paddleInvert
b skipPaddle//if neither of the beqs has triggerred by now then skip the invert and score addition
paddleInvert:

bl invertCoordinateY

//score++;
ldr r8,[r0]//naturally the only input value, the pointer to score is stored in r0
add r8,#1//loading the address of score into r1 then adding 1 to it and putting it back
str r8,[r0]

skipPaddle:


pop {lr}//pop the original link register value off the stack so it can be used for the bx lr statements that will end of assembly section
//finally if balls coordinateY ==4 then return 0, we have to make sure that we test the actual coordinateY rather than the potentialCoordinateY because otherwise the game would end every time the ball was about to touch the paddle rather than when it actually touches the floor, our code to move the ball is predicated around the idea of checking where the ball is about to be if we were to move it at the end of the function call so the ball never actually goes on the bottom row it unless it misses the paddle, otherwise it only gets next to the paddle then changes its direction
cmp r2,#4
bne skip1//if coordinateY != 4 then the game does not need to end so skip to the other ending code
mov r0,#0//otherwise set the return value to 0 and end the game
bx lr//if coordinateY==4 then set the return value to 0 and branch to the end of the assembly program
skip1:

//at this point any necessary direction changes(or lack thereof) will have been made so we can comfortably move the ball
mov r8,#0//more good housekeeping to ensure our r8 and 9 aren't just random massive numbers
mov r9,#0
add r8,r1,r3//coordinateX+DirectionX after all necessary inversions have been made
add r9,r2,r4//coordinateY+DirectionY after all necessary inversions have been made
ldr r7,=ballCoordinateX
ldr r7,[r7]//load into r7 the address of ballCOordinateX at the address r7
str r8,[r7]//changing the coordinateX value according to coordinateX+the current(changed or unchanged) directionX value
ldr r7,=ballCoordinateY//changing r7 each line we need to according to the requirement, rather than use up more registers avoiding the overwrite
ldr r7,[r7]
str r9,[r7]//changing coordinateY according to coordianteY+the(changed or unchanged) directionY value

//return 1 and end the game now that any direction changes have been made and the ball has been moved
mov r0,#1
bx lr



invertCoordinateX:
ldr r7,=ballDirectionX//making sure r7 holds the correct value
ldr r7,[r7]//so we initially load the address of the address of ballDirectionX, then we load the address of ballDirectionX into r7 here
//if directionX ==1 then make it -1 if it equals -1 then make it 1
cmp r3,#1//r3 = the value of directionX
beq invert1//if equal, branch to the 1 to -1 conversion
//if this branch hasn't triggerred then directionX must be -1 so str 1 into the directionX address
str r12,[r7]//store 1 in the address of directionX
b end//skip the invert 1 to -1 step 
invert1:
str r11,[r7]//store -1 in the address of directionX
end:
ldr r3,[r7]//updating directionX after the change
bx lr//branch to the value stored in the link register

invertCoordinateY:
ldr r7,=ballDirectionY//load into r7 the address of the address of ballDirectionY
ldr r7,[r7]//then we load into r7 the address of the ballDirecctionY
cmp r4,#1//r4 = direction Y
beq invert2//if directionY==1 branch to invert2
//otherwise r4 must be -1 not 1 so store a 1
str r12,[r7]
b end1
invert2://store a -1 because we branched equal to 1
str r11,[r7]
end1:
ldr r4,[r7]//updating the local directionY value after it was changed
bx lr