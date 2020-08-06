#include <stdio.h>
#include <stdbool.h>

void printBoard(char board[][26], int dim);
char findOpposite(char turn);
char humanColour(char computerColour);
bool isValidMove(char board[][26],int dim, int row, int col, char colour);
bool checkLegalInDirection(char board[][26], int dim, int row, int col, char colour, int deltaRow, int deltaCol);
bool positionInBounds(int dim, int row, int col);
bool moveAvailable(char colour, char board[][26], int dim);
void computerDecisionMove(char computerColour, char board[][26], int dim, int Newrow, int Newcol);
int computerTilesCountBeforeFlip(char computerColour, char board[][26], int dim);
int computerTilesCountAfterFlip(char copiedBoard[][26], int dim, int row, int col, char computerColour);
int computerTilesAtCornersAfterFlip(char copiedBoard[][26], int dim, char computerColour);
int opponentAvailableMovesAfterFlip(char copiedBoard[][26], int dim, char computerColour);
int computerTilesAtFavouredEdges(char copiedBoard[][26], int dim, char computerColour);
int computerTilesAtDisflavouredRegion1(char copiedBoard[][26], int dim, char computerColour);
int computerTilesAtDisflavouredRegion2(char copiedBoard[][26], int dim, char computerColour);
void placeTile (char board[][26], int dim, int row, int col, char colour);
void winnerIs(char board[][26], int dim);
bool gameIsOver(char board[][26], int dim);


int main(){
    
    int dim, row, col;
    char board[26][26]; //the board never be larger than 26 x 26
    
    printf("Enter the board dimension: ");
    scanf("%d", &dim);
    
    // assign all cells to indicate "Unoccupied"
    for(row = 0; row <= dim - 1; row++){
        for(col = 0; col <= dim - 1; col++){
            board[row][col] = 'U';
        }
    }
    
    //change the middle four 'U' to 'W''B''B''W'
    board[dim/2 - 1][dim/2 -1] = 'W'; //top left
    board[dim/2 - 1][dim/2] = 'B'; //top right
    board[dim/2][dim/2 - 1] = 'B'; //bottom left
    board[dim/2][dim/2] = 'W'; //bottom right
    
    char computerColour;
    printf("Computer plays (B/W): ");
    scanf(" %c", &computerColour);
    
    printBoard(board, dim); // print the board at starting position with the centred four cells being "WBBW"
    
    char turn;
    
    turn = 'B';
    
    while(!gameIsOver(board, dim)){ // game is not over
        if (turn == computerColour){
            // computer makes a move
            row = 0;
            col = 0;
            computerDecisionMove(computerColour, board, dim, row, col); // computer will play a new position(row, column), with the highest score
            
        } else {
            // human makes a move
            char rowAlphabet, colAlphabet;
            printf("Enter move for colour %c (RowCol): ", humanColour(computerColour));
            scanf(" %c %c", &rowAlphabet, &colAlphabet);
            row = rowAlphabet - 'a';
            col = colAlphabet - 'a';
            
            // check if legal, if not, game over
            if(isValidMove(board, dim, row, col, humanColour(computerColour))){
                placeTile(board, dim, row, col, humanColour(computerColour)); // flip the tiles
                printBoard(board, dim); // print out the new board
            } else{
                printf("Invalid move.\n");
                printf("%c player wins.\n", computerColour); // special case 1
                return 0; // game is over when human places an invalid move
            }
        }
        
        if(!gameIsOver(board, dim)){ // game is not over
            if (moveAvailable(findOpposite(turn), board, dim)){
                turn = findOpposite(turn);
            } else {
                printf("%c player has no valid move.\n", findOpposite(turn)); // special case 2
            }
        }
    }
    
    // neither player has a turn
    winnerIs(board, dim); // igure out the winner
    return 0;// game is over when neither player has a turn
    
}

