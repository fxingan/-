#include <stdio.h>
#include "shell.h"

void process_instruction()
{
    //printf("ll\n");
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC); // 从内存中读取指令
    uint32_t opcode = (instruction >> 26) & 0x3F; // 提取指令的操作码字段
    uint32_t funct = instruction & 0x3F;
    uint32_t rs = (instruction >> 21) & 0x1F;
    uint32_t rt = (instruction >> 16) & 0x1F;
    uint32_t rd = (instruction >> 11) & 0x1F;
    uint32_t shamt = (instruction >> 6) & 0x1F;
    int16_t immediate = (int16_t)(instruction & 0xFFFF); // 有符号立即数
    uint32_t target_addressj = (instruction & 0x03FFFFFF) << 2;// 提取跳转目标地址（26位地址左移2位）
    int16_t offset = (int16_t)(instruction & 0xFFFF); // 有符号偏移量    
    uint32_t load_address = CURRENT_STATE.REGS[rs] + offset;// 计算加载地址   
    uint32_t store_address = CURRENT_STATE.REGS[rs] + offset;// 计算存储地址
    uint32_t data_to_store = CURRENT_STATE.REGS[rt];// 获取要存储的数据   
    uint32_t target_addressi = CURRENT_STATE.PC + (offset << 2);// 计算目标地址
    // 根据操作码执行不同的操作
    switch (opcode) {
        case 0x00: // R型指令
            // 根据功能码执行不同的操作
            switch (funct) {
                case 0x00: // SLL
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x02: // SRL
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x03: // SRA
                    NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> shamt;
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x04: // SLLV
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << (CURRENT_STATE.REGS[rs] & 0x1F);
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x06: // SRLV
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x07: // SRAV
                    NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x08: // JR
                    NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                    break;
                    
                case 0x09: // JALR
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 4; // 返回地址存入rd
                    NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                    break; 
                       
                case 0x10: // MFHI
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
                    NEXT_STATE.PC += 4;
                    break;                                  
                    
                case 0x11: // MTHI
                    NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x12: // MFLO
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x13: // MTLO
                    NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x18: // MULT
                    {
                        int64_t result = (int64_t)CURRENT_STATE.REGS[rs] * (int64_t)CURRENT_STATE.REGS[rt];
                        NEXT_STATE.HI = (result >> 32) & 0xFFFFFFFF;
                        NEXT_STATE.LO = result & 0xFFFFFFFF;
                        NEXT_STATE.PC += 4;
                    }
                    break;
                    
                case 0x19: // MULTU
                    {
                        uint64_t result = (uint64_t)CURRENT_STATE.REGS[rs] * (uint64_t)CURRENT_STATE.REGS[rt];
                        NEXT_STATE.HI = (result >> 32) & 0xFFFFFFFF;
                        NEXT_STATE.LO = result & 0xFFFFFFFF;
                        NEXT_STATE.PC += 4;
                    }
                    break;
                    
                case 0x1A: // DIV
                    if (CURRENT_STATE.REGS[rt] != 0) {
                        int32_t dividend = CURRENT_STATE.REGS[rs];
                        int32_t divisor = CURRENT_STATE.REGS[rt];
                        NEXT_STATE.LO = dividend / divisor;
                        NEXT_STATE.HI = dividend % divisor;
                    } else {
                        // 处理除数为零的情况，可以抛出异常或执行默认操作
                    }
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x1B: // DIVU
                    if (CURRENT_STATE.REGS[rt] != 0) {
                        uint32_t dividend = CURRENT_STATE.REGS[rs];
                        uint32_t divisor = CURRENT_STATE.REGS[rt];
                        NEXT_STATE.LO = dividend / divisor;
                        NEXT_STATE.HI = dividend % divisor;
                    } else {
                        // 处理除数为零的情况，可以抛出异常或执行默认操作
                    }
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x20: // add
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x21: // ADDU
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x22: // sub
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x23: // SUBU
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x24: // and
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x25: // or
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x26: // XOR
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x27: // NOR
                    NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x2A: // slt
                    NEXT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt]) ? 1 : 0;
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x2B: // SLTU
                    NEXT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt]) ? 1 : 0;
                    NEXT_STATE.PC += 4;
                    break;
                    
                case 0x0C: // SYSCALL
                    // 解码SYSCALL指令
                    uint32_t v0 = CURRENT_STATE.REGS[2]; // $v0寄存器的值
                    if (v0 == 0x0A) {
                        // 如果$v0寄存器的值为0x0A，停止模拟器的运行
                        RUN_BIT = FALSE;
                    }
                    NEXT_STATE.PC += 4;
                    break;
                    
                default:
                    // 未知的功能码
                    break;
            }           
            break;

        case 0x01: // I型指令
            // 根据功能码执行不同的操作
            switch (funct) {
                case 0x00: // BLTZ
                    if ((int32_t)CURRENT_STATE.REGS[rs] < 0) {
                        NEXT_STATE.PC = target_addressi;
                    } else {
                        NEXT_STATE.PC += 4;
                    }
                    break;
                case 0x01: // BGEZ
                    if ((int32_t)CURRENT_STATE.REGS[rs] >= 0) {
                        NEXT_STATE.PC = target_addressi;
                    } else {
                        NEXT_STATE.PC += 4;
                    }
                    break;
                case 0x10: // BLTZAL
                    if ((int32_t)CURRENT_STATE.REGS[rs] < 0) {
                        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4; // 存储返回地址
                        NEXT_STATE.PC = target_addressi;
                    } else {
                        NEXT_STATE.PC += 4;
                    }
                    break;
                case 0x11: // BGEZAL
                    if ((int32_t)CURRENT_STATE.REGS[rs] >= 0) {
                        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4; // 存储返回地址
                        NEXT_STATE.PC = target_addressi;
                    } else {
                        NEXT_STATE.PC += 4;
                    }
                    break;
                default:
                    // 未知的功能码
                    break;
            }
            break;
        case 0x02: // j                                    
            // 更新程序计数器为跳转目标地址
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | target_addressj;
            break;

        case 0x03: // jal 
            // 将返回地址存储到寄存器$ra（寄存器31）
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;

            // 更新程序计数器为跳转目标地址
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | target_addressj;
            break;   
            
        case 0x04: // beq
            // 检查rs和rt寄存器的值是否相等
            if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]) {
                // 计算目标地址并更新程序计数器
                NEXT_STATE.PC += (offset << 2);
            } else {
                // 不跳转，直接更新程序计数器
                NEXT_STATE.PC += 4;
            }
            break;

        case 0x05: // bne
            // 检查rs和rt寄存器的值是否不相等
            if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]) {
                // 计算目标地址并更新程序计数器
                NEXT_STATE.PC += (offset << 2);
            } else {
                // 不跳转，直接更新程序计数器
                NEXT_STATE.PC += 4;
            }
            break;

        case 0x06: // blez
            // 检查rs寄存器的值是否小于等于零
            if ((int32_t)CURRENT_STATE.REGS[rs] <= 0) {
                // 计算目标地址并更新程序计数器
                NEXT_STATE.PC += (offset << 2);
            } else {
                // 不跳转，直接更新程序计数器
                NEXT_STATE.PC += 4;
            }
            break;

        case 0x07: // bgtz
            // 检查rs寄存器的值是否大于零
            if ((int32_t)CURRENT_STATE.REGS[rs] > 0) {
                // 计算目标地址并更新程序计数器
                NEXT_STATE.PC += (offset << 2);
            } else {
                // 不跳转，直接更新程序计数器
                NEXT_STATE.PC += 4;
            }
            break;
            
        case 0x08: // ADDI
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + immediate;
            NEXT_STATE.PC += 4;
            break;
                    
        case 0x09: // ADDIU
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + immediate;
            NEXT_STATE.PC += 4;
            break;
                    
        case 0x0A: // SLTI
            NEXT_STATE.REGS[rt] = (int32_t)CURRENT_STATE.REGS[rs] < immediate ? 1 : 0;
            NEXT_STATE.PC += 4;
            break;
                    
        case 0x0B: // SLTIU
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] < (uint32_t)immediate ? 1 : 0;
            NEXT_STATE.PC += 4;
            break;
                    
        case 0x0C: // ANDI
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & (uint32_t)immediate;
            NEXT_STATE.PC += 4;
            break;
                    
        case 0x0D: // ORI
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | (uint32_t)immediate;
            NEXT_STATE.PC += 4;
            break;
                    
        case 0x0E: // XORI
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ (uint32_t)immediate;
            NEXT_STATE.PC += 4;
            break;
                    
        case 0x0F: // LUI
            NEXT_STATE.REGS[rt] = (uint32_t)(immediate << 16);
            NEXT_STATE.PC += 4;
            break;
        
        case 0x20: // LB
            NEXT_STATE.REGS[rt] = (int32_t)(int8_t)mem_read_32(load_address);
            NEXT_STATE.PC += 4;
            break;
            
        case 0x21: // LH
            NEXT_STATE.REGS[rt] = (int32_t)(int16_t)mem_read_32(load_address);
            NEXT_STATE.PC += 4;
            break;
            
        case 0x23: // LW
            NEXT_STATE.REGS[rt] = mem_read_32(load_address);
            NEXT_STATE.PC += 4;
            break;
            
        case 0x24: // LBU
            NEXT_STATE.REGS[rt] = (uint32_t)(uint8_t)mem_read_32(load_address);
            NEXT_STATE.PC += 4;
            break;
            
        case 0x25: // LHU
            NEXT_STATE.REGS[rt] = (uint32_t)(uint16_t)mem_read_32(load_address);
            NEXT_STATE.PC += 4;
            break;
            
        case 0x28: // SB
            mem_write_32(store_address, (data_to_store & 0xFF));
            NEXT_STATE.PC += 4;
            break;
            
        case 0x29: // SH
            mem_write_32(store_address, (data_to_store & 0xFFFF));
            NEXT_STATE.PC += 4;
            break;
            
        case 0x2B: // SW
            mem_write_32(store_address, data_to_store);
            NEXT_STATE.PC += 4;
            break;   
        
            
         
        default:
            // 未知的操作码
            NEXT_STATE.PC += 4;
            break;
    }
}

