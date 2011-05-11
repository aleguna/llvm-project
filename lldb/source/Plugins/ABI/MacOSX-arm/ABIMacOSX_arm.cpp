//===-- ABIMacOSX_arm.cpp --------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ABIMacOSX_arm.h"

#include "lldb/Core/ConstString.h"
#include "lldb/Core/Error.h"
#include "lldb/Core/Module.h"
#include "lldb/Core/PluginManager.h"
#include "lldb/Core/Scalar.h"
#include "lldb/Core/Value.h"
#include "lldb/Symbol/ClangASTContext.h"
#include "lldb/Symbol/UnwindPlan.h"
#include "lldb/Target/Process.h"
#include "lldb/Target/RegisterContext.h"
#include "lldb/Target/Target.h"
#include "lldb/Target/Thread.h"

#include "llvm/ADT/Triple.h"

#include "Utility/ARM_DWARF_Registers.h"

#include <vector>

using namespace lldb;
using namespace lldb_private;

static const char *pluginName = "ABIMacOSX_arm";
static const char *pluginDesc = "Mac OS X ABI for arm targets";
static const char *pluginShort = "abi.macosx-arm";

size_t
ABIMacOSX_arm::GetRedZoneSize () const
{
    return 0;
}

//------------------------------------------------------------------
// Static Functions
//------------------------------------------------------------------
ABISP
ABIMacOSX_arm::CreateInstance (const ArchSpec &arch)
{
    static ABISP g_abi_sp;
    const llvm::Triple::ArchType arch_type = arch.GetTriple().getArch();
    if ((arch_type == llvm::Triple::arm) ||
        (arch_type == llvm::Triple::thumb))
    {
        if (!g_abi_sp)
            g_abi_sp.reset (new ABIMacOSX_arm);
        return g_abi_sp;
    }
    return ABISP();
}

bool
ABIMacOSX_arm::PrepareTrivialCall (Thread &thread, 
                                   addr_t sp, 
                                   addr_t functionAddress, 
                                   addr_t returnAddress, 
                                   addr_t arg,
                                   addr_t *this_arg,
                                   addr_t *cmd_arg) const
{
//    RegisterContext *reg_ctx = thread.GetRegisterContext().get();
//    if (!reg_ctx)
//        return false;    
//#define CHAIN_EBP
//
//#ifndef CHAIN_EBP
//    uint32_t ebpID = reg_ctx->ConvertRegisterKindToRegisterNumber (eRegisterKindGeneric, LLDB_REGNUM_GENERIC_FP);
//#endif
//    uint32_t eipID = reg_ctx->ConvertRegisterKindToRegisterNumber (eRegisterKindGeneric, LLDB_REGNUM_GENERIC_PC);
//    uint32_t espID = reg_ctx->ConvertRegisterKindToRegisterNumber (eRegisterKindGeneric, LLDB_REGNUM_GENERIC_SP);
//    
//    // Make room for the argument(s) on the stack
//    
//    if (this_arg && cmd_arg)
//        sp -= 12;
//    else if (this_arg)
//        sp -= 8;
//    else
//        sp -= 4;
//    
//    // Align the SP
//    
//    sp &= ~(0xfull); // 16-byte alignment
//    
//    // Write the argument on the stack
//    
//    Error error;
//    
//    if (this_arg && cmd_arg)
//    {
//        uint32_t cmd_argU32 = *cmd_arg & 0xffffffffull;
//        uint32_t this_argU32 = *this_arg & 0xffffffffull;
//        uint32_t argU32 = arg & 0xffffffffull;
//        
//        if (thread.GetProcess().WriteMemory(sp, &this_argU32, sizeof(this_argU32), error) != sizeof(this_argU32))
//            return false;
//        if (thread.GetProcess().WriteMemory(sp + 4, &cmd_argU32, sizeof(cmd_argU32), error) != sizeof(cmd_argU32))
//            return false;
//        if (thread.GetProcess().WriteMemory(sp + 8, &argU32, sizeof(argU32), error) != sizeof(argU32))
//            return false;
//    }
//    else if (this_arg)
//    {
//        uint32_t this_argU32 = *this_arg & 0xffffffffull;
//        uint32_t argU32 = arg & 0xffffffffull;
//                
//        if (thread.GetProcess().WriteMemory(sp, &this_argU32, sizeof(this_argU32), error) != sizeof(this_argU32))
//            return false;
//        if (thread.GetProcess().WriteMemory(sp + 4, &argU32, sizeof(argU32), error) != sizeof(argU32))
//            return false;
//    }
//    else
//    {
//        uint32_t argU32 = arg & 0xffffffffull;
//
//        if (thread.GetProcess().WriteMemory (sp, &argU32, sizeof(argU32), error) != sizeof(argU32))
//            return false;
//    }
//    
//    // The return address is pushed onto the stack.
//    
//    sp -= 4;
//    uint32_t returnAddressU32 = returnAddress;
//    if (thread.GetProcess().WriteMemory (sp, &returnAddressU32, sizeof(returnAddressU32), error) != sizeof(returnAddressU32))
//        return false;
//    
//    // %esp is set to the actual stack value.
//    
//    if (!reg_ctx->WriteRegisterFromUnsigned(espID, sp))
//        return false;
//    
//#ifndef CHAIN_EBP
//    // %ebp is set to a fake value, in our case 0x0x00000000
//    
//    if (!reg_ctx->WriteRegisterFromUnsigned(ebpID, 0x00000000))
//        return false;
//#endif
//    
//    // %eip is set to the address of the called function.
//    
//    if (!reg_ctx->WriteRegisterFromUnsigned(eipID, functionAddress))
//        return false;
//    
//    return true;
    return false;
}