//******************************************************functions*********************************************************//
void printBoard(char board[][26], int dim) {
    // print out the board configuration
    int row, col;
    
    printf("  ");
    for(col = 1; col <= dim; col++){
        printf("%c", 'a' + col - 1); //print alphabet abc... on the first row
    }
    
    printf("\n");
    
    for(row = 0; row <= dim - 1; row++){
        printf("%c ", 'a' + row); //print alphabet abc.. on the first column
        for(col = 0; col <= dim - 1; col++){
            printf("%c", board[row][col]);
        }
        printf("\n");
    }
}


char findOpposite(char turn){
    // find the opposite colour
    if (turn == 'W') {
        return 'B';
    } else {
        return 'W';
    }
}


char humanColour(char computerColour){
    //find the colour that the human is playing
    char human_colour;
    if (computerColour == 'B'){
        human_colour = 'W';
        return human_colour;
    } else{
        human_colour = 'B';
        return human_colour;
    }
}


bool isValidMove(char board[][26],int dim, int row, int col, char colour){
    // determine if the (row, col) position is a legal move for that colour
    return checkLegalInDirection(board, dim, row, col, colour, -1, -1) ||
        checkLegalInDirection(board, dim, row, col, colour, -1, 0) ||
        checkLegalInDirection(board, dim, row, col, colour, -1, 1) ||
        checkLegalInDirection(board, dim, row, col, colour, 0, -1) ||
        checkLegalInDirection(board, dim, row, col, colour, 0, 1) ||
        checkLegalInDirection(board, dim, row, col, colour, 1, -1) ||
        checkLegalInDirection(board, dim, row, col, colour, 1, 0) ||
        checkLegalInDirection(board, dim, row, col, colour, 1, 1);
}


bool checkLegalInDirection(char board[][26], int dim, int row, int col, char colour, int deltaRow, int deltaCol){
    
    // coordinate of the new position
    int newRow = row + deltaRow;
    int newCol = col + deltaCol;
    
    // return false when the new position is out of bound
    if(positionInBounds(dim, newRow, newCol) == 0)
        return false;
    
    // is true if and only if the new position is not 'U' and is not its own colour
    bool isOppositeColor = (board[newRow][newCol] != 'U' && board[newRow][newCol] != colour);
        
    if (!isOppositeColor)
        return false;
    
    for (int i = 2; ; i++) {
        newRow = row + i * deltaRow;
        newCol = col + i * deltaCol;
        
        if(positionInBounds(dim, newRow, newCol) == 0)
            return false;
        
        if (board[newRow][newCol] == 'U')
            return false; // there can't be an Unoccupied cell in the middle
        
        if (board[newRow][newCol] == colour)
            return true; // the only case it returns true
    }
    return false;
}


bool positionInBounds(int dim, int row, int col){
    return row >= 0 && row < dim && col >= 0 && col < dim;
}


bool moveAvailable(char colour, char board[][26], int dim){
    // determine if the colour has any valid move
    for(int row = 0; row <= dim - 1; row++){
        for(int col = 0; col <= dim - 1; col++){
            if(isValidMove(board, dim, row, col, colour)){
                return true;
            } else{
                continue;
            }
        }
    }
    
    return false;
}


void winnerIs(char board[][26], int dim){
    int blackCount = 0;
    int whiteCount = 0;
    for(int row = 0; row <= dim - 1; row++){
        for(int col = 0; col <= dim - 1; col++){
            if (board[row][col] == 'B'){
                blackCount++;
            }else if(board[row][col] == 'W'){
                whiteCount++;
            }else{
                continue;
            }
        }
    }
    
    if(blackCount > whiteCount){
        printf("B player wins.\n");
    } else if(blackCount < whiteCount){
        printf("W player wins.\n");
    } else{
        printf("Draw!");
    }
}


void placeTile (char board[][26], int dim, int row, int col, char colour){
    
    int deltaRow, deltaCol;
    
    for(deltaRow = -1; deltaRow <= 1; deltaRow++){
        for(deltaCol = -1; deltaCol <= 1; deltaCol++){
                    
            if (deltaRow == 0 && deltaCol == 0){
                continue;
            }
                    
            if (checkLegalInDirection(board, dim, row, col, colour, deltaRow, deltaCol) == 1){
                board[row][col] = colour; // flip the current tile
                for(int i = 1; board[row + i*deltaRow][col + i*deltaCol] != colour &&
                    positionInBounds(dim, row + i*deltaRow, col + i*deltaCol) == 1; i++){
                        board[row + i*deltaRow][col + i*deltaCol] = colour; //change the colour of the tiles in that direction
                }
            
            } else{
                continue;
            }
        }
    }
}


