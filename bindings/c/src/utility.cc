// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#include "vw/experimental/utility.h"
#include <memory>

#include "allocator.h"
#include "error_handling.h"
#include "interop_helper.h"
#include "owned_string.h"

#include "c_io_adapter.h"
#include "vw.h"

VW_DLL_PUBLIC VWErrorInfo* vw_create_error_info() noexcept
{
  return reinterpret_cast<VWErrorInfo*>(new owned_string());
}
VW_DLL_PUBLIC void vw_destroy_error_info(VWErrorInfo* errorString) noexcept
{
  delete reinterpret_cast<owned_string*>(errorString);
}

VW_DLL_PUBLIC const char* vw_error_info_get_message(const VWErrorInfo* errorString) noexcept
{
  const auto* ownedString = reinterpret_cast<const owned_string*>(errorString);
  return ownedString->string_data.c_str();
}

VW_DLL_PUBLIC VWString* vw_create_string() noexcept { return reinterpret_cast<VWString*>(new owned_string()); }
VW_DLL_PUBLIC void vw_destroy_string(VWString* vwString) noexcept { delete reinterpret_cast<owned_string*>(vwString); }
VW_DLL_PUBLIC const char* vw_string_to_c_string(const VWString* vwString) noexcept
{
  const auto* ownedString = reinterpret_cast<const owned_string*>(vwString);
  return ownedString->string_data.c_str();
}

vw_allocator global_alloc{default_allocator, default_deallocator};
VW_DLL_PUBLIC VWAllocator* vw_get_default_allocator() noexcept { return reinterpret_cast<VWAllocator*>(&global_alloc); }

// Saving
VW_DLL_PUBLIC VWStatus vw_workspace_save_model(
    VWWorkspace* workspace_handle, void* context, VWWriteFunc* writer, VWErrorInfo* err_info_container) noexcept
{
  ARG_NOT_NULL(workspace_handle, err_info_container);
  ARG_NOT_NULL(writer, err_info_container);

  auto* workspace = from_opaque(workspace_handle);
  io_buf model_buffer;
  model_buffer.add_file(VW::make_unique<c_writer>(context, writer));

  VW::save_predictor(*workspace, model_buffer, false);

  return VW_success;
}

// Will fail if workspace not setup to do this
VW_DLL_PUBLIC VWStatus vw_workspace_save_readable_model(
    VWWorkspace* workspace_handle, void* context, VWWriteFunc* writer, VWErrorInfo* err_info_container) noexcept
{
  ARG_NOT_NULL(workspace_handle, err_info_container);
  ARG_NOT_NULL(writer, err_info_container);
  io_buf model_buffer;
  model_buffer.add_file(VW::make_unique<c_writer>(context, writer));

  auto* workspace = from_opaque(workspace_handle);
  // Passing true causes the model to be outputted as text.
  VW::save_predictor(*workspace, model_buffer, true /*as_text*/);

  return VW_success;
}

// Will fail if workspace not setup to do this
VW_DLL_PUBLIC VWStatus vw_workspace_save_invert_hash_model(
    VWWorkspace* workspace_handle, void* context, VWWriteFunc* writer, VWErrorInfo* err_info_container) noexcept
{
  ARG_NOT_NULL(workspace_handle, err_info_container);
  ARG_NOT_NULL(writer, err_info_container);
  io_buf model_buffer;
  model_buffer.add_file(VW::make_unique<c_writer>(context, writer));

  // Passing true causes the model to be outputted as text. Additionally this flag causes the inverted hashes to also be
  // output.
  auto* workspace = from_opaque(workspace_handle);
  const auto saved_print_invert = workspace->print_invert;
  workspace->print_invert = true;
  VW::save_predictor(*workspace, model_buffer, true /*as_text*/);
  workspace->print_invert = saved_print_invert;
  return VW_success;
}

// The one passed in options
VW_DLL_PUBLIC VWStatus vw_workspace_get_configured_audit(
    const VWWorkspace* workspace_handle, bool* audit, VWErrorInfo* err_info_container) noexcept
{
  ARG_NOT_NULL(workspace_handle, err_info_container);
  ARG_NOT_NULL(audit, err_info_container);
  const auto* workspace = from_opaque(workspace_handle);
  *audit = workspace->audit;
  return VW_success;
}

VW_DLL_PUBLIC VWStatus vw_workspace_get_configured_input_type(
    const VWWorkspace* workspace_handle, VWInputType* input_type, VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}

VW_DLL_PUBLIC VWStatus vw_workspace_get_configured_hash_type(
    const VWWorkspace* workspace_handle, VWHashType* hash_type, VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}

VW_DLL_PUBLIC VWStatus vw_workspace_get_configured_hash_seed(
    const VWWorkspace* workspace_handle, uint64_t* hash_seed, VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}