bool
ABIMacOSX_arm::PrepareNormalCall (Thread &thread,
                                  addr_t sp,
                                  addr_t functionAddress,
                                  addr_t returnAddress,
                                  ValueList &args) const
{
//    RegisterContext *reg_ctx = thread.GetRegisterContext().get();
//    if (!reg_ctx)
//        return false;
//    Error error;
//    uint32_t ebpID = reg_ctx->ConvertRegisterKindToRegisterNumber (eRegisterKindGeneric, LLDB_REGNUM_GENERIC_FP);
//    uint32_t eipID = reg_ctx->ConvertRegisterKindToRegisterNumber (eRegisterKindGeneric, LLDB_REGNUM_GENERIC_PC);
//    uint32_t espID = reg_ctx->ConvertRegisterKindToRegisterNumber (eRegisterKindGeneric, LLDB_REGNUM_GENERIC_SP);
//    
//    // Do the argument layout
//    
//    std::vector <uint32_t> argLayout;   // 4-byte chunks, as discussed in the ABI Function Call Guide
//    
//    size_t numArgs = args.GetSize();
//    size_t index;
//    
//    for (index = 0; index < numArgs; ++index)
//    {
//        Value *val = args.GetValueAtIndex(index);
//        
//        if (!val)
//            return false;
//        
//        switch (val->GetValueType())
//        {
//        case Value::eValueTypeScalar:
//            {
//                Scalar &scalar = val->GetScalar();
//                switch (scalar.GetType())
//                {
//                case Scalar::e_void:
//                default:
//                    return false;
//                case Scalar::e_sint: 
//                case Scalar::e_uint:
//                case Scalar::e_slong:
//                case Scalar::e_ulong:
//                case Scalar::e_slonglong:
//                case Scalar::e_ulonglong:
//                    {
//                        uint64_t data = scalar.ULongLong();
//                        
//                        switch (scalar.GetByteSize())
//                        {
//                        default:
//                            return false;
//                        case 1:
//                            argLayout.push_back((uint32_t)(data & 0xffull));
//                            break;
//                        case 2:
//                            argLayout.push_back((uint32_t)(data & 0xffffull));
//                            break;
//                        case 4:
//                            argLayout.push_back((uint32_t)(data & 0xffffffffull));
//                            break;
//                        case 8:
//                            argLayout.push_back((uint32_t)(data & 0xffffffffull));
//                            argLayout.push_back((uint32_t)(data >> 32));
//                            break;
//                        }
//                    }
//                    break;
//                case Scalar::e_float:
//                    {
//                        float data = scalar.Float();
//                        uint32_t dataRaw = *((uint32_t*)(&data));
//                        argLayout.push_back(dataRaw);
//                    }
//                    break;
//                case Scalar::e_double:
//                    {
//                        double data = scalar.Double();
//                        uint32_t *dataRaw = ((uint32_t*)(&data));
//                        argLayout.push_back(dataRaw[0]);
//                        argLayout.push_back(dataRaw[1]);
//                    }
//                    break;
//                case Scalar::e_long_double:
//                    {
//                        long double data = scalar.Double();
//                        uint32_t *dataRaw = ((uint32_t*)(&data));
//                        while ((argLayout.size() * 4) & 0xf)
//                            argLayout.push_back(0);
//                        argLayout.push_back(dataRaw[0]);
//                        argLayout.push_back(dataRaw[1]);
//                        argLayout.push_back(dataRaw[2]);
//                        argLayout.push_back(dataRaw[3]);
//                    }
//                    break;
//                }
//            }
//            break;
//        case Value::eValueTypeHostAddress:
//            switch (val->GetContextType()) 
//            {
//            default:
//                return false;
//            case Value::eContextTypeClangType:
//                {
//                    void *val_type = val->GetClangType();
//                    uint32_t cstr_length;
//                    
//                    if (ClangASTContext::IsCStringType (val_type, cstr_length))
//                    {
//                        const char *cstr = (const char*)val->GetScalar().ULongLong();
//                        cstr_length = strlen(cstr);
//                        
//                        // Push the string onto the stack immediately.
//                        
//                        sp -= (cstr_length + 1);
//                        
//                        if (thread.GetProcess().WriteMemory(sp, cstr, cstr_length + 1, error) != (cstr_length + 1))
//                            return false;
//                        
//                        // Put the address of the string into the argument array.
//                        
//                        argLayout.push_back((uint32_t)(sp & 0xffffffff));
//                    }
//                    else
//                    {
//                        return false;
//                    }
//                }
//                break;
//            }
//            break;
//        case Value::eValueTypeFileAddress:
//        case Value::eValueTypeLoadAddress:
//        default:
//            return false;
//        }
//    }
//    
//    // Make room for the arguments on the stack
//    
//    sp -= 4 * argLayout.size();
//    
//    // Align the SP
//    
//    sp &= ~(0xfull); // 16-byte alignment
//    
//    // Write the arguments on the stack
//    
//    size_t numChunks = argLayout.size();
//
//    for (index = 0; index < numChunks; ++index)
//        if (thread.GetProcess().WriteMemory(sp + (index * 4), &argLayout[index], sizeof(uint32_t), error) != sizeof(uint32_t))
//            return false;
//    
//    // The return address is pushed onto the stack.
//    
//    sp -= 4;
//    uint32_t returnAddressU32 = returnAddress;
//    if (thread.GetProcess().WriteMemory (sp, &returnAddressU32, sizeof(returnAddressU32), error) != sizeof(returnAddressU32))
//        return false;
//    
//    // %esp is set to the actual stack value.
//    
//    if (!reg_ctx->WriteRegisterFromUnsigned(espID, sp))
//        return false;
//    
//    // %ebp is set to a fake value, in our case 0x0x00000000
//    
//    if (!reg_ctx->WriteRegisterFromUnsigned(ebpID, 0x00000000))
//        return false;
//    
//    // %eip is set to the address of the called function.
//    
//    if (!reg_ctx->WriteRegisterFromUnsigned(eipID, functionAddress))
//        return false;
//    
//    return true;    
    return false;
}