bool gameIsOver(char board[][26], int dim){
    for(int row = 0; row <= dim - 1; row++){
        for(int col = 0; col <= dim - 1; col++){
            if (board[row][col] == 'U'){
                return false;
            }
        }
    }
    return true;
}


void computerDecisionMove(char computerColour, char board[][26], int dim, int Newrow, int Newcol){
    int countBefore = computerTilesCountBeforeFlip(computerColour, board, dim);
    
    int highestScore = 0, oldScore = 0;
    
    // look for valid moves
    for(int row = 0; row <= dim - 1; row++){
        for(int col = 0; col <= dim - 1; col++){
            // skip to the next position if the current position is not 'Unoccupied'
            if(board[row][col] != 'U')
                continue;
            
            // for each candidate position where Black may play legally
            if (isValidMove(board, dim, row, col, computerColour)) {
                // create a copy of the entire board
                char copiedBoard[26][26];
                for(int row = 0; row <= dim - 1; row++){
                    for(int col = 0; col <= dim - 1; col++){
                        copiedBoard[row][col] = board[row][col];
                    }
                }
                
                int countAfter = computerTilesCountAfterFlip(copiedBoard, dim, row, col, computerColour);
                
                // number of computer tiles added to the board after the flip
                int flippedNumber = countAfter - countBefore - 1;
                
                // number of corners oppucpied by computer tiles after the flip
                int cornersNumber = computerTilesAtCornersAfterFlip(copiedBoard, dim, computerColour);
                
                // number of different locations the opponent can place tiles on after the tiles are flipped
                int opponentMovesNumber = opponentAvailableMovesAfterFlip(copiedBoard, dim, computerColour);
                
                // number of computer tiles occupied the flavoured edges locations
                int flavouredEdgeNumber = computerTilesAtFavouredEdges(copiedBoard, dim, computerColour);
                
                // number of computer tiles occupied the first unflavoured regions
                int disFlavouredRegion1Number = computerTilesAtDisflavouredRegion1(copiedBoard, dim, computerColour);
                
                // number of computer tiles occupied the second unflavoured regions
                int disFlavouredRegion2Number = computerTilesAtDisflavouredRegion2(copiedBoard, dim, computerColour);
                
                // score for candidate location
                int currentScore = 2 * flippedNumber + 10 * cornersNumber + 3 * opponentMovesNumber + 2 * flavouredEdgeNumber
                                    - 2 * disFlavouredRegion1Number - 2 * disFlavouredRegion2Number;
                
                if (currentScore > oldScore && currentScore > highestScore){
                    highestScore = currentScore;
                    // change the postion (row, col) to the tile that has the highest score
                    Newrow = row;
                    Newcol = col;
                }
                
                oldScore = currentScore;
            }
        }
    }
    
    //print out the decision made by the computer
    char rowAlphabet = 'a' + Newrow;
    char colAlphabet = 'a' + Newcol;
    printf("Computer places %c at %c%c.\n", computerColour, rowAlphabet, colAlphabet);
    
    placeTile(board, dim, Newrow, Newcol, computerColour); // flip the tiles
    printBoard(board, dim); // print out the new board
}


int computerTilesCountBeforeFlip(char computerColour, char board[][26], int dim){
    int count = 0;
    
    for(int row = 0; row <= dim - 1; row++){
        for(int col = 0; col <= dim - 1; col++){
            if (board[row][col] == computerColour){
                count++;
            }
        }
    }
    
    return count;
}


int computerTilesCountAfterFlip(char copiedBoard[][26], int dim, int row, int col, char computerColour){
    placeTile(copiedBoard, dim, row, col, computerColour); // play current tile and flip other tiles in the copied board
    
    // count the number of computer tiles after the flip
    int count = 0 ;
    for(int row = 0; row <= dim - 1; row++){
        for(int col = 0; col <= dim - 1; col++){
            if (copiedBoard[row][col] == computerColour){
                count++;
            }
        }
    }
    
    return count;
}

