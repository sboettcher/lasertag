#ifndef LASERTAG_PLAYER_H
#define LASERTAG_PLAYER_H

#define HIT_LOST_PERCENT 10
#define HEALTH_AT_START 100
#define FULL_AMMUNITION 50


class Player {
public:
  
  // Constructor
  Player();
  Player(int identifier);
  ~Player();
  
  // If Player got hit
  int gotHit();
  
  // If Player makes one shot
  int shooted();
  
  // returns the health in %
  int getHealth();
  
  // returns the ammunition
  int getAmmo();
  
  // returns the ammunition
  int getID();
  
private:
  
  int _health;
  int _id;
  int _ammunition;
};

#endif