static bool 
ReadIntegerArgument (Scalar &scalar,
                     unsigned int bit_width,
                     bool is_signed,
                     Process &process,
                     addr_t &current_stack_argument)
{
//    if (bit_width > 64)
//        return false; // Scalar can't hold large integer arguments
//    
//    uint64_t arg_contents;
//    uint32_t read_data;
//    Error error;
//    
//    if (bit_width > 32)
//    {
//        if (process.ReadMemory(current_stack_argument, &read_data, sizeof(read_data), error) != sizeof(read_data))
//            return false;
//        
//        arg_contents = read_data;
//        
//        if (process.ReadMemory(current_stack_argument + 4, &read_data, sizeof(read_data), error) != sizeof(read_data))
//            return false;
//        
//        arg_contents |= ((uint64_t)read_data) << 32;
//        
//        current_stack_argument += 8;
//    }
//    else {
//        if (process.ReadMemory(current_stack_argument, &read_data, sizeof(read_data), error) != sizeof(read_data))
//            return false;
//        
//        arg_contents = read_data;
//        
//        current_stack_argument += 4;
//    }
//    
//    if (is_signed)
//    {
//        switch (bit_width)
//        {
//        default:
//            return false;
//        case 8:
//            scalar = (int8_t)(arg_contents & 0xff);
//            break;
//        case 16:
//            scalar = (int16_t)(arg_contents & 0xffff);
//            break;
//        case 32:
//            scalar = (int32_t)(arg_contents & 0xffffffff);
//            break;
//        case 64:
//            scalar = (int64_t)arg_contents;
//            break;
//        }
//    }
//    else
//    {
//        switch (bit_width)
//        {
//        default:
//            return false;
//        case 8:
//            scalar = (uint8_t)(arg_contents & 0xff);
//            break;
//        case 16:
//            scalar = (uint16_t)(arg_contents & 0xffff);
//            break;
//        case 32:
//            scalar = (uint32_t)(arg_contents & 0xffffffff);
//            break;
//        case 64:
//            scalar = (uint64_t)arg_contents;
//            break;
//        }
//    }
//    
//    return true;
    return false;
}

