// printcpualert search it to remove getCPUStatus for debugging purposes

#include <iostream>
#include <cstdlib>
#include <cstdint>
#define MEMORY_BYTES (64 * 1024) // 65536

using byte = uint8_t;      // Create an alias for 8-bit integer (Most prolly for CPU instructions)
using big_byte = uint16_t; // Create an alias for 16-bit integer (Most probably for addresses)
using bit = bool;          // Create an alias for a bit

struct Memory
{
    // An 6502 can address 64K of memory hence we assigned that number in the square brackets
    byte *memory = new byte[MEMORY_BYTES]; // Each location in 0 to 65535 can hold 8-bits of data

    void makeMemoryZero()
    {
        for (int i = 0; i < MEMORY_BYTES; i++)
        {
            // Make each memory address as 0
            memory[i] = 0x00;
        }
    }
};

struct CPU
{
    // The name of variables say their function
    byte reg_AC = 0;       // Accmulator
    byte reg_X, reg_Y = 0; // Registers
    big_byte PC = 0;       // Program counter
    big_byte SP = 0;       // Stack pointer
    byte reg_instruction;

    // Flag registers
    bit flag_N;
    bit flag_V;
    bit flag_B;
    bit flag_D;
    bit flag_I;
    bit flag_Z;
    bit flag_C;

    bit opcode_reading_status = true;
    bit data_reading_status = false;

    static constexpr byte INS_LDA_IMMEDIATE = 0xA9; // LDA instruction with 0xA9 op-code
    static constexpr byte INS_LDA_ZEROPAGE = 0xA5;  // LDA instruction with 0xA5 op-code

    void reset(Memory &mem)
    {
        PC = 0xFFFC; // Initially, the PC gets resetted to the OxFFFC in the memory
        SP = 0x0000;
        flag_N = flag_V = flag_B = flag_D = flag_I = flag_Z = reg_AC = 0;
        mem.makeMemoryZero();
    }

    void invert_reading_flags()
    {
        if (opcode_reading_status == true)
        {
            opcode_reading_status = false;
        }
        else
        {
            opcode_reading_status = true;
        }

        if (data_reading_status == true)
        {
            data_reading_status = false;
        }
        else
        {
            data_reading_status = true;
        }
    }

    void run(int cycles, Memory &mem)
    {
        if (opcode_reading_status == true && data_reading_status == false)
        {
            reg_instruction = fetch_instruction(mem, cycles);
            PC++;
            // getCPUStatus(); // printcpualert
            invert_reading_flags();
        }

        if (opcode_reading_status == false && data_reading_status == true)
        {
            switch (reg_instruction)
            {
            case INS_LDA_IMMEDIATE:
            {
                // LDA in immediate addressing mode
                while (cycles--)
                {
                    reg_AC = fetch_data(mem);
                    flag_Z = (reg_AC == 0);
                    flag_N = (reg_AC & 0b10000000) > 0;
                    // getCPUStatus();
                    PC++;
                }
                invert_reading_flags();
                break;
            }
            case INS_LDA_ZEROPAGE:
            {
                while (cycles != 0)
                {
                    byte zeropage_address = fetch_data(mem);
                    cycles--;
                    reg_AC = mem.memory[zeropage_address];
                    cycles--;
                    flag_N = (reg_AC == 0);
                    flag_Z = (reg_AC & 0b10000000) > 0;
                }
                invert_reading_flags();
                break;
            }
            }
        }
    }

    byte fetch_instruction(Memory &mem, int &cycles)
    {
        byte instruction_opcode = mem.memory[PC];
        switch (instruction_opcode)
        {
        case INS_LDA_IMMEDIATE:
        {
            // LDA in immediate addressing mode
            break;
        }
        case INS_LDA_ZEROPAGE:
        {
            // LDA in zeropage addressing mode
            break;
        }
        default:
        {
            std::cout << "Instruction not handled" << std::endl;
            exit(-1);
            break;
        }
        }
        cycles--;
        return instruction_opcode;
    }

    byte fetch_data(Memory &mem)
    {
        byte data = mem.memory[PC];
        PC++;
        return data;
    }

    void getCPUStatus()
    {
        std::cout << "A: " << +reg_AC;
        std::cout << std::endl;
        std::cout << "Z: " << flag_Z << ", ";
        std::cout << "N: " << flag_N << " ";
        std::cout << std::endl;
        std::cout << "PC: " << PC << ", ";
        std::cout << "SP: " << SP << " ";
        std::cout << std::endl
                  << std::endl;
    }
};

int main()
{
    CPU Computer;
    Memory MEM;
    Computer.reset(MEM);

    MEM.memory[0xFFFC] = CPU::INS_LDA_ZEROPAGE;
    MEM.memory[0xFFFD] = 0xA5;
    MEM.memory[0xA5] = 0x04;

    Computer.run(3, MEM);
    Computer.getCPUStatus();

    return 0;
}