#include "model.h"
#include <iostream>
#include <fstream>

namespace dlib {


static bool restore_string(std::istream& is, std::string& str) {

    int sz;
    is.read((char*)&sz, 4);
    if (sz <=0 || sz > 1024)
        return false;

    str.resize(sz);
    if (is.read(&str[0], sz).gcount() != sz)
        return false;

    if (strlen(&str[0]) != sz)
        return false;

    return true;
}

static bool restore_variable_dim(std::istream& is, std::vector<int>& shape) {

    // skip need_grad flag
    char c;
    if (is.read(&c, 1).gcount() != 1)
        return false;

  int dim;
  if (is.read((char*)&dim, 4).gcount() != 4)
    return false;

  for (int i=0; i<dim; i++) {
    int d;
    if (is.read((char*)&d, 4).gcount() != 4)
      return false;
    shape.push_back(d);
  }

  return true;
}


static bool end_with(const std::string& s, const std::string& sub) {
    auto pos = s.rfind(sub);
    return pos == s.size() - sub.size();
}


bool load_zero_weights(zero_net_type& net, const std::string& path) {

    std::ifstream ifs(path, std::ifstream::binary);
    if (ifs.fail())
        return false;

    // std::cout << net << std::endl;

    // sort params in stack
    constexpr int block_w_offset = 5;
    constexpr int value_w_offset = block_w_offset + 19*10;
    constexpr int policy_w_offset = value_w_offset + 5 + 4;

    std::vector<param_data> params(policy_w_offset + 5 + 2); 

    while(true) {
        std::string key;
        std::vector<int> shape;

        if (!restore_string(ifs, key))
            break;

        int offset = -1;
        int block_sub = 0;

        if (key.find("init/") == 0) {
            offset = 0;
        } 
        else if (key.find("block_") == 0) {
            // block_7/0/bn/b
            auto pos = key.find('/');
            auto idx = std::stoi(key.substr(6, pos - 6));
            block_sub = std::stoi(key.substr(pos+1, 1));
            offset = block_w_offset + (idx-1)*10;
        }
        else if (key.find("policy_head/bn/") == 0) {
            offset = policy_w_offset;
            
        }
        else if (key == "policy_head/fc/fc/W") {
            offset = policy_w_offset + 5;
        }
        else if (key == "policy_head/fc/fc/b") {
            offset = policy_w_offset + 6;
        }
        else if (key.find("value_head/bn/") == 0) {
            offset = value_w_offset;
        }
        else if (key == "value_head/fc1/fc/W") {
            offset = value_w_offset + 5;
        }
        else if (key == "value_head/fc1/fc/b") {
            offset = value_w_offset + 6;
        }
        else if (key == "value_head/fc2/fc/W") {
            offset = value_w_offset + 7;
        }
        else if (key == "value_head/fc2/fc/b") {
            offset = value_w_offset + 8;
        }

        if (end_with(key, "conv/W")) {
            offset += 0 + block_sub*5;
        }
        else if (end_with(key, "bn/g")) {
            offset += 1 + block_sub*5;
        }
        else if (end_with(key, "bn/b")) {
            offset += 2 + block_sub*5;
        }
        else if (end_with(key, "bn/m")) {
            offset += 3 + block_sub*5;
        }
        else if (end_with(key, "bn/v")) {
            offset += 4 + block_sub*5;
        }

    
        if (!restore_variable_dim(ifs, shape))
            return false;

        int count = 1;
        for (auto d : shape) count*=d;
        std::vector<float> data(count);
        if (ifs.read((char*)(&data[0]), count*sizeof(float)).gcount() != count*sizeof(float))
            return false;

        if (offset >=0)
            params[offset] = {shape, data};
    }

    net.consume_params(params.begin());
    return true;
}


bool load_dark_weights(dark_net_type& net, const std::string& path) {

    std::ifstream ifs(path, std::ifstream::binary);
    if (ifs.fail())
        return false;

    
    std::vector<param_data> go_params; 

    int major;
        int minor;
        int revision;
        ifs.read((char*)&major, 4);
        ifs.read((char*)&minor, 4);
        ifs.read((char*)&revision, 4);
        
    if ((major*10 + minor) > 20){
        return false;
    }
        
        if ((major*10 + minor) >= 2){
            size_t seen;
            ifs.read((char*)&seen, sizeof(size_t));
        } else {
            int seen;
            ifs.read((char*)&seen, 4);
        }

    int k = 1;
    for (int i=0; i<13; ++i) {
        std::vector<float> b(256);
        std::vector<float> g(256);
        std::vector<float> m(256);
        std::vector<float> v(256);
        std::vector<float> w(256*k*3*3);
        
        ifs.read((char*)&b[0], sizeof(float)*b.size());
        ifs.read((char*)&g[0], sizeof(float)*g.size());
        ifs.read((char*)&m[0], sizeof(float)*m.size());
        ifs.read((char*)&v[0], sizeof(float)*v.size());
        ifs.read((char*)&w[0], sizeof(float)*w.size());

        go_params.push_back({std::vector<int>{256, k, 3, 3}, w});
        go_params.push_back({std::vector<int>{1, 256, 1, 1}, g});
        go_params.push_back({std::vector<int>{1, 256, 1, 1}, b});
        go_params.push_back({std::vector<int>{1, 256, 1, 1}, m});
        go_params.push_back({std::vector<int>{1, 256, 1, 1}, v});
        k = 256;
    }

    std::vector<float> b(1);
    std::vector<float> w(1*k*1*1);
    ifs.read((char*)&b[0], sizeof(float)*b.size());
    ifs.read((char*)&w[0], sizeof(float)*w.size());

    go_params.push_back({std::vector<int>{1, k, 1, 1}, w});
    go_params.push_back({std::vector<int>{1, 1}, b});

    std::vector<float> b_w(board_count*board_moves);
    auto W_d = &b_w[0];
    for (int i=0; i<board_count; i++)
        for (int j=0; j<board_moves; j++)
            *(W_d++) = (i==j) ? 1 : 0;
    *(W_d-1) = 1;

    go_params.push_back({std::vector<int>{board_count, board_moves}, b_w});

    net.consume_params(go_params.begin());
    return true;
}

//////////////////////////////////////////////////////////

zero_model::zero_model()
: max_batch_size(32)
, zero_weights_loaded(false)
{}

void zero_model::set_batch_size(size_t size) {
    max_batch_size = size;
}

bool zero_model::load_weights(const std::string& path) {

    std::ifstream ifs(path, std::ifstream::binary);
    if (ifs.fail())
        return false;

    std::string key;
    bool is_zero_model = restore_string(ifs, key);
    ifs.close();

    if (is_zero_model) {
        zero_weights_loaded = load_zero_weights(zero_net, path);
        return zero_weights_loaded;
    } else
        return load_dark_weights(dark_net, path);
}


const tensor& zero_model::forward(const tensor& input, double temperature) {
    if (input.k() == 1) {
        layer<0>(dark_net).layer_details().set_temprature(temperature);
        return dark_net.forward(input);
    } else {
        layer<0>(zero_net).layer_details().set_temprature(temperature);
        return zero_net.forward(input);
    }
}


std::vector<zero_model::netres> zero_model::predict_top(const feature& ft, int top_n, double temperature, bool darknet_backend) {

    auto probs = predict_policy(ft, temperature, true, darknet_backend);

    std::vector<netres> index(top_n);

    for(auto& e : index) e.index = -1;
    for(int i = 0; i < probs.size(); ++i) {
        int curr = i;
        for(auto& e : index) {
            if((e.index < 0) || probs[curr] > probs[e.index]) {
                int swap = curr;
                curr = e.index;
                e.index = swap;
                e.score = probs[e.index];
            }
        }
    }

    return index;
}


void zero_model::predict_policies(
                std::vector<feature>::const_iterator begin, 
                std::vector<feature>::const_iterator end, 
                std::vector<prediction>::iterator it, 
                double temperature, 
                bool darknet_backend) {

    darknet_backend = darknet_backend || !zero_weights_loaded;
    auto& input = features_to_tensor(begin, end, darknet_backend);

    int batch = input.num_samples();

    auto& out_tensor = forward(input, temperature);
    auto src = out_tensor.host();

    for (int n=0;n<batch; n++) {
        it->resize(board_moves);
        std::copy(src, src+board_moves, it->begin());
        src += board_moves;
        it++;
    }
}

void zero_model::predict_values(
                std::vector<feature>::const_iterator begin, 
                std::vector<feature>::const_iterator end,
                std::vector<float>::iterator it) {

    int batch = std::distance(begin, end);
    if (!zero_weights_loaded) {
        for (int n=0;n<batch; n++) {
            (*it++) = 0;
        }
        return;
    }

    auto& input = features_to_tensor(begin, end, false);
    auto& v_tensor = layer<6>(zero_net).forward(input);
    auto data = v_tensor.host();
    for (int n=0;n<batch; n++) {
        (*it++) = data[n];
    }
}

void zero_model::predict(
                std::vector<feature>::const_iterator begin, 
                std::vector<feature>::const_iterator end,
                std::vector<prediction_ex>::iterator it, 
                double temperature, 
                bool darknet_backend) {

    darknet_backend = darknet_backend || !zero_weights_loaded;
    
    auto& input = features_to_tensor(begin, end, darknet_backend);

    int batch = input.num_samples();
    auto& out_tensor = forward(input, temperature);
    auto src = out_tensor.host();
    auto& v_tensor = layer<6>(zero_net).get_output();
    auto data = v_tensor.host();

    for (int n=0;n<batch; n++) {
        it->first.resize(board_moves);
        std::copy(src, src+board_moves, it->first.begin());
        src += board_moves;
        if (darknet_backend) {
            it->second = 0;
        } else {
            it->second = data[n];
        }

        it++;
    }
}

std::vector<float> zero_model::predict_policy(const feature& ft, double temperature, bool suppress_invalid, bool darknet_backend) {

    std::vector<std::vector<float>> out(1);
    std::vector<feature> features{ft};

    predict_policies(features.begin(), features.end(), out.begin(), temperature, darknet_backend);
    if (suppress_invalid)
        suppress_policy(out[0], ft);
    return out[0];
}

std::vector<std::vector<float>> zero_model::predict_policy(const std::vector<feature>& features, double temperature, 
                                        bool suppress_invalid, bool darknet_backend) {

    std::vector<std::vector<float>> out(features.size());

    auto in_it = features.begin();
    auto out_it = out.begin();
    auto count = features.size();
    while (count > 0) {
        auto batch_size = std::min(count, max_batch_size);
        predict_policies(in_it, in_it+batch_size, out_it, temperature, darknet_backend);

        if (suppress_invalid) {
            for (int i=0; i<batch_size; i++) {
                suppress_policy(*(out_it+i), *(in_it+i));
            }
        }

        in_it += batch_size;
        out_it += batch_size;
        count -= batch_size;
    }

    return out;
}


float zero_model::predict_value(const feature& ft) {

    std::vector<float> out(1);
    std::vector<feature> features{ft};

    predict_values(features.begin(), features.end(), out.begin());
    return out[0];
}

std::vector<float> zero_model::predict_value(const std::vector<feature>& features) {

    std::vector<float> out(features.size());

    auto in_it = features.begin();
    auto out_it = out.begin();
    auto count = features.size();
    while (count > 0) {
        auto batch_size = std::min(count, max_batch_size);
        predict_values(in_it, in_it+batch_size, out_it);
        in_it += batch_size;
        out_it += batch_size;
        count -= batch_size;
    }

    return out;
}

zero_model::prediction_ex zero_model::predict(const feature& ft, double temperature, bool suppress_invalid, bool darknet_backend) {
    std::vector<prediction_ex> out(1);
    std::vector<feature> features{ft};

    predict(features.begin(), features.end(), out.begin(), temperature, darknet_backend);
    if (suppress_invalid)
        suppress_policy(out[0].first, ft);

    return out[0];
}

std::vector<zero_model::prediction_ex> zero_model::predict(const std::vector<feature>& features, double temperature, bool suppress_invalid, bool darknet_backend) {

    std::vector<prediction_ex> out(features.size());

    auto in_it = features.begin();
    auto out_it = out.begin();
    auto count = features.size();
    while (count > 0) {
        auto batch_size = std::min(count, max_batch_size);
        predict(in_it, in_it+batch_size, out_it, temperature, darknet_backend);

        if (suppress_invalid) {
            for (int i=0; i<batch_size; i++) {
                suppress_policy((out_it+i)->first, *(in_it+i));
            }
        }

        in_it += batch_size;
        out_it += batch_size;
        count -= batch_size;
    }

    return out;
}


const tensor& zero_model::features_to_tensor(
            std::vector<feature>::const_iterator begin, 
            std::vector<feature>::const_iterator end, 
            bool darknet_backend) {

    const int batch_size = std::distance(begin, end);

    cached_input.set_size(batch_size, darknet_backend ? 1 : num_planes, board_size, board_size);

    auto dst = cached_input.host_write_only();
    for (int n=0; n<batch_size; n++, begin++) {
            
        if (darknet_backend) {
            for (int i=0; i<board_count; i++) {
                if ((*begin)[0][i]) *dst = 1;
                else if ((*begin)[8][i]) *dst = -1;
                else *dst = 0;
                dst++;
            }
        } else {
            for (int c=0; c< num_planes; c++) {
                for (int i=0; i<board_count; i++) {
                    *(dst++) = (float)(*begin)[c][i];
                }
            }
        }
    }

    return cached_input;
}


void zero_model::suppress_policy(prediction& prob, const feature& ft) {

    auto& b0 = ft[0];
    auto& b1 = ft[8];

    for (int i=0; i<board_count; i++)
        if (b0[i] || b1[i])
            prob[i] = 0;
}

}