bool
ABIMacOSX_arm::GetArgumentValues (Thread &thread,
                                  ValueList &values) const
{
//    unsigned int num_values = values.GetSize();
//    unsigned int value_index;
//    
//    // Extract the Clang AST context from the PC so that we can figure out type
//    // sizes
//    
//    clang::ASTContext *ast_context = thread.CalculateTarget()->GetScratchClangASTContext()->getASTContext();
//    
//    // Get the pointer to the first stack argument so we have a place to start 
//    // when reading data
//    
//    RegisterContext *reg_ctx = thread.GetRegisterContext().get();
//    
//    if (!reg_ctx)
//        return false;
//    
//    addr_t sp = reg_ctx->GetSP(0);
//    
//    if (!sp)
//        return false;
//    
//    addr_t current_stack_argument = sp + 4; // jump over return address
//    
//    for (value_index = 0;
//         value_index < num_values;
//         ++value_index)
//    {
//        Value *value = values.GetValueAtIndex(value_index);
//        
//        if (!value)
//            return false;
//        
//        // We currently only support extracting values with Clang QualTypes.
//        // Do we care about others?
//        switch (value->GetContextType())
//        {
//            default:
//                return false;
//            case Value::eContextTypeClangType:
//            {
//                void *value_type = value->GetClangType();
//                bool is_signed;
//                
//                if (ClangASTContext::IsIntegerType (value_type, is_signed))
//                {
//                    size_t bit_width = ClangASTType::GetClangTypeBitWidth(ast_context, value_type);
//                    
//                    ReadIntegerArgument(value->GetScalar(),
//                                        bit_width, 
//                                        is_signed,
//                                        thread.GetProcess(), 
//                                        current_stack_argument);
//                }
//                else if (ClangASTContext::IsPointerType (value_type))
//                {
//                    ReadIntegerArgument(value->GetScalar(),
//                                        32,
//                                        false,
//                                        thread.GetProcess(),
//                                        current_stack_argument);
//                }
//            }
//                break;
//        }
//    }
//    
//    return true;
    return false;
}

