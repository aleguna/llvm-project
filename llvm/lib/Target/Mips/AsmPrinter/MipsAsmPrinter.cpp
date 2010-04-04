//===-- MipsAsmPrinter.cpp - Mips LLVM assembly writer --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format MIPS assembly language.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "mips-asm-printer"

#include "Mips.h"
#include "MipsSubtarget.h"
#include "MipsInstrInfo.h"
#include "MipsTargetMachine.h"
#include "MipsMachineFunction.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/DwarfWriter.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Target/Mangler.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetLoweringObjectFile.h" 
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/MathExtras.h"
#include <cctype>
using namespace llvm;

namespace {
  class MipsAsmPrinter : public AsmPrinter {
    const MipsSubtarget *Subtarget;
  public:
    explicit MipsAsmPrinter(formatted_raw_ostream &O, TargetMachine &TM, 
                            MCStreamer &Streamer)
      : AsmPrinter(O, TM, Streamer) {
      Subtarget = &TM.getSubtarget<MipsSubtarget>();
    }

    virtual const char *getPassName() const {
      return "Mips Assembly Printer";
    }

    bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo, 
                         unsigned AsmVariant, const char *ExtraCode);
    void printOperand(const MachineInstr *MI, int opNum, raw_ostream &O);
    void printUnsignedImm(const MachineInstr *MI, int opNum, raw_ostream &O);
    void printMemOperand(const MachineInstr *MI, int opNum, raw_ostream &O, 
                         const char *Modifier = 0);
    void printFCCOperand(const MachineInstr *MI, int opNum, raw_ostream &O, 
                         const char *Modifier = 0);
    void printSavedRegsBitmask(raw_ostream &O);
    void printHex32(unsigned int Value, raw_ostream &O);

    const char *emitCurrentABIString();
    void emitFrameDirective(raw_ostream &O);

    void printInstruction(const MachineInstr *MI, raw_ostream &O); // autogen'd.
    void EmitInstruction(const MachineInstr *MI) {
      printInstruction(MI, O);
      OutStreamer.AddBlankLine();
    }
    virtual void EmitFunctionBodyStart();
    virtual void EmitFunctionBodyEnd();
    static const char *getRegisterName(unsigned RegNo);

    virtual void EmitFunctionEntryLabel();
    void EmitStartOfAsmFile(Module &M);
  };
} // end of anonymous namespace

#include "MipsGenAsmWriter.inc"

//===----------------------------------------------------------------------===//
//
//  Mips Asm Directives
//
//  -- Frame directive "frame Stackpointer, Stacksize, RARegister"
//  Describe the stack frame.
//
//  -- Mask directives "(f)mask  bitmask, offset" 
//  Tells the assembler which registers are saved and where.
//  bitmask - contain a little endian bitset indicating which registers are 
//            saved on function prologue (e.g. with a 0x80000000 mask, the 
//            assembler knows the register 31 (RA) is saved at prologue.
//  offset  - the position before stack pointer subtraction indicating where 
//            the first saved register on prologue is located. (e.g. with a
//
//  Consider the following function prologue:
//
//    .frame  $fp,48,$ra
//    .mask   0xc0000000,-8
//       addiu $sp, $sp, -48
//       sw $ra, 40($sp)
//       sw $fp, 36($sp)
//
//    With a 0xc0000000 mask, the assembler knows the register 31 (RA) and 
//    30 (FP) are saved at prologue. As the save order on prologue is from 
//    left to right, RA is saved first. A -8 offset means that after the 
//    stack pointer subtration, the first register in the mask (RA) will be
//    saved at address 48-8=40.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Mask directives
//===----------------------------------------------------------------------===//

