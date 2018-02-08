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

#ifndef LEELA_AGENT_H_INCLUDED
#define LEELA_AGENT_H_INCLUDED

#include "config.h"
#include <string>
#include <vector>
#include <functional>
#include "GameState.h"
#include "../gtp.h"
#include "UCTSearch.h"
#include "Interface.h"

class NENG_API Leela : public IGtpAgent {
    
protected:
    std::shared_ptr<GameState> game;
    std::unique_ptr<UCTSearch> search;
        
public:
    Leela(ILeelaModel*, const std::string& logfile);
    
    std::string name();
    void clear_board();
    void komi(float);
    void time_left(int player, double t);
    void play(int player, int move);
    void pass(int player);
    void resign(int player);
    int genmove(int player, bool commit);
    int genmove();
    void ponder_on_idle();
    void ponder_enable();
    void stop_ponder();
    float final_score();
    void set_timecontrol(int maintime, int byotime, int byostones, int byoperiods);
    void game_over();
    void quit();
    void heatmap(int rotation) const;
    int get_color(int idx) const;
    bool dump_sgf(const std::string& path) const;

    void clear_cache();
    int selfplay(int playouts, const std::string& sgffile, std::function<void(int, int[])> callback);
};

class NENG_API PolicyPlayer : public Leela {
     
public:
    PolicyPlayer(ILeelaModel* model, const std::string& logfile);
    
    std::string name();
    int genmove(int player, bool commit);
    void ponder_on_idle();
};


#endif