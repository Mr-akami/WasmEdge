#pragma once

#include "ast/instruction.h"
#include "common.h"
#include "executor/entry/frame.h"
#include "stackmgr.h"
#include "storemgr.h"
#include "support/casting.h"

#include <cstdint>

namespace SSVM {
namespace Executor {

class Worker {

public:
  using Byte = uint8_t;
  using Bytes = std::vector<Byte>;
  using Instructions = std::vector<AST::Instruction *>;
  enum class State : unsigned char {
    Invalid = 0, /// Default State
    Terminated, /// Reach `return` instruction
    Unreachable, /// Reach `unreachable` instruction
    Active, /// In execution
  };

public:
  /// Worker are not allowed to create without Store and Stack.
  Worker() = delete;
  explicit Worker(StoreManager &Store, StackManager &Stack)
      : StoreMgr(Store), StackMgr(Stack), TheState(State::Active) {};

  ~Worker() = default;

  /// Prepare input data for calldatacopy
  ErrCode setArguments(Bytes &Input);
  /// Prepare Wasm bytecode for execution
  ErrCode setCode(std::vector<std::unique_ptr<AST::Instruction>> *&Instrs);
  /// Execution Wasm bytecode with given input data.
  ErrCode run();

private:
  /// Execute const numeric instructions
  ErrCode runConstNumericOp(AST::Instruction *);
  /// Execute numeric instructions
  ErrCode runNumericOp(AST::Instruction *);
  /// Execute coontrol instructions
  ErrCode runControlOp(AST::Instruction *);
  /// Execute memory instructions
  ErrCode runMemoryOp(AST::Instruction *);
  /// Execute parametric instructions
  ErrCode runParametricOp(AST::Instruction *);
  /// Execute variable instructions
  ErrCode runVariableOp(AST::Instruction *);

  /// Worker State
  State TheState = State::Invalid;
  /// Arguments
  Bytes Args;
  /// Instructions of execution code.
  Instructions Instrs;
  /// Reference to Executor's Store
  StoreManager &StoreMgr;
  /// Reference to Executor's Stack
  StackManager &StackMgr;
  /// Pointer to current frame
  FrameEntry *CurrentFrame;
};

} // namespace Executor
} // namespace SSVM
