// Copyright (c) 2017 Sony Corporation. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/** AbsoluteError
 */
#ifndef __NBLA_CUDA_FUNCTION_ABSOLUTEERROR_HPP__
#define __NBLA_CUDA_FUNCTION_ABSOLUTEERROR_HPP__

#include <nbla/cuda/common.hpp>
#include <nbla/cuda/cuda.hpp>
#include <nbla/function/absolute_error.hpp>

#include <nbla/cuda/function/utils/base_transform_binary.hpp>

namespace nbla {
/** @copydoc AbsoluteError
*/
NBLA_DECLARE_TRANSFORM_BINARY_CUDA(AbsoluteError);
}
#endif