# Copyright (c) 2017 Sony Corporation. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import pytest
import numpy as np
import nnabla as nn
import nnabla.functions as F
from nbla_test_utils import list_context

ctxs = list_context('Reshape')


def ref_reshape(x, shape):
    return x.reshape(shape).copy()


@pytest.mark.parametrize("ctx, func_name", ctxs)
@pytest.mark.parametrize("seed", [313])
@pytest.mark.parametrize("inshape, outshape", [((1, 1, 6), (1, 2, 3)), ((2, 3), (1, 6))])
def test_reshape_forward_backward(seed, inshape, outshape, ctx, func_name):
    from nbla_test_utils import function_tester
    rng = np.random.RandomState(seed)
    # Input
    inputs = [rng.randn(*inshape).astype(np.float32)]
    function_tester(rng, F.reshape, ref_reshape, inputs, func_args=[
                    outshape], ctx=ctx, func_name=func_name)
