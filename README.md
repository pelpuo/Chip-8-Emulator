# Chip 8

## Description

### Registers

* 16  
* 8 bit  
* Labelled V0-VF

### Memory

* 4kb (1 pages)  
* 0x000 \- 0xFFF  
* Address space segmentation  
  * 0x000 \- 0x1FF \= Reserved  
  * 0x050 \- 0x0A0 \= Built in character maps  
  * 0x200 \- 0xFFF \= Instructions from ROM

### Index Register

* Stores memory addresses

### Program Counter

* 16 bits

### Stack

* Can hold 16 elements

### Stack Pointer

* 8 bits  
* Can be emulated using 1 byte

### Delay timer

* 8 bits  
* Decrements at a rate of 60Hz when loaded with a value  
* During emulation, it can be adjusted to decrement at a predefined clock frequency

### Sound timer

* 8 bits  
* A single tone will buzz when it’s not zero

### Input keys

* Uses 16 keys  
* They match first 16 hex values  
* 0 \- F

### Display 

* 64x32 display memory  
* Only 2 states for each pixel (on or off)

## Instructions

34 that need emulation  
Each instruction is 2 bytes  
The first 4 bits are the opcode. Like any other ISA, opcode determines how to decode the remaining bits

* X: The second nibble. Used to look up one of the 16 registers (VX) from V0 through VF.  
* Y: The third nibble. Also used to look up one of the 16 registers (VY) from V0 through VF.  
* N: The fourth nibble. A 4-bit number.  
* NN: The second byte (third and fourth nibbles). An 8-bit immediate number.  
* NNN: The second, third and fourth nibbles. A 12-bit immediate memory address.

| Instruction | Description |
| :---- | :---- |
| 00E0 | Clear screen |
| 1NNN | Jump to address NNN |
| 6XNN | Set register VX |
| 7XNN | ADD value to register VX |
| 00EE | Return |
| 2NNN | Call address at NNN |
| 3XNN | Skip the next instruction if VX \= NN |
| 4XNN | Skip the next instruction if VX \!= NN |
| 5XY0 | Skip next instruction if VX \= VY |
| 8XY0 | Set VX \= VY |
| 8XY1 | Set VX \= VX OR VY |
| 8XY2 | Set VX \= VX AND VY |
| 8XY3 | Set VX \= VX XOR VY |
| 8XY4 | Set VX \= VX \+ VY, Set VF \= carry |
| 8XY5 | Set VX \= VX \- VY, Set VF \= NOT Borrow |
| 8XY6 | Set VX \= VX SHR 1 |
| 8XY7 | Set VX \= VY \- VX, Set VF \= NOT Borrow |
| 8XYE | Set VX \= VX SHL 1 |
| 9XY0 | Skip next instruction if VX \!= VY |
| ANNN | Set I \= NNN |
| BNNN | Jump to NNN \+ V0 |
| CXNN | Set VX \= random byte AND NN |
| DXYN | Display N-byte sprite starting at (VX, VY), set VF \= Collision |
| EX9E | Skip next if key with VX is pressed |
| EXA1 | Skip next if VX is not pressed |
| FX07 | Set VX \= delay timer value |
| FX0A | Wait for key press, store value of the key in VX |
| FX15 | Set delay timer \= VX |
| FX18 | Set sound timer \= VX |
| FX1E | Set I \= I \+ VX |
| FX29 | Set I \= Location of sprite for digit VX |
| FX33 | Store BCD representation of VX in memory I, I+1 and I+2 |
| FX55 | Store registers V0 through VX in memory starting at location I |
| FX65 | Read registers V0 through VX from memory starting at Location I |