// Create a bitmask with all callee saved registers for CPU or Floating Point 
// registers. For CPU registers consider RA, GP and FP for saving if necessary.
void MipsAsmPrinter::printSavedRegsBitmask(raw_ostream &O) {
  const TargetRegisterInfo &RI = *TM.getRegisterInfo();
  const MipsFunctionInfo *MipsFI = MF->getInfo<MipsFunctionInfo>();
             
  // CPU and FPU Saved Registers Bitmasks
  unsigned int CPUBitmask = 0;
  unsigned int FPUBitmask = 0;

  // Set the CPU and FPU Bitmasks
  const MachineFrameInfo *MFI = MF->getFrameInfo();
  const std::vector<CalleeSavedInfo> &CSI = MFI->getCalleeSavedInfo();
  for (unsigned i = 0, e = CSI.size(); i != e; ++i) {
    unsigned RegNum = MipsRegisterInfo::getRegisterNumbering(CSI[i].getReg());
    if (CSI[i].getRegClass() == Mips::CPURegsRegisterClass)
      CPUBitmask |= (1 << RegNum);
    else
      FPUBitmask |= (1 << RegNum);
  }

  // Return Address and Frame registers must also be set in CPUBitmask.
  if (RI.hasFP(*MF)) 
    CPUBitmask |= (1 << MipsRegisterInfo::
                getRegisterNumbering(RI.getFrameRegister(*MF)));
  
  if (MFI->hasCalls()) 
    CPUBitmask |= (1 << MipsRegisterInfo::
                getRegisterNumbering(RI.getRARegister()));

  // Print CPUBitmask
  O << "\t.mask \t"; printHex32(CPUBitmask, O);
  O << ',' << MipsFI->getCPUTopSavedRegOff() << '\n';

  // Print FPUBitmask
  O << "\t.fmask\t"; printHex32(FPUBitmask, O); O << ","
    << MipsFI->getFPUTopSavedRegOff() << '\n';
}

// Print a 32 bit hex number with all numbers.
void MipsAsmPrinter::printHex32(unsigned Value, raw_ostream &O) {
  O << "0x";
  for (int i = 7; i >= 0; i--) 
    O << utohexstr((Value & (0xF << (i*4))) >> (i*4));
}

//===----------------------------------------------------------------------===//
// Frame and Set directives
//===----------------------------------------------------------------------===//

/// Frame Directive
void MipsAsmPrinter::emitFrameDirective(raw_ostream &O) {
  const TargetRegisterInfo &RI = *TM.getRegisterInfo();

  unsigned stackReg  = RI.getFrameRegister(*MF);
  unsigned returnReg = RI.getRARegister();
  unsigned stackSize = MF->getFrameInfo()->getStackSize();


  O << "\t.frame\t" << '$' << LowercaseString(getRegisterName(stackReg))
                    << ',' << stackSize << ','
                    << '$' << LowercaseString(getRegisterName(returnReg))
                    << '\n';
}

/// Emit Set directives.
const char *MipsAsmPrinter::emitCurrentABIString() {  
  switch(Subtarget->getTargetABI()) {
    case MipsSubtarget::O32:  return "abi32";  
    case MipsSubtarget::O64:  return "abiO64";
    case MipsSubtarget::N32:  return "abiN32";
    case MipsSubtarget::N64:  return "abi64";
    case MipsSubtarget::EABI: return "eabi32"; // TODO: handle eabi64
    default: break;
  }

  llvm_unreachable("Unknown Mips ABI");
  return NULL;
}  

void MipsAsmPrinter::EmitFunctionEntryLabel() {
  O << "\t.ent\t" << *CurrentFnSym << '\n';
  OutStreamer.EmitLabel(CurrentFnSym);
}

/// EmitFunctionBodyStart - Targets can override this to emit stuff before
/// the first basic block in the function.
void MipsAsmPrinter::EmitFunctionBodyStart() {
  emitFrameDirective(O);
  printSavedRegsBitmask(O);
}

/// EmitFunctionBodyEnd - Targets can override this to emit stuff after
/// the last basic block in the function.
void MipsAsmPrinter::EmitFunctionBodyEnd() {
  // There are instruction for this macros, but they must
  // always be at the function end, and we can't emit and
  // break with BB logic. 
  O << "\t.set\tmacro\n"; 
  O << "\t.set\treorder\n"; 
  
  O << "\t.end\t" << *CurrentFnSym << '\n';
}


// Print out an operand for an inline asm expression.
bool MipsAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo, 
                                     unsigned AsmVariant,const char *ExtraCode){
  // Does this asm operand have a single letter operand modifier?
  if (ExtraCode && ExtraCode[0]) 
    return true; // Unknown modifier.

  printOperand(MI, OpNo, O);
  return false;
}

