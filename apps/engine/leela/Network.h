/*
    This file is part of Leela Zero.
    Copyright (C) 2017 Gian-Carlo Pascutto

    Leela Zero is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leela Zero is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Leela Zero.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include "config.h"
#include <vector>
#include <string>
#include <bitset>
#include <memory>
#include <array>
#include <mutex>
#include <unordered_map>

#ifdef USE_OPENCL
#include <atomic>
class UCTNode;
#endif

#include "FastState.h"
#include "GameState.h"
#include "Interface.h"



class Network {
public:
    enum Ensemble {
        DIRECT, RANDOM_ROTATION
    };
    using NNPlanes = InputFeature;
    using scored_node = std::pair<float, int>;
    using Netresult = std::pair<std::vector<scored_node>, float>;

    static Netresult get_scored_moves(const GameState* state,
                                        Ensemble ensemble,
                                        int rotation = -1,
                                        bool skip_cache = false);


    // File format version
    static constexpr int FORMAT_VERSION = 1;
    static constexpr auto INPUT_MOVES = 8;
    static constexpr auto INPUT_CHANNELS = 2 * INPUT_MOVES + 2;

    static void register_model(ILeelaModel*);
    static void initialize();
    static void benchmark(const GameState * state, int iterations = 1600);
    static void show_heatmap(const FastState * state, Netresult & netres,
                             bool topmoves);

    static void gather_features(const GameState* state, NNPlanes& planes);

private:
    static void fill_input_plane_pair(
        const FullBoard& board, BoardPlane& black, BoardPlane& white);
    static Netresult get_scored_moves_internal(
      const GameState * state, const NNPlanes & planes, int rotation);
    static int rotate_nn_idx(const int vertex, int symmetry);

    static ILeelaModel* model;
    static std::mutex mtx_;
};




#endif