int computerTilesAtCornersAfterFlip(char copiedBoard[][26], int dim, char computerColour){
    // tiles are already flipped in the above function (computerTilesCountAfterFlip)
    // no need to flip the tiles again in this current function
    
    // count the number of corners occupied by the computer tiles after the flip
    int count = 0;
    
    if (copiedBoard[0][0] == computerColour) // top left
        count++;
        
    if (copiedBoard[0][dim - 1] == computerColour) // top right
        count++;
        
    if(copiedBoard[dim - 1][0] == computerColour) // bottom left
        count++;
        
    if(copiedBoard[dim - 1][dim - 1] == computerColour) // bottom right
        count++;
        
    return count;
}


int opponentAvailableMovesAfterFlip(char copiedBoard[][26], int dim, char computerColour){
    // tiles are already flipped in the function (computerTilesCountAfterFlip)
    // no need to flip the tiles again in this current function
    
    int count = 0;
    
    for(int row = 0; row <= dim - 1; row++){
        for(int col = 0; col <= dim - 1; col++){
            if(isValidMove(copiedBoard, dim, row, col, humanColour(computerColour))){
                count++;
            } else{
                continue;
            }
        }
    }
    
    return count;
}


int computerTilesAtFavouredEdges(char copiedBoard[][26], int dim, char computerColour){
    int count = 0;
    
    if(dim == 0 || dim == 1 || dim == 2 || dim == 3 || dim == 4){
        return count;
    } else{
        for(int i = 1; i <= dim - 4; i++){
            if(copiedBoard[0][1 + i] == computerColour) // top edge
                count++;
        
            if(copiedBoard[dim - 1][1 + i] == computerColour) // bottom edge
                count++;
            
            if(copiedBoard[1 + i][0] == computerColour) // left edge
                count++;
            
            if(copiedBoard[1 + i][dim - 1] == computerColour) // right edge
                count++;
        }
        return count;
    }
}


int computerTilesAtDisflavouredRegion1(char copiedBoard[][26], int dim, char computerColour){ // region right above each favoured edges
    int count = 0;
    
    if(dim == 0 || dim == 1 || dim == 2 || dim == 3 || dim == 4){
        return count;
    } else{
        for(int i = 1; i <= dim - 4; i++){
            if(copiedBoard[1][1 + i] == computerColour) // top edge + 1
                count++;
        
            if(copiedBoard[dim - 2][1 + i] == computerColour) // bottom edge - 1
                count++;
            
            if(copiedBoard[1 + i][1] == computerColour) // left edge + 1
                count++;
            
            if(copiedBoard[1 + i][dim - 2] == computerColour) // right edge - 1
                count++;
        }
        return count;
    }
}


int computerTilesAtDisflavouredRegion2(char copiedBoard[][26], int dim, char computerColour){ // region around each corner
    int count = 0;
    
    if(copiedBoard[0][1] == computerColour) // topleft.1
        count++;
    if(copiedBoard[1][1] == computerColour) // topleft.2
        count++;
    if(copiedBoard[1][0] == computerColour) // topleft.3
        count++;
    if(copiedBoard[0][dim - 2] == computerColour) // topright.1
        count++;
    if(copiedBoard[1][dim - 2] == computerColour) // topright.2
        count++;
    if(copiedBoard[1][dim - 1] == computerColour) // topright.3
        count++;
    if(copiedBoard[dim - 1][1] == computerColour) // bottomleft.1
        count++;
    if(copiedBoard[dim - 2][1] == computerColour) // bottomleft.2
        count++;
    if(copiedBoard[dim - 2][0] == computerColour) // bottomleft.3
        count++;
    if(copiedBoard[dim - 1][dim - 2] == computerColour) // bottomright.1
        count++;
    if(copiedBoard[dim - 2][dim - 2] == computerColour) // bottomright.2
        count++;
    if(copiedBoard[dim - 2][dim - 1] == computerColour) // bottomright.3
        count++;
    
    return count;
}