bool
ABIMacOSX_arm::GetReturnValue (Thread &thread,
                               Value &value) const
{
//    switch (value.GetContextType())
//    {
//        default:
//            return false;
//        case Value::eContextTypeClangType:
//        {
//            // Extract the Clang AST context from the PC so that we can figure out type
//            // sizes
//            
//            clang::ASTContext *ast_context = thread.CalculateTarget()->GetScratchClangASTContext()->getASTContext();
//            
//            // Get the pointer to the first stack argument so we have a place to start 
//            // when reading data
//            
//            RegisterContext *reg_ctx = thread.GetRegisterContext().get();
//            
//            void *value_type = value.GetClangType();
//            bool is_signed;
//            
//            if (ClangASTContext::IsIntegerType (value_type, is_signed))
//            {
//                size_t bit_width = ClangASTType::GetClangTypeBitWidth(ast_context, value_type);
//                
//                unsigned eax_id = reg_ctx->GetRegisterInfoByName("eax", 0)->kinds[eRegisterKindLLDB];
//                unsigned edx_id = reg_ctx->GetRegisterInfoByName("edx", 0)->kinds[eRegisterKindLLDB];
//                
//                switch (bit_width)
//                {
//                    default:
//                    case 128:
//                        // Scalar can't hold 128-bit literals, so we don't handle this
//                        return false;
//                    case 64:
//                        uint64_t raw_value;
//                        raw_value = thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xffffffff;
//                        raw_value |= (thread.GetRegisterContext()->ReadRegisterAsUnsigned(edx_id, 0) & 0xffffffff) << 32;
//                        if (is_signed)
//                            value.GetScalar() = (int64_t)raw_value;
//                        else
//                            value.GetScalar() = (uint64_t)raw_value;
//                        break;
//                    case 32:
//                        if (is_signed)
//                            value.GetScalar() = (int32_t)(thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xffffffff);
//                        else
//                            value.GetScalar() = (uint32_t)(thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xffffffff);
//                        break;
//                    case 16:
//                        if (is_signed)
//                            value.GetScalar() = (int16_t)(thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xffff);
//                        else
//                            value.GetScalar() = (uint16_t)(thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xffff);
//                        break;
//                    case 8:
//                        if (is_signed)
//                            value.GetScalar() = (int8_t)(thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xff);
//                        else
//                            value.GetScalar() = (uint8_t)(thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xff);
//                        break;
//                }
//            }
//            else if (ClangASTContext::IsPointerType (value_type))
//            {
//                unsigned eax_id = reg_ctx->GetRegisterInfoByName("eax", 0)->kinds[eRegisterKindLLDB];
//                uint32_t ptr = thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xffffffff;
//                value.GetScalar() = ptr;
//            }
//            else
//            {
//                // not handled yet
//                return false;
//            }
//        }
//            break;
//    }
//    
//    return true;
    return false;
}

bool
ABIMacOSX_arm::CreateFunctionEntryUnwindPlan (UnwindPlan &unwind_plan)
{
    uint32_t reg_kind = unwind_plan.GetRegisterKind();
    uint32_t lr_reg_num = LLDB_INVALID_REGNUM;
    uint32_t sp_reg_num = LLDB_INVALID_REGNUM;
    uint32_t pc_reg_num = LLDB_INVALID_REGNUM;
    
    switch (reg_kind)
    {
        case eRegisterKindDWARF:
        case eRegisterKindGCC:
            lr_reg_num = dwarf_lr;
            sp_reg_num = dwarf_sp;
            pc_reg_num = dwarf_pc;
            break;
            
        case eRegisterKindGeneric:
            lr_reg_num = LLDB_REGNUM_GENERIC_RA;
            sp_reg_num = LLDB_REGNUM_GENERIC_SP;
            pc_reg_num = LLDB_REGNUM_GENERIC_PC;
            break;
    }
    
    if (lr_reg_num == LLDB_INVALID_REGNUM ||
        sp_reg_num == LLDB_INVALID_REGNUM ||
        pc_reg_num == LLDB_INVALID_REGNUM)
        return false;

    unwind_plan.Clear();
    unwind_plan.SetRegisterKind (eRegisterKindDWARF);
    
    UnwindPlan::Row row;
    
    // Our previous Call Frame Address is the stack pointer
    row.SetCFARegister (sp_reg_num);
    
    // Our previous PC is in the LR
    row.SetRegisterLocationToRegister(pc_reg_num, lr_reg_num, true);
    unwind_plan.AppendRow (row);
    
    // All other registers are the same.
    
    unwind_plan.SetSourceName (pluginName);
    return true;
}

