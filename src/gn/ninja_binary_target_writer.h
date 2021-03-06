// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOOLS_GN_NINJA_BINARY_TARGET_WRITER_H_
#define TOOLS_GN_NINJA_BINARY_TARGET_WRITER_H_

#include "base/macros.h"
#include "gn/c_tool.h"
#include "gn/config_values.h"
#include "gn/ninja_target_writer.h"
#include "gn/target.h"
#include "gn/toolchain.h"
#include "gn/unique_vector.h"

struct EscapeOptions;

// Writes a .ninja file for a binary target type (an executable, a shared
// library, or a static library).
class NinjaBinaryTargetWriter : public NinjaTargetWriter {
 public:
  NinjaBinaryTargetWriter(const Target* target, std::ostream& out);
  ~NinjaBinaryTargetWriter() override;

  void Run() override;

 protected:
  // Returns true if jumbo mode is globally enabled and allowed for |target|.
  static bool IsJumboEnabledForTarget(const Target* target);

  // Returns a list of files that should be compiled for |target| considering
  // jumbo mode. Function may use |sources| as a handy storage and return a
  // reference to it.
  static const Target::FileList& GetSourcesForTarget(const Target* target,
                                                     Target::FileList* sources);

  // Writes to the output stream a stamp rule for inputs, and
  // returns the file to be appended to source rules that encodes the
  // implicit dependencies for the current target.
  // If num_stamp_uses is small, this might return all input dependencies
  // directly, without writing a stamp file.
  // If there are no implicit dependencies and no extra target dependencies
  // are passed in, this returns an empty vector.
  std::vector<OutputFile> WriteInputsStampAndGetDep(
      size_t num_stamp_uses) const;

  // Writes the stamp line for a source set. These are not linked.
  void WriteSourceSetStamp(const std::vector<OutputFile>& object_files);

  // Gets all target dependencies and classifies them, as well as accumulates
  // object files from source sets we need to link.
  void GetDeps(UniqueVector<OutputFile>* extra_object_files,
               UniqueVector<const Target*>* linkable_deps,
               UniqueVector<const Target*>* non_linkable_deps,
               UniqueVector<const Target*>* framework_deps) const;

  // Classifies the dependency as linkable or nonlinkable with the current
  // target, adding it to the appropriate vector. If the dependency is a source
  // set we should link in, the source set's object files will be appended to
  // |extra_object_files|.
  void ClassifyDependency(const Target* dep,
                          UniqueVector<OutputFile>* extra_object_files,
                          UniqueVector<const Target*>* linkable_deps,
                          UniqueVector<const Target*>* non_linkable_deps,
                          UniqueVector<const Target*>* framework_deps) const;

  OutputFile WriteStampAndGetDep(const UniqueVector<const SourceFile*>& files,
                                 const std::string& stamp_ext) const;

  void WriteCompilerBuildLine(const SourceFile& source,
                              const std::vector<OutputFile>& extra_deps,
                              const std::vector<OutputFile>& order_only_deps,
                              const char* tool_name,
                              const std::vector<OutputFile>& outputs);

  void WriteLinkerFlags(std::ostream& out,
                        const Tool* tool,
                        const SourceFile* optional_def_file);
  void WriteLibs(std::ostream& out, const Tool* tool);
  void WriteFrameworks(std::ostream& out, const Tool* tool);

  void AddSourceSetFiles(const Target* source_set,
                         UniqueVector<OutputFile>* obj_files) const;

  // Cached version of the prefix used for rule types for this toolchain.
  std::string rule_prefix_;

 private:
  DISALLOW_COPY_AND_ASSIGN(NinjaBinaryTargetWriter);
};

#endif  // TOOLS_GN_NINJA_BINARY_TARGET_WRITER_H_
