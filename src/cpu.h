#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <chrono>
#include <random>

class CPU{

    public:
        uint8_t registers[16]{};
        uint8_t memory[4096]{};
        uint16_t indexRegister{};
        uint16_t programCounter{0x200};
        uint16_t stack[16]{};
        uint8_t stackPointer{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint8_t keypad[16]{};
        uint32_t video[64 * 32]{};
        uint16_t opcode{};

    	std::default_random_engine randGen;
	    std::uniform_int_distribution<uint8_t> randByte;        

        CPU();
        ~CPU();

        void LoadROM(const char* filename);
        void LoadFontset();

        void Cycle();
};

#endif // CPU_H