bool
ABIMacOSX_arm::CreateDefaultUnwindPlan (UnwindPlan &unwind_plan)
{
    uint32_t reg_kind = unwind_plan.GetRegisterKind();
    uint32_t fp_reg_num = LLDB_INVALID_REGNUM;
    uint32_t sp_reg_num = LLDB_INVALID_REGNUM;
    uint32_t pc_reg_num = LLDB_INVALID_REGNUM;
    
    switch (reg_kind)
    {
        case eRegisterKindDWARF:
        case eRegisterKindGCC:
            fp_reg_num = dwarf_r7; // apple uses r7 for all frames. Normal arm uses r11
            sp_reg_num = dwarf_sp;
            pc_reg_num = dwarf_pc;
            break;
            
        case eRegisterKindGeneric:
            fp_reg_num = LLDB_REGNUM_GENERIC_FP;
            sp_reg_num = LLDB_REGNUM_GENERIC_SP;
            pc_reg_num = LLDB_REGNUM_GENERIC_PC;
            break;
    }
    
    if (fp_reg_num == LLDB_INVALID_REGNUM ||
        sp_reg_num == LLDB_INVALID_REGNUM ||
        pc_reg_num == LLDB_INVALID_REGNUM)
        return false;
    
    UnwindPlan::Row row;    
    const int32_t ptr_size = 8;
    
    unwind_plan.SetRegisterKind (eRegisterKindGeneric);
    row.SetCFARegister (fp_reg_num);
    row.SetCFAOffset (2 * ptr_size);
    row.SetOffset (0);
    
    row.SetRegisterLocationToAtCFAPlusOffset(fp_reg_num, ptr_size * -2, true);
    row.SetRegisterLocationToAtCFAPlusOffset(pc_reg_num, ptr_size * -1, true);
    
    unwind_plan.AppendRow (row);
    unwind_plan.SetSourceName ("arm-apple-darwin default unwind plan");
    return true;
}

bool
ABIMacOSX_arm::RegisterIsVolatile (const RegisterInfo *reg_info)
{
    if (reg_info)
    {
        // Volatile registers include: ebx, ebp, esi, edi, esp, eip
        const char *name = reg_info->name;
        if (name[0] == 'r')
        {
            switch (name[1])
            {
                case '0': return name[2] == '\0'; // r0
                case '1': 
                    switch (name[2])
                    {
                    case '\0':
                        return true; // r1
                    case '2':
                    case '3':
                        return name[2] == '\0'; // r12 - r13
                    default:
                        break;
                    }
                    break;

                case '2': return name[2] == '\0'; // r2
                case '3': return name[2] == '\0'; // r3
                case '9': return name[2] == '\0'; // r9 (apple-darwin only...)
                    
                break;
            }
        }
        else if (name[0] == 'd')
        {
            switch (name[1])
            {
                case '0': 
                    return name[2] == '\0'; // d0

                case '1':
                    switch (name[2])
                    {
                    case '\0':
                        return true; // d1;
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        return name[3] == '\0'; // d16 - d19
                    default:
                        break;
                    }
                    break;
                    
                case '2':
                    switch (name[2])
                    {
                    case '\0':
                        return true; // d2;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        return name[3] == '\0'; // d20 - d29
                    default:
                        break;
                    }
                    break;

                case '3':
                    switch (name[2])
                    {
                    case '\0':
                        return true; // d3;
                    case '0':
                    case '1':
                        return name[3] == '\0'; // d30 - d31
                    default:
                        break;
                    }
                case '4':
                case '5':
                case '6':
                case '7':
                    return name[2] == '\0'; // d4 - d7

                default:
                    break;
            }
        }
        else if (name[0] == 's' && name[1] == 'p' && name[2] == '\0')
            return true;
    }
    return false;
}

void
ABIMacOSX_arm::Initialize()
{
    PluginManager::RegisterPlugin (pluginName,
                                   pluginDesc,
                                   CreateInstance);    
}

void
ABIMacOSX_arm::Terminate()
{
    PluginManager::UnregisterPlugin (CreateInstance);
}

//------------------------------------------------------------------
// PluginInterface protocol
//------------------------------------------------------------------
const char *
ABIMacOSX_arm::GetPluginName()
{
    return pluginName;
}

const char *
ABIMacOSX_arm::GetShortPluginName()
{
    return pluginShort;
}

uint32_t
ABIMacOSX_arm::GetPluginVersion()
{
    return 1;
}

