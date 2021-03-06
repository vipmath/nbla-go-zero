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

#ifndef GAMESTATE_H_INCLUDED
#define GAMESTATE_H_INCLUDED

#include <memory>
#include <string>
#include <vector>

#include "FastState.h"

class GameState : public FastState {
public:
    explicit GameState() = default;
    explicit GameState(const FastState* rhs) {
        // Copy in fields from base class.
        *(static_cast<FastState*>(this)) = *rhs;
        anchor_game_history();
    }
    void init_game(float komi);
    void reset_game();
    void anchor_game_history(void);

    const FastBoard& get_past_board(int moves_ago) const;

    void play_move(int color, int vertex);
    void play_move(int vertex);

    bool has_resigned() const;
    int who_resigned() const;

private:
    std::vector<std::shared_ptr<const FastBoard>> game_history;
    int m_resigned{FastBoard::EMPTY};
};

#endif