void MipsAsmPrinter::printOperand(const MachineInstr *MI, int opNum,
                                  raw_ostream &O) {
  const MachineOperand &MO = MI->getOperand(opNum);
  bool closeP = false;

  if (MO.getTargetFlags())
    closeP = true;

  switch(MO.getTargetFlags()) {
  case MipsII::MO_GPREL:    O << "%gp_rel("; break;
  case MipsII::MO_GOT_CALL: O << "%call16("; break;
  case MipsII::MO_GOT:
    if (MI->getOpcode() == Mips::LW)
      O << "%got(";
    else
      O << "%lo(";
    break;
  case MipsII::MO_ABS_HILO:
    if (MI->getOpcode() == Mips::LUi)
      O << "%hi(";
    else
      O << "%lo(";     
    break;
  }

  switch (MO.getType()) {
    case MachineOperand::MO_Register:
      O << '$' << LowercaseString(getRegisterName(MO.getReg()));
      break;

    case MachineOperand::MO_Immediate:
      O << (short int)MO.getImm();
      break;

    case MachineOperand::MO_MachineBasicBlock:
      O << *MO.getMBB()->getSymbol();
      return;

    case MachineOperand::MO_GlobalAddress:
      O << *Mang->getSymbol(MO.getGlobal());
      break;

    case MachineOperand::MO_ExternalSymbol:
      O << *GetExternalSymbolSymbol(MO.getSymbolName());
      break;

    case MachineOperand::MO_JumpTableIndex:
      O << MAI->getPrivateGlobalPrefix() << "JTI" << getFunctionNumber()
        << '_' << MO.getIndex();
      break;

    case MachineOperand::MO_ConstantPoolIndex:
      O << MAI->getPrivateGlobalPrefix() << "CPI"
        << getFunctionNumber() << "_" << MO.getIndex();
      if (MO.getOffset())
        O << "+" << MO.getOffset();
      break;
  
    default:
      llvm_unreachable("<unknown operand type>");
  }

  if (closeP) O << ")";
}

void MipsAsmPrinter::printUnsignedImm(const MachineInstr *MI, int opNum,
                                      raw_ostream &O) {
  const MachineOperand &MO = MI->getOperand(opNum);
  if (MO.getType() == MachineOperand::MO_Immediate)
    O << (unsigned short int)MO.getImm();
  else 
    printOperand(MI, opNum, O);
}

void MipsAsmPrinter::
printMemOperand(const MachineInstr *MI, int opNum, raw_ostream &O,
                const char *Modifier) {
  // when using stack locations for not load/store instructions
  // print the same way as all normal 3 operand instructions.
  if (Modifier && !strcmp(Modifier, "stackloc")) {
    printOperand(MI, opNum+1, O);
    O << ", ";
    printOperand(MI, opNum, O);
    return;
  }

  // Load/Store memory operands -- imm($reg) 
  // If PIC target the target is loaded as the 
  // pattern lw $25,%call16($28)
  printOperand(MI, opNum, O);
  O << "(";
  printOperand(MI, opNum+1, O);
  O << ")";
}

void MipsAsmPrinter::
printFCCOperand(const MachineInstr *MI, int opNum, raw_ostream &O,
                const char *Modifier) {
  const MachineOperand& MO = MI->getOperand(opNum);
  O << Mips::MipsFCCToString((Mips::CondCode)MO.getImm()); 
}

void MipsAsmPrinter::EmitStartOfAsmFile(Module &M) {
  // FIXME: Use SwitchSection.
  
  // Tell the assembler which ABI we are using
  O << "\t.section .mdebug." << emitCurrentABIString() << '\n';

  // TODO: handle O64 ABI
  if (Subtarget->isABI_EABI())
    O << "\t.section .gcc_compiled_long" << 
      (Subtarget->isGP32bit() ? "32" : "64") << '\n';

  // return to previous section
  O << "\t.previous" << '\n'; 
}

// Force static initialization.
extern "C" void LLVMInitializeMipsAsmPrinter() { 
  RegisterAsmPrinter<MipsAsmPrinter> X(TheMipsTarget);
  RegisterAsmPrinter<MipsAsmPrinter> Y(TheMipselTarget);
}
