//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// abstract_executor.h
//
// Identification: src/include/execution/executors/abstract_executor.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "storage.h"

/**
 * The AbstractExecutor implements the Volcano tuple-at-a-time iterator model.
 * This is the base class from which all executors in the project, and defines
 * the minimal interface that all executors support.
 */
class AbstractExecutor {
 public:
  /**
   * Construct a new AbstractExecutor instance.
   */
  explicit AbstractExecutor(){};

  /** Virtual destructor. */
  virtual ~AbstractExecutor() = default;

  /**
   * Initialize the executor.
   * @warning This function must be called before Next() is called!
   */
  virtual void Init() = 0;

  /**
   * Yield the next tuple from this executor.
   * @param[out] tuple The next tuple produced by this executor
   * @return `true` if a tuple was produced, `false` if there are no more tuples
   */
  virtual bool Next(Tuple *tuple) = 0;
};
