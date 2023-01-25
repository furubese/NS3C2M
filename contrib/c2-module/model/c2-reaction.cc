#include "c2-reaction.h"
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;

namespace ns3 {

C2Reaction::C2Reaction()
: m_reaction({
    {"default", {"0000", "0000"}}
    })
{
}

C2Reaction::C2Reaction(std::string reaction_file){
    setReaction(reaction_file);
}
C2Reaction::C2Reaction(reaction_map reaction){
    setReaction(reaction);
}
void C2Reaction::setReaction(reaction_map reaction){
    m_reaction = reaction;
}
void C2Reaction::setReaction(std::string reaction_file){
    std::ifstream f(reaction_file);
    json jdata = json::parse(f);
    m_reaction = convertJson(jdata);
}
const reaction_map C2Reaction::getReaction(){
    return m_reaction;
}
const reaction_vect C2Reaction::getReaction(reaction_key reaction_key){
    return m_reaction[reaction_key];
}
const reaction C2Reaction::getReaction(reaction_key reaction_key, int number){
    return m_reaction[reaction_key][number];
}

std::string C2Reaction::uint8_tToString(uint8_t value){
    std::string ret_str;
    char hexstr[4];
    sprintf(hexstr, "%02x", value);
    ret_str += hexstr;
    return ret_str;
}
std::vector<uint8_t> C2Reaction::stringToUint8_t(std::string value){
    std::vector<uint8_t> received_message;
    for (long unsigned int i=0; i<value.size(); i=i+2){
        received_message.push_back(static_cast<uint8_t>(std::stoul("0x"+value.substr(i, 2), nullptr, 16)));
    }
    return received_message;
}

reaction_map C2Reaction::convertJson(json data){
    reaction_map new_m_reaction;
    for(auto& [key, value] : data.items()){
        reaction_vect reactionList;
        for(std::string r : value){
            /*reaction reaction = stringToUint8_t(r);*/
            reaction reaction = r;
            reactionList.push_back(reaction);
        }
        new_m_reaction[key] = reactionList;
    }
    return new_m_reaction;
}

}