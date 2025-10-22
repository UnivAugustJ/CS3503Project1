 # CS3503 Project 1- BitBoard Game Engine
  ## Author
 Augustas Jasinskas
  ## Description
 My implementation of Owl Tech’s BitBoard Game Engine project was developed on C through Clion. It utilizes the Core Bit Manipulation functions necessary for Phase 1, which are: SetBit, ClearBit, GetBit, and CountBits. These are directly intertwined with my Phase 2 portion of the project, which implements all 6 of these required game features by utilizing these functions: Board Representation, Display, Move Pieces, Capture Pieces, King Promotion, and Win Detection.

  ## Test Results
  As far as I could tell, there are no issues with out of bounds, wrapping, or any bugs. I think its solid.

  ## Gameplay Instructions
  This BitBoard Checkers game is fairly simple. In an alternating fashion, starting with Red, players will take turns typing out their moves via letter-number notation (Ex:'A3 B4' for a move or 'C3 E5' for a capture). The game is over when when side removes all the others pieces from the board. Good luck!
 
  ## Notes
  I had trouble initially understanding BitWise operators and how they differed from the similar looking boolean operators. I decided to add a function that allowed the game to work with the square names in Letter-number notation rather than the bitboard position because counting every time was a nightmare for testing. Overall this project tested my understanding while
