/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef C2_REACTION_H
#define C2_REACTION_H

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;

typedef std::string reaction;
typedef std::vector<reaction> reaction_vect;
typedef std::string reaction_key;
typedef std::map<reaction_key, reaction_vect> reaction_map;

namespace ns3 {

class C2Reaction{
public:
    C2Reaction();
    C2Reaction(std::string);
    C2Reaction(reaction_map);
    void setReaction(std::string);
    void setReaction(reaction_map);
    const reaction_map  getReaction();
    const reaction_vect getReaction(reaction_key);
    const reaction getReaction(reaction_key, int);

    static std::string uint8_tToString(uint8_t value);
    static std::vector<uint8_t> stringToUint8_t(std::string value);

private:
    reaction_map m_reaction;
    reaction_map convertJson(json);
};

}

#endif /* C2_REACTION_H */
