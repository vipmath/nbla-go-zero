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

// *WARNING*
// THIS FILE IS AUTO-GENERATED BY CODE GENERATOR.
// PLEASE DO NOT EDIT THIS FILE BY HAND!
// If you want to modify this file, edit following files.
// - build-tools/code_generator/templates/src_nbla_cuda_cudnn_init_cpp_template.cpp
// - build-tools/code_generator/generator/generate_src_nbla_cuda_cudnn_init_cpp.py


#include <nbla/array/cpu_array.hpp>
#include <nbla/array_registry.hpp>
#include <nbla/cuda/array/cuda_array.hpp>
#include <nbla/cuda/cudnn/cudnn.hpp>
#include <nbla/cuda/cudnn/function/convolution.hpp>
#include <nbla/cuda/cudnn/function/deconvolution.hpp>
#include <nbla/cuda/cudnn/function/max_pooling.hpp>
#include <nbla/cuda/cudnn/function/average_pooling.hpp>
#include <nbla/cuda/cudnn/function/sigmoid.hpp>
#include <nbla/cuda/cudnn/function/tanh.hpp>
#include <nbla/cuda/cudnn/function/relu.hpp>
#include <nbla/cuda/cudnn/function/softmax.hpp>
#include <nbla/cuda/cudnn/function/batch_normalization.hpp>
#include <nbla/cuda/cudnn/function/add2.hpp>
#include <nbla/cuda/cudnn/init.hpp>
#include <nbla/function_registry.hpp>
#include <nbla/init.hpp>
namespace nbla {

void init_cudnn() {
  static bool is_initialized = false;
  if (is_initialized)
    return;

  // Init CPU features
  init_cpu();
  init_cuda();

  // Array registration
  NBLA_REGISTER_ARRAY_CREATOR(CudaArray);
  NBLA_REGISTER_ARRAY_SYNCHRONIZER(CpuArray, CudaArray,
                                   synchronizer_cpu_array_cuda_array);
  NBLA_REGISTER_ARRAY_SYNCHRONIZER(CudaArray, CpuArray,
                                   synchronizer_cuda_array_cpu_array);

  // Function registration
  typedef ConvolutionCudaCudnn<float> ConvolutionCudaCudnnf;
  NBLA_REGISTER_FUNCTION_IMPL(Convolution, ConvolutionCudaCudnnf, 2, "cuda", "cudnn", int, const vector<int> &, const vector<int> &, const vector<int> &, int);
  typedef DeconvolutionCudaCudnn<float> DeconvolutionCudaCudnnf;
  NBLA_REGISTER_FUNCTION_IMPL(Deconvolution, DeconvolutionCudaCudnnf, 2, "cuda", "cudnn", int, const vector<int> &, const vector<int> &, const vector<int> &, int);
  typedef MaxPoolingCudaCudnn<float> MaxPoolingCudaCudnnf;
  NBLA_REGISTER_FUNCTION_IMPL(MaxPooling, MaxPoolingCudaCudnnf, 2, "cuda", "cudnn", const vector<int> &, const vector<int> &, bool, const vector<int> &);
  typedef AveragePoolingCudaCudnn<float> AveragePoolingCudaCudnnf;
  NBLA_REGISTER_FUNCTION_IMPL(AveragePooling, AveragePoolingCudaCudnnf, 2, "cuda", "cudnn", const vector<int> &, const vector<int> &, bool, const vector<int> &, bool);
  typedef SigmoidCudaCudnn<float> SigmoidCudaCudnnf;
  NBLA_REGISTER_FUNCTION_IMPL(Sigmoid, SigmoidCudaCudnnf, 2, "cuda", "cudnn");
  typedef TanhCudaCudnn<float> TanhCudaCudnnf;
  NBLA_REGISTER_FUNCTION_IMPL(Tanh, TanhCudaCudnnf, 2, "cuda", "cudnn");
  typedef ReLUCudaCudnn<float> ReLUCudaCudnnf;
  NBLA_REGISTER_FUNCTION_IMPL(ReLU, ReLUCudaCudnnf, 2, "cuda", "cudnn", bool);
  typedef SoftmaxCudaCudnn<float> SoftmaxCudaCudnnf;
  NBLA_REGISTER_FUNCTION_IMPL(Softmax, SoftmaxCudaCudnnf, 2, "cuda", "cudnn", int);
  typedef BatchNormalizationCudaCudnn<float> BatchNormalizationCudaCudnnf;
  NBLA_REGISTER_FUNCTION_IMPL(BatchNormalization, BatchNormalizationCudaCudnnf, 2, "cuda", "cudnn", const vector<int> &, float, float, bool);
  typedef Add2CudaCudnn<float> Add2CudaCudnnf;
  NBLA_REGISTER_FUNCTION_IMPL(Add2, Add2CudaCudnnf, 2, "cuda", "cudnn", bool);

  is_initialized = true;
}
}
