// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

//wskekran = 16384
@16384
D=A
@wskekran
M=D

//ekranSize = 8192 - 1 = 8191 zapisuje o jeden mniejsza wartosc aby po dodaniu do adresu pierwszego elementu odrazu otrzymac adres ostatniego
@8191
D=A
@ekranSize
M=D

//wskkey = 24576
@24576
D=A
@wskkey
M=D

//while{
    (while)

    //if(wskkey!=0){
    @wskkey
    A=M
    D=M
    @else
    D;JEQ

        //screen black
        //i=ekran.size
        @ekranSize
        D=M
        @i
        M=D

        //while(i>0){
        (whileBlack)
        @i
        D=M
        @endIf
        D;JLT

            //ekran[i]=-1
            @wskekran
            D=M
            @i
            A=D+M
            M=-1           

            //i--
            @i
            M=M-1
        //}
        @whileBlack
        0;JMP


    //}
        @endIf
        0;JMP 

    //else{
    (else)
        
        //screen white
        //i=ekran.size
        @ekranSize
        D=M
        @i
        M=D

        //while(i>0){
        (whileWhite)
        @i
        D=M
        @endIf
        D;JLT

            //ekran[i]=-1
            @wskekran
            D=M
            @i
            A=D+M
            M=0           

            //i--
            @i
            M=M-1

        //}
        @whileWhite
        0;JMP
    //}
    //end if
    (endIf)
//}
@while
0;JMP