VW_DLL_PUBLIC VWStatus vw_workspace_get_configured_num_bits(
    const VWWorkspace* workspace_handle, uint64_t* num_bits, VWErrorInfo* err_info_container) noexcept
{
  ARG_NOT_NULL(workspace_handle, err_info_container);
  ARG_NOT_NULL(num_bits, err_info_container);
  const auto* workspace = from_opaque(workspace_handle);
  *num_bits = workspace->num_bits;
  return VW_success;
}

VW_DLL_PUBLIC VWStatus vw_workspace_get_configured_hasher(
    const VWWorkspace* workspace_handle, VWHasher** hasher, VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}

VW_DLL_PUBLIC VWStatus vw_hash(const uint8_t* data, size_t length, uint64_t seed, VWHashType type, uint64_t*,
    VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}

// SPEEDY overloads for direct hashing against the type
VW_DLL_PUBLIC VWStatus vw_hash_all(
    const uint8_t* data, size_t length, uint64_t seed, uint64_t*, VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}

VW_DLL_PUBLIC VWStatus vw_hash_string(
    const char* data, size_t length, uint64_t seed, uint64_t*, VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}

// Can also be done as: HASH & ((1 << num_bits) - 1)
VW_DLL_PUBLIC VWStatus vw_workspace_apply_parse_mask(
    const VWWorkspace* workspace_handle, uint64_t hash, uint64_t*, VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}

// Weights
VW_DLL_PUBLIC VWStatus vw_workspace_get_num_weights(
    const VWWorkspace* workspace_handle, uint32_t* num_weights, VWErrorInfo* err_info_container) noexcept
{
  ARG_NOT_NULL(workspace_handle, err_info_container);
  ARG_NOT_NULL(num_weights, err_info_container);
  const auto* workspace = from_opaque(workspace_handle);
  *num_weights = VW::num_weights(*workspace);
  return VW_success;
}

VW_DLL_PUBLIC VWStatus vw_workspace_get_weights_per_problem(
    const VWWorkspace* workspace_handle, size_t* parameter_width, VWErrorInfo* err_info_container) noexcept
{
  ARG_NOT_NULL(workspace_handle, err_info_container);
  ARG_NOT_NULL(parameter_width, err_info_container);
  const auto* workspace = from_opaque(workspace_handle);
  *parameter_width = VW::get_stride(*workspace);
  return VW_success;
}

VW_DLL_PUBLIC VWStatus vw_workspace_get_weights_per_problem(
    const VWWorkspace* workspace_handle, uint32_t* weights_per_problem, VWErrorInfo* err_info_container) noexcept
{
  ARG_NOT_NULL(workspace_handle, err_info_container);
  ARG_NOT_NULL(weights_per_problem, err_info_container);
  const auto* workspace = from_opaque(workspace_handle);
  *weights_per_problem = workspace->wpp;
  return VW_success;
}

VW_DLL_PUBLIC VWStatus vw_workspace_get_weight(const VWWorkspace* workspace_handle, size_t index, float** weight,
    size_t* width, VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}

VW_DLL_PUBLIC VWStatus vw_workspace_set_weight(const VWWorkspace* workspace_handle, size_t index, const float* weight,
    size_t width, VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}

VW_DLL_PUBLIC VWStatus vw_workspace_is_example_parsing_equivalent(const VWWorkspace* workspace_handle_one,
    const VWWorkspace* workspace_handle_two, const char** incompatible_feature,
    VWErrorInfo* err_info_container) noexcept
try
{
  ARG_NOT_NULL(workspace_handle_one, err_info_container);
  ARG_NOT_NULL(workspace_handle_two, err_info_container);

  const auto* w1 = from_opaque(workspace_handle_one);
  const auto* w2 = from_opaque(workspace_handle_two);

  // TODO are_features_compatible should take const parameters
  *incompatible_feature = VW::are_features_compatible(*const_cast<vw*>(w1), *const_cast<vw*>(w2));
  return VW_success;
}
CATCH_RETURN(err_info_container)

VW_DLL_PUBLIC VWStatus vw_workspace_get_weight_iterator_begin(
    const VWWorkspace* workspace_handle, VWWeightIterator** iter, VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}

// Will fail if you're already at the end.
VW_DLL_PUBLIC VWStatus vw_weight_iterator_advance(VWWeightIterator* iter, VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}

VW_DLL_PUBLIC VWStatus vw_weight_iterator_can_advance(
    const VWWeightIterator* iter, bool*, VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}

VW_DLL_PUBLIC VWStatus vw_weight_iterator_dereference(const VWWeightIterator* iter, size_t* index, float** weight,
    size_t* width, VWErrorInfo* err_info_container) noexcept
{
  return VW_not_implemented;
}
