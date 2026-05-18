#include "cpu.h"

CPU::CPU()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
  LoadFontset();
}

CPU::~CPU() {}

void CPU::LoadROM(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == nullptr) {
    std::cerr << "Failed to open ROM: " << filename << std::endl;
    std::exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  rewind(file);

  if (fileSize > (0x1000 - 0x200)) {
    std::cerr << "ROM size exceeds available memory." << std::endl;
    std::exit(EXIT_FAILURE);
  }

  fread(memory + 0x200, sizeof(uint8_t), fileSize, file);
  fclose(file);
}

void CPU::LoadFontset() {
  const unsigned int FONTSET_SIZE = 80;
  uint8_t fontset[FONTSET_SIZE] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  const unsigned int FONTSET_START_ADDRESS = 0x50;
  for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
    memory[FONTSET_START_ADDRESS + i] = fontset[i];
  }
}

void CPU::Cycle() {
  CPU::currentInstruction =
      (CPU::memory[programCounter] << 8) |
      (CPU::memory[programCounter + 1]); // accounts for 2 bytes
  int regX;
  int regY;
  int imm;
  int res;
  int regXValue;
  int byte;
  uint8_t yPos, xPos, height;
  uint32_t * screenPixel;
  uint16_t spritePixel;

  // op is the first byte
  switch (currentInstruction & 0xF000) {
    // Op = 0
    switch (currentInstruction & 0x000F) {
    // clear screen
    case 0x0000:
      memset(video, 0, sizeof(video));
      break;

    // return
    case 0x000E:
      // load RA from stack
      uint16_t returnAddr = stack[--stackPointer];
      programCounter = returnAddr;
    }

  // Op = 1
  // jump to address at NNN
  case 0x1000:
    programCounter = currentInstruction & 0x0FFF;
    break;

  // Op = 2
  // call to address at NNN
  case 0x2000:
    stack[stackPointer++] = programCounter;
    programCounter = currentInstruction & 0x0FFF;
    break;

  // Op = 3
  // Skip next instruction if regX = NN
  case 0x3000:
    regX = currentInstruction & 0x0F00;
    if (registers[regX] == (currentInstruction & 0x00FF)) {
      programCounter += 4;
    } else {
      programCounter += 2;
    }
    break;

  // Op = 4
  // Skip next instruction if regX != NN
  case 0x4000:
    regX = currentInstruction & 0x0F00;
    if (registers[regX] != (currentInstruction & 0x00FF)) {
      programCounter += 4;
    } else {
      programCounter += 2;
    }
    break;

  // Op = 5
  // Skip next if regX = regY
  case 0x5000:
    regX = (currentInstruction & 0x0F00) >> 8;
    regY = (currentInstruction & 0x00F0) >> 4;
    if (registers[regX] == registers[regY]) {
      programCounter += 4;
    } else {
      programCounter += 2;
    }
    break;

  // Op = 6
  // Set register regX to NN
  case 0x6000:
    regX = (currentInstruction & 0x0F00) >> 8;
    registers[regX] = currentInstruction & 0x00FF;
    programCounter += 2;
    break;

  // Op = 7
  // Add value NN to regX
  case 0x7000:
    regX = (currentInstruction & 0x0F00) >> 8;
    registers[regX] += currentInstruction & 0x00FF;
    programCounter += 2;
    break;

  // Op = 8
  case 0x8000:
    regX = (currentInstruction & 0x0F00) >> 8;
    regY = (currentInstruction & 0x00F0) >> 4;
    switch (currentInstruction & 0x000F) {
    // Set regX = regY
    case 0x0000:
      registers[regX] = registers[regY];
      break;
    // Set regX = regX OR regY
    case 0x0001:
      registers[regX] = registers[regX] | registers[regY];
      break;
    // Set regX = regX AND regY
    case 0x0002:
      registers[regX] = registers[regX] & registers[regY];
      break;
    // Set regX = regX XOR regY
    case 0x0003:
      registers[regX] = registers[regX] ^ registers[regY];
      break;
    // Set regX = regX + regY, set VF = Carry
    case 0x0004:
      res = registers[regX] + registers[regY];
      registers[regX] = res & 0xFFu;
      registers[0xF] = res > 255;
      break;
    // Set regX = regX - regY, set VF = Carry
    case 0x0005:
      res = registers[regX] - registers[regY];
      registers[regX] = res;
      registers[0xF] = res > 0;
      break;
    // Set regX = regX >> 1
    case 0x0006:
      registers[regX] = registers[regX] >> 1;
      break;
    // Set regX = regY - regX, set VF = Carry
    case 0x0007:
      res = registers[regY] - registers[regX];
      registers[regX] = res;
      registers[0xF] = res > 0;
      break;
    // Set regX = regX << 1
    case 0x000E:
      registers[regX] = registers[regX] << 1;
      break;
    }
    programCounter += 2;
    break;

  // Op = 9
  // Skip next instruction if regX != regY
  case 0x9000:
    regX = (currentInstruction & 0x0F00) >> 8;
    regY = (currentInstruction & 0x00F0) >> 4;
    if (registers[regX] != registers[regY]) {
      programCounter += 4;
    } else {
      programCounter += 2;
    }
    break;

  // Op = A
  // Set I = NNN
  case 0xA000:
    indexRegister = currentInstruction & 0x0FFF;
    programCounter += 2;
    break;

  // Op = B
  // Jump to NNN + V0
  case 0xB000:
    imm = currentInstruction & 0x0FFF;
    programCounter = imm + registers[0x0];
    break;

  // Op = C
  // Set regX = random byte AND NN
  case 0xC000:
    regX = (currentInstruction & 0x0F00) >> 8;
    byte = currentInstruction & 0x00FF;
    registers[regX] = randByte(randGen) & byte;
    programCounter += 2;
    break;

  // Op = D
  // Display N-byte sprite starting at (regX, regY), set VF = Collision
  case 0xD000:
    regX = (currentInstruction & 0x0F00u) >> 8u;
    regY = (currentInstruction & 0x00F0u) >> 4u;
    height = currentInstruction & 0x000Fu;

    // Wrap if going beyond screen boundaries
    xPos = registers[regX] % VIDEO_WIDTH;
    yPos = registers[regY] % VIDEO_HEIGHT;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row) {
      uint8_t spriteByte = memory[indexRegister + row];

      for (unsigned int col = 0; col < 8; ++col) {
        spritePixel = spriteByte & (0x80u >> col);
        screenPixel =
            &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

        // Sprite pixel is on
        if (spritePixel) {
          // Screen pixel also on - collision
          if (*screenPixel == 0xFFFFFFFF) {
            registers[0xF] = 1;
          }

          // Effectively XOR with the sprite pixel
          *screenPixel ^= 0xFFFFFFFF;
        }
      }
    }
    break;

  // Op = E
  case 0xE000:
    regX = (currentInstruction & 0x0F00) >> 8;
    regXValue = registers[regX];
    switch (currentInstruction & 0x00FF) {
    // Skip next if key with regX is pressed
    case 0x009E:
      if (keypad[regXValue]) {
        programCounter += 4;
      } else {
        programCounter += 2;
      }
      break;
    // Skip next if regX is not pressed
    case 0x00A1:
      if (!keypad[regXValue]) {
        programCounter += 4;
      } else {
        programCounter += 2;
      }
      break;
    }
    break;

  // Op = F
  case 0xF000:
    regX = (currentInstruction & 0x0F00) >> 8;
    regXValue = registers[regX];
    switch (currentInstruction & 0x00FF) {
    // set regX = delay timer value
    case 0x0007:
      registers[regX] = delayTimer;
      break;
    // wait for key press then store value of the key in regX
    case 0x000A:
      if (keypad[0]) {
        registers[regX] = 0;
      } else if (keypad[1]) {
        registers[regX] = 1;
      } else if (keypad[2]) {
        registers[regX] = 2;
      } else if (keypad[3]) {
        registers[regX] = 3;
      } else if (keypad[4]) {
        registers[regX] = 4;
      } else if (keypad[5]) {
        registers[regX] = 5;
      } else if (keypad[6]) {
        registers[regX] = 6;
      } else if (keypad[7]) {
        registers[regX] = 7;
      } else if (keypad[8]) {
        registers[regX] = 8;
      } else if (keypad[9]) {
        registers[regX] = 9;
      } else if (keypad[10]) {
        registers[regX] = 10;
      } else if (keypad[11]) {
        registers[regX] = 11;
      } else if (keypad[12]) {
        registers[regX] = 12;
      } else if (keypad[13]) {
        registers[regX] = 13;
      } else if (keypad[14]) {
        registers[regX] = 14;
      } else if (keypad[15]) {
        registers[regX] = 15;
      } else {
        programCounter -= 2;
      }
      break;
    // set delay timer value to regX
    case 0x0015:
      delayTimer = regXValue;
      break;
    // set sound timer value to regX
    case 0x0018:
      soundTimer = regXValue;
      break;
    // set indexRegister += regX
    case 0x001E:
      indexRegister += regXValue;
      break;
    // set indexRegister to location of sprite for digit regX
    case 0x0029:
      indexRegister = 0x50 + (5 * regXValue);
      break;
    // store BCD representation of regX in memory I, I+1 and I+2
    case 0x0033:
      memory[indexRegister + 2] = regXValue % 10;
      regXValue /= 10;
      memory[indexRegister + 1] = regXValue % 10;
      regXValue /= 10;
      memory[indexRegister] = regXValue % 10;
      break;
    // store registers V0 through regX in memory starting at location I
    case 0x0055:
      for (uint8_t i = 0; i <= regXValue; ++i) {
        memory[indexRegister + i] = registers[i];
      }
      break;
    // read registers V0 through regX from memory starting at location I
    case 0x0065:
      for (uint8_t i = 0; i <= regXValue; ++i) {
        registers[i] = memory[indexRegister + i];
      }
      break;
    }
    programCounter += 2;

  // default case
  default:
    printf("\nUnimplemented op code: %.4X\n", currentInstruction);
    exit(3);
  }
}