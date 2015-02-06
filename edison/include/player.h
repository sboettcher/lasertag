#ifndef LASERTAG_PLAYER_H
#define LASERTAG_PLAYER_H

#include <string>

#define HIT_LOST_PERCENT 10
#define FULL_HEALTH 100
#define FULL_AMMO 50

#define MAX_NAME_LENGTH 10

class Player {
  public:
    // Constructor
    Player();
    
    // The player is getting full ammo again
    void reload(int amount);
    
    // The player is getting full health again
    void refill_health(int amount);
    
    // If Player got hit he will loose a defined amount of health
    // returns new value
    int hit();
    
    // If Player fires tagger he will loose 1 ammunition
    // returns new value
    int fired();
    
    // setter/getter
    int get_health() {return m_health;}
    int get_max_health() {return m_full_health;}
    int get_ammo() {return m_ammo;}
    int get_max_ammo() {return m_full_ammo;}
    
    void set_ID(int id) {m_id = (uint8_t) id;}
    uint8_t get_ID() {return m_id;}
    
    void set_name(std::string name) {m_name = name;}
    std::string get_name() {return m_name;}
    
    void set_color(uint16_t color) {m_color = color;}
    uint16_t get_color() {return m_color;}
    
    void set_score(int score) {m_score = score;}
    int get_score() {return m_score;}

    void set_vest(bool vest) {m_vest = vest;}
    int get_vest() {return m_vest;}
  
  private:
    // member vars
    uint8_t m_id;
    int m_score;
    int m_health;
    int m_full_health;
    int m_ammo;
    int m_full_ammo;
    uint16_t m_color;
    std::string m_name;
    bool m_vest;
};

#endif
